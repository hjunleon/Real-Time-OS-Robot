/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "utils.c"
#include "UART/uart.c"
#include "motor/motor.c"
#include "ultrasound/ultrasound.c"
 
 
 unsigned int isAuto = 0;
 
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
 

void start (void *argument) {

  for (;;) {

	}
}

void end (void *argument) {

  for (;;) {

	}
}

void stop (void *argument) {

	uint8_t rx_data;
  for (;;) {
		rx_data = UART2_Receive_Poll();
		if(rx_data == 0x50) {
			TPM1_C0V = 0;
			TPM1_C1V = 0;
			TPM2_C0V = 0;
			TPM2_C1V = 0;
		}
	}
}

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	initMotor();
	if (isAuto){
		initUltrasound();
		startUltrasound();
	} else {
		initUART2(BAUD_RATE);
	}
	uint8_t rx_data;
	
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	osThreadNew(left_motor, NULL, NULL);
	osThreadNew(right_motor, NULL, NULL);
	osThreadNew(stop, NULL, NULL);
  osKernelStart();                      // Start thread execution
	
}
