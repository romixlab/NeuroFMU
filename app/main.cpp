#include "distortos/ThisThread.hpp"
#include "distortos/StaticThread.hpp"
#include "CMSIS-proxy.h"
#include "gpio.hpp"
#include "string.h"
#include "boardgoodies.hpp"
#include "spi.hpp"

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

uint8_t txrx(uint8_t tx)
{
    while ((SPI1->SR & SPI_SR_TXE) == 0);
    SPI1->DR = tx;
    while ((SPI1->SR & SPI_SR_RXNE) == 0);
    return (uint8_t)SPI1->DR;
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






template<class CSPin>
class L3GD20_20H
{
public:
    void h() {
        CSPin::high();
    }

    void l() {
        CSPin::low();
    }

private:
};

int main()
{ 
    Serial5TXPin::mode(GPIO::Mode::Alternate);
    Serial5TXPin::af(GPIO::AF::AF8);
	RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
	UART7->BRR = 0x187; // 115200 at 45MHz
	UART7->CR1 |= USART_CR1_UE_Msk | USART_CR1_TE; // usart en, tx en

    AmberLED::mode(GPIO::Mode::Output);
    //auto blinkerThread = distortos::makeAndStartStaticThread<512>(150, blinker);

    GyroCSPin::mode(GPIO::Mode::Output);
    GyroCSPin::high();

    BaroCSPin::mode(GPIO::Mode::Output);
    BaroCSPin::high();

    AccMagCSPin::mode(GPIO::Mode::Output);
    AccMagCSPin::high();

    MPUCSPin::mode(GPIO::Mode::Output);
    MPUCSPin::high();



    InternalSPI::up();

    SPI1->CR1 &=~ SPI_CR1_SPE;
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_MSTR;

    SPI1->CR1 |= SPI_CR1_SPE;
    volatile uint16_t cr1 = SPI1->CR1;
    volatile u_int16_t sr = SPI1->SR;

//    uint8_t d[16];
//    d[0] = (uint8_t)(SPI1->SR >> 8);
//    d[1] = (uint8_t)SPI1->SR;
//    d[2] = '\r'; d[3] = '\n';
//    write(d, 4);


L3GD20_20H<AmberLED> l3gd20;
l3gd20.h();

    //write(d, 4);

	while (1) {
        //GyroCSPin::low();
        AccMagCSPin::low();
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(1));

        uint8_t dummy = txrx(0b11000000 | 0x0f);
        uint8_t whoami = txrx(0x00);
        //d[1] = spiReceiveByte();
        //GyroCSPin::high();
        AccMagCSPin::high();

        char data[64];
        data[0] = dummy;
        data[1] = whoami;
        data[2] = '\r';
        data[3] = '\n';
        //sprintf(data, "read: %d %d\r\n", dummy, whoami);

        write((uint8_t *)data, 4);


        distortos::ThisThread::sleepFor(std::chrono::milliseconds(500));
	}
	return 0;
}
