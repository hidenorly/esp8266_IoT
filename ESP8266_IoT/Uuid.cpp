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
#include "Uuid.h"
#include "Random.h"
#include "StringUtil.h"
#include <FS.h>

#define LENGTH_UUID 36
#define NUM_OF_UUID_HYPEN 4

static const int hyphenIndex[NUM_OF_UUID_HYPEN] = {8, 13, 18, 23};

String Uuid::generateUuid(void)
{
	String result;

	Random::initialize();

	char szUuid[LENGTH_UUID+1];

	uint32_t chipId = ESP.getChipId();
	char macAddr[4];
	macAddr[0] = (char) (chipId & 0xff);
	macAddr[1] = (char)((chipId & 0xff00) >> 8 );
	macAddr[2] = (char)((chipId & 0xff0000) >> 16 );
	macAddr[3] = (char)((chipId & 0xff000000) >> 24 );

	for(int i=0; i<=LENGTH_UUID; i=i+2){
		char szBuf[3];
		sprintf(szBuf, "%02x", random(0,255) ^ macAddr[i % 4]);
		szUuid[i] = szBuf[0];
		szUuid[i+1] = szBuf[1];
	}

	for(int i=0; i<NUM_OF_UUID_HYPEN; i++){
		szUuid[hyphenIndex[i]] = '-';
	}
	szUuid[14] = '4';
	szUuid[LENGTH_UUID] = '\0';

	return String(szUuid);
}

bool Uuid::isShouldHypen(int index)
{
	for(int i=0; i<NUM_OF_UUID_HYPEN; i++){
		if(index == hyphenIndex[i]){
			return true;
		}
	}

	return false;
}

bool Uuid::isUuid(String uuid)
{
	bool result = true;

	if(LENGTH_UUID == uuid.length()){
		const char* szUuid = uuid.c_str();
		for(int i=0; (i<LENGTH_UUID && result); i++){
			if( isShouldHypen(i) ){
				result = ( szUuid[i]=='-' );
			} else {
				result = StringUtil::isHex(szUuid[i]);
			}
		}
	}

	return result;
}

String Uuid::getUuidByName(String name)
{
	String uuid = generateUuid();
	String fileName = "uuid-" + name;
	DEBUG_PRINTLN(fileName);

	bool foundUuidFile = SPIFFS.exists(fileName);

	if(foundUuidFile){
		File f = SPIFFS.open(fileName, "r");
		uuid = f.readStringUntil('\n');
		f.close();
		uuid.trim();
		DEBUG_PRINTLN(uuid);

		if(!isUuid(uuid)){
			uuid = generateUuid();
			foundUuidFile = false;
		}
	}

	if(!foundUuidFile){
		File f = SPIFFS.open(fileName, "w");
		f.println(uuid);
		f.close();
	}

	return uuid;
}

