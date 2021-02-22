/********************************** (C) COPYRIGHT ******************************
* File Name          :USB.C												
* Author             :QITAS                                                      
* Version            : V1.0                                                  
* Date               : 2018/06/05                                               
* Description        : A demo for USB compound device created by CH554, support 
					   keyboard , and HID-compliant device.                     
********************************************************************************/
#include 	<stdio.h>
#include 	<stdlib.h>
#include 	<string.h>

#include 	"CH554.H"
#include 	"DEBUG.H"
#include 	"USB.h"
#include 	"Key.h"

// #pragma  NOAREGS

#define 	THIS_ENDP0_SIZE         DEFAULT_ENDP0_SIZE
#define		BUFFER_SIZE				64
#define 	DUAL_BUFFER_SIZE		128
#define 	UsbSetupBuf     		((PUSB_SETUP_REQ)Ep0Buffer)

__xdata __at (0x0000) uint8_t  Ep0Buffer[THIS_ENDP0_SIZE];  								// Endpoint 0, buffer OUT/OUT��the address must be even.
__xdata __at (0x000A) uint8_t  Ep1Buffer[BUFFER_SIZE];  								// Endpoint 1, buffer IN��the address must be even.
__xdata __at (0x0050) uint8_t  Ep2Buffer[DUAL_BUFFER_SIZE];  								// Endpoint 2, buffer OUT[64]+IN[64]��the address must be even.
 
/**************************** Global variable ********************************/	
uint8_t   volatile	SetupReq, SetupLen, UsbConfig;
uint8_t	volatile	EnumOK, FLAG;
uint8_t *  pDescr;                                                                		// USB enumerate complete flag.
USB_SETUP_REQ  		SetupReqBuf;                                   					// A buffer for Setup package.

uint8_t HIDKey[8] = {0};
uint8_t TX[64] = {0};	
uint8_t RX[64] = {0};
uint8_t TXflag = 0;
uint8_t RXflag = 0;


/**************************** Device Descriptor *************************************/
__code uint8_t DevDesc[18] = {																// Device Descriptor
	0x12,0x01,
	0x10,0x01,
	0x00,0x00,
	0x00,0x08,      
    
	0x31,0x51,																		// Vendor ID   |  VID =  0X5131///413c
	0x07,0x20,																		// Product ID  |  PID = 0X2007 /// 2105
//	0x00,0x01,
//	0x05,0x21,

	0x00,0x11,																		// bcdDevice		
	0x00,0x00,
	0x00,0x01
};
/**************************** HID Report Descriptor *********************************/
__code uint8_t KeyRepDesc[65] = 															// Report Descriptor, DELL Keyboard
{
	0x05, 0x01, 		// Usage page Generatic Desktop
	0x09, 0x06, 		// Usage keyboard
	0xa1, 0x01, 		// Collation Application
	0x05, 0x07, 		// Usafe page (key code)
	0x19, 0xe0, 		// Usage Min ( E0 -->  L_CTL)
	0x29, 0xe7, 		// Usage MAX ( E7 --> R_GUI )
	0x15, 0x00, 		// Logical min
	0x25, 0x01, 		// Logical max
	0x95, 0x08, 		// Report count ( 8 )
	0x75, 0x01, 		// Report size	( 1 )
	0x81, 0x02, 		// Input ( Data, Variable, Absolute )
	0x95, 0x08, 		// Report count ( 8 )
	0x75, 0x01, 		// Report size	( 1 )
	0x81, 0x01, 		// Input ( const )
	0x05, 0x08, 		// Usage page( LED )
	0x19, 0x01, 		// Usage min ( 1 )
	0x29, 0x03, 		// Usage max ( 3 )
	0x95, 0x03, 		// Report count ( 3 )
	0x75, 0x01, 		// Report size ( 1 )
	0x91, 0x02, 		// Output ( Data, Variable, Absolute )
	0x95, 0x01, 		// Report count ( 1 )
	0x75, 0x05, 		// Report size ( 5 )
	0x91, 0x01, 		// Output ( const )
	0x05, 0x07, 		// Usage page ( key code )
	0x19, 0x00, 		// Usage min ( 0H )
	0x2a, 0xff, 0x00,	// Usage max ( FFH )
	0x15, 0x00,			// Logical min ( 0H )
	0x26, 0xff, 0x00,	// Logical max ( FFH )
	0x95, 0x06, 		// Report count ( 6 )
	0x75, 0x08, 		// Report size ( 8 )
	0x81, 0x00, 		// Input ( Data, Array, Absolute )
	0xc0				// End collection
};

