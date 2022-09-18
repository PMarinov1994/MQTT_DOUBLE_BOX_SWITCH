#pragma once
#include <ESP8266WiFi.h>
#include <FS.h>
#include <WString.h>
#include "ConfigurationFile.h"
#include "GlobalDefinitions.h"


class CConfigurationManager
{
public:
	CConfigurationManager()
		: server(80)
	{
	};

	virtual ~CConfigurationManager()
	{
	};

	bool Init();
	bool HandleConnection();

private:
	void HandleConfiguration(WiFiClient client);
	String ReadContent(String resource);

	void SendConfigurationContent(WiFiClient client);

	void FlashLed(int msDelay);

private:

    WiFiServer server;
};