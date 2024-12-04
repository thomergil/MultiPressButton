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
        Serial.println("Click!");
    }
}

