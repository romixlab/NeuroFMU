#pragma once
#include <stdint.h>
#include <stm32f4xx.h>

class GPIOConfig
{
public:
    enum class Mode {
        Input     = 0b00,
        Output    = 0b01,
        Alternate = 0b10,
        Analog    = 0b11
    };

    enum class OType {
        PushPull  = 0b00,
        OpenDrain = 0b01
    };

    enum class Speed {
        Low      = 0b00,
        Medium   = 0b01,
        High     = 0b10,
        VeryHigh = 0b11
    };

    enum class PushPull {
        NoPull   = 0b00,
        PullUp   = 0b01,
        PullDown = 0b10
    };
private:
    GPIOConfig();
};

template<uint32_t PORT, uint8_t PIN>
class GPIOPin
{
public:
    static void config(GPIOConfig::Mode mode,
                       GPIOConfig::OType otype = GPIOConfig::OType::PushPull,
                       GPIOConfig::Speed speed = GPIOConfig::Speed::Low,
                       GPIOConfig::PushPull pull = GPIOConfig::PushPull::NoPull)
    {
        reinterpret_cast<GPIO_TypeDef *>(PORT)->MODER = (uint32_t)mode << PIN * 2;
    }

    static void high()
    {
        reinterpret_cast<GPIO_TypeDef *>(PORT)->BSRR = 1u << PIN;
    }

    static void low()
    {
        reinterpret_cast<GPIO_TypeDef *>(PORT)->BSRR = 1u << (PIN + 16);
    }

private:
    GPIOPin();
};
