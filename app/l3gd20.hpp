#pragma once
#include "gpio.hpp"
#include "spi.hpp"

namespace barehw {
namespace mems {

template<typename CSPin>
class SPIRWMS6D8Driver
{
public:
    SPIRWMS6D8Driver(SPI spi) : m_spi(spi)
    {
        CSPin::mode(GPIO::Mode::Output);
        CSPin::high();
    }

    enum class TransferMode : uint8_t {
        ReadSingle     = 0b10000000,
        WriteSingle    = 0b00000000,
        ReadIncrement  = 0b11000000,
        WriteIncrement = 0b01000000,
        WriteDummy     = 0b00000000,
    };

    uint8_t readRegister(uint8_t addr)
    {
        CSPin::low();
        __asm("NOP");
        __asm("NOP");
        m_spi.wtx((uint8_t)TransferMode::ReadSingle | addr);
        m_spi.wtx((uint8_t)TransferMode::WriteDummy);
        m_spi.wrx<uint8_t>();
        uint8_t d = m_spi.wrx<uint8_t>();
        CSPin::high();
        return d;
    }

    void writeRegister(uint8_t addr, uint8_t data)
    {
        CSPin::low();
        __asm("NOP");
        __asm("NOP");
        m_spi.wtx((uint8_t)TransferMode::WriteSingle | addr);
        m_spi.wtx(data);
        m_spi.wrx<uint8_t>();
        m_spi.wrx<uint8_t>();
        CSPin::high();
    }

    void readMultiple(uint8_t fromAddr, uint8_t *to, uint8_t length)
    {
        CSPin::low();
        __asm("NOP");
        __asm("NOP");
        m_spi.wtx((uint8_t)TransferMode::ReadIncrement | fromAddr);
        m_spi.wrx<uint8_t>();
        for (uint8_t i = 0; i < length; ++i) {
            m_spi.wtx((uint8_t)TransferMode::WriteDummy);
            *to = m_spi.wrx<uint8_t>();
            to++;
        }
        CSPin::high();
    }

private:
    SPI m_spi;
};

namespace L3GD {
constexpr uint8_t WHO_AM_I = 0xf;


enum class DataRate : uint8_t {
    Rate12Hz5  = 0b0000,
    Rate25Hz   = 0b0100,
    Rate50Hz   = 0,
    Rate100Hz,
    Rate200Hz,
    Rate400Hz,
    Rate800Hz
};
}

template<typename SPIConfigX, typename CSPin>
class L3GD20
{
public:
    L3GD20() : m_spiDriver(SPIConfigX::instance())
    {
        m_spiDriver.writeRegister(0x20, 0x6f);
    }

    uint8_t isReady()
    {
        return m_spiDriver.readRegister(0xf);
    }

    void read(uint8_t *to)
    {
        while((m_spiDriver.readRegister(0x27) & 0b00001000) == 0);
        m_spiDriver.readMultiple(0x28, to, 6);
    }

private:
    SPIRWMS6D8Driver<CSPin> m_spiDriver;
};

template<typename SPIConfigX, typename CSPin>
class LSM303D
{
public:
    LSM303D() : m_spiDriver(SPIConfigX::instance())
    {
        //m_spiDriver.writeRegister(0x20, 0x6f);
    }

    uint8_t isReady()
    {
        return m_spiDriver.readRegister(0xf);
    }

private:
    SPIRWMS6D8Driver<CSPin> m_spiDriver;
};

} // namespace mems
} // namespace barehw
