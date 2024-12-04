#ifndef MULTI_PRESS_BUTTON_H
#define MULTI_PRESS_BUTTON_H

#include <uTimerLib.h>

#include <functional>
#include <vector>

// Debug output macros - comment out to disable debug printing
// #define MULTI_PRESS_BUTTON_DEBUG 1

#ifdef MULTI_PRESS_BUTTON_DEBUG
#define BUTTON_DEBUG_PRINT(x) Serial.print(x)
#define BUTTON_DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define BUTTON_DEBUG_PRINTLN(x) Serial.println(x)
#else
#define BUTTON_DEBUG_PRINT(x)
#define BUTTON_DEBUG_PRINTF(...)
#define BUTTON_DEBUG_PRINTLN(x)
#endif

// Default timing configurations
const unsigned long POLL_INTERVAL = 10;  // Polling interval in milliseconds
const unsigned long DEFAULT_MULTI_PRESS_WINDOW =
    1000;  // Time window for counting multiple presses (ms)
const unsigned long DEFAULT_BUTTON_DEBOUNCE =
    50;  // Debounce time in milliseconds
const unsigned long DEFAULT_LONG_PRESS_DURATION =
    1000;  // Long press duration in milliseconds

/**
 * MultiPressButton - A button handler that supports single, double, triple, and
 * long presses
 *
 * This class provides two distinct interfaces for handling button presses. You
 * should choose one interface style and stick with it - mixing the two
 * interfaces is not supported and may lead to unexpected behavior.
 *
 * Lambda/Callback Interface:
 * - Set handlers using setActions()
 * - Provides immediate response when possible
 * - Will trigger immediately for single press if no double/triple actions
 * defined
 * - Will trigger immediately for double press if no triple action defined
 *
 * Polling Interface:
 * - Check button states using singlePress(), doublePress(), triplePress(),
 * longPress()
 * - Gives direct control over when/how to handle presses
 * - Each check method clears its state when called
 *
 * Usage:
 * 1. Create an instance: MultiPressButton button(PIN);
 * 2. Call button.setup() in your setup()
 * 3. Call button.process() regularly in your loop()
 * 4. Either:
 *    a) Set callbacks with setActions(), or
 *    b) Poll for events with single/double/triple/longPress()
 */
class MultiPressButton {
 private:
  enum ButtonPosition { PRESSED, RELEASED };
  struct ButtonEvent {
    unsigned long timestamp;
    ButtonPosition buttonPosition;
  };

  enum StateMachineState { IDLE, PRESSING, LONG_PRESSED, WAITING_MULTIPRESS };

  unsigned pin;
  uint8_t mode;
  std::function<void()> singleAction;
  std::function<void()> doubleAction;
  std::function<void()> tripleAction;
  std::function<void()> longPressAction;
  int pressCount;
  unsigned long pressStart;
  unsigned long lastPressTime;
  ButtonPosition lastButtonPosition;
  unsigned long lastDebounceTime;
  StateMachineState state;

  // State flags for polling interface
  bool singlePressed;
  bool doublePressed;
  bool triplePressed;
  bool longPressed;

  // Timing configuration
  const unsigned long BUTTON_DEBOUNCE;
  const unsigned long LONG_PRESS_DURATION;
  const unsigned long MULTI_PRESS_WINDOW;
  static const uint8_t RING_BUFFER_SIZE = 32;  // Must be power of 2

  bool timerInitialized;
  static std::vector<MultiPressButton*> buttonList;
  volatile ButtonEvent eventBuffer[RING_BUFFER_SIZE];
  volatile uint8_t writeIndex;
  volatile uint8_t readIndex;

  // Private helper methods
  bool ICACHE_RAM_ATTR _isBufferFull() const;
  void ICACHE_RAM_ATTR _addEvent(ButtonPosition buttonPosition);
  static void ICACHE_RAM_ATTR _pollButtons();
  void _processEvent(ButtonEvent event);

 public:
  /**
   * Constructor
   * @param pin The pin number the button is connected to
   * @param mode Pin mode (INPUT_PULLUP by default)
   * @param debounce_ms Debounce time in milliseconds
   * @param longPress_ms Duration needed to trigger a long press
   * @param multi_press_window_ms Time window for detecting multiple presses
   */
  MultiPressButton(
      unsigned pin, uint8_t mode = INPUT_PULLUP,
      unsigned long debounce_ms = DEFAULT_BUTTON_DEBOUNCE,
      unsigned long longPress_ms = DEFAULT_LONG_PRESS_DURATION,
      unsigned long multi_press_window_ms = DEFAULT_MULTI_PRESS_WINDOW);

  /**
   * Initialize the button hardware and timer
   * Must be called from setup() after system initialization
   */
  void setup();

  /**
   * Process button events
   * Must be called regularly from your main loop
   */
  void process();

  /**
   * Set callback functions for button events
   * @param single Single press callback
   * @param doubl Double press callback
   * @param triple Triple press callback
   * @param longPress Long press callback
   */
  void setActions(std::function<void()> single = nullptr,
                  std::function<void()> doubl = nullptr,
                  std::function<void()> triple = nullptr,
                  std::function<void()> longPress = nullptr);

  /**
   * Poll for button events
   * Each call clears the respective event state
   * @return true if the event occurred since last check
   */
  bool singlePress();
  bool doublePress();
  bool triplePress();
  bool longPress();

  /**
   * Get the pin number this button is attached to
   * @return The pin number
   */
  unsigned getPin() const;
};

#endif  // MULTI_PRESS_BUTTON_H
