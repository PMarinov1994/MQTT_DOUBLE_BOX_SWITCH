#ifndef __GlobalDefinitions__H
#define __GlobalDefinitions__H

#pragma once

// CConfigurationManager Definitions
#define ESP8266_LED 2

const char WiFiAPPID[] = "ESP8266 ConfigWifi";
const char WiFiAPPSK[] = "123456789";

// CConfigurationFile Definitions
#define CONFIG_FILE_NAME "/config.txt"
#define CONFIG_FILE_SEPAREATOR '&'
#define CONFIG_FILE_EQUALS '='
#define CONFIG_FILE_END '#'

const char stationID[] = "ESP8266_OUTSIDE_BOX_ONE_SWITCH";

const char outside_switch_set_topic[] = "outside_box_one/#";
const char outside_switch_one_set_topic[] = "outside_box_one/switch_one_set";
const char outside_switch_two_set_topic[] = "outside_box_one/switch_two_set";

const char outside_switch_one_state_topic[] = "outside_box_one/switch_one";
const char outside_switch_two_state_topic[] = "outside_box_one/switch_two";

// CWorkingStation Definitions
#define SWITCH_ONE_PIN 3
#define SWITCH_TWO_PIN 1
#define MQTT_CONN_LED 0

#define BUILD_IN_LED 2
#define BUILD_IN_LED_ON LOW
#define BUILD_IN_LED_OFF HIGH

// !!! WARNING !!!!
// ESP.restart() will fail if done after programing
// (EPS8266 reenters boot loader mode, if not reset via reset button or power circle)
#define WAIT_BEFORE_RESTART_SEC 5 * 60

#define ELAPSED_SECONDS (millis() / 1000)

#define WIFI_WAIT_FOR_CONNECTION unsigned \
long ulRestartTime = ELAPSED_SECONDS + WAIT_BEFORE_RESTART_SEC; \
while (WiFi.status() != WL_CONNECTED) \
{\
    Print(".");\
    if (ulRestartTime < ELAPSED_SECONDS)\
        { Println("Failed to reconnect to WIFI for 5 mins. Calling ESP.restart()"); SERIAL_END; ESP.restart(); } \
    delay(1000); \
}

// Real Global Definitions
#define PRINT_LINES 0

#define SERIAL_SPEED 74880

#if (PRINT_LINES == 1)
#define Print(x) Serial.print(x)
#define Println(x) Serial.println(x)
#define SERIAL_CONFIGURE Serial.begin(SERIAL_SPEED, SERIAL_8N1, SERIAL_TX_ONLY)
#define SERIAL_END Serial.flush(); \
                   Serial.end(); \
                   delay(20)
#define PIN_MODE_SERIAL_PIN(x, y) ((void)0)
#define DIGITAL_WRITE_SERIAL_PIN(x, y) ((void)0)

#else
#define Print(x) ((void)0)
#define Println(x) ((void)0)
#define SERIAL_CONFIGURE ((void)0)
#define SERIAL_END ((void)0)
#define PIN_MODE_SERIAL_PIN(x, y) pinMode(x, y)
#define DIGITAL_WRITE_SERIAL_PIN(x, y) digitalWrite(x, y)
#endif // DEBUG

#endif // !__GlobalDefinitions__H
