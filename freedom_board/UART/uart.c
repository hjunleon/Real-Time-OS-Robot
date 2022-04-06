# include <MKL25Z4.h>
#include "uart.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"



/*
void UART2_Transmit_Poll(uint8_t data)
{
	while(!(UART2->S1 & UART_S1_TDRE_MASK));
	UART2->D = data;
}*/
uint8_t UART2_Receive_Poll(void)
{
	while(!(UART2->S1 & UART_S1_RDRF_MASK)){
		osDelay(50);
	};
	return (UART2->D);
}

uint8_t UART2_IRQHandler(void) { 
 NVIC_ClearPendingIRQ(UART2_IRQn); 
 if (UART2_S1 & UART_S1_RDRF_MASK) { 
	 //  push UART command queue
  return (UART2->D); 
	 
  //MessageObject_t messageObject; 
  //messageObject.message = serialValue; 
  //osMessageQueuePut(brainMessageQueue, &messageObject, 0, 0); 
 } 
}

void initUART2(uint32_t baud_rate)
{
	uint32_t divisor, bus_clock;
	SIM->SCGC4 |= SIM_SCGC4_UART2_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	//PORTE->PCR[UART_TX_PORTE22] &= ~PORT_PCR_MUX_MASK;
	//PORTE->PCR[UART_TX_PORTE22] |= PORT_PCR_MUX(4);

	PORTE->PCR[UART_RX_PORTE23] &= ~PORT_PCR_MUX_MASK;
	PORTE->PCR[UART_RX_PORTE23] |= PORT_PCR_MUX(4);
	
	UART2->C2 &= ~((UART_C2_TE_MASK) | (UART_C2_RE_MASK));
	
	bus_clock = (DEFAULT_SYSTEM_CLOCK)/2;
	divisor = bus_clock / (baud_rate * 16);
	UART2->BDH = UART_BDH_SBR(divisor >> 8);
	UART2->BDL = UART_BDL_SBR(divisor);
	
	UART2->C1 = 0;
	UART2->S2 = 0;
	UART2->C3 = 0;
	
	UART2->C2 |= UART_C2_RE_MASK; //(UART_C2_TE_MASK)
}
