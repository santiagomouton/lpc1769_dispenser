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
		LPC_GPIO2->FIOSET|=(1<<pinesFilas[i]);//pines de las filas en alto del teclado
	}
	//el resto de los pines están como input y pull up asi que las columnas del teclado están configuradas ya...
}

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
