#pragma once

#include <Arduino.h>
#include "X9CModel.h"

/**
 * @brief Controls an X9C digital potentiometer.
 */
class X9CResistor
{
public:
    /**
     * @brief Represents an unknown wiper position.
     */
    static constexpr uint8_t UNKNOWN_POSITION = 0xFF;

    /**
     * @brief Maximum valid wiper position.
     *
     * X9C devices provide 100 positions, ranging from 0 to 99.
     */
    static constexpr uint8_t MAX_WIPER_POSITION = 99;

    /**
     * @brief Creates an X9C digital potentiometer controller.
     *
     * @param CS Chip-select pin.
     * @param UD Up/down control pin.
     * @param INC Increment control pin.
     * @param model X9C device model.
     */
    X9CResistor(
        uint8_t CS,
        uint8_t UD,
        uint8_t INC,
        X9CModel model)
        : m_chipSelect(CS),
          m_upDown(UD),
          m_increment(INC),
          m_nominalResistance(static_cast<uint32_t>(model))
    {
    }

    /**
     * @brief Initializes the GPIO pins used by the device.
     */
    void begin();

    /**
     * @brief Returns the currently tracked wiper position.
     *
     * @return Wiper position from 0 to 99, or UNKNOWN_POSITION
     *         if the position has not yet been synchronized.
     */
    uint8_t getPosition() const
    {
        return m_currentPosition;
    }

    /**
     * @brief Resets the wiper to position 0.
     *
     * This operation is also used to synchronize the software state
     * with the physical X9C device.
     */
    void resetPosition();

    /**
     * @brief Sets the wiper position.
     *
     * @param t_position Target position from 0 to 99.
     *
     * @return true if the requested position is valid;
     *         otherwise false.
     */
    bool setPosition(uint8_t t_position);

    /**
     * @brief Increases the wiper position by one step.
     */
    void increasePosition();

    /**
     * @brief Decreases the wiper position by one step.
     */
    void decreasePosition();

    /**
     * @brief Stores the current wiper position in non-volatile memory.
     */
    void savePosition();

    /**
     * @brief Sets the approximate nominal resistance.
     *
     * The requested resistance is converted to the nearest
     * available wiper position.
     *
     * @param t_resistance Desired nominal resistance in ohms.
     *
     * @return true if the requested resistance is within the
     *         supported range; otherwise false.
     */
    bool setResistance(uint32_t t_resistance);

    /**
     * @brief Returns the nominal resistance corresponding to the
     *        current wiper position.
     *
     * @return Resistance in ohms, or 0 if the position is unknown.
     */
    uint32_t getResistance() const;

private:
    /**
     * @brief Delay used between control-signal transitions.
     */
    static constexpr uint16_t SIGNAL_DELAY_US = 5;

    /**
     * @brief Delay required for a non-volatile STORE operation.
     */
    static constexpr uint16_t STORE_TIME_DELAY_MS = 20;

    /**
     * @brief Moves the wiper a specified number of steps.
     *
     * @param t_direction HIGH to increment, LOW to decrement.
     * @param t_times Number of steps.
     */
    void changePosition(uint8_t t_direction, uint8_t t_times);

    uint8_t m_chipSelect = 0;
    uint8_t m_upDown = 0;
    uint8_t m_increment = 0;

    uint8_t m_currentPosition = UNKNOWN_POSITION;

    uint32_t m_nominalResistance = 0;
};