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
#include "lpc17XX_adc.h"
#include "lpc17XX_gpdma.h"
#include "lpc17XX_gpio.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_pinsel.h"
#endif


#include <cr_section_macros.h>
#include "Utils.h"
#include <math.h>
#include <stdlib.h>


//#include "Teclado.h"
#include "Uart3.h"
#include "Adc.h"
#include "Dma_uart3.h"
#define PRECIO_NAFTA 110
#define PRECIO_GASOIL 90
#define CAUDAL_POR_SEG 0.5
#define MAX_CAUDAL 2 //litros por segundo
#define DIGITOS_MAX	4
#define INTERVALO_ADC 0.5

GPDMA_LLI_Type DMA_LLI_Struct;
GPDMA_Channel_CFG_Type GPDMACfg;


void arreglarCadena(char *cadena);
void modificarMensajePrecioYLitros(void);
void estadosAdmin(char datoDelTeclado);
void resetEstados(void);
void resetBufferTeclado(void);
float calcularCostoPorTiempo(float cantDeCombustible);
void configurarPuertosTeclado(void);
void loopTeclado(void);
void confIntGPIOPorEINT(void);

/* CAPTURE PARA MANGUERA */
//void TIMER0_IRQHandler(void);
void configurarCapture(void);
void iniciarCapture(void);
void deshabilitarCapture(void) ;
void calcularCombustiblePorCaudal(void);
void calcularMontoAPagar(void);

void configurarEINT2();
void configuracionDMAMain(void);

float global_adc=0;//Es el caudal en litros/seg del surtidor en tiempo REAL

char ingresadoPorTeclado[10]="";

uint16_t cantidadDeLitrosACargar=0; 
float cantidadDeLitrosCargados=0.0;

char ultimaTeclaPresionada=' ';
char modoCombustible		= '0'; // 0-Espera orden, 1-Carga Nafta, 2-Carga Gasoil
char modoCarga				= '0'; // 0-Espera orden, 1-Cant de litros, 2-Hasta que llene, 3-Modo abierto
char modoIngresarCantidad	= '0'; // 1-cantidad de litros, //2-cantidad de plata
char estadoDispenser		= '0'; // 0-Deshabilitado, //1-Operativo, //2-Surtiendo nafta

/*#########Variables del Capture#########*/
uint32_t primerValor		= 0;
uint32_t segundoValor 		= 0;
uint32_t captureAcumulador 	= 0;
int captureFlag 			= 0;

/*#########Variables del teclado#########*/
int pinesFilas[] = {0,1,2,3};
int pinesColumnas[] = {4,5,6,7};
char teclas[4][4] = {{'1','2','3','A'},
                     {'4','5','6','B'},
                     {'7','8','9','C'},
                     {'*','0','#','D'}};
int cantidadDeDatosIngresadosPorTeclado=0;//es el indice del buffer del teclado para ir cagando los valores de los litros en el modo ingresar cantidad
char  bufferTeclado[10];
int montoAPagar=0; //float antes

/*#########Variables del ADC###########*/
//uint32_t acumuladorConversion 	= 0; // AL PEDO
uint16_t numeroMuestras 		= 0;//por match
// uint32_t promedioConversion 	= 0; //AL PEDO

/*#########UART3 precio y litros ######*/
uint8_t mensajePrecioYLitros[] = {"\r$0000 0000L\r"};
//uint8_t precioYLitros[] = "\r$0000 0000L"; PROBAR DSP ESTE MSJ


int main(void){

	LPC_GPIO0->FIODIR     |= (1<<22);

	retardoEnSeg(1);

	//todos los pines por defecto vienen com oentrada, pull-up y GPIO
	//sensor de llenado
	//LPC_PINCON->PINMODE0 |=(3<<18);//pongo pull down


	//Teclado ok
	resetBufferTeclado();
	configurarPuertosTeclado();
	confIntGPIOPorEINT();//configuras



		//***DMA UART***
	//char nose [] = {""};

	//retardoEnSeg(1);
	//configuracionDmaCanalUart(&nose);//este los carga
	//retardoEnSeg(1);
	//activarDmaCanalUart();//envia los datos


	//***ADC individual***
	//cantidadDeLitrosACargar=122;
	configurarAdc();//stf
	retardoEnMs(20);
	//habilitarAdcPorMatch();
	//conversionAhora();

	//Timer y capture ok

	configurarCapture();//stf
	retardoEnMs(20);
	//iniciarCapture();//stf

	LPC_GPIO0->FIOCLR |= (1<<22);  // prende el led


	/*UART SIMPLE*/
	configurarUart3();
	uint8_t info[] = "Bienvenidos al programa \n\r"
					 "Ingrese 1 para Nafta \n\r"
					 "Ingrese 2 para Gasoil \n\r";
	UART_Send(LPC_UART3,info,sizeof(info),BLOCKING);

	/*###############--------------------##################---------------------################*/
	configuracionDMAMain();

	//secuencia de test1
	/*estadosAdmin('1');
	estadosAdmin('1');
	estadosAdmin('1');
	//ingresamos el numero

	estadosAdmin('3');
	estadosAdmin('0');
	//estadosAdmin('2');
	estadosAdmin('#');*/


	//------------------------
	//secuencia de test2
	/*estadosAdmin('1');
	estadosAdmin('2');*/


	//------------------------
	//secuencia de test3
	/*estadosAdmin('1');
	estadosAdmin('3');*/


	//uint8_t  info[]="12\r\n";
	//enviarCadena(info);

	while(1){
		//enviarCadena(info);

	}

	return 0;
}
/**/

