#include "distortos/ThisThread.hpp"
#include "distortos/StaticThread.hpp"
#include "CMSIS-proxy.h"
#include "gpio.hpp"
#include "string.h"
#include "boardgoodies.hpp"
#include "spi.hpp"
#include "l3gd20.hpp"

using namespace barehw;

[[noreturn]] void blinker()
{
    while(1) {
        AmberLED::high();
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(900));
        AmberLED::low();
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
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



void test777()
{

}

int main()
{ 
    InternalSPI::up(BaudRate{SPI::BaudRate::Div4},
                    SlaveManagement{SPI::SlaveManagement::SoftwareSelected},
                    Mode{SPI::Mode::Master});

    Serial5TXPin::mode(GPIO::Mode::Alternate);
    Serial5TXPin::af(GPIO::AF::AF8);
	RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
	UART7->BRR = 0x187; // 115200 at 45MHz
	UART7->CR1 |= USART_CR1_UE_Msk | USART_CR1_TE; // usart en, tx en



    AmberLED::mode(GPIO::Mode::Output);
    auto blinkerThread = distortos::makeAndStartStaticThread<512>(150, blinker);

    mems::L3GD20<InternalSPI, GyroCSPin> l3gd20;
    mems::LSM303D<InternalSPI, AccMagCSPin> lsm303d;

    BaroCSPin::mode(GPIO::Mode::Output);
    BaroCSPin::high();

    MPUCSPin::mode(GPIO::Mode::Output);
    MPUCSPin::high();

//    uint8_t d[16];
//    d[0] = (uint8_t)(SPI1->SR >> 8);
//    d[1] = (uint8_t)SPI1->SR;
//    d[2] = '\r'; d[3] = '\n';
//    write(d, 4);




    //write(d, 4);

	while (1) {

        char data[64];
        uint8_t xyz[6];
        l3gd20.read((uint8_t *)xyz);

        int16_t x;
        uint8_t *p = (uint8_t *)&x;
        p[0] = xyz[0];
        p[1] = xyz[1];
        int16_t y = (int16_t)(((uint16_t)xyz[3] << 8) | xyz[2]);
        int16_t z = (int16_t)(((uint16_t)xyz[5] << 8) | xyz[4]);
        sprintf(data, "x:%d\r\n", x, y, z);
        write((uint8_t *)data, strlen(data));


        distortos::ThisThread::sleepFor(std::chrono::milliseconds(50));
	}
	return 0;
}
