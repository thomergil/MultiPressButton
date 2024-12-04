#include "MultiPressButton.h"

// Initialize static member
std::vector<MultiPressButton*> MultiPressButton::buttonList;

MultiPressButton::MultiPressButton(unsigned pin, uint8_t mode,
                                   unsigned long debounce_ms,
                                   unsigned long longPress_ms,
                                   unsigned long multi_press_window_ms)
    : pin(pin),
      mode(mode),
      singleAction(nullptr),
      doubleAction(nullptr),
      tripleAction(nullptr),
      longPressAction(nullptr),
      pressCount(0),
      pressStart(0),
      lastPressTime(0),
      lastButtonPosition(RELEASED),
      lastDebounceTime(0),
      state(IDLE),
      singlePressed(false),
      doublePressed(false),
      triplePressed(false),
      longPressed(false),
      BUTTON_DEBOUNCE(debounce_ms),
      LONG_PRESS_DURATION(longPress_ms),
      MULTI_PRESS_WINDOW(multi_press_window_ms),
      timerInitialized(false),
      writeIndex(0),
      readIndex(0) {}

void MultiPressButton::setup() {
  pinMode(pin, mode);
  buttonList.push_back(this);
  if (!timerInitialized) {
    TimerLib.setInterval_us(_pollButtons, POLL_INTERVAL * 1000);
    timerInitialized = true;
  }
}

bool ICACHE_RAM_ATTR MultiPressButton::_isBufferFull() const {
  return ((writeIndex + 1) & (RING_BUFFER_SIZE - 1)) == readIndex;
}

void ICACHE_RAM_ATTR
MultiPressButton::_addEvent(ButtonPosition buttonPosition) {
  if (_isBufferFull()) {
    return;
  }
  eventBuffer[writeIndex].timestamp = millis();
  eventBuffer[writeIndex].buttonPosition = buttonPosition;
  writeIndex = (writeIndex + 1) & (RING_BUFFER_SIZE - 1);
}

void ICACHE_RAM_ATTR MultiPressButton::_pollButtons() {
  for (MultiPressButton* button : buttonList) {
    ButtonPosition buttonPosition =
        digitalRead(button->getPin()) == LOW ? PRESSED : RELEASED;

    if (buttonPosition == button->lastButtonPosition) {
      continue;
    }

    unsigned long now = millis();
    if ((now - button->lastDebounceTime) >= button->BUTTON_DEBOUNCE) {
      button->_addEvent(buttonPosition);
      button->lastDebounceTime = now;
      button->lastButtonPosition = buttonPosition;
    }
  }
}

