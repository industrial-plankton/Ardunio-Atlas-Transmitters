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

#include <Arduino.h>
#include <Wire.h> //enable I2C.

#define i2c_id_ph 0x65      //default I2C pH address
#define i2c_id_temp 0x68    //default I2C temperature address
#define one_byte_read 0x01  //used in a function to read data from the device
#define two_byte_read 0x02  //used in a function to read data from the device
#define four_byte_read 0x04 //used in a function to read data from the device

// 0 .. success
// 1 .. length to long for buffer
// 2 .. address send, NACK received
// 3 .. data send, NACK received
// 4 .. other twi error (lost bus arbitration, bus error, ..)
// 5 .. timeout
static byte i2cError;

union sensor_mem_handler //declare the use of a union data type
{
  byte i2c_data[4]; //define a 4 byte array in the union
  long answ;        //define an long in the union
};
static union sensor_mem_handler move_data; //declare that we will refer to the union as move_data


// bool active_con(uint_fast8_t bus_address)
// void i2c_read(uint_fast8_t reg, uint_fast8_t number_of_bytes_to_read, uint_fast8_t bus_address);
// void i2c_write_byte(uint_fast8_t reg, uint_fast8_t data, uint_fast8_t bus_address);
// void i2c_write_long(uint_fast8_t reg, int_fast32_t data, uint_fast8_t bus_address);

bool calibration(uint_fast32_t calibration, uint_fast8_t type);
void temp_comp(uint_fast32_t compensation);
bool efficientConfig(uint_fast8_t bus_address);
long Temp_reading();
long pH_reading();