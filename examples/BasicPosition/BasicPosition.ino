#include <X9CResistor.h>

X9CResistor resistor(
    5,
    6,
    7,
    X9CModel::X9C104
);

void setup()
{
    Serial.begin(115200);

    resistor.begin();

    resistor.setPosition(50);

    Serial.print("Position: ");
    Serial.println(resistor.getPosition());

    Serial.print("Resistance: ");
    Serial.println(resistor.getResistance());
}

void loop()
{
}