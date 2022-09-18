#include "ConfigurationManager.h"
#include <LITTLEFS.h>

/*
*	\brief Initialize the component
*
*/
bool CConfigurationManager::Init()
{
    pinMode(ESP8266_LED, OUTPUT);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(WiFiAPPID, WiFiAPPSK);

    server.begin();
    return true;
}



/*
*	\brief This will loop in main
*
*/
bool CConfigurationManager::HandleConnection()
{
    FlashLed(100);

    WiFiClient client = server.available();
    if (!client)
        return true;

    String req = client.readStringUntil('\r');
    Println("Request:");
    Println(req);

    if (req.indexOf("/confirm.html") > 0)
        HandleConfiguration(client);
    else
        SendConfigurationContent(client);

    client.flush();
    Println("Disconnecting client");

    return true;
}



/*
*	\brief Handle on /confirm.html request
*
*/
void CConfigurationManager::HandleConfiguration(WiFiClient client)
{
    String req = client.readString();

    Println(req);

    int postIndex = req.lastIndexOf("\r\n\r\n");
    postIndex += 4;

    Print("PostIndex: ");
    Println(postIndex);

    Print("Request len: ");
    Println(req.length());

    if ((unsigned int)postIndex >= req.length())
        return;

    String temp = req.substring(postIndex);

    Println("just post string: ");
    Println(temp);

    CConfigurationFile configFile;
    if (configFile.SetConfiguration(temp))
    {
        String resource = ReadContent(String("/confirm.html"));
        client.print(resource);
    }
    else
        SendConfigurationContent(client);
}



/*
*	\brief Sends a file as responce to client
*
*/
String CConfigurationManager::ReadContent(String resource)
{
    File f = LittleFS.open(resource, "r");
    if (!f)
        return String();

    String s = f.readString();
    f.close();
    
    return s;
}

void CConfigurationManager::SendConfigurationContent(WiFiClient client)
{
    String resource = ReadContent(String("/config.html"));
    
    Println("Parsing configuration...");
    
    CConfigurationFile config;
    config.ParseConfiguration();

    Println("Setting configuration in responce...");

    int size = resource.length() + 100;
    char* responce = new char[size];
    memset(responce, 0, size);

    Println("Calling sprint...");

    sprintf(responce, resource.c_str(),
        config.m_ssid,
        config.m_psk,
        config.m_mqttServerIP,
        config.m_mqttServerPort);

    Println(responce);
    client.print(responce);
    delete[] responce;
}



/*
*	\brief Blink onboard LED
*
*/
void CConfigurationManager::FlashLed(int msDelay)
{
    digitalWrite(ESP8266_LED, HIGH);
    delay(msDelay);
    digitalWrite(ESP8266_LED, LOW);
    delay(msDelay);
}
