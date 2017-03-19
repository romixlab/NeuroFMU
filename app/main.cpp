#include "distortos/ThisThread.hpp"
#include "distortos/StaticThread.hpp"
#include "stm32f427xx.h"

int main()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER |= GPIO_MODER_MODE8_1; // tx af
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL8_3; // af8


	RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
	UART7->BRR = 0x187; // 115200 at 45MHz
	UART7->CR1 |= USART_CR1_UE_Msk | USART_CR1_TE; // usart en, tx en


	while (1) {
		UART7->DR = 0x77;
		distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
	}
	return 0;
}
