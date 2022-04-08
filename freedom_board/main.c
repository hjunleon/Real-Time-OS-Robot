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
#include "audio/audio.h"

#define MOVEMENT(x) ((uint8_t)(x)) >> 4 
#define FORWARD_MASK 0x4
#define BACKWARD_MASK 0x5
#define LEFT_MASK 0x6
#define RIGHT_MASK 0x7
#define AUTO_COMMAND 0x80
#define NOAUTO_COMMAND 0x81
#define INTERNET_CONNECT_CMD 0x20
#define START_TRIAL_CMD 0x21
#define END_TRIAL_CMD 0x22//230//220//300 good for spped 3 //350 abit too little for plugged in  //150//20  420 without backward braking
 //0x80, 120, 80, 200,105 130, 150, 250 just touch
 
 
unsigned int isAuto =  0;///0;
unsigned int isFirstObstacle = 0;
unsigned int isSecondObstacle = 0;
unsigned int cur_distance = 0xffff;
int state = 0;// 0 for stationary & 1 for moving

unsigned int low_ultra_dist_counts = 0;
#define DISTANCE_THRESH_CNT 1

unsigned int auto_start_pit_ticks = 0;
unsigned int auto_first_pit_ticks = 0;
unsigned int first_obstacle_pit_ticks = 0;

unsigned int justConnected = 0;
unsigned int startTrial = 0;
unsigned int endTrial = 0;


#define QUEUE_SIZE 4
osMessageQueueId_t motorMQ;
osMutexId_t motorMutex;
osSemaphoreId_t autoStopSemaphore;
osSemaphoreId_t autoStartSemaphore;
osSemaphoreId_t ultrasonicSemaphore;

/*
const osThreadAttr_t uartPriority = {
		.priority = osPriorityHigh2//osPriorityHigh//osPriorityAboveNormal//osPriorityHigh4
};

const osThreadAttr_t motorPriority = {
		.priority = osPriorityHigh//osPriorityHigh
};
 const osThreadAttr_t autoPriority = {
		.priority = osPriorityAboveNormal4//osPriorityAboveNormal
};

 const osThreadAttr_t greenPriority = {
		.priority = osPriorityNormal //osPriorityLow
}; 
 
 const osThreadAttr_t redPriority = {
		.priority = osPriorityNormal //osPriorityLow
}; 
 const osThreadAttr_t audioPriority = {
		.priority = osPriorityBelowNormal//osPriorityLow//osPriorityNormal //osPriorityBelowNormal
}; */

const osThreadAttr_t uartPriority = {
		.priority = osPriorityAboveNormal//osPriorityHigh//osPriorityAboveNormal//osPriorityHigh4
};

const osThreadAttr_t motorPriority = {
		.priority = osPriorityAboveNormal//osPriorityHigh
};
 const osThreadAttr_t autoPriority = {
		.priority = osPriorityAboveNormal//osPriorityAboveNormal
};

 const osThreadAttr_t greenPriority = {
		.priority = osPriorityNormal //osPriorityLow
}; 
 
 const osThreadAttr_t redPriority = {
		.priority = osPriorityNormal //osPriorityLow
}; 
 const osThreadAttr_t audioPriority = {
		.priority = osPriorityBelowNormal//osPriorityLow//osPriorityNormal //osPriorityBelowNormal
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
		//rx_data = UART2_Receive_Poll(); //0x44;//
		rx_data = UART2_IRQHandler(); //0x44;//

		if (rx_data == INTERNET_CONNECT_CMD){
			//justConnected = 1;
			play_internet_connect();
			continue;
		} else if (rx_data == START_TRIAL_CMD) {
			//startTrial = 1;
			//endTrial = 0;
			play_run();
			continue;
		} else if (rx_data == END_TRIAL_CMD) {
			//endTrial = 1;
			//startTrial = 0;
			play_end();
			continue;
		}
		
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
		
		osDelay(1);
	}
}

#define MOTOR_CMD_PAUSE 15//50

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

void just_stoppppp(){

	motor_cmd motorMsg1 = create_motor_cmd(0, STOP);
	osMessageQueuePut(motorMQ, &motorMsg1, NULL, 0 );
	util_delay(MOTOR_CMD_PAUSE);//osDelay(MOTOR_CMD_PAUSE);
	motor_cmd motorMsg2 = create_motor_cmd(0, STRAIGHT);
	osMessageQueuePut(motorMQ, &motorMsg2, NULL, 0 );
	state = 0;
}

#define TIME_MULTIPLIER 2.1//1.87 //1.5//3 for 18V 4.5A //1.5
#define STOP_DELAY 500//250 //1000 abit too long for 7.97V
#define HALF_TURN_DELAY 280 / TIME_MULTIPLIER//300 for speed 2 350 //300 abit too much for 7.97V
#define TURN_DELAY 1.9 * HALF_TURN_DELAY// 2.22   660 for speed 2 620 580 for 11.4V 650// 500 abit beyond 90 for 7.97V  ,, 550 too ittle for speed  18 V
#define FORWARD_DELAY 600/ TIME_MULTIPLIER // 580 f0r 11.4V 800  for 11.4V with backward brake,  1750 for 18V    550  //450 1000 abit too much for 7.97V, 800 too much for same voltage but 10cm away
#define TURN_SPEED 2

#define LEFT_FORWARD_OFFSET 15 / TIME_MULTIPLIER
#define RIGHT_FORWARD_OFFSET  LEFT_FORWARD_OFFSET * 3 // somehow *3 

#define DISTANCE_THRESH 123 * TIME_MULTIPLIER //113 for box


