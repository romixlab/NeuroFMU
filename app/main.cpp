#include "distortos/ThisThread.hpp"
#include "distortos/StaticThread.hpp"
#include "CMSIS-proxy.h"
#include "gpio.h"

#include "boardgoodies.h"

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

void spiSendByte(uint8_t b)
{
    SPI1->CR1 |= SPI_CR1_SPE;
    while ((SPI1->SR & SPI_SR_TXE) == 0);
    SPI1->DR = b;
}

uint8_t spiReceiveByte()
{
    while ((SPI1->SR & SPI_SR_RXNE) == 0);
    return (uint8_t)SPI1->DR;
}

int main()
{ 
    Serial5TXPin::mode(GPIO::Mode::Alternate);
    Serial5TXPin::af(GPIO::AF::AF8);
	RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
	UART7->BRR = 0x187; // 115200 at 45MHz
	UART7->CR1 |= USART_CR1_UE_Msk | USART_CR1_TE; // usart en, tx en

    AmberLED::mode(GPIO::Mode::Output);
    auto blinkerThread = distortos::makeAndStartStaticThread<512>(1, blinker);

    GyroCSPin::mode(GPIO::Mode::Output);
    GyroCSPin::high();

    BaroCSPin::mode(GPIO::Mode::Output);
    BaroCSPin::high();

    AccMagCSPin::mode(GPIO::Mode::Output);
    AccMagCSPin::high();

    MPUCSPin::mode(GPIO::Mode::Output);
    MPUCSPin::high();

    SCKInternal::mode(GPIO::Mode::Alternate);
    SCKInternal::af(GPIO::AF::AF5);
    SCKInternal::speed(GPIO::Speed::High);

    MOSIInternal::mode(GPIO::Mode::Alternate);
    MOSIInternal::af(GPIO::AF::AF5);
    MOSIInternal::speed(GPIO::Speed::High);

    MISOInternal::mode(GPIO::Mode::Alternate);
    MISOInternal::af(GPIO::AF::AF5);

    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 = SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_MSTR;

    uint8_t d[16];
    d[0] = (uint8_t)(SPI1->SR >> 8);
    d[1] = (uint8_t)SPI1->SR;
    d[2] = '\r'; d[3] = '\n';
    write(d, 4);

    GyroCSPin::low();
    spiSendByte(0b11000000 | 0x0f);
    spiSendByte(0x00);
    spiReceiveByte();
    d[1] = spiReceiveByte();
    GyroCSPin::high();

    write(d, 4);

	while (1) {
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(500));
	}
	return 0;
}