void estadosAdmin(char datoDelTeclado){
	if(modoCombustible == '0' && datoDelTeclado!='#'){//ok
		uint8_t info[] = "--------------------- \n\r"
						 "Ingrese 1 para cargar por cantidad de litros  \n\r"
						 "Ingrese 2 llenar tanque \n\r"
						 "Ingrese 3 para modo libre \n\r";
		UART_Send(LPC_UART3,info,sizeof(info),BLOCKING);

		modoCombustible = datoDelTeclado;
		estadoDispenser=0;
	}
	else if(modoCombustible != '0' && modoCarga == '0' && modoIngresarCantidad == '0'){//ok
		modoCarga = datoDelTeclado;
		if (modoCarga=='2'){//modo llenar tanque
			configuracionDMAMain();
			uint8_t info[] = "--------------------- \n\r"
							 "Modo de llenar tanque habilitado  \n\r";
			UART_Send(LPC_UART3,info,sizeof(info),BLOCKING);
			retardoEnMs(10);
			iniciarCapture();
			configurarEINT2();
			estadoDispenser = '1';
			activarDmaCanalUart();
		}
		else if(modoCarga=='3'){//modo carga libre
			configuracionDMAMain();
			uint8_t info[] = "--------------------- \n\r"
										 "Modo de carga libre habilitado  \n\r";//STEFANO
			UART_Send(LPC_UART3,info,sizeof(info),BLOCKING);
			retardoEnMs(10);
			iniciarCapture();
			configurarEINT2();
			estadoDispenser = '1';
			activarDmaCanalUart();
		}
	}
	else if(modoCombustible != '0' && modoCarga == '1' && modoIngresarCantidad == '0'){
		uint8_t info[] = "--------------------- \n\r"
						 "Ingrese la cantidad de litros seguido de #  \n\r";
		UART_Send(LPC_UART3,info,sizeof(info),BLOCKING);
		//estadoDispenser='0'; // ESTO PUEDE VENIR ACA
		modoIngresarCantidad = datoDelTeclado;			// ENTRADA AL VICIO
	}
	else if(modoIngresarCantidad!='0'){
		if(datoDelTeclado=='#')//yo ya ingres?? los litros que quiero y disparo todo lo necesario
		{
			configuracionDMAMain();
			cantidadDeLitrosACargar = atoi(&bufferTeclado[0]) ;//obten??s la cantidad de litros o pesos
			cantidadDeDatosIngresadosPorTeclado = 0;

			resetBufferTeclado();// ok
			estadoDispenser = '1';//dispenser operativo
			//dispar??s una configuraci??n de timer para que cargue la nafta
			iniciarCapture();//habilito el capture para el gatillo
			uint8_t info[] = "--------------------- \n\r"
							 "Surtidor habilitado  \n\r";
			UART_Send(LPC_UART3,info,sizeof(info),BLOCKING);
			//configurarEINT2();

			activarDmaCanalUart();
		}
		else{
			bufferTeclado[cantidadDeDatosIngresadosPorTeclado]=datoDelTeclado;
			cantidadDeDatosIngresadosPorTeclado++;
			estadoDispenser='0';
		}

	}
	else if(modoCombustible!='0' && modoCarga!='0'  && datoDelTeclado!='1'){//si es dsitinto de 1 espero q sea un 2 o 3
		estadoDispenser='1';
	}
	return;
}

