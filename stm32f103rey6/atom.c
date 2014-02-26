/*
 * atom.c
 *
 *  Created on: 10.08.2012
 *      Author: pfeiffer
 */

#include "cpu.h"

unsigned int atomic_set_return(unsigned int *p, unsigned int uiVal)
{
    //unsigned int cspr = disableIRQ();		//crashes
    dINT();
    unsigned int uiOldVal = *p;
    *p = uiVal;
    //restoreIRQ(cspr);						//crashes
    eINT();
    return uiOldVal;
}
