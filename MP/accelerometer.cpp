#include "address_map_nios2.h"
#include "accelerometer.h"

//same as accelerometer_example.c functions
/********************************************************************************
 * This program demonstrates the use of the Accelerometer in the Computer
*System.
 *
 * It performs the following:
 *  1. Reads the x-axis value from the accelerometer
 *  2. Finds the change in the x-axis
 *  3. Displays the associated tilt of the board on the LEDs
********************************************************************************/

/*
 * The base address for the accelerometer registers.
 * The address register is at a +0 offset. The data register is at a +1 offset.
 * Use a volatile pointer for I/O registers (volatile means that IO load
 * and store instructions will be used to access these pointer locations,
 * instead of regular memory loads and stores).
 */

volatile char* AccelerometerBase = (volatile char*)ACCELEROMETER_BASE;

// Writes 'data' to the given register ('reg') of the ADXL345 accelerometer.
void accelerometer::ADXL345_WRITE(char reg, char data) {
    *(AccelerometerBase) = reg;
    *(AccelerometerBase + 1) = data;
}

// Reads the data from the given register ('reg') of the ADXL345 accelerometer.
char accelerometer::ADXL345_READ(char reg) {
    *(AccelerometerBase) = reg;
    return *(AccelerometerBase + 1);
}

// Returns the associated LED value from the x_axis value
int accelerometer::LED_val(int x_axis, int signed_bit) {
    switch (x_axis) {
    case 0x0:
        return 0x18;
    case 0x1:
        return signed_bit ? 0x8 : 0x10;
    case 0x2:
        return signed_bit ? 0xc : 0x30;
    case 0x3:
        return signed_bit ? 0x4 : 0x20;
    case 0x4:
        return signed_bit ? 0x6 : 0x60;
    case 0x5:
        return signed_bit ? 0x2 : 0x40;
    case 0x6:
        return signed_bit ? 0x3 : 0xc0;
    default:
        return signed_bit ? 0x1 : 0x80;
    }
}
