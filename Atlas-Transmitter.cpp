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

#define one_byte_read 0x01  //used in a function to read data from the device
#define two_byte_read 0x02  //used in a function to read data from the device
#define four_byte_read 0x04 //used in a function to read data from the device

// 0 .. success
// 1 .. length to long for buffer
// 2 .. address send, NACK received
// 3 .. data send, NACK received
// 4 .. other twi error (lost bus arbitration, bus error, ..)
// 5 .. timeout
static byte i2cError = 0;
byte CheckI2C()
{
    return i2cError;
}

union sensor_mem_handler //declare the use of a union data type
{
    byte i2c_data[4]; //define a 4 byte array in the union
    long answ;        //define an long in the union
};
static union sensor_mem_handler move_data; //declare that we will refer to the union as move_data

//*************************************************************************************************************************
//*************************************************************************************************************************

//used to read 1,2,and 4 bytes: i2c_read(starting register,number of bytes to read)
void i2c_read(uint_fast8_t reg, uint_fast8_t number_of_bytes_to_read, uint_fast8_t bus_address)
{

    uint_fast8_t i; //counter

    Wire.beginTransmission(bus_address); //call the device by its ID number
    Wire.write(reg);                     //transmit the register that we will start from
    i2cError = Wire.endTransmission();   //end the I2C data transmission

    Wire.requestFrom(bus_address, number_of_bytes_to_read); //call the device and request to read X bytes
    for (i = number_of_bytes_to_read; i > 0; i--)
    {
        move_data.i2c_data[i - 1] = Wire.read();
    }                                  //with this code we read multiple bytes in reverse
    i2cError = Wire.endTransmission(); //end the I2C data transmission
}

//*************************************************************************************************************************
//*************************************************************************************************************************

void i2c_write_byte(uint_fast8_t reg, uint_fast8_t data, uint_fast8_t bus_address)
{                                        //used to write a single byte to a register: i2c_write_byte(register to write to, byte data)
    Wire.beginTransmission(bus_address); //call the device by its ID number
    Wire.write(reg);                     //transmit the register that we will start from
    Wire.write(data);                    //write the byte to be written to the register
    i2cError = Wire.endTransmission();   //end the I2C data transmission
}

//*************************************************************************************************************************
//*************************************************************************************************************************

//used to write a 4 bytes to a register: i2c_write_long(register to start at, long data )
void i2c_write_long(uint_fast8_t reg, unsigned long data, uint_fast8_t bus_address)
{

    int_fast8_t i; //counter
    move_data.answ = data;

    Wire.beginTransmission(bus_address); //call the device by its ID number
    Wire.write(reg);                     //transmit the register that we will start from
    for (i = 3; i >= 0; i--)
    { //with this code we write multiple bytes in reverse
        Wire.write(move_data.i2c_data[i]);
    }
    i2cError = Wire.endTransmission(); //end the I2C data transmission
}

//*************************************************************************************************************************
//*************************************************************************************************************************

