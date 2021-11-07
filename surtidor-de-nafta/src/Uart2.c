#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17XX_adc.h"
#include "lpc17XX_gpdma.h"
#include "lpc17XX_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_uart.h"
#endif

#include <cr_section_macros.h>

void configurarUart3Sant(void){
	//configuracion de pin Tx
	PINSEL_CFG_Type PinTx;
	PinTx.Funcnum = 2;
	PinTx.OpenDrain = 0;
	PinTx.Pinmode = 0;
	PinTx.Pinnum = 0;
	PinTx.Portnum = 0;
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

	UARTFIFOConfigStruct.FIFO_DMAMode = ENABLE;
	//Inicializa FIFO
	UART_FIFOConfig(LPC_UART3, &UARTFIFOConfigStruct);
	//Habilita transmision
	UART_TxCmd(LPC_UART3, ENABLE);
	return;
}

