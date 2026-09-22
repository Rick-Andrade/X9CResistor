#include "X9CResistor.h"


void X9CResistor::begin()
{
    pinMode(m_chipSelect, OUTPUT);
    pinMode(m_upDown, OUTPUT);
    pinMode(m_increment, OUTPUT);

    digitalWrite(m_chipSelect, HIGH);
    digitalWrite(m_upDown, LOW);
    digitalWrite(m_increment, HIGH);
}

void X9CResistor::resetPosition()
{
    changePosition(LOW, MAX_WIPER_POSITION);
    m_currentPosition = 0;
}

bool X9CResistor::setPosition(uint8_t t_position)
{
    if (t_position > MAX_WIPER_POSITION)
    {
        return false;
    }

    if (m_currentPosition == UNKNOWN_POSITION)
    {
        resetPosition();
    }

    if (m_currentPosition > t_position)
    {
        changePosition(
            LOW,
            m_currentPosition - t_position
        );
    }
    else if (m_currentPosition < t_position)
    {
        changePosition(
            HIGH,
            t_position - m_currentPosition
        );
    }

    return true;
}

void X9CResistor::increasePosition()
{
    if (m_currentPosition == UNKNOWN_POSITION)
    {
        resetPosition();
    }

    if (m_currentPosition < MAX_WIPER_POSITION)
    {
        changePosition(HIGH, 1);
    }
}

void X9CResistor::decreasePosition()
{
    if (m_currentPosition == UNKNOWN_POSITION)
    {
        resetPosition();
    }

    if (m_currentPosition > 0)
    {
        changePosition(LOW, 1);
    }
}

void X9CResistor::changePosition(
    uint8_t t_direction,
    uint8_t t_times)
{
    if (t_times == 0)
    {
        return;
    }

    digitalWrite(m_upDown, t_direction);
    delayMicroseconds(SIGNAL_DELAY_US);

    digitalWrite(m_increment, HIGH);
    delayMicroseconds(SIGNAL_DELAY_US);

    digitalWrite(m_chipSelect, LOW);
    delayMicroseconds(SIGNAL_DELAY_US);

    for (uint8_t i = 0; i < t_times; ++i)
    {
        // HIGH -> LOW moves the wiper
        digitalWrite(m_increment, LOW);
        delayMicroseconds(SIGNAL_DELAY_US);

        if (m_currentPosition != UNKNOWN_POSITION)
        {
            if (t_direction == HIGH)
            {
                if (m_currentPosition < MAX_WIPER_POSITION)
                {
                    ++m_currentPosition;
                }
            }
            else
            {
                if (m_currentPosition > 0)
                {
                    --m_currentPosition;
                }
            }
        }

        // Prepare the next HIGH -> LOW transition.
        // On the last iteration INC remains LOW so
        // raising CS does not store the position.
        if (i < (t_times - 1))
        {
            digitalWrite(m_increment, HIGH);
            delayMicroseconds(SIGNAL_DELAY_US);
        }
    }

    delayMicroseconds(SIGNAL_DELAY_US);

    // INC is LOW here: CS HIGH means NO STORE.
    digitalWrite(m_chipSelect, HIGH);

    delayMicroseconds(SIGNAL_DELAY_US);

    // Restore idle state after the device is deselected.
    digitalWrite(m_increment, HIGH);
}

void X9CResistor::savePosition()
{
    // CS must rise while INC is HIGH to trigger STORE.
    digitalWrite(m_increment, HIGH);
    delayMicroseconds(SIGNAL_DELAY_US);

    digitalWrite(m_chipSelect, LOW);
    delayMicroseconds(SIGNAL_DELAY_US);

    digitalWrite(m_chipSelect, HIGH);

    // Wait for the non-volatile STORE operation.
    delay(STORE_TIME_DELAY_MS);
}

bool X9CResistor::setResistance(uint32_t t_resistance)
{
    if (m_nominalResistance == 0)
    {
        return false;
    }

    if (t_resistance > m_nominalResistance)
    {
        return false;
    }

    const uint32_t position =
        (t_resistance * MAX_WIPER_POSITION +
         m_nominalResistance / 2) /
        m_nominalResistance;

    return setPosition(
        static_cast<uint8_t>(position)
    );
}

uint32_t X9CResistor::getResistance() const
{
    if (m_currentPosition == UNKNOWN_POSITION ||
        m_nominalResistance == 0)
    {
        return 0;
    }

    return
        (static_cast<uint32_t>(m_currentPosition) *
         m_nominalResistance +
         MAX_WIPER_POSITION / 2) /
        MAX_WIPER_POSITION;
}