//calibration multiplyied by 1,000 , type: 1 = temperature, 2 = low , 3 = mid, 4 = high
bool calibration(unsigned long calibration, byte type)
{

    const uint_fast8_t calibration_value_register = 0x08;        //register to read / write
    const uint_fast8_t calibration_request_register = 0x0C;      //register to read / write
    const uint_fast8_t calibration_confirmation_register = 0x0D; //register to read
    const uint_fast8_t cal_clear = 0x01;                         //clear calibration
    const uint_fast8_t temperaturecalibrate = 0x02;                         //calibrate to value

    // const byte cal_low = 0x02;											//calibrate to a low-point pH value (pH 4)
    // const byte cal_mid = 0x03;											//calibrate to a mid-point pH value (pH 7)
    // const byte cal_high = 0x04;											//calibrate to a high-point pH value(pH 10)

    if (type == 0x01)
    {                                                                            //if calibration type is temperature else pH
        i2c_write_long(calibration_value_register, calibration, i2c_id_temp);    //write the 4 bytes of the long to the calibration register
        i2c_write_byte(calibration_request_register, temperaturecalibrate, i2c_id_temp);    //write the calibration command to the calibration control register
        delay(10);                                                               //wait for the calibration event to finish
        i2c_read(calibration_confirmation_register, one_byte_read, i2c_id_temp); //read from the calibration control register to confirm it is set correctly
    }
    else if (type == 5)
    {
        i2c_write_byte(calibration_request_register, cal_clear, i2c_id_ph); //write the calibration clear command to the calibration control register
        delay(10);                                               //wait for the calibration event to finish
        i2c_read(calibration_confirmation_register, one_byte_read, i2c_id_ph);
    }
    else if (type == 6)
    {
        i2c_write_byte(calibration_request_register, cal_clear, i2c_id_temp); //write the calibration clear command to the calibration control register
        delay(10);                                               //wait for the calibration event to finish
        i2c_read(calibration_confirmation_register, one_byte_read, i2c_id_temp);
    }
    else
    {
        i2c_write_long(calibration_value_register, calibration, i2c_id_ph);    //write the 4 bytes of the unsigned long to the calibration register
        i2c_write_byte(calibration_request_register, type, i2c_id_ph);         //write the calibration command to the calibration control register
        delay(165);                                                            //wait for the calibration event to finish
        i2c_read(calibration_confirmation_register, one_byte_read, i2c_id_ph); //read from the calibration control register to confirm it is set correctly
    }

    return true; // move_data.i2c_data[0];
}

void temp_comp(uint_fast32_t compensation)
{                                                                //compensation = temp* 100
    const uint_fast8_t temperature_compensation_register = 0x0E; //register to write

    i2c_write_long(temperature_compensation_register, compensation, i2c_id_ph); //write the 4 bytes of the long to the compensation register
}

//*************************************************************************************************************************
//*************************************************************************************************************************

bool efficientConfig(uint_fast8_t bus_address)
{
    Wire.beginTransmission(bus_address); //call the device by its ID number
    Wire.write(0x04);                    //transmit the register that we will start from
    Wire.write(0x00);                    //Set INT Pin Mode (0 disable, 2 high on new, 4 low on new, 8 invert on new)
    Wire.write(0x01);                    //Set LED Mode (1 blink, 0 off)
    Wire.write(0x01);                    //Set Hib Mode (1 read, 0 sleep)
    i2cError = Wire.endTransmission();   //end the I2C data transmission

    i2c_read(0x06, one_byte_read, bus_address); //read from the active / hibernate control register to confirm it is set correctly

    return move_data.i2c_data[0];
}

//*************************************************************************************************************************
//*************************************************************************************************************************

// RTD /= 1000;
long Temp_reading()
{
    const uint_fast8_t RTD_register = 0x0E; //register to read
    // float RTD = 0;													//used to hold the new RTD value

    i2c_read(RTD_register, four_byte_read, i2c_id_temp); //I2C_read(OEM register, number of bytes to read)
    long temp = move_data.answ;
    return temp; //move_data.answ;                               //move the 4 bytes read into a float
                 // RTD /= 1000;														//divide by 1000 to get the decimal point
                 // Serial.print("RTD= ");
                 // Serial.println(RTD, 3);	                                        //print info from register block
}

//  pH /= 1000;
long pH_reading()
{
    const uint_fast8_t pH_register = 0x16; //register to read
    // float pH = 0;													//used to hold the new pH value

    i2c_read(pH_register, four_byte_read, i2c_id_ph); //I2C_read(OEM register, number of bytes to read)
    return move_data.answ;                            //move the 4 bytes read into a float
                                                      // pH /= 1000;														//divide by 1000 to get the decimal point
                                                      // Serial.print("pH= ");
                                                      // Serial.println(pH);	                                        //print info from register block
}