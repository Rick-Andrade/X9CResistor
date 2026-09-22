#pragma once

#include <Arduino.h>

enum class X9CModel : uint32_t
{
    X9C102 = 1000,
    X9C103 = 10000,
    X9C503 = 50000,
    X9C104 = 100000
};
