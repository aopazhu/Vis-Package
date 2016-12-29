#ifndef __I2C_H
#define __I2C_H
//#include "stm32f10x_lib.h"
#include "sys.h"
/*********************************************************
- I2C �궨�壺
- ���ģ��I2Cʱ�Ĺܽţ���PC12ģ��CLK����PC11ģ��SDA
- ������Ӧ�õ�ʱ��Ҫ��GPIOC��ʱ�� 
*********************************************************/
#define IIC_SCL_PORT GPIOB
#define IIC_SCL_CLK RCC_APB2Periph_GPIOB 
#define IIC_SCL_PIN GPIO_Pin_8
#define IIC_SDA_PORT GPIOB
#define IIC_SDA_CLK RCC_APB2Periph_GPIOB
#define IIC_SDA_PIN GPIO_Pin_9

/*********************************************************
- I2C�궨�壺
- SDA��������:
- SDA_IN(): �˴��ǽ�SDA����Ϊ����Ϊ����ģʽ
- SDA_OUT():�˴��ǽ�SDA_OUT����Ϊ�������ģʽ
˵�������������õĿ���д������ȻҲ�����ÿ⺯����ʼ����
��ΪIIC��SDA�ߣ���Ҫ��Ϊ����ҲҪ��Ϊ����� 
*********************************************************/

#define SDA_IN() 	{GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=0X00000080;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFFFF0F;GPIOB->CRH|=0X00000030;}


/*********************************************************
- I2C�궨�壺
- I2Cʱ�Ӳ���:
- Set_IIC_SCL: �˴��ǽ�SCL��Ϊ�ߵ�ƽ
- Clr_IIC_SCL: �˴��ǽ�SCL��Ϊ�͵�ƽ
- ˵����ͨ���������궨��Ϳ���ģ��IIC��ʱ�ӵ�ƽSCL�ĸߵͱ仯�� 
*********************************************************/
#define Set_IIC_SCL 	PBout(8)=1;
#define Clr_IIC_SCL 	PBout(8)=0;


/*********************************************************
- I2C�궨�壺
- I2C�����߲���:
- Set_IIC_SDA: �˴��ǽ�SDA��Ϊ�ߵ�ƽ
- Clr_IIC_SDA: �˴��ǽ�SDA��Ϊ�͵�ƽ
- READ_SDA : �˴��Ƕ�ȡSDA���ϵ�λ����
- ˵����ͨ���������궨��Ϳ���ģ��IIC��������SDA��ƽ�ĸߵͱ仯�� 
*********************************************************/
#define Set_IIC_SDA 	PBout(9)=1;
#define Clr_IIC_SDA 	PBout(9)=0; 
#define READ_SDA 			PBin(9)


/*********************************************************
- I2Cģ����������������� 
*********************************************************/
//IIC���в�������
void IIC_Init(void); //��ʼ��IIC��IO�� 
void IIC_Start(void); //����IIC��ʼ�ź�
void IIC_Stop(void); //����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd); //IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); //IIC�ȴ�ACK�ź�
void IIC_Ack(void); //IIC����ACK�ź�
void IIC_NAck(void); //IIC������ACK�ź�

/*********************************************************
- I2C�ⲿ���������������� 
*********************************************************/
void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr); 
#endif
