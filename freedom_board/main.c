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

#include "led/led.h"
 
#define MOVEMENT(x) ((uint8_t)(x)) >> 4 
#define FORWARD_MASK 0x4
#define BACKWARD_MASK 0x5
#define LEFT_MASK 0x6
#define RIGHT_MASK 0x7
#define AUTO_COMMAND 0x80
#define NOAUTO_COMMAND 0x81
#define DISTANCE_THRESH 300
 //0x80, 120, 80, 200,105 130, 150, 250 just touch
 
 
unsigned int isAuto =  0;///0;
unsigned int isFirstObstacle = 0;
unsigned int isSecondObstacle = 0;
unsigned int cur_distance = 0xffff;
int state = 0;// 0 for stationary & 1 for moving

unsigned int low_ultra_dist_counts = 0;
#define DISTANCE_THRESH_CNT 2


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
		.priority = osPriorityAboveNormal
};

 const osThreadAttr_t ultraPriority = {
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
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STRAIGHT;
					state = 0;
				} else {
					motorMsg.direction = LEFT;
					state = 1;
				}
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			} else if (move == RIGHT_MASK) {
				level = get_level(rx_data);
				motorMsg.level = level;
				if (level == 0){
					motorMsg.direction = STRAIGHT;
					state = 0;
				} else {
					motorMsg.direction = RIGHT;
					state = 1;
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

#define MOTOR_CMD_PAUSE 50//50

void motor_thread(void *argument){
	motor_cmd motorMsg;
	for(;;){
		osMessageQueueGet(motorMQ, &motorMsg, NULL, osWaitForever);
		osMutexAcquire(motorMutex,osWaitForever);
		set_motors(motorMsg);
		osDelay(MOTOR_CMD_PAUSE);
		osMutexRelease(motorMutex);
	}
}

void ultra_thread(void *argument){
	for (;;) {
		osSemaphoreAcquire(ultrasonicSemaphore,osWaitForever);
	}
}

void just_stoppppp(){

	motor_cmd motorMsg1 = create_motor_cmd(0, STOP);
	osMessageQueuePut(motorMQ, &motorMsg1, NULL, 0 );
	util_delay(MOTOR_CMD_PAUSE);//osDelay(MOTOR_CMD_PAUSE);
	motor_cmd motorMsg2 = create_motor_cmd(0, STRAIGHT);
	osMessageQueuePut(motorMQ, &motorMsg2, NULL, 0 );
	state = 0;
}
#define STOP_DELAY 400 //1000 abit too long for 7.97V
#define TURN_DELAY 475 // 500 abit beyond 90 for 7.97V
#define HALF_TURN_DELAY 320 //300 abit too much for 7.97V
#define FORWARD_DELAY 450  //1000 abit too much for 7.97V, 800 too much for same voltage but 10cm away
void auto_obstacle_avoid(void){
	motor_cmd motorMsg;
	just_stoppppp();
	osDelay(STOP_DELAY);
	
	motorMsg = create_motor_cmd(5, LEFT);
	osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
	osDelay(HALF_TURN_DELAY);
	state = 1;
	
	just_stoppppp();
	osDelay(STOP_DELAY);
	
	
	motorMsg = create_motor_cmd(5, FORWARD);
	osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
	osDelay(TURN_DELAY);
	state = 1;
	
	just_stoppppp();
	osDelay(STOP_DELAY);
	
	for(uint8_t i = 0; i < 3; i += 1){		
		motorMsg = create_motor_cmd(5, RIGHT);
		osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
		osDelay(TURN_DELAY);
		state = 1;
		
		just_stoppppp();
		osDelay(STOP_DELAY);
		
		motorMsg = create_motor_cmd(5, FORWARD);
		osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
		osDelay(FORWARD_DELAY);
		state = 1;
		
		just_stoppppp();
		osDelay(STOP_DELAY);
	}
	
	motorMsg = create_motor_cmd(5, LEFT);
	osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
	osDelay(HALF_TURN_DELAY);
	state = 1;
	
	just_stoppppp();
	osDelay(STOP_DELAY);
}	

void auto_thread(void *argument){
	unsigned int isRun = 0;
	for (;;) {
		if(!isRun){
			// for code that runs once in this loop
			osSemaphoreAcquire(autoStartSemaphore, osWaitForever);
			low_ultra_dist_counts  = 0;
			startUltrasound();
			cur_distance = 0xffff;
			isFirstObstacle = 0;
			isSecondObstacle = 0;
			osDelay(2000);
		}
		isRun = 1;
		
		if (ultra_dist  > 5 && ultra_dist < DISTANCE_THRESH){   //ultra_dist > 40 && 
			low_ultra_dist_counts += 1;
			if (isFirstObstacle){
				if (low_ultra_dist_counts >= DISTANCE_THRESH_CNT){					
					// stop at start point
					isSecondObstacle = 1;
					just_stoppppp();
					low_ultra_dist_counts = 0; 
				}
			} else {
				if (low_ultra_dist_counts >= DISTANCE_THRESH_CNT){					
					// go around obstacle
					isFirstObstacle = 1;
					auto_obstacle_avoid();
					low_ultra_dist_counts = 0;
				}
			}
			state = 0;
			osDelay(1000);
		} else{
			low_ultra_dist_counts = 0;
			if(!get_move_state() && !isSecondObstacle){				
				//state = 1;
				motor_cmd motorMsg = create_motor_cmd(2, FORWARD);
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			}
		} 
		if(!isAuto){
			//stopUltrasound();
			osSemaphoreRelease(autoStartSemaphore);
			isRun = 0;
		}
	}
}
 
//_____
void redLED_thread(void *argument){
	for (;;) {
		switch (get_move_state()) {
			case 1: //moving
				redLedMove();
			break;
			default: //stopped
				redLedStop();
			break;
				
		}
	}
}

void greenLED_thread(void *argument){
	for (;;) {
		switch (get_move_state()) {
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
	initMotor();
	initUltrasound();
	initUART2(BAUD_RATE);
	initGreenLED();
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	motorMQ = osMessageQueueNew(QUEUE_SIZE, sizeof(motor_cmd), NULL);  //8
	motorMutex = osMutexNew(NULL);
	autoStartSemaphore = osSemaphoreNew(1, 0, NULL);
	ultrasonicSemaphore = osSemaphoreNew(1, 0, NULL);
	osThreadNew(handle_UART, NULL, &uartPriority); //NULL
	osThreadNew(motor_thread, NULL, &motorPriority);
	osThreadNew(auto_thread, NULL, &autoPriority);
	//osThreadNew(ultra_thread, NULL, &ultraPriority);
	//osThreadNew(redLED_thread, NULL, NULL);
	osThreadNew(greenLED_thread, NULL, NULL);
  osKernelStart();// Start thread execution
	for(;;){}
}
