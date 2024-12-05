#include <MultiPressButton.h>

MultiPressButton buttonA(4);  // First button on pin 4
MultiPressButton buttonB(5);  // Second button on pin 5

void
setup()
{
    Serial.begin(9600);

    buttonA.setup();
    buttonB.setup();

    // Lambda style for button A
    buttonA.setActions(
        []() {
            Serial.println("Button A: Single click!");
        },
        []() {
            Serial.println("Button A: Double click!");
        });

    // Could mix styles (though not on the same button)
    buttonB.setup();  // Using polling style for button B
}

void
loop()
{
    buttonA.update();
    buttonB.update();

    // Poll button B
    if (buttonB.singlePress()) {
        Serial.println("Button B: Single click!");
    }
    if (buttonB.doublePress()) {
        Serial.println("Button B: Double click!");
    }
}
