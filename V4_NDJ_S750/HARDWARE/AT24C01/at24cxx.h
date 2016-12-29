#ifndef __AT24CXX_H
#define __AT24CXX_H
#include "i2c.h"


#define		Zero		0
#define		Mod0		1
#define		Mod1		2
#define		Mod2		3
#define		Mod3		4
#define		Mod4		5
#define		Mod5		6
#define		Mod6		7
#define		FEW0		8
// #define		Pair		2
// #define		Wsalt		3
// #define		Tfull		4

#define		Adress_offset		2

#define		Adress_Zero			16

#define		Adress_Mod0			(64)
#define		Adress_Mod1			(64+6)
#define		Adress_Mod2			(64+12)
#define		Adress_Mod3			(64+18)
#define		Adress_Mod4			(64+24)
#define		Adress_Mod5			(64+36)
#define		Adress_Mod6			(64+48)

#define		Adress_FEW0			255
// #define		Adress_Vfull		Adress_Vzero+Adress_offset
// #define		Adress_Pair			Adress_Vfull+Adress_offset
// #define		Adress_Wsalt		Adress_Pair+Adress_offset
// #define		Adress_Tfull		Adress_Wsalt+Adress_offset

/***************************************************************
- ˵�������²�����AT24Cxx��Ѱַ�ռ䣬C0x ,X ��ʾXK ��C01��ʾ1K
- 127��ʾ2^7 1Kbit/8=128Byte 128�ֽ�
- 255��ʾ2^8 2Kbit/8=256Byte 256�ֽ� 
- 512��ʾ2^9 4Kbit/8=512Byte 512�ֽ� 
- 
***************************************************************/
#define AT24C01 127
#define AT24C02 255
#define AT24C04 511
#define AT24C08 1023
#define AT24C16 2047
#define AT24C32 4095
#define AT24C64 8191
#define AT24C128 16383
#define AT24C256 32767 
/**************************************************************
--����ʹ�õ���24c01�����Զ���EE_TYPEΪAT24C01
**************************************************************/
#define EE_TYPE AT24C02

/**************************************************************
--EEPROM�Ĳ�������
--24CXX��������
**************************************************************/

u8 AT24CXX_ReadOneByte(u16 ReadAddr); //ָ����ַ��ȡһ���ֽ�
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite); //ָ����ַд��һ���ֽ�

void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//ָ����ַ��ʼд��ָ�����ȵ�����
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len); //ָ����ַ��ʼ��ȡָ����������

void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite); //��ָ����ַ��ʼд��ָ�����ȵ�����
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead); //��ָ����ַ��ʼ����ָ�����ȵ�����

u8 AT24CXX_Check(void); //�������
void AT24CXX_Init(void); //��ʼ��IIC

void Save_Target(float	Target_Data,	u8 Target_Name,u8 offset);
float Read_Target(u8 Target_Name,u8 offset);
void Cover_eeprom(void);
#endif
