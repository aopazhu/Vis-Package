#include "at24cxx.h" 
#include "delay.h"
/***************************************************************************
- ����������STM32f103 EEPORM��ʼ������
- ����ģ�飺STM32 EEPROM����
- �������ԣ��ⲿ��ʹ�û�ʹ��
- ����˵������
- ����˵������
- �������ܣ�ʵ��I2C�ĳ�ʼ����
***************************************************************************/
void AT24CXX_Init(void)
{
//	RCC->APB2ENR|=3<<2;     //ʹ��PORT A Bʱ��
// 	GPIOA->CRH|=0X30000000;		//PA15���������
// 	GPIOA->ODR&=~0X00008000;
// 	GPIOB->CRL|=0X00333000;		//PB3 4 5���������
// 	GPIOB->ODR&=~0X00000038; 
		
	IIC_Init();
}
/***************************************************************************
- ����������STM32f103 �����ȡEEPORMһ���ֽڵĺ���
- ����ģ�飺STM32 EEPROM����
- �������ԣ��ⲿ��ʹ�û�ʹ��
- ����˵����ReadAddrΪ����ȡEEPROM�ֽڵĵ�ַ
- ����˵������
- �������ܣ�������Ҫ��ȡ�ĵ�ַ������
- ����˵����Ҫ�����ȡEEPROM��ַ�е�һ���ֽڵĲ���Ϊ:
(1)���Ϳ�ʼSTART�ź�
(2)��������������ַ�����һλΪ0
(3)�ӻ���⵽�������͵ĵ�ַ���Լ��ĵ�ַ��ͬʱ����һ��Ӧ���ź�ACK���������ȴ�ACK
(4)���ʹ��������ֽڵ�ַ
(5)�ȴ�һ��ACK
(6)���·���START��ʼ�ź�
(7)����7λ������ַ�����һλΪ1����ʾ��EEPROM��
(8)�ȴ�ACK����ȡһ���ֽڡ�
(9)I2Cֹͣ�����ض�ȡ�����ݡ�
����ע��AT24C0xϵ�еĵ�ַ�����ֽں�һ�ֽ�֮�֣���������������������
***************************************************************************/
u8 AT24CXX_ReadOneByte(u16 ReadAddr) 
{ 
	u8 temp=0; 
	IIC_Start(); 
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XAA); //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(ReadAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack(); 
	}
	else 
		IIC_Send_Byte(0XA0+((ReadAddr/256)<<1)); //����������ַ0XA0,д����

	IIC_Wait_Ack();

	IIC_Send_Byte(ReadAddr%256); //���͵͵�ַ
	IIC_Wait_Ack(); 
	IIC_Start(); 
	IIC_Send_Byte(0XA1); //�������ģʽ 
	IIC_Wait_Ack(); 
	temp=IIC_Read_Byte(0); 
	IIC_Stop();//����һ��ֹͣ���� 
	return temp;
}

/***************************************************************************
- ����������STM32f103 ����дEEPORMһ���ֽڵĺ���
- ����ģ�飺STM32 EEPROM����
- �������ԣ��ⲿ��ʹ�û�ʹ��
- ����˵����WriteAddr :д�����ݵ�Ŀ�ĵ�ַ ��DataToWrite:Ҫд�������
- ����˵������
- �������ܣ�������Ҫ��ȡ�ĵ�ַ������
- ����˵����Ҫ�����ȡEEPROM��ַ�е�һ���ֽڵĲ���Ϊ:
(1)���Ϳ�ʼSTART�ź�
(2)��������������ַ�����һλΪ0
(3)�ӻ���⵽�������͵ĵ�ַ���Լ��ĵ�ַ��ͬʱ����һ��Ӧ���ź�ACK
(4)���ʹ��������ֽڵ�ַ
(5)�ȴ�һ��ACK
(6)�����ֽ�����
(7)�ȴ�һ��ACK
(8)I2Cֹͣ�����ض�ȡ�����ݡ�
***************************************************************************/
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite)
{ 
	IIC_Start(); 
	if(EE_TYPE>AT24C16)
	{
		IIC_Send_Byte(0XA0); //����д����
		IIC_Wait_Ack();
		IIC_Send_Byte(WriteAddr>>8);//���͸ߵ�ַ
		IIC_Wait_Ack(); 
	}
	else
	{
		IIC_Send_Byte(0XA0+((WriteAddr/256)<<1)); //����������ַ0XA0,д���� 
	} 
	IIC_Wait_Ack(); 
	IIC_Send_Byte(WriteAddr%256); //���͵͵�ַ
	IIC_Wait_Ack(); 
	IIC_Send_Byte(DataToWrite); //�����ֽ� 
	IIC_Wait_Ack(); 
	IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10); 
}


