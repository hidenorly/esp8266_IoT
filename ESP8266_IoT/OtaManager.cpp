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
#define NO_GLOBAL_ARDUINOOTA
#include <ArduinoOTA.h>
#include "OtaManager.h"

#ifndef OTA_TIME_OUT
	#define OTA_TIME_OUT (1000*60*5)	// 5min
#endif

bool OtaManager::mbEnableOta = false;
OtaManager::OTA_STATUS OtaManager::mStatus = STATUS_IDLE;
ArduinoOTAClass* OtaManager::mpArduinoOta = NULL;
int OtaManager::mStartOtaTime = 0;


OtaManager::OtaManager(int modePin, int longPressPeriod):mLongPressPeriod(longPressPeriod),mLastPressed(0)
{
	mpModePinDetector = new GpioDetector(modePin, false, 0);
}

OtaManager::~OtaManager()
{
	delete mpModePinDetector;
	mpModePinDetector = NULL;
	delete mpArduinoOta;
	mpArduinoOta = NULL;
}

void OtaManager::enableOta(bool bEnable)
{
	if( mbEnableOta != bEnable) {
		if (!bEnable) {
			// disable OTA
			if( STATUS_IDLE == mStatus ){
				if(mpArduinoOta) {
					delete mpArduinoOta;
					mpArduinoOta = NULL;
					mStatus = STATUS_IDLE;
				}
				mbEnableOta = false;
				DEBUG_PRINTLN("OTA is disabled.");
			}
		} else {
			// enable OTA
			if(NULL == mpArduinoOta) {
				mpArduinoOta = new ArduinoOTAClass();

				mpArduinoOta->setHostname("esp8266");

				mpArduinoOta->onStart([]() {
					mStatus = STATUS_RUNNING_OTA;
					DEBUG_PRINTLN("OTA is starting.");
				});
				mpArduinoOta->onEnd([]() {
					mStatus = STATUS_IDLE;
					DEBUG_PRINTLN("OTA is done.");
				});
				mpArduinoOta->onProgress([](unsigned int progress, unsigned int total) {
					DEBUG_PRINT("OTA Progress:");
					DEBUG_PRINT(String(progress / (total / 100)));
					DEBUG_PRINTLN("%%");
				});
				mpArduinoOta->onError([](ota_error_t error) {
					DEBUG_PRINT("Error on OTA : ");
					DEBUG_PRINTLN(String(error));
					mStatus = STATUS_ERROR;
				});

				mpArduinoOta->begin();
				DEBUG_PRINTLN("OTA is enabled.");
				mStartOtaTime = millis();
			}
		}

		mbEnableOta = bEnable;
	}
}

bool OtaManager::getOtaEnabled(void)
{
	return mbEnableOta;
}


void OtaManager::handle(void)
{
	if( getOtaEnabled() ){
		if( (millis()-mStartOtaTime)>OTA_TIME_OUT ){
			enableOta(false);
		} else {
			if( mpArduinoOta){
				mpArduinoOta->handle();
			}
		}
	} else {
		if(mpModePinDetector){
			mpModePinDetector->update();
			bool curPress = mpModePinDetector->getStatus();
			if( curPress ){
				if ((millis()-mLastPressed) > mLongPressPeriod ) {
					// long pressed
					enableOta(true);
				}
			} else {
				mLastPressed = millis();
			}
		}
	}
}

OtaManager::OTA_STATUS OtaManager::getOtaStatus(void)
{
	return mStatus;
}
