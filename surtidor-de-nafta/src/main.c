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
#include <math.h>
#include <stdlib.h>

//#include "Teclado.h"
//#include "Uart.h"


void estadosAdmin(char datoDelTeclado);
void resetEstados(void);
void resetBufferTeclado(void);
float calcularCosto(int cantDeCombustible);
void configurarPuertosTeclado(void);
void loopTeclado(void);
void confIntGPIOPorEINT(void);
void surtirHastaLlenar();
void surtirHastaClickear();
//void EINT3_IRQHandler(void);

char ingresadoPorTeclado[10]="";

int precioNafta=110;
int precioGasoil=90;

int cantidadDeLitrosACargar=0;

char modoCombustible='0';//0-Espera orden, 1-Carga Nafta, 2-Carga Gasoil
char modoCarga='0';//0-Espera orden, 1-Cant de litros, 2-Hasta que llene, 3-Modo abierto
char modoIngresarCantidad='0';//1-cantidad de litros, //2-cantidad de plata
char estadoDispenser='0';//1-llenando, 2-Esperando evento de llenado


/*#########Variables del teclado#########*/
int pinesFilas[] = {9,8,7,6};
int pinesColumnas[] = {5,4,3,2};
char teclas[4][4] = {{'1','2','3','A'},
                     {'4','5','6','B'},
                     {'7','8','9','C'},
                     {'*','0','#','D'}};
int cantidadDeDatosIngresadosPorTeclado=0;
char  bufferTeclado[10];



int main(void){

	uint32_t relojCpu = SystemCoreClock;
	//printf("%d",2);
	resetBufferTeclado();

	LPC_GPIO0->FIODIR     |= (1<<22);
	//configurarPuertosTeclado();


	while(1){
		LPC_GPIO0->FIOSET |= (1<<22);  // apaga el led
		retardoEnSeg(1);
		LPC_GPIO0->FIOCLR |= (1<<22);  // prende el led
		retardoEnSeg(1);

		//secuencia de test1
		estadosAdmin('1');
		estadosAdmin('1');
		estadosAdmin('1');
		//ingresamos el numero
		estadosAdmin('1');
		estadosAdmin('0');
		estadosAdmin('0');
		estadosAdmin('#');
		//------------------------
		//secuencia de test2
		estadosAdmin('1');
		estadosAdmin('2');

	}

	return 0;
}
/**/

void estadosAdmin(char datoDelTeclado){
	if(modoCombustible=='0'){//ok
		modoCombustible=datoDelTeclado;
	}
	else if(modoCombustible!='0' && modoCarga=='0' && modoIngresarCantidad=='0'){//ok
		modoCarga=datoDelTeclado;
	}
	else if(modoCombustible!='0' && modoCarga=='1' && modoIngresarCantidad=='0'){
		modoIngresarCantidad=datoDelTeclado;
	}
	else if(modoCombustible!='0' && modoCarga=='2' && modoIngresarCantidad=='0'){
		surtirHastaLlenar();
	}
	else if(modoCombustible!='0' && modoCarga=='2' && modoIngresarCantidad=='0'){
		surtirHastaClickear();
	}
	else if(modoIngresarCantidad!='0'){
		if(datoDelTeclado=='#')
		{
			cantidadDeLitrosACargar=atoi(&bufferTeclado[0]) ;//obtenés la cantidad de litros o pesos
			cantidadDeDatosIngresadosPorTeclado=0;
			resetEstados();
			resetBufferTeclado();
			estadoDispenser='1';
			//disparás una configuración de timer para que cargue la nafta
		}
		else{
			bufferTeclado[cantidadDeDatosIngresadosPorTeclado]=datoDelTeclado;
			cantidadDeDatosIngresadosPorTeclado++;
			estadoDispenser='2';
		}

	}
	else if(modoCombustible!='0' && modoCarga!='0'  && datoDelTeclado!='1'){//si es dsitinto de 1 espero q sea un 2 o 3
		estadoDispenser='1';
	}
	return;
}

void surtirHastaLlenar()
{

}
void surtirHastaClickear(){

}

void resetEstados(void){
	modoCombustible='0';//0-Espera orden, 1-Carga Nafta, 2-Carga Gasoil
	modoCarga='0';//0-Espera orden, 1-Cant de litros, 2-Hasta que llene, 3-Modo abierto
	modoIngresarCantidad='0';//1-cantidad de litros, //2-cantidad de plata
	estadoDispenser='0';//1-llenando, 2-Esperando evento de llenado
}
void resetBufferTeclado(void){
	for(int i=0; i<10;i++){
		bufferTeclado[i]=' ';
	}
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
}


/*############################################Teclado############################################*/
void configurarPuertosTeclado(){
	int i;
	for(i=0 ; i<4; i++){
		LPC_GPIO2->FIODIR|=(1<<pinesFilas[i]);//pines como output de las filas del teclado
		LPC_GPIO2->FIOCLR|=(1<<pinesFilas[i]);//pines de las filas en alto del teclado//Tenía puesto un SET, hay que ver q pasa
	}
	//el resto de los pines están como input y pull up asi que las columnas del teclado están configuradas ya...
}

/*Detecta la tecla que fue presionada, la podemos llamar desde el main periódicamete o por interrupciones externas*/
void loopTeclado(){
	//Barrido por las filas
	char teclaPresionada=' ';
	for (int nL=0; nL<4; nL++)
	{
	   LPC_GPIO2->FIOCLR|=(1<<pinesFilas[nL]);
	   //Barrido en columnas buscando un LOW
	   for (int nC=0; nC<4; nC++) {
		   if (! LPC_GPIO2->FIOPIN & (1<<pinesColumnas[nC]))//sie stán en low los pines
		   {
	          teclaPresionada=teclas[nL][nC];
	          while(!LPC_GPIO2->FIOPIN & (1<<pinesColumnas[nC]))//(digitalRead(pinesColumnas[nC]) == LOW)
	          {}
	        }
	   }
	   LPC_GPIO2->FIOSET|=(1<<pinesFilas[nL]);
	   }
	   retardoEnMs(10);
	   estadosAdmin(teclaPresionada);
}

/*condiguración para habilitar interrupciones por GPIO para el teclado matricial*/
void confIntGPIOPorEINT(void){
	for(int i=0; i<4;i++){
		LPC_GPIOINT -> IO2IntEnF |= ((1 << pinesColumnas[i])); //Selecciono la interrupcion por flanco de bajada
		LPC_GPIOINT -> IO2IntClr |= ((1 << pinesColumnas[i])); //Limpia la bandera
	}
	NVIC_SetPriority(EINT3_IRQn, 0); 	//Prioridad para esta interrupcion
	NVIC_EnableIRQ(EINT3_IRQn); 		//Habilita las interrupciones por GPIO
}

void EINT3_IRQHandler(void){
	NVIC_DisableIRQ(EINT3_IRQn);
	for(int i=0; i<4;i++){
		LPC_GPIOINT -> IO2IntClr |= ((1 << pinesColumnas[i])); //Limpia la bandera
	}
	loopTeclado();
	NVIC_EnableIRQ(EINT3_IRQn);
}



