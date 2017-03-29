#pragma once
#include "magic.hpp"
#include "gpio.hpp"

class SPI
{
public:
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


template<typename SCKPin, typename MOSIPin, typename MISOPin, GPIO::AF af, uint32_t SPI>
class SPIConfigX
{
public:
    static void clock()
    {
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    }

    static void gpio()
    {
            SCKPin::mode(GPIO::Mode::Alternate);
            SCKPin::af(af);
            SCKPin::speed(GPIO::Speed::High);

            MOSIPin::mode(GPIO::Mode::Alternate);
            MOSIPin::af(af);
            MOSIPin::speed(GPIO::Speed::High);

            MISOPin::mode(GPIO::Mode::Alternate);
            MISOPin::af(af);
    }

    template <typename ...T>
    static void up(T ...args) {
        uint16_t duplex = magic::get<Duplex>(args...);
        uint16_t frameFormat = magic::get<FrameFormat>(args...);
        uint16_t slaveManagement = magic::get<SlaveManagement>(args...);
        uint16_t baudRate = magic::get<BaudRate>(args...);
        uint16_t mode = magic::get<Mode>(args...);
        uint16_t sampling = magic::get<Sampling>(args...);
        instance()->CR1 = duplex | frameFormat | slaveManagement
                        | baudRate | mode | sampling;
    }

    static SPI_TypeDef* instance()
    {
        return reinterpret_cast<SPI_TypeDef *>(SPI);
    }

private:
    SPIConfigX();
};

