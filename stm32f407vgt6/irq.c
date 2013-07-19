#include "irq.h"
#include "stm32f4xx.h"

int inISR(void)
{
    return (__get_IPSR() & 0xFF);
}

unsigned int disableIRQ(void)
{
    // FIXME PRIMASK is the old CPSR (FAULTMASK ??? BASEPRI ???)
    //PRIMASK lesen
    unsigned int uiPriMask = __get_PRIMASK();
    __disable_irq();
    return uiPriMask;
}

void restoreIRQ(unsigned oldPRIMASK)
{
    //PRIMASK lesen setzen
    __set_PRIMASK(oldPRIMASK);
}