__code uint8_t ComRepDesc[34] = 															// Report Descriptor, Composite device
{
	0x06, 0x00, 0xff, 	// Usage page Vendor defined
	0x09, 0x01, 		// Usage keyboard
	0xa1, 0x01, 		// Collation Application
	0x09, 0x02, 		// Mouse
	0x15, 0x00, 		// Logical min ( 0H )
	0x26, 0x00, 0xff,	// Logical max ( FFH )
	0x75, 0x08,  		// Report size ( 08H )
	0x95, 0x40, 		// Report count ( 40H )
	0x81, 0x06,  		// Input ( Data, Relative, Wrap )
	0x09, 0x02, 		// Mouse
	0x15, 0x00,  		// Logical min ( 0H )
	0x26, 0x00, 0xff,	// Logical max ( FFH )
	0x75, 0x08, 		// Report size ( 08H )
	0x95, 0x40, 		// Report count ( 40H )
	0x91, 0x06, 		// Output ( Data, Relative, Wrap )
	0xc0,
};

// ����������

__code uint8_t CfgDesc[9+9+9+7+9+9+7+7] =
{
    0x09,0x02,0x42,0x00,0x02,0x01,0x00,0xA0,0x32,             						// Configure Descriptor
//   blen type  tlen     interface   
//	0x09,0x02,0x22,0x00,0x01,0x01,0x00,0xA0,0x32,

	0x09,0x04,0x00,0x00,0x01,0x03,0x01,0x01,0x00,             						// Interface Descriptor, keyboard
    0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(KeyRepDesc),0x00,             		// HID Descriptor
    0x07,0x05,0x81,0x03,0x08,0x00,0x0a,                       						// Endpoint Descriptor, 50ms

//  0x09,0x02,0x29,0x00,0x01,0x01,0x00,0xA0,0x32,             						// Configure Descriptor	

//  0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x00,             						// Interface Descriptor, Composite device�� 2 EPs
	0x09,0x04,0x01,0x00,0x02,0x03,0x00,0x00,0x00,             						// Interface Descriptor, Composite device�� 2 EPs
    0x09,0x21,0x10,0x01,0x21,0x01,0X22,sizeof(ComRepDesc),0x00,             		// HID Descriptor
    0x07,0x05,0x82,0x03,0x40,0x00,0x32,                        						// Endpoint Descriptor, 10ms, IN
	0x07,0x05,0x02,0x03,0x40,0x00,0x0a												// Endpoint Descriptor,	10ms, OUT
};

// ����������
__code uint8_t	MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// ������Ϣ
__code uint8_t	MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };
// ��Ʒ��Ϣ
__code uint8_t	MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '5', 0, '4', 0 };

