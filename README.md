# X9CResistor

A lightweight Arduino library for controlling **X9C series digital potentiometers** by either **wiper position** or **resistance in ohms**.

Designed to provide a simple and portable API for Arduino-compatible boards, including ESP8266 and ESP32, while keeping direct control over the X9C non-volatile memory.

```cpp
resistor.setPosition(50);
```

or simply:

```cpp
resistor.setResistance(25000);
```

---

## ✨ Features

* Set the wiper position from **0 to 99**
* Increase or decrease the wiper position one step at a time
* Set the desired resistance directly in **ohms**
* Get the estimated resistance for the current wiper position
* Explicitly save the current position to the X9C **non-volatile memory**
* Automatic synchronization when the initial wiper position is unknown
* Support for all X9C resistance variants covered by the datasheet
* Simple 3-wire control interface: `CS`, `U/D`, and `INC`
* Arduino compatible
* PlatformIO compatible
* Hardware-independent implementation using the standard Arduino GPIO API

---

## 🔧 Supported Devices

| Device | Nominal Resistance | Wiper Positions |
| ------ | -----------------: | --------------: |
| X9C102 |               1 kΩ |             100 |
| X9C103 |              10 kΩ |             100 |
| X9C503 |              50 kΩ |             100 |
| X9C104 |             100 kΩ |             100 |

The wiper position ranges from:

```text
0 ... 99
```

The library automatically converts between the selected position and the nominal resistance of the configured X9C model.

---

## 📦 Installation

### Arduino IDE

Download or clone this repository and place the library folder inside your Arduino libraries directory:

```text
Arduino/
└── libraries/
    └── X9CResistor/
```

Restart the Arduino IDE and include the library:

```cpp
#include <X9CResistor.h>
```

---

### PlatformIO

Add the GitHub repository to your `platformio.ini`:

```ini
lib_deps =
    https://github.com/Rick-Andrade/X9CResistor.git
```

Then include:

```cpp
#include <X9CResistor.h>
```

---

### Git

Clone the repository:

```bash
git clone https://github.com/Rick-Andrade/X9CResistor.git
```

---

## 🚀 Quick Start

```cpp
#include <Arduino.h>
#include <X9CResistor.h>

constexpr uint8_t CS_PIN  = 14;
constexpr uint8_t UD_PIN  = 12;
constexpr uint8_t INC_PIN = 13;

X9CResistor resistor(
    CS_PIN,
    UD_PIN,
    INC_PIN,
    X9CModel::X9C104
);

void setup()
{
    Serial.begin(115200);

    resistor.begin();

    // Set the potentiometer to approximately 25 kΩ
    resistor.setResistance(25000);

    Serial.print("Wiper position: ");
    Serial.println(resistor.getPosition());

    Serial.print("Estimated resistance: ");
    Serial.print(resistor.getResistance());
    Serial.println(" ohms");
}

void loop()
{
}
```

For an `X9C503`, requesting:

```cpp
resistor.setResistance(25000);
```

will automatically select the nearest available wiper position.

---

## 🎚️ Position Control

You can control the X9C directly using its 100 available wiper positions.

### Set an absolute position

```cpp
resistor.setPosition(50);
```

### Increase one step

```cpp
resistor.increasePosition();
```

### Decrease one step

```cpp
resistor.decreasePosition();
```

### Read the current position

```cpp
uint8_t position = resistor.getPosition();
```

---

## ⚡ Resistance Control

Instead of calculating the wiper position manually, the library can work directly with resistance values.

```cpp
resistor.setResistance(25000);
```

For example, with:

```cpp
X9CModel::X9C503
```

the library knows that the nominal end-to-end resistance is:

```text
50 kΩ
```

and automatically calculates the nearest available wiper position.

The current nominal resistance can be retrieved with:

```cpp
uint32_t resistance = resistor.getResistance();
```

---

## 💾 Non-Volatile Position Storage

Changing the wiper position does **not automatically write to the X9C non-volatile memory**.

This is intentional.

You can change the position multiple times:

```cpp
resistor.setPosition(20);
resistor.setPosition(40);
resistor.setPosition(60);
```

and explicitly store the final value only when required:

```cpp
resistor.savePosition();
```

This helps avoid unnecessary writes to the internal non-volatile memory.

After a power cycle, the X9C automatically restores the previously stored physical wiper position.

> **Note:** The microcontroller cannot directly read the internal X9C counter after power-up. Therefore, the software position initially remains unknown until the library synchronizes it.

---

## 🔄 Position Synchronization

When the library does not know the current physical wiper position, it can synchronize the device using:

