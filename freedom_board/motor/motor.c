# include <MKL25Z4.h>
#define PTB0_Pin 0
#define PTB1_Pin 1
#define PTB2_Pin 2
#define PTB3_Pin 3

void initMotor(void)   //initPWM
{
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);

	PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
	
	PORTB->PCR[PTB2_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB2_Pin] |= PORT_PCR_MUX(3);
	
	PORTB->PCR[PTB3_Pin] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[PTB3_Pin] |= PORT_PCR_MUX(3);
	
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
	SIM->SCGC6 |= SIM_SCGC6_TPM2_MASK;
	
	SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
	
	
	TPM1->MOD = 7500;
	TPM2->MOD = 7500;

	TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB (1));
	
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM1_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB (1));
	
	TPM2->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM2->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
	TPM2->SC &= ~(TPM_SC_CPWMS_MASK);
	
	TPM2_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB (1));
	
	TPM2_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	TPM2_C1SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB (1));
}
void left_motor (void *argument) {
 
	uint8_t rx_data;
  for (;;) {
		rx_data = UART2_Receive_Poll();
		if(rx_data == 0x40){
			TPM1_C0V = 3750/2;
			TPM1_C1V = 0;
		} else if(rx_data == 0x41) {
			TPM1_C0V = 0;
			TPM1_C1V = 3750/2;
		} else if(rx_data == 0x44) {
			TPM1_C0V = 3750;
			TPM1_C1V = 0;
		} else if(rx_data == 0x45) {
			TPM1_C0V = 0;
			TPM1_C1V = 3750;
		} else if(rx_data == 0x48) {
			TPM1_C0V = 0;
			TPM1_C1V = 0;
		}
	}
}

void right_motor (void *argument) {
 
	uint8_t rx_data;
  for (;;) {
		rx_data = UART2_Receive_Poll();
		if(rx_data == 0x42){
			TPM2_C0V = 3750/2;
			TPM2_C1V = 0;
		} else if(rx_data == 0x43) {
			TPM2_C0V = 0;
			TPM2_C1V = 3750/2;
		} else if(rx_data == 0x46) {
			TPM2_C0V = 3750;
			TPM2_C1V = 0;
		} else if(rx_data == 0x47) {
			TPM2_C0V = 0;
			TPM2_C1V = 3750;
		} else if(rx_data == 0x49) {
			TPM2_C0V = 0;
			TPM2_C1V = 0;
		}
	}
}