void auto_obstacle_avoid(void){
	motor_cmd motorMsg;
	just_stoppppp();
	osDelay(100);
	
	motorMsg = create_motor_cmd(5, BACKWARD);
	osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
	osDelay(20);	
	
	just_stoppppp();
	osDelay(200);
	
	motorMsg = create_motor_cmd(TURN_SPEED, LEFT);
	osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
	osDelay(HALF_TURN_DELAY);
	//state = 1;
	
	just_stoppppp();
	osDelay(STOP_DELAY);
	
	
	motorMsg = create_motor_cmd(2, FORWARD);
	osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
	osDelay(FORWARD_DELAY - LEFT_FORWARD_OFFSET);
	
	just_stoppppp();
	osDelay(STOP_DELAY);
	
	/*
	just_stoppppp();
	osDelay(100);
	
	
	motorMsg = create_motor_cmd(1, BACKWARD);
	osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
	osDelay(10);	
	
	
	just_stoppppp();
	osDelay(200);
	*/
	for(uint8_t i = 0; i < 3; i += 1){		
		motorMsg = create_motor_cmd(TURN_SPEED, RIGHT);
		osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
		osDelay(TURN_DELAY);
		
		just_stoppppp();
		osDelay(STOP_DELAY);
		
		motorMsg = create_motor_cmd(2, FORWARD);
		osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
		osDelay(FORWARD_DELAY + RIGHT_FORWARD_OFFSET);
		
		
		just_stoppppp();
		osDelay(STOP_DELAY);
		
		/*
		just_stoppppp();
		osDelay(100);
				
		motorMsg = create_motor_cmd(1, BACKWARD);
		osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
		osDelay(10);	
		
		
		just_stoppppp();
		osDelay(200);
		*/
	}
	
	motorMsg = create_motor_cmd(TURN_SPEED, LEFT);
	osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
	osDelay(HALF_TURN_DELAY );
	
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
			play_run();
			osDelay(500);
			auto_start_pit_ticks = getPITTick();
		}
		isRun = 1;
		if (isFirstObstacle && !isSecondObstacle && (getPITTick() - auto_first_pit_ticks >= first_obstacle_pit_ticks)){
			// stop at start point
			isSecondObstacle = 1;
			osDelay(25); // stop beyond the line 
			just_stoppppp();
			play_end();
			stopUltrasound();
		}
		if (ultra_dist > 0 && ultra_dist < DISTANCE_THRESH){   //ultra_dist > 40 && 
			low_ultra_dist_counts += 1;
			if (isFirstObstacle){
				/*
				if (low_ultra_dist_counts >= DISTANCE_THRESH_CNT){					
					// stop at start point
					isSecondObstacle = 1;
					just_stoppppp();
					low_ultra_dist_counts = 0; 
					endTrial = 1;
					startTrial = 0;
				}*/
			} else {
				
				
				if (low_ultra_dist_counts >= DISTANCE_THRESH_CNT){					
					// get time taken to first obstacle
					auto_first_pit_ticks = getPITTick();
					first_obstacle_pit_ticks = auto_first_pit_ticks - auto_start_pit_ticks;
					// stop ultrasound and PIT 
					//stopUltrasound();
					disableTPM2();
					
					// go around obstacle
					isFirstObstacle = 1;
					auto_obstacle_avoid();
					osDelay(500);
					auto_first_pit_ticks = getPITTick();
					low_ultra_dist_counts = 0;
				}
			}
		} else{
			low_ultra_dist_counts = 0;
			if(!get_move_state() && !isSecondObstacle){				
				motor_cmd motorMsg = create_motor_cmd(5, FORWARD); //2 seems to drift, 3 also drifting, 4 drifted somehow
				osMessageQueuePut(motorMQ, &motorMsg, NULL, 0 );
			}
		} 
		if(!isAuto){
			just_stoppppp();
			stop_music();
			stopUltrasound();
			osSemaphoreRelease(autoStartSemaphore);
			isRun = 0;
		}
		
		osDelay(20);
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
		osDelay(150);
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
		osDelay(150);
	}
}

void switchConnectionMusic(void){
	
	changeAudio = 1;
}

void audio_thread(void *argument){
	for (;;) {
		/*
		if (justConnected == 1) {
			play_internet_connect();
			justConnected = 0;
		} else if (startTrial == 1) {
			play_run();
		} else if (endTrial == 1) {
			play_end();
		}*/
		play_audio();
		osDelay(500);
		//osDelay(100);
	}
}
//_____

int main (void) {
 
  // System Initialization

  SystemCoreClockUpdate();
	initMotor();
	initUltrasound();
	initUART2(BAUD_RATE);
	initRedLED();
	initGreenLED();
	initAudio();
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	
	motorMQ = osMessageQueueNew(QUEUE_SIZE, sizeof(motor_cmd), NULL);  //8
	motorMutex = osMutexNew(NULL);
	autoStartSemaphore = osSemaphoreNew(1, 0, NULL);
	ultrasonicSemaphore = osSemaphoreNew(1, 0, NULL);
	osThreadNew(handle_UART, NULL, &uartPriority); //NULL
	osThreadNew(motor_thread, NULL, &motorPriority);
	osThreadNew(auto_thread, NULL, &autoPriority);
	//osThreadNew(ultra_thread, NULL, &ultraPriority);
	osThreadNew(redLED_thread, NULL, &redPriority);
	osThreadNew(greenLED_thread, NULL, &greenPriority);
	osThreadNew(audio_thread, NULL, &audioPriority);
  osKernelStart();// Start thread execution
	for(;;){}
}
