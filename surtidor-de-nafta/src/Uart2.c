#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17XX_uart.h"
#endif

#include <cr_section_macros.h>

void configurarUart3(void){
	//configuracion de pin Tx
	PINSEL_CFG_Type PinTx;
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinTx);
	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	//inicializa periferico
	//configuracion por defecto:
	//				- BD 9600
	//				- 8 bits
	UART_ConfigStructInit(&UARTConfigStruct);
	UART_Init(LPC_UART3, &UARTConfigStruct);
	//Configuracion por defecto de la FIFO, excepto por asociar  el DMA
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UARTFIFOConfigStruct->FIFO_DMAMode = ENABLE;
	//Inicializa FIFO
	UART_FIFOConfig(LPC_UART3, &UARTFIFOConfigStruct);
	//Habilita transmision
	UART_TxCmd(LPC_UART3, ENABLE);
	return;
}

