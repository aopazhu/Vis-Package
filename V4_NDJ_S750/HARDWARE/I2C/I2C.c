#include "i2c.h"
#include "delay.h"
/***************************************************************************
- 功能描述：STM32f103 模拟IIC初始化函数
- 隶属模块：STM32 IIC操作
- 函数属性：外部，使用户使用
- 参数说明：无
- 返回说明：无
- 函数实现步骤：
（1) 配置好模拟IIC的PC11和PC12的引脚状态
（2）并打开时钟
（3）将SCL和SDA拉为高电平，使总线处于空闲状态
***************************************************************************/
void IIC_Init(void)
{ 
//	RCC->APB2ENR|=1<<3;     //使能PORT B时钟		
// 	GPIOB->CRL&=0X00FFFFFF;
// 	GPIOB->CRL|=0X33000000;
// 	GPIOB->ODR|=0X000000C0;
	
GPIO_InitTypeDef GPIO_InitStructure;
RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE ); 
//打开GPIOC的时钟 

GPIO_InitStructure.GPIO_Pin = IIC_SCL_PIN|IIC_SDA_PIN;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_Init(IIC_SCL_PORT, &GPIO_InitStructure);
//配置SCL,也就是PB8为推挽输出模式

// GPIO_InitStructure.GPIO_Pin = IIC_SDA_PIN;
// GPIO_Init(IIC_SDA_PORT, &GPIO_InitStructure);
//配置SDA,也就是PB9为浮空输入模式


Set_IIC_SCL;
Set_IIC_SDA;
//将SCL和SDA都拉为高电平，表示IIC总线处于高电平
}
/***************************************************************************
- 功能描述：STM32f103 模拟IIC起始信号函数
- 隶属模块：STM32 IIC操作
- 函数属性：外部，使用户使用
- 参数说明：无
- 返回说明：无
- 函数实现步骤：
（1）首先将SDA配置为输出
（2）在SCL为高电平期间，SDA有下降沿的变化
***************************************************************************/
void IIC_Start(void)
{
SDA_OUT() 	//sda线输出
Set_IIC_SDA
Set_IIC_SCL
delay_us(4);
Clr_IIC_SDA		//START:when CLK is high,DATA change form high to low 
delay_us(4);
Clr_IIC_SCL		//钳住I2C总线，准备发送或接收数据 
} 
/***************************************************************************
- 功能描述：STM32f103 模拟IIC终止信号函数
- 隶属模块：STM32 IIC操作
- 函数属性：外部，使用户使用
- 参数说明：无
- 返回说明：无
- 函数实现步骤：
（1）首先将SDA配置为输出
（2）在SCL为高电平期间，SDA有上升沿的变化
***************************************************************************/
void IIC_Stop(void)
{
SDA_OUT();//sda线输出
Clr_IIC_SCL;
Clr_IIC_SDA;//STOP:when CLK is high DATA change form low to high
delay_us(4);
Set_IIC_SCL; 
Set_IIC_SDA;//发送I2C总线结束信号
delay_us(4); 
}

/***************************************************************************
- 功能描述：STM32f103 模拟IIC等待应答函数
- 隶属模块：STM32 IIC操作
- 函数属性：外部，使用户使用
- 参数说明：无 
- 返回说明：返回值：1，接收应答失败；0，接收应答成功
- 函数说明：
（1）首先将SDA配置为输入
（2）如果返回0，表示接收应答成功；如果为1，接收应答失败
（3）注意在IIC_STOP()中 ,将SDA置为了输出，所以再满足250条件后，产中IIC停止线号，
对输出线SDA读SDA输入值是为0的，所以才退出循环。
（4）此函数为STM32主机等待从机的ACK函数
***************************************************************************/
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN(); //SDA设置为输入 
	Set_IIC_SDA;
	delay_us(1); 
	Set_IIC_SCL;
	delay_us(1); 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	Clr_IIC_SCL;//时钟输出0 
	return 0; 
} 
/***************************************************************************
- 功能描述：STM32f103 模拟IIC应答信号函数
- 隶属模块：STM32 IIC操作
- 函数属性：外部，使用户使用
- 参数说明：无
- 返回说明：无
- 函数实现步骤：
（1）首先将SDA配置为输出
（2）在SCL的一个时钟周期期间，SDA为低 
（3）此函数为STM32主机向从机发送ACK函数
***************************************************************************/
//产生ACK应答
void IIC_Ack(void)
{
Clr_IIC_SCL;
SDA_OUT();
Clr_IIC_SDA;
delay_us(2);
Set_IIC_SCL;
delay_us(2);
Clr_IIC_SCL;
}
/***************************************************************************
- 功能描述：STM32f103 模拟IIC不应答信号函数
- 隶属模块：STM32 IIC操作
- 函数属性：外部，使用户使用
- 参数说明：无
- 返回说明：无
- 函数实现步骤：
（1）首先将SDA配置为输出
（2）在SCL的一个时钟周期期间，SDA为高电平
（4）此函数为STM32主机不向从机发送ACK
***************************************************************************/ 
void IIC_NAck(void)
{
Clr_IIC_SCL;
SDA_OUT();
Set_IIC_SDA;
delay_us(2);
Set_IIC_SCL;
delay_us(2);
Clr_IIC_SCL;
} 
/***************************************************************************
- 功能描述：STM32f103 模拟IIC 发送一个字节函数
- 隶属模块：STM32 IIC操作
- 函数属性：外部，使用户使用
- 参数说明：无
- 返回说明：无
- 函数实现步骤：
（1）首先将SDA配置为输出
（2）在SCL为低电平期间，置位SDA的数据为0或者1，延时后拉高SCL,取走数据，循环8次
***************************************************************************/ 
void IIC_Send_Byte(u8 txd)
{ 
u8 t; 
SDA_OUT(); 
Clr_IIC_SCL;//拉低时钟开始数据传输
for(t=0;t<8;t++)
{ 
//IIC_SDA=(txd&0x80)>>7;
if ((txd&0x80)>>7) 
Set_IIC_SDA
else Clr_IIC_SDA;
txd<<=1; 
delay_us(2); 
Set_IIC_SCL;
delay_us(2); 
Clr_IIC_SCL; 
delay_us(2);
} 
}

/***************************************************************************
- 功能描述：STM32f103 模拟IIC 发送一个字节函数
- 隶属模块：STM32 IIC操作
- 函数属性：外部，使用户使用
- 参数说明：ack 用于决定是是否回应ACK
- 返回说明：无
- 函数实现步骤：
（1）首先将SDA配置为输入
（2）在SCL的一个时钟周期后，读SDA的数据，保存在receive中
（3）读1个字节，ack=1时，发送ACK，ack=0，发送nACK

***************************************************************************/ 
u8 IIC_Read_Byte(unsigned char ack)
{
unsigned char i,receive=0;
SDA_IN();//SDA设置为输入
for(i=0;i<8;i++ )
{
Clr_IIC_SCL; 
delay_us(2);
Set_IIC_SCL;
receive<<=1;
if(READ_SDA)receive++; 
delay_us(1); 
} 
if (!ack)
IIC_NAck();//发送nACK
else
IIC_Ack(); //发送ACK 
return receive;
}
