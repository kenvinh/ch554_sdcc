
/********************************** (C) COPYRIGHT *******************************
* File Name          : Touch_Key.C
* Author             : WCH
* Version            : V1.0
* Date               : 2017/01/20
* Description        : CH554 ������������������á�ͨ��ѡ����л����жϴ�������   
*******************************************************************************/

#include "ch554.h"                                                       
#include "debug.h"
#include "Key.h"
#include <stdio.h>

// #pragma  NOAREGS

/*******************************************************************************
* Function Name  : TouchKeyQueryCylSet1Or2ms(uint8_t cyl)
* Description    : ����������ѯ��������
* Input          : uint8_t cyl ʱ������                  								
                   1   2ms
                   0   1ms									 
* Output         : None
* Return         : None
*******************************************************************************/
void TouchKeyQueryCylSet1Or2ms(uint8_t cyl)
{
  if(cyl)
  {
    TKEY_CTRL |= bTKC_2MS;
  }
  else
  {
    TKEY_CTRL &= ~bTKC_2MS;
  }
}

/*******************************************************************************
* Function Name  : TouchKeyChanelSelect(uint8_t ch)
* Description    : ��������ͨ��ѡ��
* Input          : uint8_t ch ����ͨ��
* Output         : None
* Return         : �ɹ� SUCCESS
                   ʧ�� FAIL
*******************************************************************************/
uint8_t TouchKeyChanelSelect(uint8_t ch)
{
    TKEY_CTRL &= 0xF8;
    if(ch == 0){;}                                                            //�رյ��ݼ�⣬����1ms����2ms��ʱ�ж�
    else if(ch == 1){P1_DIR_PU &= 0xFE; TKEY_CTRL |= 0x01;}                   //TIN0(P10)
    else if(ch == 2){P1_DIR_PU &= 0xFD; TKEY_CTRL |= 0x02;}                   //TIN0(P11)
    else if(ch == 3){P1_DIR_PU &= 0xEF; TKEY_CTRL |= 0x03;}                   //TIN0(P14)
    else if(ch == 4){P1_DIR_PU &= 0xDF; TKEY_CTRL |= 0x04;}                   //TIN0(P15)
    else if(ch == 5){P1_DIR_PU &= 0xBF; TKEY_CTRL |= 0x05;}                   //TIN0(P16)
    else if(ch == 6){P1_DIR_PU &= 0x7F; TKEY_CTRL |= 0x06;}                   //TIN0(P17)
    else if(ch == 7){TKEY_CTRL |= 0x07;}                                      //�������ݼ�⣬���ǲ�����ͨ��
    else return FAIL;
		
#if INTERRUPT_TouchKey
    IE_TKEY = 1;                                                               //ʹ��Touch_Key�ж�
#endif
    return SUCCESS;
}

#if INTERRUPT_TouchKey
/*******************************************************************************
* Function Name  : Touch_KeyInterrupt(void)
* Description    : Touch_Key �жϷ������
*******************************************************************************/
void	Touch_KeyInterrupt( void ) interrupt INT_NO_TKEY using 1                //Touch_Key�жϷ������,ʹ�üĴ�����1
{ 
    UINT16 KeyData;
    IE_TKEY = 0;                                                              //���ж�	
    KeyData = TKEY_DAT;                                                       //����87us,����ȡ��
#ifdef DE_PRINTF
      printf("DATA: %04x\n",KeyData&0x7FFF);
#endif
}
#endif