/***************************************************************************
- ����������STM32f103 ����дEEPORMָ����ַ��ʼд�볤��ΪLen������
- ����ģ�飺STM32 EEPROM����
- �������ԣ��ⲿ��ʹ�û�ʹ��
- ����˵����WriteAddr :д�����ݵ�Ŀ�ĵ�ַ ��DataToWrite:���������׵�ַ
Len:Ҫд�����ݵĳ���2,4
- ����˵������
- �������ܣ���
- ����˵�������ǵ���len�� AT24CXX_WriteOneByte�������ú�������д��16bit����32bit������.
***************************************************************************/
void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len)
{ 
	u8 t;
	for(t=0;t<Len;t++)
	{
		AT24CXX_WriteOneByte(WriteAddr+t,(DataToWrite>>(8*t))&0xff);
	} 
}
/***************************************************************************
- ����������STM32f103 ����дEEPORMָ����ַ��ʼ��������ΪLen������
- ����ģ�飺STM32 EEPROM����
- �������ԣ��ⲿ��ʹ�û�ʹ��
- ����˵����ReadAddr :��ʼ�����ĵ�ַ
Len:Ҫд�����ݵĳ���2,4
- ����˵������
- �������ܣ���
- ����˵�������ǵ���len��AT24CXX_ReadOneByte�������ú������ڶ���16bit����32bit������.
***************************************************************************/
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len)
{ 
	u8 t;
	u32 temp=0;
	for(t=0;t<Len;t++)
	{
		temp<<=8;
		temp+=AT24CXX_ReadOneByte(ReadAddr+Len-t-1); 
	}
	return temp; 
}
/***************************************************************************
- ����������STM32f103���AT24CXX�Ƿ�����
- ����ģ�飺STM32 EEPROM����
- �������ԣ��ⲿ��ʹ�û�ʹ��
- ����˵������
- ����˵��������1:���ʧ�� ����0:���ɹ�
- �������ܣ���
- ����˵������������24XX�����һ����ַ(127)���洢��־��. ����0��ʾ����������1����
***************************************************************************/
u8 AT24CXX_Check(void)
{
	u8 temp;
	temp=AT24CXX_ReadOneByte(Adress_FEW0);//����ÿ�ο�����дAT24CXX 
	if(temp==0XAA)
		return 0; 
	else//�ų���һ�γ�ʼ�������
	{
		AT24CXX_WriteOneByte(Adress_FEW0,0XAA);
// 		temp=AT24CXX_ReadOneByte(127); 
// 		if(temp==0X55)
// 			return 0;
	}
	return 1; 
}

/***************************************************************************
- ����������STM32f103��AT24CXX�����ָ����ַ��ʼ����ָ������������
- ����ģ�飺STM32 EEPROM����
- �������ԣ��ⲿ��ʹ�û�ʹ��
- ����˵����ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
pBuffer :���������׵�ַ
NumToRead:Ҫ�������ݵĸ���
- ����˵������
- �������ܣ���
***************************************************************************/
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead)
{
	while(NumToRead)
	{
		*pBuffer++=AT24CXX_ReadOneByte(ReadAddr++); 
		NumToRead--;
	}
}

/***************************************************************************
- ����������STM32f103��AT24CXX�����ָ����ַ��ʼд��ָ������������
- ����ģ�飺STM32 EEPROM����
- �������ԣ��ⲿ��ʹ�û�ʹ��
- ����˵����ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
pBuffer :���������׵�ַ�������������ݴ洢����pBuffer��
NumToRead:Ҫ�������ݵĸ���
- ����˵������
- �������ܣ���
***************************************************************************/

void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite)
{
	while(NumToWrite--)
	{
		AT24CXX_WriteOneByte(WriteAddr,*pBuffer);
		WriteAddr++;
		pBuffer++;
	}
}


////////////////////////////////////////////////////////////////
void Save_Target(float	Target_Data,	u8 Target_Name,u8 offset)
{
	u32	Target;
	switch(Target_Name)
	{
			case	Zero:
					Target=(u32)(Target_Data*100);
					AT24CXX_WriteLenByte((Adress_Zero+(offset*Adress_offset)),Target,Adress_offset);
					break;
			case	Mod0:
					Target=(u32)(Target_Data*1000);
					AT24CXX_WriteLenByte((Adress_Mod0+(offset*Adress_offset)),Target,Adress_offset);
					break;
			case	Mod1:
					Target=(u32)(Target_Data*1000);
					AT24CXX_WriteLenByte((Adress_Mod1+(offset*Adress_offset)),Target,Adress_offset);
					break;
			case	Mod2:
					Target=(u32)(Target_Data*1000);
					AT24CXX_WriteLenByte((Adress_Mod2+(offset*Adress_offset)),Target,Adress_offset);
					break;
			case	Mod3:
					Target=(u32)(Target_Data*1000);
					AT24CXX_WriteLenByte((Adress_Mod3+(offset*Adress_offset)),Target,Adress_offset);
					break;
			case	Mod4:
					Target=(u32)(Target_Data*1000);
					AT24CXX_WriteLenByte((Adress_Mod4+(offset*Adress_offset)),Target,Adress_offset);
					break;
			case	Mod5:
					Target=(u32)(Target_Data*1000);
					AT24CXX_WriteLenByte((Adress_Mod5+(offset*Adress_offset)),Target,Adress_offset);
					break;
			case	Mod6:
					Target=(u32)(Target_Data*1000);
					AT24CXX_WriteLenByte((Adress_Mod6+(offset*Adress_offset)),Target,Adress_offset);
					break;
			default:break;
	}
		
}

