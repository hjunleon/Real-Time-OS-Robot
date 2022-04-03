#include <MKL25Z4.h> // Device header    

#define MASK(x) (1<<(x))

int greenPins[10] = {0, 3, 4, 5, 6, 10, 11, 12, 13, 16};
int i;

void initRedLED() {
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	PORTC->PCR[7] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[7] |= PORT_PCR_MUX(1);
	
	PTC->PDDR |= MASK(7);
	PTC->PCOR |= MASK(7);
}

void initGreenLED() {
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
  PORTC->PCR[0] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[0] |= PORT_PCR_MUX(1);
	PORTC->PCR[3] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[3] |= PORT_PCR_MUX(1);
	PORTC->PCR[4] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[4] |= PORT_PCR_MUX(1);
	PORTC->PCR[5] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[5] |= PORT_PCR_MUX(1);
	PORTC->PCR[6] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[6] |= PORT_PCR_MUX(1);
	PORTC->PCR[10] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[10] |= PORT_PCR_MUX(1);
	PORTC->PCR[11] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[11] |= PORT_PCR_MUX(1);
	PORTC->PCR[12] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[12] |= PORT_PCR_MUX(1);
	PORTC->PCR[13] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[13] |= PORT_PCR_MUX(1);
	PORTC->PCR[16] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[16] |= PORT_PCR_MUX(1);
	
	PTC->PDDR |= MASK(0) | MASK(3) | MASK(4) | MASK(5) | MASK(6) | MASK(10) | MASK(11) | MASK(12) | MASK(13) | MASK(16);
	PTC->PCOR |= MASK(0) | MASK(3) | MASK(4) | MASK(5) | MASK(6) | MASK(10) | MASK(11) | MASK(12) | MASK(13) | MASK(16);
}

void redLedMove () {
		PTC->PCOR |= MASK(7);
		PTC->PSOR |= MASK(7);
		osDelay(500);
		PTC->PCOR |= MASK(7);
		osDelay(500);
}

void redLedStop () {
		PTC->PCOR |= MASK(7);
		PTC->PSOR |= MASK(7);
		osDelay(250);
		PTC->PCOR |= MASK(7);
		osDelay(250);
}

void greenLedMove () {
		PTC->PCOR |= MASK(0) | MASK(3) | MASK(4) | MASK(5) | MASK(6) | MASK(10) | MASK(11) | MASK(12) | MASK(13) | MASK(16);
		for (i = 0; i < 10; i++) {
			PTC->PSOR |= MASK(greenPins[i]);
			osDelay(200);
			PTC->PCOR |= MASK(greenPins[i]);
		}
}

void greenLedStop () {
		PTC->PCOR |= MASK(0) | MASK(3) | MASK(4) | MASK(5) | MASK(6) | MASK(10) | MASK(11) | MASK(12) | MASK(13) | MASK(16);
		PTC->PSOR |= MASK(0) | MASK(3) | MASK(4) | MASK(5) | MASK(6) | MASK(10) | MASK(11) | MASK(12) | MASK(13) | MASK(16);
}


