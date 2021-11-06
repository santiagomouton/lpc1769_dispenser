#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17XX_adc.h"
#include "lpc17XX_timer.h"
#include "lpc17XX_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_pinsel.h"
#endif

#include <cr_section_macros.h>
#include "Adc.h"

/*	HACE USO DE EINT0 PARA INICIO DE CONVERSION */
void configurarAdc(void) {
	/* configuracion puerto de entrada para conversion 			 */
	PINSEL_CFG_Type pinAnalog;
	pinAnalog.Portnum = PINSEL_PORT_0;
	pinAnalog.Pinnum = PINSEL_PIN_23;
	pinAnalog.Funcnum = PINSEL_FUNC_1;
	pinAnalog.Pinmode = PINSEL_PINMODE_TRISTATE;
	PINSEL_ConfigPin(&pinAnalog);

	TIM_TIMERCFG_Type struct_timer;
	TIM_MATCHCFG_Type struct_match;

	struct_timer.PrescaleOption = TIM_PRESCALE_USVAL;
	struct_timer.PrescaleValue  = 10000;				// 	El timer aumenta cada 10mS

	struct_match.MatchChannel		= 0;
	struct_match.IntOnMatch			= DISABLE;
	struct_match.ResetOnMatch		= ENABLE;
	struct_match.StopOnMatch		= DISABLE;
	struct_match.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	struct_match.MatchValue			= 25;
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &struct_timer);
	TIM_ConfigMatch(LPC_TIM1, &struct_match);
	TIM_Cmd(LPC_TIM1, DISABLE);

	ADC_Init(LPC_ADC, 200000);							// Frecuencia de 200k
	ADC_ChannelCmd(LPC_ADC,0,ENABLE);					// Habilita el canal 0
	ADC_PowerdownCmd(LPC_ADC, 0);
	return;
}

void habilitarAdcPorMatch(void) {
	ADC_IntConfig(LPC_ADC,ADC_ADINTEN0,ENABLE);			// Habilito las interrupciones por canal 0
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT10);			// Configuro inicio de conversion por EINT0
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_FALLING);	// Arranca por Mat1.0 en bajada
	ADC_PowerdownCmd(LPC_ADC, 1);
	TIM_Cmd(LPC_TIM1, ENABLE);
	NVIC_EnableIRQ(ADC_IRQn);
	return;
}

void deshabilitarAdcPorMatch(void) {
	ADC_IntConfig(LPC_ADC,ADC_ADINTEN0,RESET);		// Deshabilito las interrupciones
	ADC_PowerdownCmd(LPC_ADC, 0);					// Configuro ADC como no operativo
	TIM_Cmd(LPC_TIM1, DISABLE);
	NVIC_DisableIRQ(ADC_IRQn);
	return;
}

void conversionAhora(void) {
	ADC_IntConfig(LPC_ADC,ADC_ADINTEN0,ENABLE);		// Habilito las interrupciones por canal 0
	ADC_PowerdownCmd(LPC_ADC, 1);					// ADC power on
	LPC_ADC->ADCR |=  ADC_START_NOW;     			// Convertir Ahora
	NVIC_EnableIRQ(ADC_IRQn);
}

