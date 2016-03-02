#include "bsp_spi_nrf.h"
#include "bsp_usart1.h"

u8 RX_BUF[RX_PLOAD_WIDTH];
u8 TX_BUF[TX_PLOAD_WIDTH];
u8 TX_ADDRESS[TX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};
u8 RX_ADDRESS[RX_ADR_WIDTH] = {0x34,0x43,0x10,0x10,0x01};

void Delay(__IO u32 nCount)
{
  for(; nCount != 0; nCount--);
} 

void SPI_NRF_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure); 
		  
  NRF_CSN_HIGH(); 
 
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  SPI_Init(SPI1, &SPI_InitStructure);

  SPI_Cmd(SPI1, ENABLE);
}


u8 SPI_NRF_RW(u8 dat)
{  	
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI1, dat);		
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_I2S_ReceiveData(SPI1);
}

u8 SPI_NRF_WriteReg(u8 reg,u8 dat)
{
 	u8 status;
	NRF_CE_LOW();

  NRF_CSN_LOW();
				
	status = SPI_NRF_RW(reg);
	
	SPI_NRF_RW(dat); 
	   
  NRF_CSN_HIGH();	

  return(status);
}


u8 SPI_NRF_ReadReg(u8 reg)
{
 	u8 reg_val;

	NRF_CE_LOW();

 	NRF_CSN_LOW();

	SPI_NRF_RW(reg); 

	reg_val = SPI_NRF_RW(NOP);

	NRF_CSN_HIGH();		
   	
	return reg_val;
}	


u8 SPI_NRF_ReadBuf(u8 reg,u8 *pBuf,u8 bytes)
{
 	u8 status, byte_cnt;

	NRF_CE_LOW();

	NRF_CSN_LOW();
		
	status = SPI_NRF_RW(reg); 


	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)		  
	  pBuf[byte_cnt] = SPI_NRF_RW(NOP);

	NRF_CSN_HIGH();	
		
 	return status;
}


u8 SPI_NRF_WriteBuf(u8 reg ,u8 *pBuf,u8 bytes)
{
	u8 status,byte_cnt;
	
	NRF_CE_LOW();
	
	NRF_CSN_LOW();			

  status = SPI_NRF_RW(reg); 

	for(byte_cnt=0;byte_cnt<bytes;byte_cnt++)
		SPI_NRF_RW(*pBuf++); 	 

	NRF_CSN_HIGH();			
  
  return (status);		
}

void NRF_RX_Mode(void)
{
	NRF_CE_LOW();	

  SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);

  SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);  

  SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01); 

  SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);   

  SPI_NRF_WriteReg(NRF_WRITE_REG+RX_PW_P0,RX_PLOAD_WIDTH);   

  SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);

  SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG, 0x0f);
	
  NRF_CE_HIGH();
}    


void NRF_TX_Mode(void)
{  
	NRF_CE_LOW();		

  SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,TX_ADDRESS,TX_ADR_WIDTH);

  SPI_NRF_WriteBuf(NRF_WRITE_REG+RX_ADDR_P0,RX_ADDRESS,RX_ADR_WIDTH);

  SPI_NRF_WriteReg(NRF_WRITE_REG+EN_AA,0x01);

  SPI_NRF_WriteReg(NRF_WRITE_REG+EN_RXADDR,0x01);

  SPI_NRF_WriteReg(NRF_WRITE_REG+SETUP_RETR,0x1a);

  SPI_NRF_WriteReg(NRF_WRITE_REG+RF_CH,CHANAL);

  SPI_NRF_WriteReg(NRF_WRITE_REG+RF_SETUP,0x0f);
	
  SPI_NRF_WriteReg(NRF_WRITE_REG+CONFIG,0x0e);
  
	NRF_CE_HIGH();
	
  Delay(0xffff);
}


u8 NRF_Check(void)
{
	u8 buf[5]={0xC2, 0xC2, 0xC2, 0xC2, 0xC2};

	u8 buf1[5], i;
	 
	SPI_NRF_WriteBuf(NRF_WRITE_REG+TX_ADDR,buf,5);

	SPI_NRF_ReadBuf(TX_ADDR,buf1,5); 
	             
	for(i = 0; i < 5; i++)
		if(buf1[i] != 0xC2)
			break;
	       
	if(i == 5)
		return SUCCESS;
	else
		return ERROR;
}


u8 NRF_Tx_Dat(u8 *txbuf)
{
	u8 state;  

	NRF_CE_LOW();
			
  SPI_NRF_WriteBuf(WR_TX_PLOAD,txbuf, TX_PLOAD_WIDTH);
  
 	NRF_CE_HIGH();
	  	                     
	while(NRF_Read_IRQ() != 0); 	
	               
	state = SPI_NRF_ReadReg(STATUS);
           
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state); 	

	SPI_NRF_WriteReg(FLUSH_TX,NOP);


	if(state&MAX_RT)
			 return MAX_RT; 

	else if(state&TX_DS)
		 	return TX_DS;
	else						  
			return ERROR;
} 


u8 NRF_Rx_Dat(u8 *rxbuf)
{
	u8 state; 
	NRF_CE_HIGH();

	while(NRF_Read_IRQ()!=0); 
	
	NRF_CE_LOW();
	              
	state=SPI_NRF_ReadReg(STATUS);
	 
	      
	SPI_NRF_WriteReg(NRF_WRITE_REG+STATUS,state);


	if(state&RX_DR)
	{
	  SPI_NRF_ReadBuf(RD_RX_PLOAD,rxbuf,RX_PLOAD_WIDTH);
	     SPI_NRF_WriteReg(FLUSH_RX,NOP);
	  return RX_DR; 
	}
	else    
		return ERROR;              
}


/*********************************************END OF FILE**********************/
