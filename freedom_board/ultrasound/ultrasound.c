#include <MKL25Z4.h> // Device header    
#include <stdio.h> 
#include <stdlib.h>

#include "ultrasound.h"
#include "../utils.h"

#define ECHO_PIN 2
#define TRIGGER_PIN 3
#define MASK(x) (1 << (x))

volatile unsigned int ultra_dist = 0;
unsigned int echo_is_on = 0;
unsigned int pit_ldval = 0x6fffff;  //0x2fffff   THIS IS THE DURATION THE ULTRASONIC SENSOR PINGS

// TODO: Check for echo timeout

/**
CALL 	
	initUltrasound();
	startUltrasound();
	in main.c

*/

void initEcho(void)
{
	initTPM2();
}

void initTrigger(void){
	// enable clock for PortD
	SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	PORTD->PCR[TRIGGER_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[TRIGGER_PIN] |= PORT_PCR_MUX(1);
	
	// Set PORT D trigger bit to output
	PTD->PDDR |= MASK(TRIGGER_PIN);
	
}

/**
*
* Setups drivers to communicate with ultrasonic sensor
*
*/
void initUltrasound(void){
	initEcho(); 
	initTrigger();
}

/**
*
* Setup periodic interrupt timer (PIT) that calls the function that sends TRIG signal to the ultrasonic sensor
* Currently fixed to pit_ldval
*/
void startUltrasound(void){   
	initPIT();
}

// Echo timer
void initTPM2() {
	SIM_SCGC6 |= SIM_SCGC6_TPM2_MASK;
	
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1); //MCGFLLCLK clock or MCGPLLCLK/2
	
	SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	PORTA_PCR1 &= ~PORT_PCR_MUX_MASK;
	PORTA_PCR1 |= PORT_PCR_MUX(3);
	PORTA_PCR2 &= ~PORT_PCR_MUX_MASK;
	PORTA_PCR2 |= PORT_PCR_MUX(3);
	
	TPM2_SC &= ~(TPM_SC_CMOD_MASK | TPM_SC_PS_MASK | TPM_SC_CPWMS_MASK);
	TPM2_SC |= TPM_SC_PS(4); //16
	TPM2_SC |= TPM_SC_TOIE(1);
	startTPM2();
	
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSA_MASK | TPM_CnSC_ELSB_MASK);
	
	TPM2_MOD = 60000; 
	
	TPM2_C1SC |= TPM_CnSC_CHIE_MASK;
	TPM2_C0V = 30; 
		
	NVIC_SetPriority(TPM2_IRQn, 0);
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	NVIC_EnableIRQ(TPM2_IRQn);
}

void disableTPM2(void){
	NVIC_DisableIRQ(TPM2_IRQn);
	stopTPM2();
	resetTPM2();
}

unsigned int getTPMC1Value(void){
	return TPM2_C1V;
}

// either works
void startMeasure(){
	stopTPM2();
	//Enable Output Compare Mode on Channel 0, to generate 10 microsec high pulse when timer starts
	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK)); 
	TPM2_C0SC |= (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK);
	resetTPM2();
	startTPM2();
}


void stopTPM2(){
		TPM2_SC &= ~TPM_SC_CMOD_MASK;  // stop
}

void resetTPM2(){
		TPM2_CNT = 0;  //reset
}

void startTPM2(){
		TPM2_SC |= TPM_SC_CMOD(1);  // start counting
}

void getDistance(void){
	if (!echo_is_on){
		// start timer
		stopTPM2();
		resetTPM2();
		startTPM2();
		echo_is_on = 1;
	} else {
		// end timer, get distance
		int echo_time = getTPMC1Value();
		timeToDistance(echo_time);
		echo_is_on = 0;
	}
}

/*
*
* Converts the duration of pulse to actual distance
* Dev note: check if magic number actually works
*/
void timeToDistance(unsigned int duration){
	ultra_dist = duration * 0.05715;//* 0.034 / 2;
}

void onTrigger(void){
	PTD->PSOR |= MASK(TRIGGER_PIN);
}

void offTrigger(void){
	PTD->PCOR |= MASK(TRIGGER_PIN);
}
void TPM2_IRQHandler(void){
	NVIC_ClearPendingIRQ(TPM2_IRQn);
	if (TPM2_STATUS & TPM_STATUS_CH1F_MASK) {   
		TPM2_STATUS |= TPM_STATUS_CH1F_MASK;
		getDistance();
	} else {
		TPM2_STATUS |= TPM_STATUS_TOF_MASK;
	}
}


void clearTIF(void){
	PIT_TFLG0 |= ~PIT_TFLG_TIF_MASK;
	//PIT_TFLG0 = 1;
}

void initPIT(void){
	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	PIT_MCR &= ~PIT_MCR_FRZ_MASK;
	PIT_MCR |= PIT_MCR_FRZ(0);
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	PIT_MCR |= PIT_MCR_MDIS(0);
	
	PIT_LDVAL0 &= ~PIT_LDVAL_TSV_MASK;  // using this channel
	PIT_LDVAL0 |= PIT_LDVAL_TSV(pit_ldval);
	
	clearTIF();
	
	PIT_TCTRL0 &= ~PIT_TCTRL_TIE_MASK;
	PIT_TCTRL0 |= PIT_TCTRL_TIE(1);
	
	
	PIT_TCTRL0 &= ~PIT_TCTRL_TEN_MASK;
	PIT_TCTRL0 |= PIT_TCTRL_TEN(1);
	
	//Enable Interrupts
	NVIC_SetPriority(PIT_IRQn,2);
	NVIC_ClearPendingIRQ(PIT_IRQn);
	NVIC_EnableIRQ(PIT_IRQn); 
	
}



void PIT_IRQHandler(void){
	// clear pending interrupts
	NVIC_ClearPendingIRQ(PIT_IRQn);
	if (PIT_TFLG0 & PIT_TFLG_TIF_MASK) {
		startMeasure();
	}
	clearTIF();
}

//unsigned int extern_get_distance(void){
//	return cur_distance;
//}

