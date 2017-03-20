#pragma once
#include "gpio.h"

// Internal SPI interface
typedef GPIOPinX<GPIO::A, 5>    SCKInternal;
typedef GPIOPinX<GPIO::A, 6>    MISOInternal;
typedef GPIOPinX<GPIO::A, 7>    MOSIInternal;

// L3GD20H Gyro
typedef GPIOPinX<GPIO::C, 13>   GyroCSPin;
typedef GPIOPinX<GPIO::B, 0>    GyroReadyPin;

// MS5611 Baro
typedef GPIOPinX<GPIO::D, 7>    BaroCSPin;

// LSM303D Acc & Mag
typedef GPIOPinX<GPIO::C, 15>   AccMagCSPin;
typedef GPIOPinX<GPIO::B, 4>    AccReadyPin;
typedef GPIOPinX<GPIO::B, 1>    MagReadyPin;

// MPU6000 Acc & Gyro
typedef GPIOPinX<GPIO::C, 2>    MPUCSPin;
typedef GPIOPinX<GPIO::D, 15>   MPUReadyPin;

// Indication
typedef GPIOPinX<GPIO::E, 12>   AmberLED;

// Serial 4 - UART8
typedef GPIOPinX<GPIO::E, 0>    Serial4RXPin;
typedef GPIOPinX<GPIO::E, 1>    Serial4TXPin;

// Serial 5 - UART7
typedef GPIOPinX<GPIO::E, 7>    Serial5RXPin;
typedef GPIOPinX<GPIO::E, 8>    Serial5TXPin;
