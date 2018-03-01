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
#include "UPnPDevice.h"
#include <ESP8266WiFi.h>
#include <Time.h>
#include "TimeUtil.h"

UPnPDevice::UPnPDevice(String deviceName, int port, String urn, String uuid, String serviceUrl):mDeviceName(deviceName),mBootId(0),mPort(port),mUrn(urn),mUuid(uuid),mpHttpd(NULL),mServiceUrl(serviceUrl)
{
	//
}

UPnPDevice::~UPnPDevice(void)
{
	if(mpHttpd){
		delete mpHttpd;
		mpHttpd = NULL;
	}
}

void UPnPDevice::enable(bool bEnable)
{
	if(mEnable!=bEnable){
		if(bEnable){
			if(!mpHttpd){
				mpHttpd = new ESP8266WebServer(mPort);
			}
			if(mpHttpd){
				_setupHandlers();
				mpHttpd->begin();
				DEBUG_PRINT("UPnP:");
				DEBUG_PRINT(mDeviceName);
				DEBUG_PRINTLN(":httpd enabled");
			}
			mEnable = true;
		} else {
			if(mpHttpd){
				DEBUG_PRINT("UPnP:");
				DEBUG_PRINT(mDeviceName);
				DEBUG_PRINTLN(":httpd disabled");
				_tearDownHandlers();
				mpHttpd->stop();
				mpHttpd->close();
				delete mpHttpd;
				mpHttpd=NULL;
			}
			mEnable = false;
		}
	}
}

void UPnPDevice::_setupHandlers(void)
{
	if(mpHttpd){
		mpHttpd->on("/", [&]() { _handleRoot(); });
		mpHttpd->on(mServiceUrl.c_str(), [&]() { _handleService(); });
		mpHttpd->on("/upnp/control/basicevent1", [&]() { _handleUpnpControl(); });
		mpHttpd->on("/eventservice.xml", [&]() { _handleEventservice(); });
	}
}

void UPnPDevice::_tearDownHandlers(void)
{
	if(mpHttpd){
	}
}


bool UPnPDevice::getEnabled(void)
{
	return mEnable;
}

void UPnPDevice::handle(void)
{
	if(mEnable && mpHttpd){
		mpHttpd->handleClient();
	}
}

String UPnPDevice::handleMSearch(String query)
{
	String response;

	if(mEnable){
		response =
			"HTTP/1.1 200 OK\r\n"
			"CACHE-CONTROL: max-age=86400\r\n"
			"DATE: "+TimeUtil::getDateTimeFromTime(now())+" GMT\r\n"
			"EXT:\r\n"
			"LOCATION: http://" + WiFi.localIP().toString() + ":" + String(mPort) + mServiceUrl+"\r\n"
			"OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n"
			"01-NLS: "+String(mBootId++)+"\r\n"
			"SERVER: Unspecified, UPnP/1.0, Unspecified\r\n"
			"ST: urn:"+mUrn+"\r\n"
			"USN: uuid:" + mUuid + "::urn:"+mUrn+"\r\n"
			"X-User-Agent: hidenorly\r\n\r\n";
	}

	return response;
}

void UPnPDevice::_handleRoot(void)
{
	if(mEnable && mpHttpd){
		mpHttpd->send(200, "text/plain", "It works!");
	}
}
