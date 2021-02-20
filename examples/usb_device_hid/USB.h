/********************************** (C) COPYRIGHT *******************************
* File Name          :Compound_Dev.C											*	
* Author             : WCH                                                      *
* Version            : V1.2                                                     *
* Date               : 2017/02/24                                               *
* Description        : A demo for USB compound device created by CH554, support *
					   keyboard , and HID-compliant device.                     *
********************************************************************************/


#ifndef	__COMPOUND_H__
#define __COMPOUND_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <ch554.h>
#include <ch554_usb.h>

extern __idata uint16_t 	TouchKeyButton;	

extern uint8_t 	TX[64];
extern uint8_t 	RX[64];
extern uint8_t 	TXflag;
extern uint8_t 	RXflag;

extern	void 	USBDeviceInit();
extern	void 	TXflagHandle(uint8_t val);
	
#endif
/**************************** END *************************************/
