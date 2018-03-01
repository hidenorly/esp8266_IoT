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

#ifndef __OTA_MANAGER_H__
#define __OTA_MANAGER_H__

#include "base.h"
#include "GpioDetector.h"

#define NO_GLOBAL_ARDUINOOTA
#include <ArduinoOTA.h>

class OtaManager
{
public:
    OtaManager(int modePin, int longPressPeriod);
    virtual ~OtaManager();

    void handle(void);

    void enableOta(bool bEnable);
    bool getOtaEnabled(void);

    typedef enum {
        STATUS_IDLE,
        STATUS_RUNNING_OTA,
        STATUS_ERROR
    } OTA_STATUS;

    OTA_STATUS getOtaStatus(void);

protected:
	static bool mbEnableOta;
	int mLongPressPeriod;
	GpioDetector* mpModePinDetector;
	int mLastPressed;
	static OTA_STATUS mStatus;
	static ArduinoOTAClass* mpArduinoOta;
	static int mStartOtaTime;
};

#endif /* __OTA_MANAGER_H__ */
