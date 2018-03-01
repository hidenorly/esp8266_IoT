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

#ifndef __UPNPDEVICE_WIFISWITCH_H__
#define __UPNPDEVICE_WIFISWITCH_H__

#include "UPnPDevice.h"

class UPnPDeviceWiFiSwitch : public UPnPDevice
{
public:
	UPnPDeviceWiFiSwitch(String deviceName, int port, String urn, String uuid, String serviceUrl);
	virtual ~UPnPDeviceWiFiSwitch();

protected:
	String _makeStateResponse(bool bState);

	virtual void onSetSwitchState(bool bState);
	virtual bool onGetState(void);

	virtual void _handleService(void);
	virtual void _handleUpnpControl(void);
	virtual void _handleEventservice(void);

	bool mSwitchStatus;
};

#endif /* __UPNPDEVICE_WIFISWITCH_H__ */
