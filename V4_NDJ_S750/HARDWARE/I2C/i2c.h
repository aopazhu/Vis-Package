#ifndef __I2C_H
#define __I2C_H
//#include "stm32f10x_lib.h"
#include "sys.h"
/*********************************************************
- I2C 宏定义：
- 软件模拟I2C时的管脚，用PC12模拟CLK，用PC11模拟SDA
- 所以在应用的时候，要打开GPIOC的时钟 
*********************************************************/
#define IIC_SCL_PORT GPIOB
#define IIC_SCL_CLK RCC_APB2Periph_GPIOB 
#define IIC_SCL_PIN GPIO_Pin_8
#define IIC_SDA_PORT GPIOB
#define IIC_SDA_CLK RCC_APB2Periph_GPIOB
#define IIC_SDA_PIN GPIO_Pin_9

/*********************************************************
- I2C宏定义：
- SDA方向设置:
- SDA_IN(): 此处是将SDA定义为配置为输入模式
- SDA_OUT():此处是将SDA_OUT配置为推挽输出模式
说明：这里是配置的快速写法，当然也可以用库函数初始化，
因为IIC的SDA线，即要作为输入也要作为输出。 
*********************************************************/

#define SDA_IN() 	{GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=0X00000080;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=0X00000030;}


/*********************************************************
- I2C宏定义：
- I2C时钟操作:
- Set_IIC_SCL: 此处是将SCL拉为高电平
- Clr_IIC_SCL: 此处是将SCL拉为低电平
- 说明：通过这两个宏定义就可以模拟IIC的时钟电平SCL的高低变化了 
*********************************************************/
#define Set_IIC_SCL 	PBout(8)=1;
#define Clr_IIC_SCL 	PBout(8)=0;


/*********************************************************
- I2C宏定义：
- I2C数据线操作:
- Set_IIC_SDA: 此处是将SDA拉为高电平
- Clr_IIC_SDA: 此处是将SDA拉为低电平
- READ_SDA : 此处是读取SDA线上的位数据
- 说明：通过这两个宏定义就可以模拟IIC的数据线SDA电平的高低变化了 
*********************************************************/
#define Set_IIC_SDA 	PBout(9)=1;
#define Clr_IIC_SDA 	PBout(9)=0; 
#define READ_SDA 			PBin(9)


/*********************************************************
- I2C模拟操作函数的声明： 
*********************************************************/
//IIC所有操作函数
void IIC_Init(void); //初始化IIC的IO口 
void IIC_Start(void); //发送IIC开始信号
void IIC_Stop(void); //发送IIC停止信号
void IIC_Send_Byte(u8 txd); //IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); //IIC等待ACK信号
void IIC_Ack(void); //IIC发送ACK信号
void IIC_NAck(void); //IIC不发送ACK信号

/*********************************************************
- I2C外部操作函数的声明： 
*********************************************************/
void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr); 
#endif
