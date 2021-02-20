/********************************** (C) COPYRIGHT ******************************
* File Name          :	main.c											
* Author             : 	QITAS                                                      
* Version            :  V1.0                                                     
* Date               :  2018/06/05                                               
* Description        :  A demo for HID compatible device.           
********************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <ch554.h>
#include <ch554_usb.h>
#include <debug.h>
#include	"USB.h"
#include 	"Key.h"

extern uint8_t 	FLAG;												// Trans complete flag
extern uint8_t 	EnumOK;												// Enum ok flag
extern uint8_t 	TX[64];
extern uint8_t 	RX[64];
__idata uint16_t 		TouchKeyButton = 0;							
//uint8_tX  		TSTDATA[64]  _at_ 0x00E0; 

void CfgCLK(void)  
{
	SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	CLOCK_CFG = CLOCK_CFG & ~ MASK_SYS_CK_SEL | 0x06;  // 24MHz		
	SAFE_MOD = 0x00;
}

void main( void )
{
	uint8_t i;
	CfgCLK();                                 //Configure sys
	mDelaymS(10);                             
	mInitSTDIO( );                            // Init UART0

	USBDeviceInit();                          // Configure the USB module,the Endpoint and the USB Interrupt
	EA = 1;	
	UEP1_T_LEN = 0;                           // Reset the trans length register
	UEP2_T_LEN = 0;                                                  
	FLAG = 0;
	EnumOK = 0;
	TouchKeyQueryCylSet1Or2ms(1);	          //set query cycle
	TouchKeyChanelSelect(3);                  //set query chanel	
	for( i = 1; i < 64 ;i ++ )
	{
		TX[i]=i+1;
	}
	while(1)
	{
        if( EnumOK == 1 )
        {					
            TXflagHandle(2);				
        }
        else
        {
            if(TKEY_CTRL&bTKC_IF)           //wait query finsh,max 2ms ��ȡ��ֵ��׼ֵ�����������
            {							
				TouchKeyButton = TKEY_DAT; 
            }							
        }
		if(RXflag);
		i++;
    }
}

/**************************** END *************************************/
