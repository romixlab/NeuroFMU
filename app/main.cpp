#include "distortos/ThisThread.hpp"
#include "distortos/ThisThread-Signals.hpp"
#include "distortos/StaticThread.hpp"
#include "distortos/DynamicThread.hpp"
#include "CMSIS-proxy.h"
#include "gpio.hpp"
#include "string.h"
#include "boardgoodies.hpp"
#include "spi.hpp"
#include "l3gd20.hpp"
#include "lsm303d.hpp"

using namespace distortos;
using namespace barehw;
using namespace mems;


[[noreturn]] void blinker()
{
    while(1) {
        const SignalSet signalSet {SignalSet::full};
        const auto waitResult = ThisThread::Signals::wait(signalSet);
        AmberLED::low();
        auto &signalInformation = waitResult.second;
        if (signalInformation.getSignalNumber() == 0)
            distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
        else if (signalInformation.getSignalNumber() == 1)
            distortos::ThisThread::sleepFor(std::chrono::milliseconds(500));
        AmberLED::high();
    }
}

void write(uint8_t *p, uint8_t len)
{
    for (uint32_t idx = 0; idx < len; ++idx) {
        while ((UART7->SR & USART_SR_TC) == 0);
        UART7->DR = *p;
        p++;
    }
}

void blink(uint8_t n)
{
    for (uint8_t i = 0; i < n; ++i) {
        AmberLED::low();
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
        AmberLED::high();
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
    }
}

extern "C" void EXTI4_IRQHandler()
{
    write((uint8_t *)".", 1);
    EXTI->PR |= EXTI_PR_PR4;
}

int main()
{ 
    distortos::ThisThread::sleepFor(std::chrono::milliseconds(10));

    InternalSPI::up(BaudRate{SPI::BaudRate::Div4},
                    SlaveManagement{SPI::SlaveManagement::SoftwareSelected},
                    Mode{SPI::Mode::Master});

    Serial5TXPin::mode(GPIO::Mode::Alternate);
    Serial5TXPin::af(GPIO::AF::AF8);
	RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
	UART7->BRR = 0x187; // 115200 at 45MHz
	UART7->CR1 |= USART_CR1_UE_Msk | USART_CR1_TE; // usart en, tx en

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;


    AmberLED::mode(GPIO::Mode::Output);
    AmberLED::high();
    //auto blinkerThread = distortos::makeAndStartDynamicThread<512, true, 31, 0>(150, blinker);
//    DynamicThread blinkerThread = makeAndStartDynamicThread(512, true, 31, 0, 150,
//                                                            SchedulingPolicy::roundRobin, blinker);



    BaroCSPin::mode(GPIO::Mode::Output);
    BaroCSPin::high();

    MPUCSPin::mode(GPIO::Mode::Output);
    MPUCSPin::high();

//    AccMagCSPin::mode(GPIO::Mode::Output);
//    AccMagCSPin::high();

    GyroCSPin::mode(GPIO::Mode::Output);
    GyroCSPin::high();


//    L3GD20::L3GD20<InternalSPI, GyroCSPin> l3gd20;

//    l3gd20.up(L3GD20::Mode{L3GD20::Rate95::LPF::CutOff12Hz5,
//                           L3GD20::Rate95::HPF::CutOff0Hz9});

    mems::LSM303D::LSM303D<InternalSPI, AccMagCSPin> lsm303d;
    lsm303d.up();

    AccReadyPin::mode(GPIO::Mode::Input);
    //AccReadyPin::pull(GPIO::Pull::Down);
    AccReadyPin::exti(GPIO::ExtiEdge::Rising);
    NVIC_EnableIRQ(EXTI4_IRQn);


write((uint8_t *)"~~~~~~~\r\n", 9);

    char data[64];
    sprintf(data, "Ctrl1 = %d\r\nCtrl2 = %d\r\nAccOk = %d\r\n",
            lsm303d.m_spiDriver.readRegister(mems::LSM303D::Ctrl1Addr),
            lsm303d.m_spiDriver.readRegister(mems::LSM303D::Ctrl2Addr),
            lsm303d.m_spiDriver.readRegister(mems::LSM303D::WhoAmIAddr));
//    sprintf(data, "whoami = %d\r\nCtrl1 = %d\r\n",
//            l3gd20.m_spiDriver.readRegister(L3GD20::WhoAmIAddr),
//            l3gd20.m_spiDriver.readRegister(L3GD20::Ctrl1Addr));
    write((uint8_t *)data, strlen(data));

	while (1) {
//        uint8_t xyz[6];
//        //l3gd20.read((uint8_t *)xyz);
//        lsm303d.read((uint8_t *)xyz);

//        int16_t x;
//        uint8_t *p = (uint8_t *)&x;
//        p[0] = xyz[0];
//        p[1] = xyz[1];
//        int16_t y = (int16_t)(((uint16_t)xyz[3] << 8) | xyz[2]);
//        int16_t z = (int16_t)(((uint16_t)xyz[5] << 8) | xyz[4]);
//        sprintf(data, "x:%d %d %d\r\n", x, y, z);
//        write((uint8_t *)data, strlen(data));

//        blinkerThread.generateSignal(0);
////        blinkerThread.generateSignal(1);
////        blinkerThread.generateSignal(2);
//        distortos::ThisThread::sleepFor(std::chrono::milliseconds(1000));
//        blinkerThread.generateSignal(1);
        //write(Serial4TXPin::read() ? (uint8_t *)"1\r\n" : (uint8_t *)"0\r\n", 3);
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
	}
	return 0;
}
