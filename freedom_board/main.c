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

#include "led.h"
 
#define MOVEMENT(x) ((uint8_t)(x)) >> 4 
#define FORWARD_MASK 0x4
#define BACKWARD_MASK 0x5
#define LEFT_MASK 0x6
#define RIGHT_MASK 0x7
#define AUTO_COMMAND 0x80
#define NOAUTO_COMMAND 0x81
 
unsigned int isAuto =  0;///0;
int state = 0;// 0 for stationary & 1 for moving

#define QUEUE_SIZE 4
osMessageQueueId_t motorMQ;
osMutexId_t motorMutex;
osSemaphoreId_t autoStopSemaphore;
osSemaphoreId_t autoStartSemaphore;
osSemaphoreId_t ultrasonicSemaphore;

const osThreadAttr_t uartPriority = {
		.priority = osPriorityAboveNormal//osPriorityHigh4
};

const osThreadAttr_t motorPriority = {
		.priority = osPriorityAboveNormal
};
 const osThreadAttr_t autoPriority = {
		.priority = osPriorityHigh
};
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

unsigned int get_level(uint8_t rx_data){
	rx_data = rx_data & 0xF;
	if (rx_data == 1){
		return 1;
	} else if (rx_data == 2) {
		return 2;
	} else if (rx_data == 3) {
		return 3;
	} else if (rx_data == 4) {
		return 4;
	} else if (rx_data == 5) {
		return 5;
	} 
	return 0;
}

void handle_UART(void *argument){
	uint8_t rx_data;
	unsigned int level = 0;
	motor_cmd motorMsg;
  for (;;) {
		rx_data = UART2_Receive_Poll(); //0x44;//
		if (!isAuto){
			uint8_t move = MOVEMENT(rx_data);
			if (move == FORWARD_MASK){
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STOP;
					state = 0;
				} else {
					motorMsg.direction = FORWARD;
					state = 1;
				}
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			} else if (move == BACKWARD_MASK) {
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STOP;
					state = 0;
				} else {
					motorMsg.direction = BACKWARD;
					state = 1;
				}
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			} else if (move == LEFT_MASK) {
				state = 1;
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STRAIGHT;
				} else {
					motorMsg.direction = LEFT;
				}
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			} else if (move == RIGHT_MASK) {
				state = 1;
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STRAIGHT;
				} else {
					motorMsg.direction = RIGHT;
				}
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			} else if (rx_data == AUTO_COMMAND) {
				//osSemaphoreAcquire(autoStopSemaphore, 0);
				osSemaphoreRelease(autoStartSemaphore);
				isAuto = 1;
			}
		} else {
			if (rx_data == NOAUTO_COMMAND){
				isAuto = 0;
				osSemaphoreAcquire(autoStartSemaphore,osWaitForever);
				//osSemaphoreRelease(autoStopSemaphore);
			}
		}
		
	}
}

void motor_thread(void *argument){
	motor_cmd motorMsg;
	for(;;){
		osMessageQueueGet(motorMQ, &motorMsg, NULL, osWaitForever);
		osMutexAcquire(motorMutex,osWaitForever);
		set_motors(motorMsg);
		osDelay(50);
		osMutexRelease(motorMutex);
	}
}

void ultra_thread(void *argument){
	for (;;) {

	}
}

void auto_thread(void *argument){
	unsigned int isRun = 0;
	for (;;) {
		if(!isRun){
			osSemaphoreAcquire(autoStartSemaphore, osWaitForever);
		}
		isRun = 1;
		motor_cmd motorMsg;
		motorMsg.direction = LEFT;
		motorMsg.level = 5;
		osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
		
		if(!isAuto){
			motorMsg.direction = STRAIGHT;
			motorMsg.level = 0;
			osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			//motorMsg.direction = STOP;
			//motorMsg.level = 0;
			//osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			osSemaphoreRelease(autoStartSemaphore);
			isRun = 0;
			
		}
	}
}

//_____
void redLED_thread(void *argument){
	for (;;) {
		switch (state) {
			case 1: //moving
				redLedMove();
			break;
			default: //stopped
				redLedStop();
			break;
				
		}
	}
}

void green_thread(void *argument){
	for (;;) {
		switch (state) {
			case 1: //moving
				greenLedMove();
			break;
			default: //stopped
				greenLedStop();
			break;
				
		}
	}
}
//_____

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
	//disable_motor();
	initMotor();
	initUART2(BAUD_RATE);
	/*
	if (isAuto){
		//stop();
		initUltrasound();
		startUltrasound();
		osThreadNew(ultra_thread, NULL, NULL);
		
			//Move the bot close enough, then make a circle turn.
		
	} else {
		
	}*/
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	motorMQ = osMessageQueueNew(QUEUE_SIZE, sizeof(motor_cmd), NULL);  //8
	autoStartSemaphore = osSemaphoreNew(1, 0, NULL);
	ultrasonicSemaphore = osSemaphoreNew(1, 0, NULL);
	motorMutex = osMutexNew(NULL);
	osThreadNew(handle_UART, NULL, &uartPriority); //NULL
	osThreadNew(motor_thread, NULL, &motorPriority);
	//osThreadNew(auto_thread, NULL, NULL);
	osThreadNew(redLED_thread, NULL, NULL);
	osThreadNew(greenLED_thread, NULL, NULL);
  osKernelStart();// Start thread execution
	for(;;){}
}
