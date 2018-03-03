/* 
 Copyright (C) 2018 hidenorly

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

#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <limits.h>
#include <esp8266_peri.h>

class Random
{
public:
	static void initialize(void){ randomSeed(RANDOM_REG32); };
	static int getRandom(int range=INT_MAX){ return random(range); }; 
};

#endif /* __RANDOM_H__ */