/*******************************************************************************
* Function Name  : USBDeviceInit()
* Description    : Configure USB mode ��USB device init configure.Configure tie Endpoint, compound device, 
				           Endpoint 0 control trans, Endpoint 1/2 interrupt(IN).
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceInit()
{
	IE_USB = 0;
	USB_CTRL = 0x00;                                                           // Device mode
	
    UEP1_DMA = (uint16_t)Ep1Buffer;                                                      // Endpoint 1 uploard address
    UEP0_DMA = (uint16_t)Ep0Buffer;                                                      // Endpoint 0 trans address
    UEP2_DMA = (uint16_t)Ep2Buffer;                                                      // Endpoint 2 uploard address	
	
    UEP4_1_MOD = UEP4_1_MOD | bUEP1_TX_EN & ~(bUEP1_RX_EN | bUEP1_BUF_MOD | bUEP4_RX_EN | bUEP4_TX_EN); // Endpoint 1 sigle 64 byte send buffer, Endpoint 4 disable
    UEP1_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;                                 // Endpoint 1 auto Toggle flag, In translation return NAK
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;                                 // Endpoint 0 OUT trans return ACK, IN trans return NAK    

    UEP2_3_MOD = UEP2_3_MOD | bUEP2_TX_EN | bUEP2_RX_EN & ~bUEP2_BUF_MOD;      // Endpoint 2 sigle 64 byte send buffer OUT[64]+IN[64] (OUT first)
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_T_RES_NAK;                                 // Endpoint 2 auto Toggle flag, IN trans return NAK
		
	USB_DEV_AD = 0x00;                                                         // Initial the device address
	UDEV_CTRL = bUD_PD_DIS;                                                    // Disable DM/PM PULL_DOWN.
	USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                      // Enable inter PULL_UP. Auto return NAK, during interrupt routing, if the interrupt flag.
	UDEV_CTRL |= bUD_PORT_EN;                                                  // Enable USB port.
	USB_INT_FG = 0xFF;                                                         // Clear interrupt flag.
	USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;                  // Enable USB interrupt
	IE_USB = 1;                                                                // Enable USB interrupt
}
/*******************************************************************************
* Function Name  : Enp1IntIn()
* Description    : USB Device mode Endpoint 1 upload data, Interrupt mode
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Enp1IntIn( void )
{			
    memcpy( Ep1Buffer, HIDKey, sizeof(HIDKey));                              			// Upload data
    UEP1_T_LEN = sizeof(HIDKey);                                             			// Upload length
    UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                			// Return ACK
    while(( UEP1_CTRL & MASK_UEP_T_RES ) == UEP_T_RES_ACK);                  			// Waiting upload complete, avoid overwriting	
}
/*******************************************************************************
* Function Name  : Enp2TX()
* Description    : USB Device mode Endpoint 2 upload data, Interrupt mode 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Enp2TX( void )
{	
    memcpy( &Ep2Buffer[64], TX, sizeof(TX));                         					// Upload data
    UEP2_T_LEN = sizeof(TX);                                           					// Upload length
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                			// Return ACK
    while(( UEP2_CTRL & MASK_UEP_T_RES ) == UEP_T_RES_ACK);                  			// Waiting upload complete, avoid overwriting	
}			

/*******************************************************************************
* Function Name  : Enp2TX()
* Description    : USB Device mode Endpoint 2 upload data, Interrupt mode 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void Enp2RX()
{
	uint8_t len;	
	len = USB_RX_LEN; 
	if(len>3)
	{
		memcpy(RX,Ep2Buffer, sizeof(RX)); 
		memcpy(TX,RX, sizeof(RX)); 
		RXflag = len;
		Enp2TX();
	}
	//UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_R_RES | UEP_R_RES_NAK;                 
	
}
/*******************************************************************************
* Function Name  : Enp2TX()
* Description    : USB Device mode Endpoint 2 upload data, Interrupt mode 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void USB_Setup()
{
              
	
}
/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : USB ISR
*******************************************************************************/
void DeviceInterrupt( void ) __interrupt (INT_NO_USB)                      			//USB ISR, Using register 1
{
    uint8_t len;			
	
    if( UIF_TRANSFER )                                                           		//USB trans flag
    {	
		switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP) )			
        {
			case UIS_TOKEN_IN | 1:                                                  		// Endpoint 1 upload
			{
				UEP1_T_LEN = 0;                                                     		// Reset length
				UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           		// Default return NAK
				FLAG = 1;                                                           		// Upload complete flag
				break;	
			}	 			
			case UIS_TOKEN_IN | 2:                                                 			// Endpoint 2 upload
			{
				UEP2_T_LEN = 0;                                                     		// Reset length
				UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           		// Default return NAK
				break;                                                                         
			}
			 case UIS_TOKEN_OUT | 2:                                                 		// Eendpoint 2 download 
			{
				Enp2RX();
				UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_R_RES | UEP_T_RES_NAK;           		// Default return NAK
				break;	 
			}				
			case UIS_TOKEN_SETUP | 0: // SETUP packet
			{		
				len = USB_RX_LEN;
				if( len == (sizeof(USB_SETUP_REQ)) )
				{
					SetupLen = UsbSetupBuf->wLengthL;
					if(UsbSetupBuf->wLengthH || SetupLen > 0x7F )
					{
						SetupLen = 0x7F;    												// Total length less than 0X7F
					}
					len = 0;                                                         		// Default length is 0
					if( (UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD )    /* �Ǳ�׼���� */
					{
						if( (UsbSetupBuf->bRequestType == 0x21)&&(UsbSetupBuf->bRequest == HID_SET_REPORT) )//Init ok
						{			
							EnumOK = 1;														// Class require, SET REPORT, indicate the enumerate complete											
						}
						if( (UsbSetupBuf->bRequestType == 0x21)&&(UsbSetupBuf->bRequest == HID_SET_IDLE) )
						{					
							UEP0_T_LEN = 0;                                                 //Status stage complete, upload 0 data packet, end the control trans
		//					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;
						}
						len = 0;                                                  		// Fail
					}
					else
                    {																							// Deal with the standard require
						SetupReq = UsbSetupBuf->bRequest;
						switch( SetupReq )                                                  // Require code
						{
						case USB_GET_DESCRIPTOR:
                        {
							switch( UsbSetupBuf->wValueH )
							{
                                case 1:                                                       	// Device descriptor// �豸������				
                                    pDescr = DevDesc;                                         	// Write device descriptor to buffer
                                    len = sizeof( DevDesc );	
                                    break;	
                                case 2:                                                       	// Configuration descriptor	 // ����������				
                                    pDescr = CfgDesc;                                         	// Write configuration descriptor to buffer
                                    len = sizeof( CfgDesc );	
                                    break;
                                case 3:                                          // �ַ���������
                                {
                                    switch( UsbSetupBuf->wValueL ) 
                                    {
                                        case 1:
                                            pDescr = (uint8_t *)( &MyManuInfo[0] );
                                            len = sizeof( MyManuInfo );
                                            break;
                                        case 2:
                                            pDescr = (uint8_t *)( &MyProdInfo[0] );
                                            len = sizeof( MyProdInfo );
                                            break;
                                        case 0:
                                            pDescr = (uint8_t *)( &MyLangDescr[0] );
                                            len = sizeof( MyLangDescr );
                                            break;
                                        default:
                                            len = 0xFF;                               // ��֧�ֵ��ַ���������
                                            break;
                                    }
                                    break;
                                }
                                case 0x22:                                                    	// HID report descriptor						
                                {
                                    if( UsbSetupBuf->wIndexL == 0 )                           	// Interface index 0 HID report --> keyboard
                                    {						
                                        pDescr = KeyRepDesc;                                  	// Write to buffer
                                        len = sizeof( KeyRepDesc );	
                                    }
        //							if( UsbSetupBuf->wIndexL == 0 )                       	// Interface index 1 HID report --> Composite device
        //							{								
        //								pDescr = ComRepDesc;                              	// Write to buffer
        //								len = sizeof( ComRepDesc );
        //							}                                
                                    else if(UsbSetupBuf->wIndexL == 1)                       	// Interface index 1 HID report --> Composite device
                                    {								
                                        pDescr = ComRepDesc;                              	    // Write to buffer
                                        len = sizeof( ComRepDesc );
                                    }
                                    else
                                    {
                                        len = 0xff;                                         // Add interface index, if required
                                    }                              
                                    break;
                                }
                                default:
                                    len = 0xff;                                               	// Length equal 0 if code error
                                    break;	
							}
                            
							if ( len == 0xff ){	
								break;                                                    	// Error code, jump out the swith sentence
							}											
							if ( SetupLen > len ){	
								SetupLen = len;                                           	// Limit the total length 
							}	
							len = SetupLen >= 8 ? 8 : SetupLen;                           	// Upload this length
							memcpy( Ep0Buffer, pDescr, len );                             	// Upload data
							SetupLen -= len;	
							pDescr += len;	
							break;	
                        }	
						case USB_SET_ADDRESS:	
							SetupLen = UsbSetupBuf->wValueL;                              	// Save the device address					
							break;
						case USB_GET_CONFIGURATION:
                        {
							Ep0Buffer[0] = UsbConfig;
							if ( SetupLen >= 1 )
							{
								len = 1;
							}					
							break;
                        }
						case USB_SET_CONFIGURATION:
                        {
							UsbConfig = UsbSetupBuf->wValueL;				
							break;
                        }
						case 0x0A:
							break;
						case USB_CLEAR_FEATURE:                                            	//Clear Feature
                        {
							if ( (UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP ) 
							{
							   switch( UsbSetupBuf->wIndexL )
							   {
								  case 0x82:
									   UEP2_CTRL = UEP2_CTRL & ~ (bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
									   break;
								  case 0x81:
									   UEP1_CTRL = UEP1_CTRL & ~ (bUEP_T_TOG | MASK_UEP_T_RES) | UEP_T_RES_NAK;
									   break;
								  case 0x01:
									   UEP1_CTRL = UEP1_CTRL & ~ (bUEP_R_TOG | MASK_UEP_R_RES) | UEP_R_RES_ACK;
									   break;
								  default:
									   len = 0xFF;                                         	// Error endpoint
									   break;
								}
							 }
							else
							{
								len = 0xFF;                                                	// Deal with the error endpoint
							}
							break;
                        }
						case USB_SET_FEATURE:                                          		// Set Feature 					
						{
                            if( (UsbSetupBuf->bRequestType & 0x1F) == 0x00 )               
							{
								if( ( ((uint16_t)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL ) == 0x01 )
								{
									if( CfgDesc[ 7 ] & 0x20 )
									{
										 /* Awake the device */
									}
									else
									{
										len = 0xFF;                                        	// Error routing
									}
								}
								else
								{
									len = 0xFF;                                            	// Error routing
								}
							}
							else 
                            {
								if( (UsbSetupBuf->bRequestType & 0x1F) == 0x02 )            // Set endpoint
								{
									if( ( ((uint16_t)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL ) == 0x00 )
									{
										switch( ((uint16_t)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL )
										{
											case 0x82:
												UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* Endpoint 2 IN STALL */
												break;

											case 0x02:
												UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* Endpoint 2 OUT STALL */
												break;

											case 0x81:
												UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* Endpoint 1 IN STALL */
												break;

											default:
												len = 0xFF;                                   	// Error routing
												break;
										}
									}
									else
									{
										len = 0xFF;                                      		// Error routing
									}
								}
								else
								{
									len = 0xFF;                                          		// Error routing
								}
                            }
                            break;
                        }
						case USB_GET_STATUS:													// Get status
						{						
							Ep0Buffer[0] = 0x00;
							Ep0Buffer[1] = 0x00;
							if ( SetupLen >= 2 )
							{
								len = 2;
							}
							else
							{
                                len = SetupLen;
							}
							 break;
						}
						default:
							len = 0xff;                                                    		// Error routing
							break;
						}
					}
				}
				else
				{
					len = 0xff;                                                         		//Packet length error
				}
				if(len == 0xff)
				{
					SetupReq = 0xFF;
	 //               UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;	// Control endpoint should not return STALL, 
																								// if Send STALL, that means the device has no ability to communicate with the host
				}
				else if(len <= 8)                                                       		// Upload the data or return the 0 data packet
				{		
					UEP0_T_LEN = len;		
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;		// Default data packet is DATA1, and return ACK
				}		
				else		
				{		
					UEP0_T_LEN = 0;  															// Upload 0 data packet, incase the host go to status stage ahead, and get fault code
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;		// Default data packet is DATA1, and return ACK
				}		
				break;
			}			
			case UIS_TOKEN_IN | 0:   // Endpoint 0 IN
			{		
				switch(SetupReq)
				{
				case USB_GET_DESCRIPTOR:
					len = SetupLen >= 8 ? 8 : SetupLen;                                 		// Current length
					memcpy( Ep0Buffer, pDescr, len );                                   		// Upload the data
					SetupLen -= len;
					pDescr += len;
					UEP0_T_LEN = len;
					UEP0_CTRL ^= bUEP_T_TOG;                                           			//Toggle Sysc flag 
					break;
				case USB_SET_ADDRESS:
					USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
					UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
					break;
				default:
	//                UEP0_T_LEN = 0;                                                     		//Status stage complete, upload 0 data packet, end the control trans
	//                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;		
					break;
				}
				if(len == 0xff)
				{
					UEP0_T_LEN = 0;                                                     		//Status stage complete, upload 0 data packet, end the control trans
					UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;		
				}
				break;
			}
			case UIS_TOKEN_OUT | 0:  															// Endpoint 0 OUT
			{
				len = USB_RX_LEN;
				if( SetupReq == 0x09 )
				{
					if( Ep0Buffer[0] )
					{

					}
					else if( Ep0Buffer[0] == 0)
					{

					}
				}
				UEP0_T_LEN = 0;  															// Upload 0 data packet, incase the host go to status stage ahead, and get fault code
				UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_ACK;									// Default data packet is DATA1, and return ACK
				break;
			}
			default:
				break;
		}
        UIF_TRANSFER = 0;                                                           	// Reset the trans interrupt
    }// end if( UIF_TRANSFER )  
    if( UIF_BUS_RST )                                                                	// Device mode Bus reset
    {
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP1_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK;
		UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
		USB_DEV_AD = 0x00;
		UIF_SUSPEND = 0;
		UIF_TRANSFER = 0;
		UIF_BUS_RST = 0;                                                            		// Reset Reset interrupt
    }		
    if( UIF_SUSPEND )                                                               	// USB Suspended and Awake interrupt
    {		
        UIF_SUSPEND = 0;		
        if ( USB_MIS_ST & bUMS_SUSPEND )                                            		// Suspended
        {		
            while ( XBUS_AUX & bUART0_TX );  												// Waiting send complete ...		
            SAFE_MOD = 0x55;		
            SAFE_MOD = 0xAA;		
            WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                               			// Action on USB Bus, or RXD0, will awake the device
            PCON |= PD;                                                           			// LPM
            SAFE_MOD = 0x55;		
            SAFE_MOD = 0xAA;		
            WAKE_CTRL = 0x00;		
        }		
    }		
    else 
	{                                                                         			// Unexpected interrupt
        USB_INT_FG = 0xFF;                                                         		// Reset the interrupt
    }
}

/*******************************************************************************
* Function Name  : static SendKey( char *p)
* Description    : ASCII to key code
* Input          : char *p
* Output         : None
* Return         : None
*******************************************************************************/

static void SendKey( char *p )
{
	char c = *p;		
	if( (c >= 'a') && (c <= 'z' ))
	{
		c = c - 'a' + 'A';
	}	
	if( (c >= 'A') && (c <= 'Z' ))
	{
		HIDKey[2] = c - 'A' + 4;
	}
	else
	{
		if( c >= '1' && c <= '9' )	HIDKey[2] = c - '1' + 0X1E;
		else
		{
			switch ( c )
			{
				case '`' :
					HIDKey[0] = 0X08;
					HIDKey[2] = 0X15;
					break;
				case '\\':
					HIDKey[2] = 0x31;
					break;
				case ' ':
					HIDKey[2] = L_SHIFT;
					break;
				case '\r':
					HIDKey[2] = ENTER;
					break;
				case ':':
					HIDKey[0] = 0x02;
					HIDKey[2] = 0x33;
					break;
				case '+':
					HIDKey[0] = 0x000;
					HIDKey[2] = 0x57;
					break;
				case '_':
					HIDKey[0] = 0X02;
					HIDKey[2] = 0X2D;
					break;
				case '/':
					HIDKey[0] = L_CTL + L_ALT;
					HIDKey[2] = 0X16;
					break;
				case '0':
					HIDKey[2] = 0X27;
					break;
				case '.':
					HIDKey[2] = 0X37;
					break;
				case '~':
					HIDKey[0] = L_ALT;
					HIDKey[2] = 0X05;
					break;
				case '!':
					HIDKey[0] = L_ALT;
					HIDKey[2] = 0X08;
					break;
				default:
					break;
			}
		}
	}
	
	mDelaymS( 20 );																				//emulate the press down and pop up
	while(FLAG == 0);                                           								// Upload the key
	Enp1IntIn();						
	while(FLAG == 0);   																					
	mDelaymS( 20 );
	HIDKey[0] = 0X00;     						
	HIDKey[2] = 0X00;                                              								// Reset the key
	while(FLAG == 0);                                           						
	Enp1IntIn();			
	while(FLAG == 0); 
}


/*******************************************************************************
* Function Name  : extern HIDValueHandle( void )
* Description    : Upload the HID code
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

char __code sPath[] = "`   E:\\vck1\\hid_recv.exe    \r   ";						    // The path for the software. SPACE for delay
char *pStr = sPath;

char __code s1Path[] = "`   \r  ";						
char *p1Str = s1Path;

void TSTKey(void)
{
	uint16_t KeyData;
	if(TKEY_CTRL&bTKC_IF)		                                  
	{
		KeyData = TKEY_DAT; 
		if (KeyData < (TouchKeyButton-100))	        //100 �����ȵ���	
		{
			TXflag =0x04 ;
		}
#ifdef DE_PRINTF
     printf("B.=  %04x\n",KeyData&0x7FFF);		
#endif		
	}
}

void TXflagHandle(uint8_t val)
{	
	TSTKey();
	if(TXflag & 0X01)
	{				
		SendKey(pStr);																	    // Upload path
		pStr++;	
		if(*pStr == '\0')
		{
			SendKey( "~" );																	// Upload ALT+B
			mDelaymS( 200 );	
			TXflag &= 0xFE;
		}	
	}	
	else if(TXflag & 0X02)
	{				
		SendKey(p1Str);																	    // Upload path
		pStr++;	
		if(*p1Str == '\0')
		{
			SendKey( "~" );																	// Upload ALT+B
			mDelaymS( 200 );	
			TXflag &= 0xFE;
		}
	}
	else if(TXflag & 0x04)	    					
	{
		char btn[3];
		btn[0]= val+ 0x30;
		//SendKey(btn);
		SendKey("` ");
		//TX[0] = val;	
		//Enp2TX();
		TXflag &= 0xFB;		
	}
	else if(TXflag & 0x08)	    					
	{
		SendKey("A");
		//TX[0] = val;	
		//Enp2TX();
		TXflag &= 0xFB;		
	}
}

/**************************** END *************************************/
