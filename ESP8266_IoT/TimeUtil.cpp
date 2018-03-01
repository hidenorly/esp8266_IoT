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

#include "base.h"
#include <Time.h>
#include "TimeUtil.h"

const char* szWeek[] PROGMEM = {
	"",
	"Sun",
	"Mon",
	"Tue",
	"Wed",
	"Thu",
	"Fri",
	"Sat"
};

const char* szMonth[] PROGMEM = {
	"",
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec",
};

String TimeUtil::getDateTimeFromTime(time_t n)
{
	char tm[30];
	sprintf(tm, "%s, %02d %s %04d %02d:%02d:%02d", szWeek[weekday(n)], day(n), szMonth[month(n)] , year(n), hour(n), minute(n), second(n));

	return String(tm);
}

