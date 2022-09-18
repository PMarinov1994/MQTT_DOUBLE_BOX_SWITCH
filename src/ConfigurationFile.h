#pragma once
#include <FS.h>
#include "GlobalDefinitions.h"


class CConfigurationFile
{
public:
    CConfigurationFile()
        : m_ssid(NULL)
        , m_psk(NULL)
        , m_mqttServerIP(NULL)
        , m_mqttServerPort(0)
    {
        m_mapSize = 6;
        m_map = new ParamMap[6];
        m_map[0] = { "ssid", &m_ssid, NULL, ParamType::CHAR_ARRAY };
        m_map[1] = { "psk", &m_psk, NULL, ParamType::CHAR_ARRAY };
        m_map[2] = { "mqtt", &m_mqttServerIP, NULL, ParamType::CHAR_ARRAY };
        m_map[3] = { "mqttPort", NULL, &m_mqttServerPort, ParamType::INT };
    };
    virtual ~CConfigurationFile();

    bool SetConfiguration(String config);
    void ParseConfiguration();

    // Config Params
    char* m_ssid;
    char* m_psk;
    char* m_mqttServerIP;
    int m_mqttServerPort;

protected:

    enum StateParser
    {
        WAIT_EQUALS,
        GOT_EQUALS,
        WAIT_SEPARATOR,
        GOT_SEPARATOR,
        END_OF_FILE
    };

    enum ParamType
    {
        CHAR_ARRAY,
        INT
    };

    // Fill this struct when adding params
    // in the config file.
    struct ParamMap
    {
        String m_key;
        char** m_charArr;
        int* m_int;
        ParamType m_type;
    };

private:

    void StringToCharPtr(String str, char** ch);

    bool ExtractConfigFileData(String str);
    void SetConfigParam(String configParam, String key);

    String GetConfigFileData();

private:

    ParamMap * m_map;
    int m_mapSize;
};