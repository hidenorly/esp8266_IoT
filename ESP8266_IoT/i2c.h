/* 
 Copyright (C) 2016 hidenorly

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#ifndef __I2C_H__
#define __I2C_H__

#include <Wire.h>
#include <stdint.h>

typedef uint8_t BYTE;
typedef uint16_t WORD;

class I2C
{
  public:
    I2C(int slaveAddress);
    ~I2C();

    void init(void);
    
    void write(BYTE command, BYTE* data, int nSize);
    void read(BYTE command, BYTE* data, int nSize);

    void writeByte(BYTE command, BYTE data);
    BYTE readByte(BYTE command);
    void writeWord(BYTE command, WORD data);
    WORD readWord(BYTE command);

  protected:
    BYTE mSLA;

    const int GPIO_SDA=2;
    const int GPIO_SCL=14;
};

#endif // __I2C_H__
