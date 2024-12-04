# MultiPressButton

A multiplatform library for handling button input with support for **single**, **double**, **triple**, and **long presses**. It supports callback style and polling style (though you cannot use both styles simultaneously for one button). It is robust against delays, e.g., blocking network traffic in that **no button presses are lost during blocking operations**.

## Sample: callback style

```cpp
#include <MultiPressButton.h>

MultiPressButton button(4);  // Button connected to pin 4

void setup() {
    Serial.begin(9600);
    button.setup();
    
    // React to button presses with lambdas
    button.setActions(
        []() { Serial.println("Single press!"); },
        []() { Serial.println("Double press!"); },
        []() { Serial.println("Triple press!"); },
        []() { Serial.println("Long press!"); }
    );
}

void loop() {
    button.process();  // Must call this regularly
}
```
## Sample: polling style

```cpp
#include <MultiPressButton.h>

MultiPressButton button(4);  // Button connected to pin 4

void setup() {
    Serial.begin(9600);
    button.setup();
}

void loop() {
    button.process();  // Must call this regularly
    
    if (button.singlePress()) {
        Serial.println("Single press!");
    }
    if (button.doublePress()) {
        Serial.println("Double press!");
    }
    if (button.triplePress()) {
        Serial.println("Triple press!");
    }
    if (button.longPress()) {
        Serial.println("Long press!");
    }
}
```

## Features
- Single, double, triple press detection
- Long press support
- Debouncing built in
- Two interface styles:
  - Callback/Lambda based interface for immediate reactions
  - Polling interface for more control
- Interrupt-driven design robust against blocking code
- Support for multiple buttons

## Robust Button Handling

Button events are captured in an interrupt service routine (ISR) and stored in a ring buffer, making the library robust against blocking operations in your main loop. You won't miss button presses even if your code:

- Makes network calls
- Writes to flash memory  
- Controls NeoPixels
- Uses `delay()`
- Performs long calculations

Just remember to call `button.process()` regularly to handle any queued events. It is OK to call `button.process()` multiple times in a loop.

## Interface Styles

Choose one interface style and stick with it. Mixing the two interfaces is not supported.

### 1. Lambda/Callback Style
Best when you want to respond immediately to button presses:

```cpp
button.setActions(
    []() { /* single press */ },
    []() { /* double press */ },
    []() { /* triple press */ },
    []() { /* long press */ }
);
```

### 2. Polling Style
Best when you want direct control over when to handle button events:

```cpp
void loop() {
    button.process();
    
    if (button.singlePress()) {
        // Handle single press
    }
    if (button.doublePress()) {
        // Handle double press
    }
    if (button.triplePress()) {
        // Handle triple press
    }
    if (button.longPress()) {
        // Handle long press
    }
}
```

## Installation
Clone or copy this repository into `library/MultiPressButton`

## Configuration
The button behavior can be customized during construction:

```cpp
MultiPressButton button(
    4,                           // Pin number
    INPUT_PULLUP,                // Pin mode (default)
    50,                          // Debounce time in ms (default)
    1000,                        // Long press duration in ms (default)
    1000                         // Multi-press window in ms (default)
);

// All parameters after pin are optional:
MultiPressButton simple(4);      // Equivalent to above with all defaults

// Common customizations:
MultiPressButton quick(4, INPUT_PULLUP, 20);     // Faster 20ms debounce
MultiPressButton slow(4, INPUT_PULLUP, 50, 2000); // 2 second long press
```

## Hardware Setup
Connect your button between the input pin and ground. The library uses the internal pull-up resistor by default (`INPUT_PULLUP` mode).

```
VCC (not used with INPUT_PULLUP)
        │
        ┳ 10k resistor (not needed with INPUT_PULLUP)
        │
Pin ────┫
        │
        ┃ Button/Switch
        │
GND ────┛
```

## Dependencies
- [uTimerLib library](https://github.com/Naguissa/uTimerLib)
- Arduino framework supporting C++11 or later

## Contributing
Pull requests welcome! Please follow the existing code style and add unit tests for any new features.

## See also

https://github.com/poelstra/arduino-multi-button; similar, but does not seem to work during blocking calls, and does not support a callback interface.

## License
MIT License - feel free to use this in your own projects.
