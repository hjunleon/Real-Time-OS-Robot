# include <MKL25Z4.h>

#define PTB0_Pin 0
#define PTB1_Pin 1
#define C 1431
#define D 1276
#define E 1136
#define F 1074
#define G 956
#define A 852
#define B 759
#define MUSICAL_NOTE_CNT 52
#define END_NOTE_CNT 28


void initPWM(void)
{
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
  
  PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);

  PORTB->PCR[PTB1_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[PTB1_Pin] |= PORT_PCR_MUX(3);
  
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
  
  SIM->SOPT2 &= ~SIM_SOPT2_TPMSRC_MASK;
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);
  
  
  TPM1->MOD = 7500;

  TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
  TPM1->SC |= (TPM_SC_CMOD(1) | TPM_SC_PS(7));
  TPM1->SC &= ~(TPM_SC_CPWMS_MASK);
  
  TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
  TPM1_C0SC |= (TPM_CnSC_ELSB(1) | TPM_CnSC_MSB (1));
}

static void delay(volatile uint32_t nof){
  while(nof != 0) {
    __asm("NOP");
    nof--;
  }
}

void initAudio(void) {
	initPWM();
}

int song[52] = {
	C,F,F,G,F,E,D,D,
	D,G,G,A,G,F,E,C,
	C,A,A,B,A,G,F,D,
	C,C,D,G,E,F,

	C,F,F,F,E,
	E,F,E,D,C,
	G,A,G,F,C,C,
	C,C,D,G,E,F,
};
 
int end[28] = {C,C,G,G,A,A,G,F,F,E,E,D,D,C,G,G,F,F,E,E,D,G,G,F,F,E,E,D};
    
int main(void){
  SystemCoreClockUpdate();
	audio_thread(*arguement);
}


void audio_thread(void *arguement) {
	int i = 0;
	int j = 0;
	
	while(1) {
		if (end == 0) {
				TPM0->MOD = (song[i]);
				TPM0_C0V = (song[i])/2; 
				delay(1000000);
				TPM0_C2V = 0;
				delay(1000000);
				i++;
				if (i == MUSICAL_NOTE_CNT)
				i = 0;
			}			
			else {
				TPM0->MOD = (song[j]);
				TPM0_C0V = (end[j])/2;
				delay(1000000);
				TPM0_C2V = 0;
				delay(1000000);
				j = (j + 1) % END_NOTE_CNT;			
				}
			}
		}
