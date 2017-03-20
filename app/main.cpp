#include "distortos/ThisThread.hpp"
#include "distortos/StaticThread.hpp"
#include "CMSIS-proxy.h"
#include "gpio.h"

#include "boardgoodies.h"

void blinker()
{
    while(1) {
        AmberLED::high();
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
        AmberLED::low();
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
    }
}

int main()
{ 
    Serial5TXPin::mode(GPIO::Mode::Alternate);
    Serial5TXPin::af(GPIO::AF::AF8);


	RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
	UART7->BRR = 0x187; // 115200 at 45MHz
	UART7->CR1 |= USART_CR1_UE_Msk | USART_CR1_TE; // usart en, tx en

    AmberLED::mode(GPIO::Mode::Output);
    auto blinkerThread = distortos::makeAndStartStaticThread<256>(1, blinker);

    Serial4RXPin::mode(GPIO::Mode::Input);
    Serial4RXPin::pull(GPIO::Pull::Up);

	while (1) {


        UART7->DR = Serial4RXPin::read() ? 0x77 : 0x79;
        distortos::ThisThread::sleepFor(std::chrono::seconds{1});
	}
	return 0;
}