void resetEstados(void){
	modoCombustible='0';//0-Espera orden, 1-Carga Nafta, 2-Carga Gasoil
	modoCarga='0';//0-Espera orden, 1-Cant de litros, 2-Hasta que llene, 3-Modo abierto
	modoIngresarCantidad='0';//1-cantidad de litros, //2-cantidad de plata
	estadoDispenser='0';//1-llenando, 2-Esperando evento de llenado
	captureFlag=0;
	cantidadDeLitrosCargados=0;
	cantidadDeLitrosACargar=0;
	montoAPagar=0;
	global_adc=0.0;
}
void resetBufferTeclado(void){
	for(int i=0; i<10;i++){
		bufferTeclado[i]=' ';
	}
}

//cantidad de combustible en litros
float calcularCostoPorTiempo(float cantDeCombustible){
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
		LPC_GPIO2->FIOSET|=(1<<pinesFilas[i]);//pines de las filas en alto del teclado//Ten??a puesto un SET, hay que ver q pasa//OJO!!!
		LPC_GPIO2->FIODIR &=~ (1<<pinesColumnas[i]);//pines como input de las columnas del teclado
		LPC_GPIO2->FIOCLR|=(1<<pinesColumnas[i]);
		LPC_PINCON->PINMODE4 |= (3<<(pinesColumnas[i]*2));//{4,5,6,7};
	}
	//el resto de los pines est??n como input y pull up asi que las columnas del teclado est??n configuradas ya...
}

/*condiguraci??n para habilitar interrupciones por GPIO para el teclado matricial*/
void confIntGPIOPorEINT(void){
	for(int i=0; i<4;i++){
		LPC_GPIOINT -> IO2IntEnR |= ((1 << pinesColumnas[i])); //Selecciono la interrupcion por flanco de bajada
		LPC_GPIOINT -> IO2IntClr |= ((1 << pinesColumnas[i])); //Limpia la bandera
	}
	NVIC_SetPriority(EINT3_IRQn, 0); 	//Prioridad para esta interrupcion
	NVIC_EnableIRQ(EINT3_IRQn); 		//Habilita las interrupciones por GPIO
}

/*Detecta la tecla que fue presionada, la podemos llamar desde el main peri??dicamete o por interrupciones externas*/
void loopTeclado(){
	//Barrido por las filas
	char teclaPresionada=' ';
	int numCol=0;
	if(LPC_GPIOINT->IO2IntStatR & (1<<4)){numCol=0;}
	else if(LPC_GPIOINT->IO2IntStatR & (1<<5)){numCol=1;}
	else if(LPC_GPIOINT->IO2IntStatR & (1<<6)){numCol=2;}
	else if(LPC_GPIOINT->IO2IntStatR & (1<<7)){numCol=3;}

	for (int nL=0; nL<4; nL++)
	{
	   LPC_GPIO2->FIOCLR|=(1<<pinesFilas[nL]);
	   //Barrido en columnas buscando un LOW
	   if ( ((LPC_GPIO2->FIOPIN) & (1<<pinesColumnas[numCol]))==0 ){//sie st??n en low los pines
		   ultimaTeclaPresionada=teclas[nL][numCol];
		   teclaPresionada=teclas[nL][numCol];
		   nL=4;
	   }
	   LPC_GPIO2->FIOSET|=(1<<pinesFilas[nL]);
	 }
	for(int i=0;i<4;i++){
		LPC_GPIO2->FIOSET|=(1<<pinesFilas[i]);
	}
	 retardoEnMs(20);
	 estadosAdmin(teclaPresionada);

}



void EINT3_IRQHandler(void){
	NVIC_DisableIRQ(EINT3_IRQn);
	loopTeclado();
	for(int i=0; i<4;i++){
		LPC_GPIOINT -> IO2IntClr |= ((1 << pinesColumnas[i])); //Limpia la bandera
	}
	NVIC_EnableIRQ(EINT3_IRQn);
}



//########################## TIMER 0 y CAPTURE####################################