//////////////////////////////////////////////////////////////////////////
float Read_Target(u8 Target_Name,u8 offset)
{
	float	Target_Data=0;
	u32	Target;
	switch(Target_Name)
	{
			case	Zero:
					Target=AT24CXX_ReadLenByte((Adress_Zero+(offset*Adress_offset)),Adress_offset);
					Target_Data=(float)Target/100;
					break;
			case	Mod0:
					Target=AT24CXX_ReadLenByte((Adress_Mod0+(offset*Adress_offset)),Adress_offset);
					Target_Data=(float)Target/1000;
					if((Target_Data>2)||(Target_Data<0.5))
					{
						Target=1000;
						Target_Data=1;
						AT24CXX_WriteLenByte((Adress_Mod0+(offset*Adress_offset)),Target,Adress_offset);
					}
					break;
			case	Mod1:
					Target=AT24CXX_ReadLenByte((Adress_Mod1+(offset*Adress_offset)),Adress_offset);
					Target_Data=(float)Target/1000;
					if((Target_Data>2)||(Target_Data<0.5))
					{
						Target=1000;
						Target_Data=1;
						AT24CXX_WriteLenByte((Adress_Mod1+(offset*Adress_offset)),Target,Adress_offset);
					}
					break;
			case	Mod2:
					Target=AT24CXX_ReadLenByte((Adress_Mod2+(offset*Adress_offset)),Adress_offset);
					Target_Data=(float)Target/1000;
					if((Target_Data>2)||(Target_Data<0.5))
					{
						Target=1000;
						Target_Data=1;
						AT24CXX_WriteLenByte((Adress_Mod2+(offset*Adress_offset)),Target,Adress_offset);
					}
					break;
			case	Mod3:
					Target=AT24CXX_ReadLenByte((Adress_Mod3+(offset*Adress_offset)),Adress_offset);
					Target_Data=(float)Target/1000;
					if((Target_Data>2)||(Target_Data<0.5))
					{
						Target=1000;
						Target_Data=1;
						AT24CXX_WriteLenByte((Adress_Mod3+(offset*Adress_offset)),Target,Adress_offset);
					}
					break;
			case	Mod4:
					Target=AT24CXX_ReadLenByte((Adress_Mod4+(offset*Adress_offset)),Adress_offset);
					Target_Data=(float)Target/1000;
					if((Target_Data>2)||(Target_Data<0.5))
					{
						Target=1000;
						Target_Data=1;
						AT24CXX_WriteLenByte((Adress_Mod4+(offset*Adress_offset)),Target,Adress_offset);
					}
					break;
			case	Mod5:
					Target=AT24CXX_ReadLenByte((Adress_Mod5+(offset*Adress_offset)),Adress_offset);
					Target_Data=(float)Target/1000;
					if((Target_Data>2)||(Target_Data<0.5))
					{
						Target=1000;
						Target_Data=1;
						AT24CXX_WriteLenByte((Adress_Mod5+(offset*Adress_offset)),Target,Adress_offset);
					}
					break;
			case	Mod6:
					Target=AT24CXX_ReadLenByte((Adress_Mod6+(offset*Adress_offset)),Adress_offset);
					Target_Data=(float)Target/1000;
					if((Target_Data>2)||(Target_Data<0.5))
					{
						Target=1000;
						Target_Data=1;
						AT24CXX_WriteLenByte((Adress_Mod6+(offset*Adress_offset)),Target,Adress_offset);
					}
					break;
			default:break;
	}
	return(Target_Data);
}

void Cover_eeprom(void)
{
	u8 offset;
	for(offset=0;offset<3;offset++)
	{
		AT24CXX_WriteLenByte((Adress_Zero+(offset*Adress_offset)),0,Adress_offset);
	}
	
	for(offset=0;offset<21;offset++)
	{
		AT24CXX_WriteLenByte((Adress_Mod0+(offset*Adress_offset)),1000,Adress_offset);
	}
	AT24CXX_WriteLenByte(Adress_FEW0,0xAA,1);	
}

