# What's esp8266_IoT

This is ESP8266 based IoT (Internet of Things) base code.

This base code includes:
 - WiFi Client configuration through Web browser
 - WiFi SoftAP for initial settings (initially it is kicked or you can forcely invoke this mode by GPIO0 Low at boot time.)
 - NTP client when WiFi client is successfully connected, NTP will be starting.
 - Basic demo to print current time periodically.

# Instruction

This base code depends on followings. Please install them on your environment.

Thank you so much for the great libraries.

## General steps to include zip library

On Arduino IDE,

1. Sketch
2. Include library
3. Install Zip library
4. Specify the following zip-ed libraries.

## Time library (by PaulStoffregen-san)

```
$ git clone https://github.com/PaulStoffregen/Time.git Time
$ cd Time
$ git archive HEAD --output=../Time.zip
```

## NTP library (by exabugs)

```
$ git clone https://github.com/exabugs/sketchLibraryNTP NTP
$ cd NTP
$ git archive HEAD --output=../NTP.zip
```

# Confirmed environment

* ESP WROOM02
* ESP-07/12E

## Arduino configuration

* Arduino IDE : 1.6.8

* MCU board : Generic ESP8266 Module
* Flash Mode : DIO (default)
* Flash Frequency : 40MHz (default)
* CPU Frequency : 80MHz (default)
* Flash Size : 512k (64K SPIFFS) (default)
* Debug port : disabled (default)
* Debug level : nothing (default)
* Reset Method : ck (default)
* Upload Speed : 115200 (default)

## Hardware

* GPIO0: High (execution) / Low (flashing)
* GPIO2: High
* GPIO15: Low

Please note that GPIO0 : Low within 1sec after deasserting reset will bring you to set WiFi AP mode. You can configure the pin by ```MODE_PIN``` defined in ```ESP8266_IoT.ino```

