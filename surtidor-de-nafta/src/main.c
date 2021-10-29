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
#include "Uart.h"


void estadosAdmin(void);

char ingresadoPorTeclado[10]="";

int precioNafta=110;
int precioGasoil=90;

char modoCombustible='0';//0-Espera orden, 1-Carga Nafta, 2-Carga Gasoil
char modoCarga='0';//0-Espera orden, 1-Cant de litros, 2-Hasta que llene, 3-Modo abierto
char estadoDispenser='0';//1-llenando, 2-Esperando evento de llenado




int main(void){

	uint32_t relojCpu = SystemCoreClock;



	LPC_GPIO0->FIODIR     |= (1<<22);
	//configurarPuertosTeclado();


	while(1){
		LPC_GPIO0->FIOSET |= (1<<22);  // prende el led
		retardoEnSeg(1);
		//retardoEnSeg(3);
		LPC_GPIO0->FIOCLR |= (1<<22);  // apaga el led
		retardoEnSeg(1);

	}

	return 0;
}

/*
void estadosAdmin(char datoDelTeclado){
	if(modoCombustible=='0'){
		modoCombustible=datoDelTeclado;
	}
	else if(modoCombustible!='0' && modoCarga=='0'){
		modoCarga=datoDelTeclado;
	}
	else{
		estadoDispenser=datoDelTeclado;
	}
}

void resetEstados(void){
	modoCombustible='0';//0-Espera orden, 1-Carga Nafta, 2-Carga Gasoil
	modoCarga='0';//0-Espera orden, 1-Cant de litros, 2-Hasta que llene, 3-Modo abierto
	estadoDispenser='0';//1-llenando, 2-Esperando evento de llenado
}

//cantidad de combustible en litros
float calcularCosto(int cantDeCombustible){
	if(modoCombustible=='1'){
		return precioNafta*cantDeCombustible;
	}
	else if(modoCombustible=='2'){
		return precioGasoil*cantDeCombustible;
	}
	return -1.0;
}*/




