/**
 * \file
 * \brief Main code block.
 *
 * \author Copyright (C) 2016 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include "distortos/ThisThread.hpp"
#include "distortos/StaticThread.hpp"
#include "distortos/chip/ChipOutputPin.hpp"

namespace
{
	void ledBlinker(distortos::chip::ChipOutputPin &led, const std::chrono::milliseconds period)
	{
		while (1) {
			led.set(!led.get());
			distortos::ThisThread::sleepFor(period);
		}
	}

}

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Main code block of template application
 *
 * Constantly calls functionFromFolder() and sleeps for one second.
 *
 * \return doesn't return
 */

int main()
{
	// distortos::chip::ChipOutputPin led(distortos::chip::Pin::pe12);
	// auto ledBlinkerThread = distortos::makeAndStartStaticThread<256>(1, ledBlinker,
	// 	std::ref(led), std::chrono::milliseconds{200});
	// ledBlinkerThread.join();

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
