/*
===============================================================================
 Name        : surtidor-de-nafta.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include "Utils.h"
#include "Teclado.h"

//void retardo(void);

int main(void){

	uint32_t relojCpu = SystemCoreClock;


	LPC_GPIO0->FIODIR     |= (1<<22);
	//configurarPuertosTeclado();


	while(1){
		LPC_GPIO0->FIOSET |= (1<<22);  // prende el led
		retardoEnSeg(3);
		LPC_GPIO0->FIOCLR |= (1<<22);  // apaga el led
		retardoEnSeg(3);

	}

	return 0;
}

/*void retardo(void){

	uint32_t contador;
	for(contador =0 ; contador<10000000; contador++){};

}*/




