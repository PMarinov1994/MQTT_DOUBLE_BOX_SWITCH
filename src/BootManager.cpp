#include "BootManager.h"
#include "GlobalDefinitions.h"

/*
*	\brief
*
*/
CBootManager::CBootManager()
{
    // pinMode(GPIO_RX, INPUT);
}



/*
*	\brief
*
*/
bool CBootManager::IsConfigBoot()
{
    return false;
    // bool pinState = digitalRead(GPIO_RX) == LOW ? true : false;

    // Println("GPIO3 'Rx' = ");
    // Println(digitalRead(GPIO_RX));

    // return pinState;
}
