# include <MKL25Z4.h>
# include "../uart/uart.h"
#define PTB0_Pin 0
#define PTB1_Pin 1
#define PTC1_Pin 1
#define PTC2_Pin 2
//#define PTB2_Pin 2
//#define PTB3_Pin 3
int straight_level = 0;
int side_level = 0;

void initMotor(void)   //initPWM
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
	
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);

	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
	
	PORTC->PCR[PTC1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[PTC1_Pin] |= PORT_PCR_MUX(4);
	
	PORTC->PCR[PTC2_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTC->PCR[PTC2_Pin] |= PORT_PCR_MUX(4);
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	
	TPM1->MOD = 7500;
	TPM0->MOD = 7500;

	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB (1));
	
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB (1));
	
	TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM0->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB (1));
	
	TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM0_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB (1));
}
void left_motor (void *argument) {
	uint8_t rx_data;
  for (;;) {
		rx_data = UART2_Receive_Poll(); //0x44;//
		if(rx_data == 0x40){ // Forward
			TPM1_C0V = 3750/2;  // forward
			TPM1_C1V = 0;  // back
		} else if(rx_data == 0x41) {  // Backward
			TPM1_C0V = 0;
			TPM1_C1V = 3750/2;
		} else if(rx_data == 0x44) { // double forward
			TPM1_C0V = 3750;
			TPM1_C1V = 0;
		} else if(rx_data == 0x45) { // double backward
			TPM1_C0V = 0;
			TPM1_C1V = 3750;
		} else if(rx_data == 0x48) {  // halt
			TPM1_C0V = 0;
			TPM1_C1V = 0;
		}
	}
}

/*
TODO: have more discrete values (so long as within 1 byte)
Can't spam esp32 with onchange
*/
void right_motor (void *argument) {
 
	uint8_t rx_data;
  for (;;) {
		rx_data = UART2_Receive_Poll(); //0x46;//
		if(rx_data == 0x42){  // forward
			TPM0_C0V = 3750/2;
			TPM0_C1V = 0;
		} else if(rx_data == 0x43) { // Backward
			TPM0_C0V = 0;
			TPM0_C1V = 3750/2;
		} else if(rx_data == 0x46) { // double forward
			TPM0_C0V = 3750;
			TPM0_C1V = 0;
		} else if(rx_data == 0x47) { // double backward
			TPM0_C0V = 0;
			TPM0_C1V = 3750;
		} else if(rx_data == 0x49) { // halt
			TPM0_C0V = 0;
			TPM0_C1V = 0;
		}
	}
}

void stop (void *argument) {

	uint8_t rx_data;
  for (;;) {
		rx_data = UART2_Receive_Poll();//0x50;//
		if(rx_data == 0x50) {
			TPM1_C0V = 0;
			TPM1_C1V = 0;
			TPM2_C0V = 0;
			TPM2_C1V = 0;
		}
	}
}


void disable_motor(void){
		TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	
	
	TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	
	TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
}
