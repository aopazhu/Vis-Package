#include "stm32f10x.h"
#include "print.h"
#include "delay.h"	
#include"12864.h"

void PRINT_InitPort(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;	//定义结构体		
	
	RCC_APB2PeriphClockCmd(Print_CLK, ENABLE);  //使能功能复用IO时钟，不开启复用时钟不能显示

	GPIO_InitStructure.GPIO_Pin  = Print_STB;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(Print_C_groud , &GPIO_InitStructure);    

	GPIO_InitStructure.GPIO_Pin  = Print_BUSY;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;   
	GPIO_Init(Print_C_groud , &GPIO_InitStructure);   
	
	GPIO_InitStructure.GPIO_Pin  = Print_Data;		
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  
	GPIO_Init(Print_D_group , &GPIO_InitStructure);     

	PRT_Data(0xFF);
	PRT_STB(1);
}
 
u8 PRINT_Check(void)
{
		u16 delay_cnt=0;
		while(PRT_BUSY&&(delay_cnt<50))
		{	
			delay_ms(10);
			delay_cnt++;
		}
		if(delay_cnt>=50)
		{	return 1;}
		else
		{	return 0;}
}

void PRINT_Write_Data(u8 data)
{
		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); 
    USART_SendData(USART1,(uint8_t)data); 
}

void PRINT_Init(void)
{
 		PRINT_Write_Data(0x1B);
 		PRINT_Write_Data(0x40);
}

u8 PRINT_Str(u8* tmp_str,u8 len)
{		
	u8 relen=0;
	while(((*tmp_str)!='\0')&&(len))
	{
			if((*tmp_str)==0x0A)
			{	PRINT_Write_Data(0x0D);}
			PRINT_Write_Data(*tmp_str);
			tmp_str++;
			len--;
			relen++;
	}
	return relen;
}

void PRINT_test(void)
{
		PRINT_Write_Data('1');
		PRINT_Write_Data('2');
		PRINT_Write_Data('3');
		PRINT_Write_Data('4');
		PRINT_Write_Data('5');
		PRINT_Write_Data('6');
		PRINT_Write_Data('7');
		PRINT_Write_Data('8');
		PRINT_Write_Data('9');
		PRINT_Write_Data('0');

		PRINT_Write_Data('a');
		PRINT_Write_Data('b');
		PRINT_Write_Data('c');
		PRINT_Write_Data('d');
		PRINT_Write_Data('e');
		PRINT_Write_Data('f');
		PRINT_Write_Data('g');
		PRINT_Write_Data('h');
		PRINT_Write_Data('i');
		PRINT_Write_Data('j');	
		
		PRINT_Write_Data(0x0A);	
		//PRINT_Write_Data(0x0D);
		
		PRINT_Str("hello world!\n",20);
}


void PRINT_module(_calendar_obj tmp_rtc,u8 r_t,u8 s_t,float wend,float tmp_vis,float tmp_pct)
{
	print_st print_FIFO;
	
	print_FIFO.wr_ptr=(u8)sprintf(&(print_FIFO.print_buf[0]),"*********************\n");
	print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"%04d-%02d-%02d  %02d:%02d:%02d\n",tmp_rtc.w_year,tmp_rtc.w_month,tmp_rtc.w_date,tmp_rtc.hour,tmp_rtc.min,tmp_rtc.sec);
	print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"ROTOR %s #   % .1f C\n",&r_dis[r_t][0],wend);
	print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED     %s RPM\n",&s_dis[s_t][0]);
	/*
	switch(s_t)
	{
			case 0:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED   0.1 RPM\n");
				break;
			case 1:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED   0.2 RPM\n");
				break;
			case 2:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED   0.5 RPM\n");
				break;
			case 3:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED     1 RPM\n");
				break;
			case 4:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED     2 RPM\n");
				break;
			case 5:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED     5 RPM\n");
				break;
			case 6:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED    10 RPM\n");
				break;
			case 7:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED    20 RPM\n");
				break;
			case 8:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED    50 RPM\n");
				break;
			case 9:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED   100 RPM\n");
				break;
			case 10:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED   200 RPM\n");
				break;
			default:
				print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"SPEED    %d RPM\n",50);
				break;
	}	
	*/
	//print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"DATA      %.1f mPa.S\n",tmp_vis);
	print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"DATA     %.1f cp\n",tmp_vis);
	print_FIFO.wr_ptr+=(u8)sprintf(&(print_FIFO.print_buf[print_FIFO.wr_ptr]),"PERCENT    %.1f %%\n",tmp_pct);
	
	PRINT_Str((u8*)&(print_FIFO.print_buf[0]),print_FIFO.wr_ptr);

}



/*
void PRINT_value(u8 *value_ptr,void* para1,void* para2,...)
{
		
		while(*value_ptr!=0)
		{
				if(*value_ptr!='%')
				{
						//do
						//{
							value_ptr++;
							switch(*value_ptr)
							{
									case 'd':
										
										break;
									case 'f':
										
										break;
									default:break;
							}
						//}while();
						
				}
				else
				{
						PRINT_Write_Data(*value_ptr++);
				}
		}
}

void int2char(int32 tmp_int)
{
		
}
*/
