#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include "Utils.h"


int pinesFilas[] = {9,8,7,6};
int pinesColumnas[] = {5,4,3,2};
char teclas[4][4] = {{'1','2','3','A'},
                     {'4','5','6','B'},
                     {'7','8','9','C'},
                     {'*','0','#','D'}};


void configurarPuertosTeclado(){
	int i;
	for(i=0 ; i<4; i++){
		LPC_GPIO2->FIODIR|=(1<<pinesFilas[i]);//pines como output de las filas del teclado
		LPC_GPIO2->FIOCLR|=(1<<pinesFilas[i]);//pines de las filas en alto del teclado//set en el otro
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
