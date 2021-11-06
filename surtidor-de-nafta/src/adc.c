#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17XX_adc.h"
#include "lpc17XX_gpdma.h"
#include "lpc17XX_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_pinsel.h"
#endif

#include <cr_section_macros.h>

//##################Conversion analogico-digital y paso de datos a UART#######
/*	HACE USO DE EINT0 PARA INICIO DE CONVERSION */
void configurarAdc(void) {
	/* configuracion puerto de entrada para conversion 			 */
	PINSEL_CFG_Type pinAnalog;
	pinAnalog.Portnum = PINSEL_PORT_0;
	pinAnalog.Pinnum = PINSEL_PIN_23;
	pinAnalog.Funcnum = PINSEL_FUNC_1;
	pinAnalog.Pinmode = PINSEL_PINMODE_TRISTATE;
	PINSEL_ConfigPin(&pinAnalog);
	/* configuracion puerto de entrada para interrupcion externa */
	PINSEL_CFG_Type pinEINT0;
	pinAnalog.Portnum = PINSEL_PORT_2;
	pinAnalog.Pinnum = PINSEL_PIN_10;
	pinAnalog.Funcnum = PINSEL_FUNC_1;
	pinAnalog.Pinmode = PINSEL_PINMODE_PULLUP;
	PINSEL_ConfigPin(&pinEINT0);
	/* configuracion con driver del ADC 						 */
	ADC_Init(LPC_ADC, 200000);						// Frecuencia de 200k
	ADC_ChannelCmd(LPC_ADC,0,ENABLE);				// Habilita el canal 0
	ADC_PowerdownCmd(LPC_ADC, 0);
	return;
}

void habilitarAdc(void) {
	//ADC_ChannelCmd(LPC_ADC,0,ENABLE);
	ADC_StartCmd(LPC_ADC, ADC_START_ON_EINT0);		// Configuro inicio de conversion por EINT0
	ADC_IntConfig(LPC_ADC,ADC_ADINTEN0,ENABLE);		// Habilito las interrupciones por canal 0
	return;
}

void deshabilitarAdc(void) {
	ADC_PowerdownCmd(LPC_ADC, RESET);				// Configuro ADC como no operativo
	ADC_IntConfig(LPC_ADC,ADC_ADINTEN0,RESET);		// Deshabilito las interrupciones
	return;
}

