#pragma once
#include "gpio.hpp"
#include "spi.hpp"
#include "magic.hpp"

#include <utility>

namespace barehw {
namespace mems {
namespace L3GD20 {

constexpr uint8_t WhoAmIAddr  = 0xf;
constexpr uint8_t WhoAmIValue = 0b11010100;
constexpr uint8_t Ctrl1Addr   = 0x20;
constexpr uint8_t Ctrl2Addr   = 0x21;
constexpr uint8_t Ctrl3Addr   = 0x22;
constexpr uint8_t Ctrl4Addr   = 0x23;
constexpr uint8_t Ctrl5Addr   = 0x24;

namespace Rate95 {
enum class LPF : uint8_t {
    CutOff12Hz5 = 0b00,
    CutOff25Hz  = 0b01
};
enum class HPF : uint8_t {
    CutOff7Hz2   = 0b0000,
    CutOff3Hz5   = 0b0001,
    CutOff1Hz8   = 0b0010,
    CutOff0Hz9   = 0b0011,
    CutOff0Hz45  = 0b0100,
    CutOff0Hz18  = 0b0101,
    CutOff0Hz09  = 0b0110,
    CutOff0Hz045 = 0b0111,
    CutOff0Hz018 = 0b1000,
    CutOff0Hz009 = 0b1001,
    NoFilter     = 0xff
};
}

namespace Rate190 {
enum class LPF : uint8_t {
    CutOff12Hz5 = 0b00,
    CutOff25Hz  = 0b01,
    CutOff50Hz  = 0b10,
    CutOff70Hz  = 0b11
};
enum class HPF : uint8_t {
    CutOff13Hz5  = 0b0000,
    CutOff7Hz2   = 0b0001,
    CutOff3Hz5   = 0b0010,
    CutOff1Hz8   = 0b0011,
    CutOff0Hz9   = 0b0100,
    CutOff0Hz45  = 0b0101,
    CutOff0Hz18  = 0b0110,
    CutOff0Hz09  = 0b0111,
    CutOff0Hz045 = 0b1000,
    CutOff0Hz018 = 0b1001,
    NoFilter     = 0xff
};
}

namespace Rate380 {
enum class LPF : uint8_t {
    CutOff20Hz   = 0b00,
    CutOff25Hz   = 0b01,
    CutOff50Hz   = 0b10,
    CutOff100Hz  = 0b11
};
enum class HPF : uint8_t {
    CutOff27Hz   = 0b0000,
    CutOff13Hz5  = 0b0001,
    CutOff7Hz2   = 0b0010,
    CutOff3Hz5   = 0b0011,
    CutOff1Hz8   = 0b0100,
    CutOff0Hz9   = 0b0101,
    CutOff0Hz45  = 0b0110,
    CutOff0Hz18  = 0b0111,
    CutOff0Hz09  = 0b1000,
    CutOff0Hz045 = 0b1001,
    NoFilter     = 0xff
};
}

namespace Rate760 {
enum class LPF : uint8_t {
    CutOff30Hz   = 0b00,
    CutOff35Hz   = 0b01,
    CutOff50Hz   = 0b10,
    CutOff100Hz  = 0b11
};
enum class HPF : uint8_t {
    CutOff51Hz4  = 0b0000,
    CutOff27Hz   = 0b0001,
    CutOff13Hz5  = 0b0010,
    CutOff7Hz2   = 0b0011,
    CutOff3Hz5   = 0b0100,
    CutOff1Hz8   = 0b0101,
    CutOff0Hz9   = 0b0110,
    CutOff0Hz45  = 0b0111,
    CutOff0Hz18  = 0b1000,
    CutOff0Hz09  = 0b1001,
    NoFilter     = 0xff
};
}

struct Mode : magic::Argument<std::pair<uint8_t, uint8_t> >
{
    explicit Mode(Rate95::LPF lpf, Rate95::HPF hpf = Rate95::HPF::NoFilter) :
        Argument(std::make_pair((uint8_t)lpf << 4, (uint8_t)hpf)) {}
    explicit Mode(Rate190::LPF lpf, Rate190::HPF hpf = Rate190::HPF::NoFilter) :
        Argument(std::make_pair(0b01000000 | ((uint8_t)lpf << 4), (uint8_t)hpf)) {}
    explicit Mode(Rate380::LPF lpf, Rate380::HPF hpf = Rate380::HPF::NoFilter) :
        Argument(std::make_pair(0b10000000 | ((uint8_t)lpf << 4), (uint8_t)hpf)) {}
    explicit Mode(Rate760::LPF lpf, Rate760::HPF hpf = Rate760::HPF::NoFilter) :
        Argument(std::make_pair(0b11000000 | ((uint8_t)lpf << 4), (uint8_t)hpf)) {}
};

struct SampleRate : magic::Argument<uint8_t>
{
    enum Rate {
        _250dps  = 0b00,
        _500dps  = 0b01,
        _2000dps = 0b10
    };
    explicit SampleRate(Rate sr = Rate::_250dps) : Argument((uint8_t)sr) {}
};

template<typename SPIConfigX, typename CSPin>
class L3GD20
{
public:
    L3GD20() : m_spiDriver(SPIConfigX::instance())
    {
    }

    template <typename ...T>
    void up(T ...args)
    {
        auto mode = magic::get<Mode>(args...).m_v;
        uint8_t ctrl1 = mode.first;
        m_spiDriver.writeRegister(Ctrl1Addr, ctrl1 | 0b00001111); // power down disable, xyz enable

        uint8_t ctrl2 = mode.second;
        if (ctrl2 != 0xff) { // argument provided
            m_spiDriver.writeRegister(Ctrl2Addr, ctrl2); // High Pass normal mode, select cut off freq
            m_spiDriver.writeRegister(Ctrl5Addr, 0b00010000); // High Pass enable
        }
    }

    uint8_t isReady()
    {
        uint8_t r = m_spiDriver.readRegister(0xf);
        return r;
    }

    void read(uint8_t *to)
    {
        while((m_spiDriver.readRegister(0x27) & 0b00001000) == 0);
        m_spiDriver.readMultiple(0x28, to, 6);
    }

    SPIRWMS6D8Driver<CSPin> m_spiDriver;
};
} // namespace L3G20
} // namespace mems
} // namespace barehw
