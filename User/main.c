#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_spi_nrf.h"
#include "bsp_adc.h"
#include "bsp_keys.h"

// ADC1转换的电压值通过MDA方式传到SRAM
extern __IO uint16_t ADC_ConvertedValue[4]; //四路电压值 16BIts表示
extern void deal(void);

u8 status;	//用于判断接收/发送状态
u8 txbuf[8];	 //发送缓冲
u8 i;

int main(void)                  
{   
	USART1_Config();
  SPI_NRF_Init();
	ADC1_Init();
	Keys_Init();
	

  /*检测NRF模块与MCU的连接*/
  status = NRF_Check(); 

  /*判断连接状态*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF与MCU连接成功！\r\n");  
  else	  
    printf("\r\n  NRF与MCU连接失败，请重新检查接线。\r\n");

  while(1)
  {
		
		printf("		ADC %d  %d  %d  %d\r\n", ADC_ConvertedValue[0], 
		ADC_ConvertedValue[1], ADC_ConvertedValue[2], ADC_ConvertedValue[3]);
		
		if(Key_Scan(GPIOB, GPIO_Pin_8, 0) == KEY_ON) { txbuf[2] = 0x00; txbuf[3] = 0x01; }
			else if(Key_Scan(GPIOB, GPIO_Pin_15, 0) == KEY_ON) { txbuf[2] = 0x01; txbuf[3] = 0x00; }
				else if(Key_Scan(GPIOB, GPIO_Pin_14, 0) == KEY_ON) { txbuf[2] = 0x01; txbuf[3] = 0x01; }
					else if(Key_Scan(GPIOB, GPIO_Pin_13, 0) == KEY_ON) { txbuf[2] = 0xff; txbuf[3] = 0xff; }
						else deal();
		
		printf("		deal %d  %d  %d  %d\r\n", txbuf[0], txbuf[1], txbuf[2], txbuf[3]);
		
		
		printf("\r\n 主机端 进入发送模式\r\n");
		
    NRF_TX_Mode();
    status = NRF_Tx_Dat(txbuf);	  
		
    switch(status)
    {
      case MAX_RT:
        printf("\r\n 主机端 没接收到应答信号，发送次数超过限定值，发送失败。 \r\n");
      break;

      case ERROR:
        printf("\r\n 未知原因导致发送失败。 \r\n");
      break;

      case TX_DS:
        printf("\r\n 主机端 接收到 从机端 的应答信号，发送成功！ \r\n");	 		
      break;  								
   }			   	
  }
}
/*********************************************END OF FILE**********************/
