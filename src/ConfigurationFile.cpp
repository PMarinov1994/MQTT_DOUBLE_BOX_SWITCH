#include "ConfigurationFile.h"
#include <LITTLEFS.h>

CConfigurationFile::~CConfigurationFile()
{
    if (m_ssid)
        delete[] m_ssid;

    if (m_psk)
        delete[] m_psk;

    if (m_mqttServerIP)
        delete[] m_mqttServerIP;

    if (m_map)
        delete[] m_map;
}

/*
*
*
*/
bool CConfigurationFile::SetConfiguration(String config)
{
    File confFile = LittleFS.open(CONFIG_FILE_NAME, "r");
    if (confFile)
    {
        String config = confFile.readString();
        confFile.close();

        ExtractConfigFileData(config);
    }

    ExtractConfigFileData(config);

    if (!(m_ssid && strlen(m_ssid) > 0) ||
        !(m_psk && strlen(m_psk) > 0) ||
        !(m_mqttServerIP && strlen(m_mqttServerIP) > 0) ||
        !(m_mqttServerPort > 0))
    {
        return false;
    }

    confFile = LittleFS.open(CONFIG_FILE_NAME, "w+");
    if (!confFile)
    {
        Println("Fail to create or open file");
        return false;
    }

    String configStr = GetConfigFileData();

    confFile.print(configStr);
    confFile.close();

    return true;
}



/*
*
*
*/
void CConfigurationFile::ParseConfiguration()
{
    File f = LittleFS.open(CONFIG_FILE_NAME, "r");
    if (!f)
    {
        Println("Failed to open file /config.txt");
        return;
    }

    String config = f.readString();
    f.close();

    Println("Parsing config...");
    ExtractConfigFileData(config);

    Print("SSID: ");
    Println(m_ssid);

    Print("PSK: ");
    Println(m_psk);

    Print("MQTT: ");
    Println(m_mqttServerIP);

    Print("MQTT Port: ");
    Println(m_mqttServerPort);
}


// [orig buffer] <- Pointer1 <- Pointer2
void CConfigurationFile::StringToCharPtr(String str, char ** ch)
{
    if (*ch)
        delete[] * ch;

    unsigned int chLen = str.length() + 1;
    *ch = new char[chLen];
    memset(*ch, 0, chLen);
    str.toCharArray(*ch, chLen);
}



bool CConfigurationFile::ExtractConfigFileData(String str)
{
    unsigned int currPoss = 0;
    unsigned int configLen = str.length();

    String tmpParam;
    String tmpKey;

    if (currPoss == configLen)
        return false;

    StateParser currState = StateParser::WAIT_EQUALS;
    while (true)
    {
        if (currPoss >= configLen)
            break;

        char ch = str.charAt(currPoss++);

        if (CONFIG_FILE_EQUALS == ch)
            currState = StateParser::GOT_EQUALS;

        if (CONFIG_FILE_SEPAREATOR == ch)
            currState = StateParser::GOT_SEPARATOR;

        if (currPoss >= configLen)
            currState = StateParser::END_OF_FILE;

        switch (currState)
        {
        case StateParser::WAIT_EQUALS:

            tmpKey += ch;
            break;

        case StateParser::GOT_EQUALS:

            currState = StateParser::WAIT_SEPARATOR;
            break;

        case StateParser::WAIT_SEPARATOR:

            tmpParam += ch;
            break;

        case StateParser::END_OF_FILE:

            // Fall through to parse the last param.
            // Append char if it is not an EQUAL sign.
            if (CONFIG_FILE_EQUALS != ch)
                tmpParam += ch;

        case StateParser::GOT_SEPARATOR:

            if (tmpParam.length() == 0)
                return false;

            SetConfigParam(tmpParam, tmpKey);
            tmpParam = "";
            tmpKey = "";
            currState = StateParser::WAIT_EQUALS;
            break;
        }
    }

    return true;
}



void CConfigurationFile::SetConfigParam(String configParam, String key)
{
    Print("Config param: ");
    Print(key);
    Print(" Value: ");
    Println(configParam);

    for (int i = 0; i < m_mapSize; ++i)
    {
        if (m_map[i].m_key != key)
            continue;

        switch (m_map[i].m_type)
        {
        case ParamType::CHAR_ARRAY:
            StringToCharPtr(configParam, m_map[i].m_charArr);
            Print("Char member: ");
            Println(*m_map[i].m_charArr);
            break;
        case ParamType::INT:
            *(m_map[i].m_int) = configParam.toInt();
            Print("Int member: ");
            Println(*m_map[i].m_int);
            break;
        }
    }
}



String CConfigurationFile::GetConfigFileData()
{
    String result;

    for (int i = 0; i < m_mapSize; ++i)
    {
        result += m_map[i].m_key;
        result += CONFIG_FILE_EQUALS;

        switch (m_map[i].m_type)
        {
        case ParamType::CHAR_ARRAY:

            result += *(m_map[i].m_charArr);
            break;
        case ParamType::INT:

            result += *(m_map[i].m_int);
            break;
        }

        if (i < m_mapSize - 1)
            result += CONFIG_FILE_SEPAREATOR;
    }

    return result;
}
