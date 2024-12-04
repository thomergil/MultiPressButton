#include <MultiPressButton.h>

MultiPressButton button(4);

void
setup()
{
    Serial.begin(9600);
    button.setup();
}

void
loop()
{
    button.process();

    if (button.singlePress()) {
        Serial.println("Single click!");
    }
    if (button.doublePress()) {
        Serial.println("Double click!");
    }
    if (button.triplePress()) {
        Serial.println("Triple click!");
    }
    if (button.longPress()) {
        Serial.println("Long press!");
    }
}

