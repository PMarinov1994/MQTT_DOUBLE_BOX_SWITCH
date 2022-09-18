#pragma once
#include <Arduino.h>

#define GPIO_RX 3
#define GPIO_TX 1

class CBootManager
{
public:
	CBootManager();
	virtual ~CBootManager() {};

	bool IsConfigBoot();
};