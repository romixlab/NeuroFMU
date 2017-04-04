#pragma once
#include <stdint.h>
#include "CMSIS-proxy.h"

namespace barehw {

class GPIO
{
public:
    enum Port {
        A = GPIOA_BASE,
        B = GPIOB_BASE,
        C = GPIOC_BASE,
        D = GPIOD_BASE,
        E = GPIOE_BASE,
        F = GPIOF_BASE,
        G = GPIOG_BASE,
        H = GPIOH_BASE,
        I = GPIOI_BASE,
        J = GPIOJ_BASE,
        K = GPIOK_BASE,
    };

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

    enum class Pull {
        No       = 0b00,
        Up       = 0b01,
        Down     = 0b10
    };

    enum class AF {
        AF0   = 0b0000,
        AF1   = 0b0001,
        AF2   = 0b0010,
        AF3   = 0b0011,
        AF4   = 0b0100,
        AF5   = 0b0101,
        AF6   = 0b0110,
        AF7   = 0b0111,
        AF8   = 0b1000,
        AF9   = 0b1001,
        AF10  = 0b1010,
        AF11  = 0b1011,
        AF12  = 0b1100,
        AF13  = 0b1101,
        AF14  = 0b1110,
        AF15  = 0b1111
    };

private:
    GPIO();
};

template<uint32_t PORT>
class GPIOClock
{
public:
    static void enable()
    {
        RCC->AHB1ENR |= (PORT >> 10);
    }
private:
    GPIOClock();
};

template<uint32_t PORT, uint8_t PIN, uint8_t AFRidx = PIN >= 8>
class GPIOAFConfig;

template<uint32_t PORT, uint8_t PIN>
class GPIOAFConfig<PORT, PIN, 0>
{
public:
    static void config(GPIO::AF af)
    {
        uint32_t afr = reinterpret_cast<GPIO_TypeDef *>(PORT)->AFR[0];
        afr &=~ (0b1111 << (PIN * 4));
        afr |= (uint32_t)af << (PIN * 4);
        reinterpret_cast<GPIO_TypeDef *>(PORT)->AFR[0] = afr;
    }
private:
    GPIOAFConfig();
};

template<uint32_t PORT, uint8_t PIN>
class GPIOAFConfig<PORT, PIN, 1>
{
public:
    static void config(GPIO::AF af)
    {
        uint32_t afr = reinterpret_cast<GPIO_TypeDef *>(PORT)->AFR[1];
        afr &=~ (0b1111 << ((PIN - 8) * 4));
        afr |= (uint32_t)af << ((PIN - 8) * 4);
        reinterpret_cast<GPIO_TypeDef *>(PORT)->AFR[1] = afr;
    }
private:
    GPIOAFConfig();
};

template<uint32_t PORT, uint8_t PIN>
class GPIOPinX
{
public:
    static void mode(GPIO::Mode mode = GPIO::Mode::Input)
    {
        uint32_t moder = reinterpret_cast<GPIO_TypeDef *>(PORT)->MODER;
        moder &=~ (0b11 << (PIN * 2));
        moder |= (uint32_t)mode << PIN * 2;
        reinterpret_cast<GPIO_TypeDef *>(PORT)->MODER = moder;
    }

    static void otype(GPIO::OType otype = GPIO::OType::PushPull)
    {
        uint32_t otyper = reinterpret_cast<GPIO_TypeDef *>(PORT)->OTYPER;
        otyper &=~ (0b1 << PIN);
        otyper |= (uint32_t)otype << PIN;
        reinterpret_cast<GPIO_TypeDef *>(PORT)->OSPEEDR = otyper;
    }

    static void speed(GPIO::Speed speed = GPIO::Speed::Low)
    {
        uint32_t ospeedr = reinterpret_cast<GPIO_TypeDef *>(PORT)->OSPEEDR;
        ospeedr &=~ (0b11 << (PIN * 2));
        ospeedr |= (uint32_t)speed << PIN * 2;
        reinterpret_cast<GPIO_TypeDef *>(PORT)->OSPEEDR = ospeedr;
    }

    static void pull(GPIO::Pull pull = GPIO::Pull::No)
    {
        uint32_t pupdr = reinterpret_cast<GPIO_TypeDef *>(PORT)->PUPDR;
        pupdr &=~ (0b11 << (PIN * 2));
        pupdr |= (uint32_t)pull << PIN * 2;
        reinterpret_cast<GPIO_TypeDef *>(PORT)->PUPDR = pupdr;
    }

    static void af(GPIO::AF af)
    {
        GPIOAFConfig<PORT, PIN>::config(af);
    }

    static void high()
    {
        reinterpret_cast<GPIO_TypeDef *>(PORT)->BSRR = 0b1 << PIN;
    }

    static void low()
    {
        reinterpret_cast<GPIO_TypeDef *>(PORT)->BSRR = 0b1 << (PIN + 16);
    }

    static bool read()
    {
        return reinterpret_cast<GPIO_TypeDef *>(PORT)->IDR & (0b1 << PIN);
    }

private:
    GPIOPinX();
};

} // namespace barehw
