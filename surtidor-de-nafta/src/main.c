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
//#include "lpc17xx_adc.h"
//#include "lpc17XX_gpio.h"
#endif


#include <cr_section_macros.h>
#include "Utils.h"
#include <math.h>
#include <stdlib.h>


//#include "Teclado.h"
//#include "Uart.h"
#define PRECIO_NAFTA 110
#define PRECIO_GASOIL 90


void estadosAdmin(char datoDelTeclado);
void resetEstados(void);
void resetBufferTeclado(void);
float calcularCosto(int cantDeCombustible);
void configurarPuertosTeclado(void);
void loopTeclado(void);
void confIntGPIOPorEINT(void);
void surtirHastaLlenar();
void surtirPorCapture();

/* CAPTURE PARA MANGUERA */
//void TIMER0_IRQHandler(void);
void configurarCapture(void);
void iniciarCapture(void);
void dashabilitarCapture(void) ;

void configurarEINT2();



char ingresadoPorTeclado[10]="";

uint16_t cantidadDeLitrosACargar=0; 

char ultimaTeclaPresionada=' ';
char modoCombustible		= '0'; // 0-Espera orden, 1-Carga Nafta, 2-Carga Gasoil
char modoCarga				= '0'; // 0-Espera orden, 1-Cant de litros, 2-Hasta que llene, 3-Modo abierto
char modoIngresarCantidad	= '0'; // 1-cantidad de litros, //2-cantidad de plata
char estadoDispenser		= '0'; // 1-llenando, 2-Esperando evento de llenado

/*#########Variables del Capture#########*/
uint32_t primerValor		= 0;
uint32_t segundoValor 		= 0;
uint32_t captureAcumulador 	= 0;
uint8_t captureFlag 		= 0;

/*#########Variables del teclado#########*/
int pinesFilas[] = {0,1,2,3};
int pinesColumnas[] = {4,5,6,7};
char teclas[4][4] = {{'1','2','3','A'},
                     {'4','5','6','B'},
                     {'7','8','9','C'},
                     {'*','0','#','D'}};
int cantidadDeDatosIngresadosPorTeclado=0;
char  bufferTeclado[10];



int main(void){

	//uint32_t relojCpu = SystemCoreClock;
	//printf("%d",2);
	resetBufferTeclado();

	LPC_GPIO0->FIODIR     |= (1<<22);

	//Teclado ok
	configurarPuertosTeclado();
	confIntGPIOPorEINT();

	//Timer y capture ok
	configurarCapture();

	//Eint surtidor dejado en plataforma
	configurarEINT2();

	LPC_GPIO0->FIOCLR |= (1<<22);  // prende el led
	while(1){
		//LPC_GPIO0->FIOSET |= (1<<22);  // apaga el led
		//retardoEnSeg(1);
		//LPC_GPIO0->FIOCLR |= (1<<22);  // prende el led
		//retardoEnSeg(1);
/*
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
		estadosAdmin('2');*/

	}

	return 0;
}
/**/

