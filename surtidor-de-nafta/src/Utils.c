#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

void retardo(int tiempoEnSeg){
	for(int contador=0 ; contador<(tiempoEnSeg*10000000); contador++){};
}

void retardoEnMs(int tiempoEnMs){
	for(int contador=0 ; contador<tiempoEnMs; contador++){};
}
