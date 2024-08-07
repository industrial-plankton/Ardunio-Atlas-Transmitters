#ifndef ATLAS_TRANSMITTERS_H
#define ATLAS_TRANSMITTERS_H
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
#include <Wire.h>

#include <i2c_Helper.h> // https://github.com/industrial-plankton/i2c_Helper.git

#define i2c_id_ph 0x65   // default I2C pH address
#define i2c_id_temp 0x68 // default I2C temperature address

// bool active_con(uint_fast8_t bus_address)
// void i2c_read(uint_fast8_t reg, uint_fast8_t number_of_bytes_to_read, uint_fast8_t bus_address);
// void i2c_write_byte(uint_fast8_t reg, uint_fast8_t data, uint_fast8_t bus_address);
// void i2c_write_long(uint_fast8_t reg, int_fast32_t data, uint_fast8_t bus_address);

bool calibration(unsigned long calibration, unsigned char type);
void temp_comp(unsigned long compensation);
bool efficientConfig(unsigned char bus_address);
long Temp_reading();
long pH_reading();
int BroadCastChangeAddress(const unsigned char newAddress);
int ChangeAddress(const unsigned char oldAddress, const unsigned char newAddress);

enum TransmitterType : unsigned char
{
    pH = 1,
    temperature = 2
};

enum CalibrationType : unsigned char
{
    Clear = 1,
    Temp_Cal = 2,
    Low_Cal = 2,
    Mid_Cal = 3,
    High_Cal = 4
};
class Atlas
{
private:
    /* data */
    const TransmitterType TrType;
    const unsigned char i2cAddress;
    // TwoWire Bus;

public:
    Atlas(const TransmitterType TrType, const unsigned char i2cAddress) : TrType{TrType}, i2cAddress{i2cAddress} //, Bus{Wire}
    {
        Initialize();
    }
    // Atlas(const TransmitterType TrType, const unsigned char i2cAddress, TwoWire Bus) : TrType{TrType}, i2cAddress{i2cAddress}, Bus{Bus}
    // {
    // }
    void Initialize() const;
    long Read() const;
    void Calibrate(const unsigned long calibrationValue, const CalibrationType type) const;
    void temp_compensate(const unsigned long compensation) const;
};

#endif