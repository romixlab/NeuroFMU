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
  explicit Duplex(uint16_t mode = (uint16_t)SPI::Duplex::Full) : Argument(mode) {}
};

struct FrameFormat : magic::Argument<uint16_t> {
  explicit FrameFormat(uint16_t format = (uint16_t)SPI::FrameFormat::FrameIs8Bit) : Argument(format) {}
};

struct SlaveManagement : magic::Argument<uint16_t> {
  explicit SlaveManagement(uint16_t sm = (uint16_t)SPI::SlaveManagement::SoftwareSelected) : Argument(sm) {}
};

struct BaudRate : magic::Argument<uint16_t> {
  explicit BaudRate(uint16_t baud = (uint16_t)SPI::BaudRate::Div256) : Argument(baud) {}
};

struct Mode : magic::Argument<uint16_t> {
  explicit Mode(uint16_t mode = (uint16_t)SPI::Mode::Master) : Argument(mode) {}
};

struct Sampling : magic::Argument<uint16_t> {
  explicit Sampling(uint16_t sampling = (uint16_t)SPI::Sampling::NormalLeading) : Argument(sampling) {}
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

    template <typename ...T> static void getVolume(T ...args) {
//      int w = magic::get<Width>(args...);
//      int h = magic::get<Height>(args...);
    }

    static SPI_TypeDef* instance()
    {
        return reinterpret_cast<SPI_TypeDef *>(SPI);
    }

private:
    SPIConfigX();
};

