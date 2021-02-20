
#define INTERRUPT_TouchKey   0   //����Key�жϷ�ʽ


#define		L_WIN 					0X08
#define 	L_ALT 					0X04
#define		L_SHIFT					0X02
#define 	L_CTL					0X01
#define 	R_WIN 					0X80
#define 	R_ALT 					0X40
#define 	R_SHIFT					0X20
#define 	R_CTL					0X10
#define 	SPACE					0X2C
#define		ENTER					0X28

#define MOUSE 0


/*******************************************************************************
* Function Name  : TouchKeyQueryCylSet(uint8_t cyl)
* Description    : ����������ѯ��������
* Input          : uint8_t cyl ʱ������                  								
                   1   2ms
                   0   1ms									 
* Output         : None 
* Return         : None
*******************************************************************************/
void TouchKeyQueryCylSet1Or2ms(uint8_t cyl);

/*******************************************************************************
* Function Name  : TouchKeyChanelSelect(uint8_t ch)
* Description    : ��������ͨ��ѡ��
* Input          : uint8_t ch ����ͨ��
* Output         : None
* Return         : �ɹ� SUCCESS
                   ʧ�� FAIL
*******************************************************************************/
uint8_t TouchKeyChanelSelect(uint8_t ch);

