#include <MultiPressButton.h>

MultiPressButton button(4);

void
setup()
{
    Serial.begin(9600);
    button.setup();
    button.setActions(
        []() {
            Serial.println("Single click!");
        },
        []() {
            Serial.println("Double click!");
        },
        []() {
            Serial.println("Triple click!");
        },
        []() {
            Serial.println("Long press!");
        });
}

void
loop()
{
    button.process();
}

