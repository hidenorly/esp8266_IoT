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

#include "base.h"
#include "i2c.h"

I2C::I2C(int slaveAddress)
{
  mSLA = slaveAddress & 0x7F;
//  init();
}

I2C::~I2C()
{
}

void I2C::init(void)
{
  Wire.begin(GPIO_SDA,GPIO_SCL);
}

void I2C::write(BYTE command, BYTE* data, int nSize)
{
  Wire.beginTransmission(mSLA);
  Wire.write(command);
  for(int i=0; i<nSize; i++){
    Wire.write(data[i]);
  }
  Wire.endTransmission(true);
}

void I2C::read(BYTE command, BYTE* data, int nSize)
{
#if 1
  for(int i=0; i<nSize; i++){
    data[i]=0;
  }
  Wire.beginTransmission(mSLA);
  Wire.write(command);
  Wire.endTransmission(false);
  Wire.requestFrom((uint8_t)mSLA, (size_t)nSize, (bool)false);
  for(int i=0; (i<nSize && Wire.available()); i++){
    data[nSize-i-1] = Wire.read();
  }
  Wire.endTransmission(true);
#else
  for(int i=0; i<nSize; i++){
    data[i]=0;
  }
  for(int i=0; i<nSize; i++){
    Wire.beginTransmission(mSLA);
    Wire.write(command+i);
    Wire.endTransmission(false);
    Wire.requestFrom((uint8_t)mSLA, (size_t)1, (bool)false);
    data[nSize-i-1] = Wire.read();
    Wire.endTransmission(true);
    delay(5);
  }
#endif
}

void I2C::writeByte(BYTE command, BYTE data)
{
  write(command, &data, 1);
}

BYTE I2C::readByte(BYTE command)
{
  BYTE ret = 0;
  read(command, &ret, 1);

  return ret;
}

void I2C::writeWord(BYTE command, WORD data)
{
  write(command, (BYTE*)(&data), 2);
}

WORD I2C::readWord(BYTE command)
{
  WORD ret=0;

  read(command, (BYTE*)(&ret), 2);

  return ret;
}

