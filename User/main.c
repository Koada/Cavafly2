#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_spi_nrf.h"
#include "bsp_adc.h"
#include "bsp_keys.h"

// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
extern __IO uint16_t ADC_ConvertedValue[4]; //��·��ѹֵ 16BIts��ʾ
extern void deal(void);

u8 status;	//�����жϽ���/����״̬
u8 txbuf[8];	 //���ͻ���
u8 i;

int main(void)                  
{   
	USART1_Config();
  SPI_NRF_Init();
	ADC1_Init();
	Keys_Init();
	

  /*���NRFģ����MCU������*/
  status = NRF_Check(); 

  /*�ж�����״̬*/  
  if(status == SUCCESS)	   
    printf("\r\n      NRF��MCU���ӳɹ���\r\n");  
  else	  
    printf("\r\n  NRF��MCU����ʧ�ܣ������¼����ߡ�\r\n");

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
		
		
		printf("\r\n ������ ���뷢��ģʽ\r\n");
		
    NRF_TX_Mode();
    status = NRF_Tx_Dat(txbuf);	  
		
    switch(status)
    {
      case MAX_RT:
        printf("\r\n ������ û���յ�Ӧ���źţ����ʹ��������޶�ֵ������ʧ�ܡ� \r\n");
      break;

      case ERROR:
        printf("\r\n δ֪ԭ���·���ʧ�ܡ� \r\n");
      break;

      case TX_DS:
        printf("\r\n ������ ���յ� �ӻ��� ��Ӧ���źţ����ͳɹ��� \r\n");	 		
      break;  								
   }			   	
  }
}
/*********************************************END OF FILE**********************/