void MultiPressButton::_processEvent(ButtonEvent event) {
  switch (state) {
    case IDLE:
    case WAITING_MULTIPRESS:
      if (event.buttonPosition == PRESSED) {
        BUTTON_DEBUG_PRINTLN(
            "New press detected in IDLE or WAITING_MULTIPRESS state");
        longPressed = false;
        pressStart = event.timestamp;
        state = PRESSING;
      }

      if (state == WAITING_MULTIPRESS &&
          ((event.timestamp - lastPressTime) > MULTI_PRESS_WINDOW)) {
        BUTTON_DEBUG_PRINTLN("Multi-press window timeout with " +
                             String(pressCount) + " presses");

        if (pressCount >= 3) {
          singlePressed = false;
          doublePressed = false;
          if (tripleAction) {
            BUTTON_DEBUG_PRINTLN("Executing delayed triple press action");
            tripleAction();
            triplePressed = false;
          } else {
            BUTTON_DEBUG_PRINTLN("Setting triplePressed flag");
            triplePressed = true;
          }
        } else if (pressCount == 2) {
          singlePressed = false;
          triplePressed = false;
          if (doubleAction) {
            BUTTON_DEBUG_PRINTLN("Executing delayed double press action");
            doubleAction();
            doublePressed = false;
          } else {
            BUTTON_DEBUG_PRINTLN("Setting doublePressed flag");
            doublePressed = true;
          }
        } else if (pressCount == 1) {
          doublePressed = false;
          triplePressed = false;
          if (singleAction) {
            BUTTON_DEBUG_PRINTLN("Executing delayed single press action");
            singleAction();
            singlePressed = false;
          } else {
            BUTTON_DEBUG_PRINTLN("Setting singlePressed flag");
            singlePressed = true;
          }
        }

        BUTTON_DEBUG_PRINTLN("Setting pressCount to 0 and returning to IDLE");
        pressCount = 0;
        state = IDLE;
      }
      break;

    case PRESSING:
      if (event.buttonPosition == PRESSED) {
        if (!longPressed && pressCount == 0 &&
            (event.timestamp - pressStart >= LONG_PRESS_DURATION)) {
          BUTTON_DEBUG_PRINTLN("Long press detected");
          longPressed = true;
          if (longPressAction) {
            BUTTON_DEBUG_PRINTLN("Executing long press action");
            longPressAction();
            longPressed = false;
          } else {
            BUTTON_DEBUG_PRINTLN("No long press action defined");
          }
          state = LONG_PRESSED;
        }
      } else {
        pressCount++;
        lastPressTime = event.timestamp;
        BUTTON_DEBUG_PRINTLN("Valid press " + String(pressCount) + " detected");

        if (pressCount == 1 && singleAction && !doubleAction && !tripleAction) {
          BUTTON_DEBUG_PRINTLN("Executing immediate single press action");
          singleAction();
          pressCount = 0;
          state = IDLE;
        } else if (pressCount == 2 && doubleAction && !tripleAction) {
          BUTTON_DEBUG_PRINTLN("Executing immediate double press action");
          doubleAction();
          pressCount = 0;
          state = IDLE;
        } else if (pressCount >= 3 && tripleAction) {
          BUTTON_DEBUG_PRINTLN("Executing immediate triple press action");
          tripleAction();
          pressCount = 0;
          state = IDLE;
        } else {
          BUTTON_DEBUG_PRINTLN("Entering WAITING_MULTIPRESS state");
          state = WAITING_MULTIPRESS;
        }
      }
      break;

    case LONG_PRESSED:
      if (event.buttonPosition == RELEASED) {
        BUTTON_DEBUG_PRINTLN("Long press ended, returning to IDLE");
        pressCount = 0;
        state = IDLE;
      }
      break;
  }
}

bool MultiPressButton::singlePress() {
  bool value = singlePressed;
  singlePressed = false;
  return value;
}

bool MultiPressButton::doublePress() {
  bool value = doublePressed;
  doublePressed = false;
  return value;
}

bool MultiPressButton::triplePress() {
  bool value = triplePressed;
  triplePressed = false;
  return value;
}

bool MultiPressButton::longPress() {
  bool value = longPressed;
  longPressed = false;
  return value;
}

void MultiPressButton::setActions(std::function<void()> single,
                                  std::function<void()> doubl,
                                  std::function<void()> triple,
                                  std::function<void()> longPress) {
  singleAction = single;
  doubleAction = doubl;
  tripleAction = triple;
  longPressAction = longPress;
}

unsigned MultiPressButton::getPin() const { return pin; }

void MultiPressButton::process() {
  while (readIndex != writeIndex) {
    ButtonEvent event = {eventBuffer[readIndex].timestamp,
                         eventBuffer[readIndex].buttonPosition};
    BUTTON_DEBUG_PRINTF(
        "Event: %s at %lu\n",
        event.buttonPosition == PRESSED ? "PRESSED" : "RELEASED",
        event.timestamp);
    _processEvent(event);
    readIndex = (readIndex + 1) & (RING_BUFFER_SIZE - 1);
  }

  ButtonEvent timeoutCheck = {millis(), lastButtonPosition};
  _processEvent(timeoutCheck);
}
