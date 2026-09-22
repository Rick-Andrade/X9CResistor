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

    if (resistor.setResistance(25000))
    {
        Serial.print("Position: ");
        Serial.println(resistor.getPosition());

        Serial.print("Nominal resistance: ");
        Serial.print(resistor.getResistance());
        Serial.println(" ohms");
    }

    // Persist current position in the X9C NVM.
    resistor.savePosition();
}

void loop()
{
}