//Ac?? se configura el Capture
void configurarCapture(void) {
	LPC_SC->PCONP        |= (1<<1); 			// Por defecto timer 0 y 1 estan siempre prendidos
	LPC_SC->PCLKSEL0     |= (3<<2); 			// Configuracion del clock de periforico clk/8 = 12,5Mhz
	LPC_PINCON->PINSEL3  |= (3<<20);        	// Configuracion pin 1.26 como capture0.0
	LPC_PINCON->PINMODE3 |= (3<<20);         	// Configuracion como pull-down
	LPC_TIM0->CTCR       &= ~(3<<0);			// Timer Mode
	LPC_TIM0->PR 		  = 1250000  - 1;		// Deseado 0,1 segundos (menor q el adc q es de 0.5) = 10 periodo ==>  PR = 12,5Mhz/10 = 1,25Mhz
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

//Handler de capture o Gatillo surtidor
void TIMER0_IRQHandler(void)
{

	LPC_TIM0->IR |= (1<<0); //Clear Interrupt Flag
	if(captureFlag==0)//Cuando aprieto el bot??n entro (est?? en cero)
	{
		estadoDispenser='2';
		primerValor = LPC_TIM0->CR0;
		captureFlag = 1;//gatillo presionado
		habilitarAdcPorMatch();
		//OJO ver DMA
	}
	else//cuando suelto el bot??n entro
	{
		deshabilitarAdcPorMatch();
		estadoDispenser='1';
		segundoValor 		= LPC_TIM0->CR0;
		captureAcumulador 	+= ( segundoValor - primerValor );//OJO capaz q lo podemos sacar
		captureFlag  		= 0;//gatillo suelto
		conversionAhora();
		// QUIZAS HAGA FALTA UN DELAY
		if(numeroMuestras>0){
			cantidadDeLitrosCargados = cantidadDeLitrosCargados +(global_adc*( (segundoValor*0.1) -(((numeroMuestras-1)*0.5)+(primerValor*0.1))  )  );
		}
		else{
			cantidadDeLitrosCargados = cantidadDeLitrosCargados +(global_adc*( (segundoValor*0.1) -(((numeroMuestras)*0.5)+(primerValor*0.1))  )  );
		}
		calcularMontoAPagar();
		modificarMensajePrecioYLitros();
     	primerValor=0;
     	segundoValor=0;
     	numeroMuestras=0;
		//capaz tengamos q resetear captures  (1er y 2do valor)
	}
}

void calcularCombustiblePorCaudal(){


}

void deshabilitarCapture(void) {
	LPC_TIM0->CCR		 &= ~(1<<0);
	LPC_TIM0->TCR        &= ~(1<<0);			// Timer deshabilitado
	LPC_TIM0->TCR        |= (1<<1);				// Timer reset.

	NVIC_DisableIRQ(TIMER0_IRQn);
	return;
}


//##################EINT 2 manguera reposada en un soporte#####################
void configurarEINT2(){
	LPC_PINCON->PINSEL4 |= (1<<24) ;		// p2.12 como EINT2
	LPC_PINCON->PINMODE4 |= (3<<24);		// pull-down
	LPC_SC->EXTMODE     |= (1<<2); 			// Selecciona interrupcion por flanco
	LPC_SC->EXTPOLAR    &= ~(1<<2); 			// Interrumpe cuando el flanco es de subida
	LPC_SC->EXTINT      |= (1<<2);   		// Limpia bandera de interrupci???n
	NVIC_EnableIRQ(EINT2_IRQn);				// Habilita de interrupciones externas.
}

//suponemos que cuando pogo en la base, no estoy con el gatillo apretado
void EINT2_IRQHandler(void){//consigna de EINT
	NVIC_DisableIRQ(EINT2_IRQn);
	deshabilitarAdcPorMatch();
	deshabilitarCapture();
    calcularMontoAPagar();
    resetEstados();
    desactivarDmaCanalUart();
    LPC_SC->EXTINT |= (1<<2); //Limpia bandera de interrupcion
    return;
}

void calcularMontoAPagar()
{
	if(modoCombustible=='1'){
		montoAPagar=cantidadDeLitrosCargados*PRECIO_NAFTA;
	}
	else if(modoCombustible=='2'){
		montoAPagar=cantidadDeLitrosCargados*PRECIO_GASOIL;
	}


}


//##################Realiza la conversion, saca el precio y verifica que ya no este habilitado#####################
void ADC_IRQHandler(void) {
	if( LPC_ADC->ADSTAT & 1 ){
		numeroMuestras += 1;
		global_adc= ( ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0) * 2.0)/4096;
		if (modoIngresarCantidad=='1' && captureFlag==1){
			cantidadDeLitrosCargados=cantidadDeLitrosCargados+(global_adc*0.5);
			calcularMontoAPagar();
			modificarMensajePrecioYLitros();
		}//si el captureFlag es ==2 se hace en el timer dentro del else
		if(modoIngresarCantidad=='1' && cantidadDeLitrosCargados>=cantidadDeLitrosACargar){//solo me fijo en los litros cargados si estoy en ese modo xq si no va a haber cualquier cosa
			deshabilitarAdcPorMatch();
			deshabilitarCapture();
			resetEstados();
			desactivarDmaCanalUart();
			//Se deshabilita la EINT en el handler
		}
		if(modoCarga=='2'){
			cantidadDeLitrosCargados=cantidadDeLitrosCargados+(global_adc*0.5);
			if( LPC_GPIO0->FIOPIN & (1<<9) ){//se fija si se llen?? el tanque
				deshabilitarAdcPorMatch();
				deshabilitarCapture();
				resetEstados();
				desactivarDmaCanalUart();
				//deshabilitarEINT;---PENDIENTE STEFANO/SANTI
			}
			calcularMontoAPagar();
			modificarMensajePrecioYLitros();
		}
		if(modoCarga=='3'){
				cantidadDeLitrosCargados=cantidadDeLitrosCargados+(global_adc*0.5);
				calcularMontoAPagar();
				modificarMensajePrecioYLitros();
		}

		//acumuladorConversion += ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);

		//para pasar por UART OJO, ver DMA
		//uint8_t ascciValue[4];								// Arreglo de valores de la conversion
    	//itoa(conversionValor, ascciValue, 10);				// Conversion de entero a string

    	/*if(!captureFlag) {
    		promedioConversion = acumuladorConversion / numeroMuestras;
    		deshabilitarAdcPorMatch();
    	}*/

	}
	//LPC_ADC->ADSTAT &= ~( 1 << 16 ); // Bajo la bandera de interrupcion del ADC // ver lo del flag d einterrupcion de ADC
	//xq este registro es de s??lo lectura
	return;
}


