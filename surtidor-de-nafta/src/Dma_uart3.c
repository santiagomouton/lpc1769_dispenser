#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17XX_uart.h"
#include "lpc17XX_gpdma.h"
#endif

#include <cr_section_macros.h>
#include "Dma_uart3.h"


void configuracionDmaCanalUart(uint8_t* valorConversion) {
	GPDMA_Channel_CFG_Type GPDMACfg;
	GPDMA_LLI_Type DMA_LLI_Struct;
	//Prepare DMA link list item structure
	DMA_LLI_Struct.SrcAddr= (uint32_t)valorConversion;
	DMA_LLI_Struct.DstAddr= (uint32_t)&(LPC_UART3->THR);
	DMA_LLI_Struct.NextLLI= (uint32_t)&DMA_LLI_Struct;
	DMA_LLI_Struct.Control= sizeof(valorConversion)
					  //default src width 8 bit
			 	 	  //default dest width 8 bit
			| (1<<26) //source increment
			;
	// Desabilito la interrupcion de GDMA
	NVIC_DisableIRQ(DMA_IRQn);
	GPDMA_Init();
	// Setup GPDMA channel --------------------------------
	// canal 0
	GPDMACfg.ChannelNum = 0;
	// Origen
	GPDMACfg.SrcMemAddr = (uint32_t)(valorConversion);
	// Destino
	GPDMACfg.DstMemAddr = 0;
	// Tamano de transferencia
	GPDMACfg.TransferSize = sizeof(valorConversion);
	GPDMACfg.TransferWidth = 0;
	// Tipo de transferencia
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	GPDMACfg.SrcConn = 0;
	// Destino Periferico
	GPDMACfg.DstConn = GPDMA_CONN_UART3_Tx;
	// Asocio Linker List Item
	GPDMACfg.DMALLI = (uint32_t)&DMA_LLI_Struct;
	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg);
	return;
}

void activarDmaCanalUart(void){
	GPDMA_ChannelCmd(0, ENABLE);
}

void desactivarDmaCanalUart(void){
	GPDMA_ChannelCmd(0, DISABLE);
}

