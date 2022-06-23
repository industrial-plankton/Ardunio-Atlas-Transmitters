/*
MIT License

Copyright (c) 2021 industrial-plankton

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "Atlas-Transmitter.h"

#define one_byte_read 0x01  // used in a function to read data from the device
#define two_byte_read 0x02  // used in a function to read data from the device
#define four_byte_read 0x04 // used in a function to read data from the device

//*************************************************************************************************************************
//*************************************************************************************************************************

// TODO create function overloads that takes a TwoWire object as a parameter to enable non-default bus usage (change existing functions to call to new overloaded ones with default bus)
//*************************************************************************************************************************
//*************************************************************************************************************************

// calibration multiplyied by 1,000 , type: 1 = temperature, 2 = low , 3 = mid, 4 = high
bool calibration(const unsigned long calibration, const unsigned int type)
{
    const unsigned char calibration_value_register = 0x08;        // register to read / write
    const unsigned char calibration_request_register = 0x0C;      // register to read / write
    const unsigned char calibration_confirmation_register = 0x0D; // register to read
    const unsigned char cal_clear = 0x01;                         // clear calibration
    const unsigned char temperaturecalibrate = 0x02;              // calibrate to value

    // const byte cal_low = 0x02;											//calibrate to a low-point pH value (pH 4)
    // const byte cal_mid = 0x03;											//calibrate to a mid-point pH value (pH 7)
    // const byte cal_high = 0x04;											//calibrate to a high-point pH value(pH 10)

    if (type == 0x01)
    {                                                                                    // if calibration type is temperature else pH
        i2c_write_long(calibration_value_register, calibration, i2c_id_temp);            // write the 4 bytes of the long to the calibration register
        i2c_write_byte(calibration_request_register, temperaturecalibrate, i2c_id_temp); // write the calibration command to the calibration control register
        delay(10);                                                                       // wait for the calibration event to finish
        return i2c_read(calibration_confirmation_register, i2c_id_temp);                 // read from the calibration control register to confirm it is set correctly
    }
    else if (type == 5)
    {
        i2c_write_byte(calibration_request_register, cal_clear, i2c_id_ph); // write the calibration clear command to the calibration control register
        delay(10);                                                          // wait for the calibration event to finish
        return i2c_read(calibration_confirmation_register, i2c_id_ph);
    }
    else if (type == 6)
    {
        i2c_write_byte(calibration_request_register, cal_clear, i2c_id_temp); // write the calibration clear command to the calibration control register
        delay(10);                                                            // wait for the calibration event to finish
        return i2c_read(calibration_confirmation_register, i2c_id_temp);
    }
    else
    {
        i2c_write_long(calibration_value_register, calibration, i2c_id_ph); // write the 4 bytes of the unsigned long to the calibration register
        i2c_write_byte(calibration_request_register, type, i2c_id_ph);      // write the calibration command to the calibration control register
        delay(165);                                                         // wait for the calibration event to finish
        return i2c_read(calibration_confirmation_register, i2c_id_ph);      // read from the calibration control register to confirm it is set correctly
    }
}

void temp_comp(const unsigned long compensation)
{                                                                 // compensation = temp* 100
    const unsigned char temperature_compensation_register = 0x0E; // register to write

    i2c_write_long(temperature_compensation_register, compensation, i2c_id_ph); // write the 4 bytes of the long to the compensation register
}

//*************************************************************************************************************************
//*************************************************************************************************************************

bool efficientConfig(const unsigned char bus_address)
{
    Wire.beginTransmission(bus_address); // call the device by its ID number
    Wire.write(0x04);                    // transmit the register that we will start from
    Wire.write(0x00);                    // Set INT Pin Mode (0 disable, 2 high on new, 4 low on new, 8 invert on new)
    Wire.write(0x01);                    // Set LED Mode (1 blink, 0 off)
    Wire.write(0x01);                    // Set Hib Mode (1 read, 0 sleep)
    i2cError = Wire.endTransmission();   // end the I2C data transmission

    if (bus_address != 0)
    {
        return i2c_read(0x06, one_byte_read, bus_address).asByte; // read from the active / hibernate control register to confirm it is set correctly
    }
    else
    {
        return true;
    }
}

//*************************************************************************************************************************
//*************************************************************************************************************************

// RTD /= 1000;
long Temp_reading()
{
    const unsigned char RTD_register = 0x0E; // register to read
    // float RTD = 0;													//used to hold the new RTD value

    return i2c_read(RTD_register, four_byte_read, i2c_id_temp).asLong; // I2C_read(OEM register, number of bytes to read)
    // const long temp = move_data.asLong;
    // return temp; // move_data.answ;                               //move the 4 bytes read into a float
    //  RTD /= 1000;														//divide by 1000 to get the decimal point
    //  Serial.print("RTD= ");
    //  Serial.println(RTD, 3);	                                        //print info from register block
}

//  pH /= 1000;
long pH_reading()
{
    const unsigned char pH_register = 0x16; // register to read
    // float pH = 0;													//used to hold the new pH value

    return i2c_read(pH_register, four_byte_read, i2c_id_ph).asLong; // I2C_read(OEM register, number of bytes to read)
    // return move_data.answ;                                   // move the 4 bytes read into a float
    //  pH /= 1000;														//divide by 1000 to get the decimal point
    //  Serial.print("pH= ");
    //  Serial.println(pH);	                                        //print info from register block
}

int GetType(const byte bus_address)
{
    const byte DeviceTypeRegister = 0x00;
    return i2c_read(DeviceTypeRegister, bus_address);
}

int BroadCastChangeAddress(unsigned char newAddress)
{
    const byte BroadCastAddress = 0x00;
    const byte unlock_register = 0x02;
    const byte address_reg = 0x03;

    // Turn off all LEDs / disable Transmitters
    Wire.beginTransmission(BroadCastAddress); // call the device by its ID number
    Wire.write(0x04);                         // transmit the register that we will start from
    Wire.write(0x00);                         // Set INT Pin Mode (0 disable, 2 high on new, 4 low on new, 8 invert on new)
    Wire.write(0x00);                         // Set LED Mode (1 blink, 0 off)
    Wire.write(0x00);                         // Set Hib Mode (1 read, 0 sleep)
    i2cError = Wire.endTransmission();        // end the I2C data transmission

    // Unlock Address
    i2c_write_byte(unlock_register, 0x55, BroadCastAddress);
    i2c_write_byte(unlock_register, 0xAA, BroadCastAddress);
    // Write new Address
    i2c_write_byte(address_reg, newAddress, BroadCastAddress);

    // Turn on LED at new address to confirm it worked/comms are good
    i2c_write_byte(0x05, 0x01, newAddress);

    return GetType(newAddress);
}

int ChangeAddress(unsigned char oldAddress, unsigned char newAddress)
{
    const byte unlock_register = 0x02;
    const byte address_reg = 0x03;

    // Turn off all LEDs / disable Transmitters
    Wire.beginTransmission(oldAddress); // call the device by its ID number
    Wire.write(0x04);                   // transmit the register that we will start from
    Wire.write(0x00);                   // Set INT Pin Mode (0 disable, 2 high on new, 4 low on new, 8 invert on new)
    Wire.write(0x00);                   // Set LED Mode (1 blink, 0 off)
    Wire.write(0x00);                   // Set Hib Mode (1 read, 0 sleep)
    i2cError = Wire.endTransmission();  // end the I2C data transmission

    // Unlock Address
    i2c_write_byte(unlock_register, 0x55, oldAddress);
    i2c_write_byte(unlock_register, 0xAA, oldAddress);
    // Write new Address
    i2c_write_byte(address_reg, newAddress, oldAddress);

    // Turn on LED at new address to confirm it worked/comms are good
    i2c_write_byte(0x05, 0x01, newAddress);

    return GetType(newAddress);
}

void Atlas::Initialize() const
{
    efficientConfig(i2cAddress);
}

long Atlas::Read() const
{
    if (i2cAddress == 0)
    {
        return 0;
    }
    unsigned char readReg; // register to read

    switch (TrType)
    {
    case TransmitterType::pH:
        readReg = 0x16;
        break;
    case TransmitterType::temperature:
        readReg = 0x0E;
        break;

    default:
        return -1;
    }

    return i2c_read(readReg, four_byte_read, i2cAddress).asLong; // I2C_read(OEM register, number of bytes to read)
    // return move_data.answ;                                       // move the 4 bytes read into a long
}

void Atlas::Calibrate(const unsigned long calibrationValue, const CalibrationType type) const
{
    if (i2cAddress == 0)
    {
        return; // Dont allow broadcast
    }
    const unsigned char calibration_value_register = 0x08;        // register to read / write
    const unsigned char calibration_request_register = 0x0C;      // register to read / write
    const unsigned char calibration_confirmation_register = 0x0D; // register to read

    if (TrType == TransmitterType::temperature && type > 2)
        return;

    if (type != CalibrationType::Clear)
    {
        i2c_write_long(calibration_value_register, calibrationValue, i2cAddress); // write the 4 bytes of the unsigned long to the calibration register
    }
    i2c_write_byte(calibration_request_register, type, i2cAddress);         // write the calibration command to the calibration control register
    delay(165);                                                             // wait for the calibration event to finish
    i2c_read(calibration_confirmation_register, one_byte_read, i2cAddress); // read from the calibration control register to confirm it is set correctly
}

void Atlas::temp_compensate(const unsigned long compensation) const // compensation = temp* 100
{
    if (i2cAddress == 0)
    {
        return;
    }
    if (TrType != TransmitterType::pH)
        return;

    const unsigned char temperature_compensation_register = 0x0E; // register to write

    i2c_write_long(temperature_compensation_register, compensation, i2cAddress); // write the 4 bytes of the long to the compensation register
}