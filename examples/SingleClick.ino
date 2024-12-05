#include <MultiPressButton.h>

MultiPressButton button(4);

void
setup()
{
    Serial.begin(9600);
    button.setup();
    button.setActions([]() {
        Serial.println("Click!");
    });
}

void
loop()
{
    button.update();
}

