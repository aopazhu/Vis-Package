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
- 说明：以下参数是AT24Cxx的寻址空间，C0x ,X 表示XK 如C01表示1K
- 127表示2^7 1Kbit/8=128Byte 128字节
- 255表示2^8 2Kbit/8=256Byte 256字节 
- 512表示2^9 4Kbit/8=512Byte 512字节 
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
--板子使用的是24c01，所以定义EE_TYPE为AT24C01
**************************************************************/
#define EE_TYPE AT24C02

/**************************************************************
--EEPROM的操作函数
--24CXX驱动函数
**************************************************************/

u8 AT24CXX_ReadOneByte(u16 ReadAddr); //指定地址读取一个字节
void AT24CXX_WriteOneByte(u16 WriteAddr,u8 DataToWrite); //指定地址写入一个字节

void AT24CXX_WriteLenByte(u16 WriteAddr,u32 DataToWrite,u8 Len);//指定地址开始写入指定长度的数据
u32 AT24CXX_ReadLenByte(u16 ReadAddr,u8 Len); //指定地址开始读取指定长度数据

void AT24CXX_Write(u16 WriteAddr,u8 *pBuffer,u16 NumToWrite); //从指定地址开始写入指定长度的数据
void AT24CXX_Read(u16 ReadAddr,u8 *pBuffer,u16 NumToRead); //从指定地址开始读出指定长度的数据

u8 AT24CXX_Check(void); //检查器件
void AT24CXX_Init(void); //初始化IIC

void Save_Target(float	Target_Data,	u8 Target_Name,u8 offset);
float Read_Target(u8 Target_Name,u8 offset);
void Cover_eeprom(void);
#endif
