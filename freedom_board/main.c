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
 
 
#define MOVEMENT(x) ((uint8_t)(x)) >> 4 
#define FORWARD_MASK 0x4
#define BACKWARD_MASK 0x5
#define LEFT_MASK 0x6
#define RIGHT_MASK 0x7
#define AUTO_COMMAND 0x80
 
unsigned int isAuto =  0;///0;

#define QUEUE_SIZE 4
osMessageQueueId_t motorMQ;
osSemaphoreId_t autoModeSemaphore;
osSemaphoreId_t ultrasonicSemaphore;

const osThreadAttr_t uartPriority = {
		.priority = osPriorityHigh4
};

const osThreadAttr_t motorPriority = {
		.priority = osPriorityAboveNormal
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
		if (!isAuto){
			rx_data = UART2_Receive_Poll(); //0x44;//
			uint8_t move = MOVEMENT(rx_data);
			if (move == FORWARD_MASK){
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STOP;
				} else {
					motorMsg.direction = FORWARD;
				}
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			} else if (move == BACKWARD_MASK) {
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STOP;
				} else {
					motorMsg.direction = BACKWARD;
				}
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			} else if (move == LEFT_MASK) {
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STRAIGHT;
				} else {
					motorMsg.direction = LEFT;
				}
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			} else if (move == RIGHT_MASK) {
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STRAIGHT;
				} else {
					motorMsg.direction = RIGHT;
				}
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			} else if (rx_data == AUTO_COMMAND) {
				osSemaphoreRelease(autoModeSemaphore);
			}
		} else {
			
		}
		
	}
}

void motor_thread(void *argument){
	motor_cmd motorMsg;
	for(;;){
		osMessageQueueGet(motorMQ, &motorMsg, NULL, osWaitForever);
		set_motors(motorMsg);
	}
}

void ultra_thread(void *argument){
	for (;;) {

	}
}

void auto_thread(void *argument){
	for (;;) {
		osSemaphoreAcquire(autoModeSemaphore, osWaitForever);
	}
}
/*
void brain_thread(void){
	for(;;){
		MessageObject_t messageObject;
		osMessageQueueGet(brainMessageQueue, &messageObject, NULL, osWaitForever);
		osMessageQueuePut(motorMessageQueue, &motorMessage, 0, 0);
	}
}
*/
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
	
	motorMQ = osMessageQueueNew(8, sizeof(motor_cmd), NULL);
	autoModeSemaphore = osSemaphoreNew(1, 0, NULL);
	ultrasonicSemaphore = osSemaphoreNew(1, 0, NULL);
	osThreadNew(handle_UART, NULL, NULL); 
	osThreadNew(motor_thread, NULL, NULL);
  osKernelStart();// Start thread execution
	
}
