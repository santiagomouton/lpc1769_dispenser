#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17XX_uart.h"
#endif

#include <cr_section_macros.h>

void configurarUart3(){
	//Configuracion UART3
	LPC_PINCON -> PINSEL0 |= (1<< 1);   // pin 0.0 como TXD3
	LPC_SC     -> PCONP   |= (1<<25);   // enciendo periferico uart3
	LPC_UART3  -> LCR     |= (1<< 7);   // DLAB=1 para habilitar registros
	LPC_UART3  -> DLL      = 163;
	LPC_UART3  -> DLM      = 0;         // para BR=9600
	LPC_UART3  -> LCR     &=~(1<< 7);   // vuelvo a 0 DLAB
	LPC_UART3  -> LCR     |= (0b11<<0); // 8 bits

}

void enviarString (uint8_t arreglo){

}
