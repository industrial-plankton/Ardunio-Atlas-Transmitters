# Ardunio-Atlas-Transmitters

Simplified and limited interaction with Atlas Scientific OEM temperature and pH Transmitters, for microcontroller only usage.  

Derived from https://atlas-scientific.com/files/oem-ph-sample-code.pdf and https://atlas-scientific.com/files/oem-rtd-sample-code.pdf

Ex:
```C++
#include <Ardunio-Atlas-Transmitter.h>

Wire.begin(); //enable I2C
delay(10);
efficientConfig(i2c_id_ph);
delay(10);
efficientConfig(i2c_id_temp);

// calibrate temperature to 25C
calibration(25, 1)

// calibrate low pH to 4
calibration(4, 2)
// calibrate mid pH to 7
calibration(7, 2)
// calibrate high pH to 10
calibration(10, 2)

// take reading
unsigned int temperature = Temp_reading();

// set pH temperature compensation
temp_comp(temperature)

// take reading
unsigned int pH = pH_reading();

```
