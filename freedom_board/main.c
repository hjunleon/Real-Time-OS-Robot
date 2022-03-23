/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#include "utils.h"
#include "UART/uart.h"
#include "motor/motor.h"
#include "ultrasound/ultrasound.h"
 
 unsigned int isAuto =  0;///0;
 
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



int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	//disable_motor();
	initMotor();
	if (isAuto){
		//stop();
		initUltrasound();
		startUltrasound();
		/*
			Move the bot close enough, then make a circle turn.
		*/
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