```cpp
resistor.resetPosition();
```

The library moves the wiper toward the lower limit until it reaches position:

```text
0
```

This establishes a known reference point between the software state and the physical device.

Calling:

```cpp
resistor.setPosition(...);
```

automatically performs this synchronization if the current position is unknown.

---

## 🔌 Hardware Interface

The X9C uses three digital control signals:

| Signal | Description             |
| ------ | ----------------------- |
| `CS`   | Chip Select             |
| `U/D`  | Up / Down direction     |
| `INC`  | Wiper increment control |

Example connection:

```text
Microcontroller             X9C

GPIO ---------------------> CS
GPIO ---------------------> U/D
GPIO ---------------------> INC

VCC  ---------------------> VCC
GND  ---------------------> VSS
```

The potentiometer terminals are:

```text
RH  ─────┐
         │
        [ R ]
         │
RW  <────┤  Wiper
         │
        [ R ]
         │
RL  ─────┘
```

---

## 📚 API

### `begin()`

Initializes the GPIO pins used to communicate with the X9C.

```cpp
resistor.begin();
```

---

### `setPosition(position)`

Moves the wiper to a position between `0` and `99`.

```cpp
bool success = resistor.setPosition(50);
```

Returns `false` if the requested position is outside the valid range.

---

### `getPosition()`

Returns the currently tracked wiper position.

```cpp
uint8_t position = resistor.getPosition();
```

If the position has not yet been synchronized, the returned value is:

```cpp
X9CResistor::UNKNOWN_POSITION
```

---

### `increasePosition()`

Moves the wiper one position upward.

```cpp
resistor.increasePosition();
```

---

### `decreasePosition()`

Moves the wiper one position downward.

```cpp
resistor.decreasePosition();
```

---

### `resetPosition()`

Moves the wiper to position `0` and synchronizes the software state with the physical device.

```cpp
resistor.resetPosition();
```

---

### `setResistance(resistance)`

Sets the closest available wiper position for the requested resistance.

```cpp
resistor.setResistance(25000);
```

The value must be expressed in **ohms**.

---

### `getResistance()`

Returns the estimated nominal resistance corresponding to the current wiper position.

```cpp
uint32_t resistance = resistor.getResistance();
```

---

### `savePosition()`

Stores the current physical wiper position in the X9C non-volatile memory.

```cpp
resistor.savePosition();
```

Use this only when the current position needs to survive a power cycle.

---

## 🧪 Example: Wiper Sweep

```cpp
#include <Arduino.h>
#include <X9CResistor.h>

X9CResistor resistor(
    14,
    12,
    13,
    X9CModel::X9C103
);

void setup()
{
    Serial.begin(115200);

    resistor.begin();
    resistor.resetPosition();
}

void loop()
{
    for (uint8_t position = 0; position <= 90; position += 10)
    {
        resistor.setPosition(position);

        Serial.print("Position: ");
        Serial.print(resistor.getPosition());

        Serial.print(" | Resistance: ");
        Serial.print(resistor.getResistance());

        Serial.println(" ohms");

        delay(500);
    }

    resistor.setPosition(99);

    delay(2000);

    resistor.resetPosition();

    delay(2000);
}
```

---

## ⚠️ Important Notes

### Resistance is nominal

`getResistance()` calculates resistance based on the nominal resistance of the selected X9C model.

The actual physical resistance can differ because of:

* device tolerance;
* wiper resistance;
* temperature;
* manufacturing variation;
* external circuit loading.

For precise applications, measure or calibrate the actual resistance.

### Discrete resistance values

The X9C does not provide continuously variable resistance.

There are only:

```text
100 wiper positions
```

Therefore, `setResistance()` selects the **closest available position** to the requested resistance.

### Non-volatile memory

`setPosition()` and `setResistance()` do not automatically write the new position to non-volatile memory.

Use:

```cpp
resistor.savePosition();
```

when persistence is required.

---

## 🗂️ Project Structure

```text
X9CResistor/
│
├── src/
│   ├── X9CResistor.cpp
│   ├── X9CResistor.h
│   └── X9CModel.h
│
├── examples/
│   ├── BasicPosition/
│   │   └── BasicPosition.ino
│   │
│   └── ResistanceControl/
│       └── ResistanceControl.ino
│
├── library.properties
├── library.json
├── README.md
└── LICENSE
```

---

## 🤝 Contributing

Contributions, bug reports and suggestions are welcome.

If you find an issue or have an idea for improving the library, feel free to open an issue or submit a pull request.

---

## 📄 License

This project is released under the **MIT License**.

See the `LICENSE` file for details.