void estadosAdmin(char datoDelTeclado){
	if(modoCombustible == '0' && datoDelTeclado!='#'){//ok
		modoCombustible = datoDelTeclado;
		estadoDispenser=0;
	}
	else if(modoCombustible != '0' && modoCarga == '0' && modoIngresarCantidad == '0'){//ok
		modoCarga = datoDelTeclado;
	}
	else if(modoCombustible != '0' && modoCarga == '1' && modoIngresarCantidad == '0'){
		modoIngresarCantidad = datoDelTeclado;
	}
	else if(modoCombustible!='0' && modoCarga=='2' && modoIngresarCantidad=='0'){
		surtirHastaLlenar();
	}
	else if(modoCombustible!='0' && modoCarga=='2' && modoIngresarCantidad=='0'){
		surtirPorCapture();
	}
	else if(modoIngresarCantidad!='0'){
		if(datoDelTeclado=='#')
		{
			cantidadDeLitrosACargar = atoi(&bufferTeclado[0]) ;//obtenés la cantidad de litros o pesos
			cantidadDeDatosIngresadosPorTeclado = 0;
			resetEstados();
			resetBufferTeclado();
			estadoDispenser = '1';
			//disparás una configuración de timer para que cargue la nafta
			//iniciarCapture(void);
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
void surtirPorCapture(){
	iniciarCapture();
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
		return PRECIO_NAFTA*cantDeCombustible;
	}
	else if(modoCombustible=='2'){
		return PRECIO_GASOIL*cantDeCombustible;
	}
	return -1.0;
}


/*############################################Teclado############################################*/
void configurarPuertosTeclado(){
	int i;
	for(i=0 ; i<4; i++){
		LPC_GPIO2->FIODIR|=(1<<pinesFilas[i]);//pines como output de las filas del teclado
		LPC_GPIO2->FIOSET|=(1<<pinesFilas[i]);//pines de las filas en alto del teclado//Tenía puesto un SET, hay que ver q pasa//OJO!!!
		LPC_GPIO2->FIODIR &=~ (1<<pinesColumnas[i]);//pines como input de las columnas del teclado
		LPC_GPIO2->FIOCLR|=(1<<pinesColumnas[i]);
		LPC_PINCON->PINMODE4 |= (3<<(pinesColumnas[i]*2));//{4,5,6,7};
	}
	//el resto de los pines están como input y pull up asi que las columnas del teclado están configuradas ya...
}

/*condiguración para habilitar interrupciones por GPIO para el teclado matricial*/
void confIntGPIOPorEINT(void){
	for(int i=0; i<4;i++){
		LPC_GPIOINT -> IO2IntEnR |= ((1 << pinesColumnas[i])); //Selecciono la interrupcion por flanco de bajada
		LPC_GPIOINT -> IO2IntClr |= ((1 << pinesColumnas[i])); //Limpia la bandera
	}
	NVIC_SetPriority(EINT3_IRQn, 0); 	//Prioridad para esta interrupcion
	NVIC_EnableIRQ(EINT3_IRQn); 		//Habilita las interrupciones por GPIO
}

/*Detecta la tecla que fue presionada, la podemos llamar desde el main periódicamete o por interrupciones externas*/
void loopTeclado(){
	//Barrido por las filas
	char teclaPresionada=' ';
	int asd=LPC_GPIOINT->IO2IntStatR;
	int nose=0;
	int numCol=0;
	if(LPC_GPIOINT->IO2IntStatR & (1<<4)){numCol=0;}
	else if(LPC_GPIOINT->IO2IntStatR & (1<<5)){numCol=1;}
	else if(LPC_GPIOINT->IO2IntStatR & (1<<6)){numCol=2;}
	else if(LPC_GPIOINT->IO2IntStatR & (1<<7)){numCol=3;}

	for (int nL=0; nL<4; nL++)
	{
	   LPC_GPIO2->FIOCLR|=(1<<pinesFilas[nL]);
	   //Barrido en columnas buscando un LOW
	   if ( ((LPC_GPIO2->FIOPIN) & (1<<pinesColumnas[numCol]))==0 ){//sie stán en low los pines
		   ultimaTeclaPresionada=teclas[nL][numCol];
		   teclaPresionada=teclas[nL][numCol];
		   nL=4;
	   }
	   LPC_GPIO2->FIOSET|=(1<<pinesFilas[nL]);
	 }
	for(int i=0;i<4;i++){
		LPC_GPIO2->FIOSET|=(1<<pinesFilas[i]);
	}
	 retardoEnMs(1000);
	 estadosAdmin(teclaPresionada);

}



void EINT3_IRQHandler(void){
	NVIC_DisableIRQ(EINT3_IRQn);
	loopTeclado();
	for(int i=0; i<4;i++){
		LPC_GPIOINT -> IO2IntClr |= ((1 << pinesColumnas[i])); //Limpia la bandera
	}
	int asd=LPC_GPIOINT->IO2IntStatR;
	NVIC_EnableIRQ(EINT3_IRQn);
}



//##########################TIMER0, TIMER 0 y CAPTURE####################################


//Acá se configura el Capture
void configurarCapture(void) {
	LPC_SC->PCONP        |= (1<<1); 			// Por defecto timer 0 y 1 estan siempre prendidos
	LPC_SC->PCLKSEL0     |= (3<<2); 			// Configuracion del clock de periforico clk/8 = 12,5Mhz
	LPC_PINCON->PINSEL3  |= (3<<20);        	// Configuracion pin 1.26 como capture0.0
	LPC_PINCON->PINMODE3 &= ~(3<<20);         	// Configuracion como pull-up
	LPC_TIM0->CTCR       &= ~(3<<0);			// Timer Mode
	LPC_TIM0->PR 		  = 6250000 - 1;		// Deseado 0,5 segundos = 2 periodo ==>  PR = 12,5Mhz/2 = 6,250Mhz
	LPC_TIM0->CCR        |= (1<<1)|(1<<2);		// Conf capture, interrupcion, carga del timer por flanco de subida y bajada.
	LPC_TIM0->TCR        &= ~(1<<0);			// Timer disabled for counting
	LPC_TIM0->TCR        |= (1<<1);				// Timer reset.
	return;
}

//aca empieza a contar el timer
void iniciarCapture(void) {
	LPC_TIM0->CCR	|= (1<<0);
	LPC_TIM0->TCR   |= (1<<0);		// Timer enabled for counting
	LPC_TIM0->TCR   &= ~(1<<1);		// Timer no reset.
	NVIC_EnableIRQ(TIMER0_IRQn);
	return;
}

void TIMER0_IRQHandler(void)
{
	LPC_TIM0->IR |= (1<<0); //Clear Interrupt Flag
	if(!captureFlag)//TC has overflowed
	{
		primerValor = LPC_TIM0->CR0;
		captureFlag = 1;
	}
	else
	{
		segundoValor 		= LPC_TIM0->CR0;
		captureAcumulador 	+= ( segundoValor - primerValor );
		captureFlag  		= 0;
	}
}

void dashabilitarCapture(void) {
	LPC_TIM0->CCR		 &= ~(1<<0);
	LPC_TIM0->TCR        &= ~(1<<0);			// Timer deshabilitado
	LPC_TIM0->TCR        |= (1<<1);				// Timer reset.

	NVIC_DisableIRQ(TIMER0_IRQn);
	return;
}


//##################EINT 2 manguera reposada en un soporte#####################
void configurarEINT2(){
	LPC_PINCON->PINSEL4 |= (1<<24) ;//p2.12 como EINT2
	LPC_SC->EXTINT      |= (1<<2);   //Limpia bandera de interrupci�n
	LPC_SC->EXTMODE     |= (1<<2); //Selecciona interrupcion por flanco
	LPC_SC->EXTPOLAR    |= (1<<2); //Interrumpe cuando el flanco es de subida
	NVIC_EnableIRQ(EINT2_IRQn);    // Habilita de interrupciones externas.
}

void EINT2_IRQHandler(void){//consigna de EINT
    /*LPC_TIM2->PR = (1 << cont);
    cont++;
    LPC_TIM2->TCR |= (1 << 1);
    LPC_TIM2->TCR &= ~(1 << 1);*/
	dashabilitarCapture();
    LPC_SC->EXTINT |= (1<<2); //Limpia bandera de interrupci�n

    return;
}

