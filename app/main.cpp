#include "distortos/ThisThread.hpp"
#include "distortos/StaticThread.hpp"
#include "stm32f4xx.h"
#include "gpio.h"

/**
 * SPI int: sck - pa5, miso - pa6, mosi - pa7
 * L3GD20H Gyro, !gyro_cs pc13, gyro_drdy pb0
 * MS5611 Baro, !baro_cs pd7
 * LSM303D Acc,Mag, !accel_mag_cs pc15, accel_drdy pb4, mag_drdy pb1
 * MPU6000, !mpu_cs pc2, mpu_drdy pd15
 */

typedef GPIOPin<GPIOE_BASE, 12> AmberLED;

int main()
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;
	GPIOE->MODER |= GPIO_MODER_MODE8_1; // tx af
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL8_3; // af8


	RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
	UART7->BRR = 0x187; // 115200 at 45MHz
	UART7->CR1 |= USART_CR1_UE_Msk | USART_CR1_TE; // usart en, tx en

    AmberLED::config(GPIOConfig::Mode::Output);

	while (1) {
        AmberLED::high();
		distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
        AmberLED::low();
        distortos::ThisThread::sleepFor(std::chrono::milliseconds(100));
	}
	return 0;
}
