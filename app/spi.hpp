#pragma once
#include "magic.hpp"
#include "gpio.hpp"

namespace barehw {

class SPI
{
public:
    SPI(SPI_TypeDef *cmsisSPI) : m_spi(cmsisSPI)
    { }

    enum class Duplex : uint16_t {
        Full       = 0,
        FullRXOnly = SPI_CR1_RXONLY,
        HalfTXOnly = SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE,
        HalfRXOnly = SPI_CR1_BIDIMODE
    };

    enum class FrameFormat : uint16_t {
        FrameIs8Bit  = 0,
        FrameIs16Bit = SPI_CR1_DFF
    };

    enum class SlaveManagement : uint16_t {
        Hardware         = 0,
        SoftwareSelected = SPI_CR1_SSM | SPI_CR1_SSI,
        SoftwareSkipped  = SPI_CR1_SSM
    };

    enum class BaudRate : uint16_t {
        Div2             = 0,
        Div4             = SPI_CR1_BR_0,
        Div8             = SPI_CR1_BR_1,
        Div16            = SPI_CR1_BR_1 | SPI_CR1_BR_0,
        Div32            = SPI_CR1_BR_2,
        Div64            = SPI_CR1_BR_2 | SPI_CR1_BR_0,
        Div128           = SPI_CR1_BR_2 | SPI_CR1_BR_1,
        Div256           = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0
    };

    enum class Mode : uint16_t {
        Master = SPI_CR1_MSTR,
        Slave  = 0
    };

    enum class Sampling : uint16_t {
        NormalLeading    = 0,
        NormalTrailing   = SPI_CR1_CPHA,
        InvertedLeading  = SPI_CR1_CPOL,
        InvertedTrailing = SPI_CR1_CPOL | SPI_CR1_CPHA
    };

    template<typename T>
    void wtx(T x)
    {
        while ((m_spi->SR & SPI_SR_TXE) == 0);
        m_spi->DR = x;
    }

    template<typename T>
    T wrx()
    {
        while ((m_spi->SR & SPI_SR_RXNE) == 0);
        return m_spi->DR;
    }

private:
    SPI_TypeDef *m_spi;

};

struct Duplex : magic::Argument<uint16_t> {
  explicit Duplex(SPI::Duplex duplex = SPI::Duplex::Full) : Argument((uint16_t)duplex) {}
};

struct FrameFormat : magic::Argument<uint16_t> {
  explicit FrameFormat(SPI::FrameFormat fm = SPI::FrameFormat::FrameIs8Bit) : Argument((uint16_t)fm) {}
};

struct SlaveManagement : magic::Argument<uint16_t> {
  explicit SlaveManagement(SPI::SlaveManagement sm = SPI::SlaveManagement::SoftwareSelected) : Argument((uint16_t)sm) {}
};

struct BaudRate : magic::Argument<uint16_t> {
  explicit BaudRate(SPI::BaudRate baud = SPI::BaudRate::Div256) : Argument((uint16_t)baud) {}
};

struct Mode : magic::Argument<uint16_t> {
  explicit Mode(SPI::Mode mode = SPI::Mode::Master) : Argument((uint16_t)mode) {}
};

struct Sampling : magic::Argument<uint16_t> {
  explicit Sampling(SPI::Sampling sampling = SPI::Sampling::NormalLeading) : Argument((uint16_t)sampling) {}
};


template<typename SCKPin, typename MOSIPin, typename MISOPin, GPIO::AF af, uint32_t CMSIS_SPI>
class SPIConfigX
{
public:
    template <typename ...T>
    static void up(T ...args) {
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

        SCKPin::mode(GPIO::Mode::Alternate);
        SCKPin::af(af);
        SCKPin::speed(GPIO::Speed::High);

        MOSIPin::mode(GPIO::Mode::Alternate);
        MOSIPin::af(af);
        MOSIPin::speed(GPIO::Speed::High);

        MISOPin::mode(GPIO::Mode::Alternate);
        MISOPin::af(af);

        uint16_t duplex = magic::get<Duplex>(args...);
        uint16_t frameFormat = magic::get<FrameFormat>(args...);
        uint16_t slaveManagement = magic::get<SlaveManagement>(args...);
        uint16_t baudRate = magic::get<BaudRate>(args...);
        uint16_t mode = magic::get<Mode>(args...);
        uint16_t sampling = magic::get<Sampling>(args...);
        instance()->CR1 = duplex | frameFormat | slaveManagement
                        | baudRate | mode | sampling | SPI_CR1_SPE;
    }

    static SPI_TypeDef* instance()
    {
        return reinterpret_cast<SPI_TypeDef *>(CMSIS_SPI);
    }

private:
    SPIConfigX();
};

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
        asm("nop"); asm("nop");
        m_spi.wtx((uint8_t)TransferMode::ReadSingle | addr);
        m_spi.wtx((uint8_t)TransferMode::WriteDummy);
        m_spi.wrx<uint8_t>();
        uint8_t d = m_spi.wrx<uint8_t>();
        CSPin::high();
        for (uint8_t i = 0; i < 10; ++i) asm("nop");
        return d;
    }

    void writeRegister(uint8_t addr, uint8_t data)
    {
        CSPin::low();
        asm("nop"); asm("nop");
        m_spi.wtx((uint8_t)TransferMode::WriteSingle | addr);
        m_spi.wtx(data);
        m_spi.wrx<uint8_t>();
        m_spi.wrx<uint8_t>();
        CSPin::high();
        for (uint8_t i = 0; i < 10; ++i) asm("nop");
    }

    void readMultiple(uint8_t fromAddr, uint8_t *to, uint8_t length)
    {
        CSPin::low();
        asm("nop"); asm("nop");
        m_spi.wtx((uint8_t)TransferMode::ReadIncrement | fromAddr);
        m_spi.wrx<uint8_t>();
        for (uint8_t i = 0; i < length; ++i) {
            m_spi.wtx((uint8_t)TransferMode::WriteDummy);
            *to = m_spi.wrx<uint8_t>();
            to++;
        }
        CSPin::high();
        for (uint8_t i = 0; i < 10; ++i) asm("nop");
    }

private:
    SPI m_spi;
};

} // namespace barehw