//################## Modificar el mensaje de precio y litros cargados #####################
void modificarMensajePrecioYLitros() {
	uint8_t precio[DIGITOS_MAX]={"0000"};
	uint8_t litros[DIGITOS_MAX]={"0000"};               // POSIBLE PROBLEMA, POR CONVERTIR A STRING UN FLOAT
	itoa(montoAPagar, precio, 10);				// Conversion de entero a string de montoAPagar
	itoa((int)cantidadDeLitrosCargados, litros, 10);	// Conversion de entero a string de cantidadDeLitrosCargados
	//$0000 0000L // uint8_t mensajePrecioYLitros[] = "\r$0000 0000L\n\r";
	arreglarCadena(precio);
	arreglarCadena(litros);
	for(int i = 0; i<DIGITOS_MAX; i++) {
		mensajePrecioYLitros[i+2] = precio[i];
		mensajePrecioYLitros[i+7] = litros[i];
	}
}

void configuracionDMAMain(void){

		DMA_LLI_Struct.SrcAddr= (uint32_t)mensajePrecioYLitros;
		DMA_LLI_Struct.DstAddr= (uint32_t)&(LPC_UART3->THR);
		DMA_LLI_Struct.NextLLI= (uint32_t)&DMA_LLI_Struct;
		DMA_LLI_Struct.Control= sizeof(mensajePrecioYLitros)
								  //default src width 8 bit
						 	 	  //default dest width 8 bit
						| (1<<26) //source increment
						| (2<<12)
						;
		// Desabilito la interrupcion de GDMA
		NVIC_DisableIRQ(DMA_IRQn);

		//configuracionDmaCanalUart(&mensajePrecioYLitros);


		//Prepare DMA link list item structure


		GPDMA_Init();
		// Setup GPDMA channel --------------------------------
		// canal 0
		GPDMACfg.ChannelNum = 0;
		// Origen
		GPDMACfg.SrcMemAddr = (uint32_t)(mensajePrecioYLitros);
		// Destino
		GPDMACfg.DstMemAddr = 0;
		// Tamano de transferencia
		GPDMACfg.TransferSize = sizeof(mensajePrecioYLitros);
		GPDMACfg.TransferWidth = 0;
		// Tipo de transferencia
		GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
		GPDMACfg.SrcConn = 0;
		// Destino Periferico
		GPDMACfg.DstConn = GPDMA_CONN_UART3_Tx;
		// Asocio Linker List Item
		GPDMACfg.DMALLI = (uint32_t)&DMA_LLI_Struct;
		// Setup channel with given parameter
		GPDMA_Setup(&GPDMACfg);
}

void arreglarCadena(char *cadena){
    int posicion = -1;
	for(int i=DIGITOS_MAX-1; i>0; i--){
	    if(cadena[i] == '\0' || cadena[i] == '\n'){
	        posicion = i;

	    }
    }
    if(posicion > 0) {
        for(int k = 0; k<(DIGITOS_MAX - posicion); k++) {
        	cadena[3] = cadena[2];
    	    cadena[2] = cadena[1];
    	    cadena[1] = cadena[0];
    	    cadena[0] = '0';
        }
    }
}

