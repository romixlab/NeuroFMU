#pragma once
#include "spi.hpp"
#include "gpio.hpp"
#include "magic.hpp"

namespace barehw {
namespace mems {
namespace LSM303D {

constexpr static uint8_t WhoAmIAddr = 0xf;
constexpr static uint8_t WhoAmIValue = 0b01001001;
constexpr static uint8_t Ctrl0Addr = 0x1f;
constexpr static uint8_t Ctrl1Addr = 0x20;
constexpr static uint8_t Ctrl2Addr = 0x21;
constexpr static uint8_t Ctrl3Addr = 0x22;
constexpr static uint8_t Ctrl4Addr = 0x23;
constexpr static uint8_t Ctrl5Addr = 0x24;
constexpr static uint8_t Ctrl6Addr = 0x25;
constexpr static uint8_t Ctrl7Addr = 0x26;

namespace Accelerometer {
enum class Rate : uint8_t {
    PowerDown = 0b0000,
    _3Hz125   = 0b0001,
    _6Hz25    = 0b0010,
    _12Hz5    = 0b0011,
    _25Hz     = 0b0100,
    _50Hz     = 0b0101,
    _100Hz    = 0b0110,
    _200Hz    = 0b0111,
    _400Hz    = 0b1000,
    _800Hz    = 0b1001,
    _1600Hz   = 0b1010,
};

enum class FullScale : uint8_t {
    _2g  = 0b000,
    _4g  = 0b001,
    _6g  = 0b010,
    _8g  = 0b011,
    _16g = 0b100
};
}

template<typename SPIConfigX, typename CSPin>
class LSM303D
{
public:
    LSM303D() : m_spiDriver(SPIConfigX::instance())
    {
    }

    void up()
    {
        m_spiDriver.writeRegister(Ctrl2Addr, 0x00);
        m_spiDriver.writeRegister(Ctrl1Addr, 0x57);
        m_spiDriver.writeRegister(Ctrl3Addr, 0b00000100);
        m_spiDriver.writeRegister(Ctrl5Addr, 0x64);
        m_spiDriver.writeRegister(Ctrl6Addr, 0x20);
        m_spiDriver.writeRegister(Ctrl7Addr, 0x0);
    }

    bool isReady()
    {
        return m_spiDriver.readRegister(WhoAmIAddr) == WhoAmIValue;
    }

    void read(uint8_t *to)
    {
        while((m_spiDriver.readRegister(0x27) & 0b00001000) == 0);
        m_spiDriver.readMultiple(0x28, to, 6);
    }

    SPIRWMS6D8Driver<CSPin> m_spiDriver;
};

} // LSM303D
} // mems
} // barehw
