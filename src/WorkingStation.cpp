#include "WorkingStation.h"
#include "ConfigurationFile.h"



/*
*	\brief Initialize the component
*
*/
bool CWorkingStation::Init()
{
    PIN_MODE_SERIAL_PIN(SWITCH_ONE_PIN, OUTPUT);
    PIN_MODE_SERIAL_PIN(SWITCH_TWO_PIN, OUTPUT);

    DIGITAL_WRITE_SERIAL_PIN(SWITCH_ONE_PIN, m_switchOneState);
    DIGITAL_WRITE_SERIAL_PIN(SWITCH_TWO_PIN, m_switchTwoState);

    pinMode(BUILD_IN_LED, OUTPUT);
    digitalWrite(BUILD_IN_LED, BUILD_IN_LED_OFF);

    pinMode(MQTT_CONN_LED, OUTPUT);
    digitalWrite(MQTT_CONN_LED, HIGH);

    CConfigurationFile configFile;
    configFile.ParseConfiguration();

    int ssidLen = strlen(configFile.m_ssid) + 1;
    int pskLen = strlen(configFile.m_psk) + 1;

    if (m_ssid)
        delete[] m_ssid;

    m_ssid = new char[ssidLen];

    if (m_psk)
        delete[] m_psk;

    m_psk = new char[pskLen];

    memcpy(m_ssid, configFile.m_ssid, ssidLen);
    memcpy(m_psk, configFile.m_psk, pskLen);

    ConnectToWifi();

    IPAddress mqttServerIPAddr;
    mqttServerIPAddr.fromString(configFile.m_mqttServerIP);

    m_client.setServer(mqttServerIPAddr, configFile.m_mqttServerPort);
    
    MQTT_CALLBACK_SIGNATURE = std::bind(
        &CWorkingStation::MQTT_Callback,
        this,
        std::placeholders::_1,
        std::placeholders::_2,
        std::placeholders::_3);

    m_client.setCallback(callback);

    // Connect to MQTT Broker.
    ReconnectMQTT();

    // Check the supply voltage.
    float espVcc = (ESP.getVcc() / 1000.0) + 0.005; // In Volts
    Print("Battery VCC: ");
    Println(espVcc);

    return true;
}

uint8_t uiLedState = LOW;

/*
*	\brief This will loop in main
*/
void CWorkingStation::Work()
{
    // Check WiFi state
    // and if down - try to reconnect
    if (WL_CONNECTED != WiFi.status())
    {
        Print("WIFI DOWN! Reconnecting");

        digitalWrite(BUILD_IN_LED, BUILD_IN_LED_OFF);
        digitalWrite(MQTT_CONN_LED, HIGH);

        WIFI_WAIT_FOR_CONNECTION
    }
    else if (!m_client.connected())
    {
        Println("Client is disconnected. Will try to reconnect in a moment.");

        digitalWrite(BUILD_IN_LED, BUILD_IN_LED_OFF);
        digitalWrite(MQTT_CONN_LED, HIGH);

        this->ReconnectMQTT();
    }
    else
        m_client.loop();

    delay(10);
}

/*
********************** MQTT ERRORS **********************
* 
* TODO: This needs to be made into map and returned somewhere.
    -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
    -3 : MQTT_CONNECTION_LOST - the network connection was broken
    -2 : MQTT_CONNECT_FAILED - the network connection failed
    -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
    0 : MQTT_CONNECTED - the client is connected
    1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
    2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
    3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
    4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
    5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
*/

/*
*	\brief This will loop in main
*/
bool CWorkingStation::ReconnectMQTT()
{
    unsigned long ulRestartTime = ELAPSED_SECONDS + WAIT_BEFORE_RESTART_SEC;
    while (!m_client.connected())
    {
        if (ulRestartTime < ELAPSED_SECONDS)
        {
            Println("Failed to reconnect to MQTT Brocker for 5 mins. Calling ESP.restart()");
            SERIAL_END;
            ESP.restart();
        }

        if (m_client.connect(stationID))
        {
            Println("Client connected!");
            PublishSwitchState();
        }
        else
        {
            Print("failed, rc=");
            Print(m_client.state());
            Println(" try again in 2 seconds");
            // Wait 2 seconds before retrying
            delay(2000);
        }
    }

    ulRestartTime = ELAPSED_SECONDS + WAIT_BEFORE_RESTART_SEC;
    while (!m_client.subscribe(outside_switch_set_topic, MQTTQOS0))
    {
        if (ulRestartTime < ELAPSED_SECONDS)
        {
            Println("Failed to subscribe to MQTT Topic for 5 mins. Calling ESP.restart()");
            SERIAL_END;
            ESP.restart();
        }

        Println("Subscribe failed. Trying again...");
        delay(500);
    };

    digitalWrite(BUILD_IN_LED, BUILD_IN_LED_ON);
    digitalWrite(MQTT_CONN_LED, LOW);

    return true;
}



void CWorkingStation::ConnectToWifi()
{
    WiFi.mode(WIFI_STA);
    delay(20);

    WiFi.begin(m_ssid, m_psk);

    Print("Connecting");
    WIFI_WAIT_FOR_CONNECTION

    Println("");
    Print("Connected, IP address: ");
    Println(WiFi.localIP());

    // The ESP8266 tries to reconnect automatically when the connection is lost
    WiFi.setAutoReconnect(true);
}

void CWorkingStation::PublishSwitchState()
{
    if (!m_client.connected())
        return;

    String pinValue(m_switchOneState);
    bool pubStateResult = m_client.publish(outside_switch_one_state_topic, pinValue.c_str(), false);
    
    Print("'outside_box_one/switch_one' = ");
    Println(pinValue);
    Print(" Publish result ");
    Println(pubStateResult);

    pinValue = String(!m_switchTwoState);
    pubStateResult &= m_client.publish(outside_switch_two_state_topic, pinValue.c_str(), false);

    Print("'outside_box_one/switch_two' = ");
    Println(pinValue);
    Print(" Publish result ");
    Println(pubStateResult);

    if (!pubStateResult)
        ESP.restart();
}



void CWorkingStation::MQTT_Callback(char* topic, uint8_t* payload, unsigned int length)
{
    if (0 == length)
        return;

    String pinValue((char)payload[0]);
    if (0 == strcmp(topic, outside_switch_one_set_topic))
    {
        Println("Got a MQTT_Callback with outside_box_one/switch_one_set");

        m_switchOneState = pinValue == "0" ? LOW : HIGH;
        DIGITAL_WRITE_SERIAL_PIN(SWITCH_ONE_PIN, m_switchOneState);
        PublishSwitchState();

        return;
    }

    if (0 == strcmp(topic, outside_switch_two_set_topic))
    {
        Println("Got a MQTT_Callback with outside_box_one/switch_two_set");

        m_switchTwoState = pinValue == "0" ? HIGH : LOW;
        DIGITAL_WRITE_SERIAL_PIN(SWITCH_TWO_PIN, m_switchTwoState);
        PublishSwitchState();

        return;
    }
}