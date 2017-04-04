#pragma once
#include "gpio.hpp"
#include "spi.hpp"

// Internal SPI interface
typedef barehw::GPIOPinX<barehw::GPIO::A, 5>    SCKInternal;
typedef barehw::GPIOPinX<barehw::GPIO::A, 6>    MISOInternal;
typedef barehw::GPIOPinX<barehw::GPIO::A, 7>    MOSIInternal;
typedef barehw::SPIConfigX<SCKInternal, MOSIInternal, MISOInternal,
                   barehw::GPIO::AF::AF5, SPI1_BASE> InternalSPI;

// I2C 2
typedef barehw::GPIOPinX<barehw::GPIO::B, 10>   I2CTwoSCL;
typedef barehw::GPIOPinX<barehw::GPIO::B, 11>   I2CTwoSDA;

// L3GD20H Gyro
typedef barehw::GPIOPinX<barehw::GPIO::C, 13>   GyroCSPin;
typedef barehw::GPIOPinX<barehw::GPIO::B, 0>    GyroReadyPin;

// MS5611 Baro
typedef barehw::GPIOPinX<barehw::GPIO::D, 7>    BaroCSPin;

// LSM303D Acc & Mag
typedef barehw::GPIOPinX<barehw::GPIO::C, 15>   AccMagCSPin;
typedef barehw::GPIOPinX<barehw::GPIO::B, 4>    AccReadyPin;
typedef barehw::GPIOPinX<barehw::GPIO::B, 1>    MagReadyPin;

// MPU6000 Acc & Gyro
typedef barehw::GPIOPinX<barehw::GPIO::C, 2>    MPUCSPin;
typedef barehw::GPIOPinX<barehw::GPIO::D, 15>   MPUReadyPin;

// Indication
typedef barehw::GPIOPinX<barehw::GPIO::E, 12>   AmberLED;

// Serial 4 - UART8
typedef barehw::GPIOPinX<barehw::GPIO::E, 0>    Serial4RXPin;
typedef barehw::GPIOPinX<barehw::GPIO::E, 1>    Serial4TXPin;

// Serial 5 - UART7
typedef barehw::GPIOPinX<barehw::GPIO::E, 7>    Serial5RXPin;
typedef barehw::GPIOPinX<barehw::GPIO::E, 8>    Serial5TXPin;
