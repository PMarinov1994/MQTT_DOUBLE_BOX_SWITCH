/*
 Name:		esp8266_mqttSwitch.ino
 Created:	3/2/2021 12:03:21 PM
 Author:	Plamen
*/

#define DEBUF_OUTPUT

// MQTT headers
#include <PubSubClient.h>


// Sensor I2C headers
#include <Wire.h>

// Wifi headers
#include <WiFiUdp.h>
#include <WiFiServerSecure.h>
#include <WiFiServer.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESP8266WiFiType.h>
#include <ESP8266WiFiSTA.h>
#include <ESP8266WiFiScan.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266WiFiGeneric.h>
#include <ESP8266WiFiAP.h>
#include <ESP8266WiFi.h>

// FileSystem headers
#include <FS.h>
#include <LITTLEFS.h>

// Project headers
#include "ConfigurationManager.h"
#include "BootManager.h"
#include "WorkingStation.h"
#include "GlobalDefinitions.h"

// Setup variables
CConfigurationManager configurationManager;
CWorkingStation workStation;

bool isConfigurationMode = false;

ADC_MODE(ADC_VCC);

/*
*	\brief
*
*/
void setup()
{
    // This depends on the Global #define PRINT_LINES
    // * 0 Serial monitor will be disable.
    //   TX will be set to HIGH and power the I2C devices.
    // * 1 Serial monitor will only transmit (TX is active)
    //   No devices should be connected to I2C power line.
    // ** RX will always be input GPIO for firmware boot mode.
    //    Has a hardware PULL_UP resistor.
    // *** LOW Firmware starts web server for configurations.
    // *** HIGHT (Normal) Firmware will read and post sensor data.
    SERIAL_CONFIGURE;
    //
    Println("");

    bool littleFsEn = LittleFS.begin();
    Print("LittleFS Status: ");
    Println(littleFsEn);

    workStation.Init();
}



/*
*	\brief
*
*/
void loop()
{
    workStation.Work();
}
