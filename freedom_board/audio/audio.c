# include <MKL25Z4.h>
#include "music_notes.c"
#include "audio.h"
#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"

#define PTB0_Pin 0
#define PTB1_Pin 1

#define ONE_BEAT 350//320 // 175000//500000 (suitable for russian anthem)
#define TWO_BEAT ONE_BEAT * 2
#define FOUR_BEAT TWO_BEAT * 2
#define ONE_HALF_BEAT ONE_BEAT * 1.5
#define THREE_BEAT ONE_BEAT * 3
#define HALF_BEAT ONE_BEAT / 2
#define TRIPLET_BEAT ONE_BEAT / 3
#define QUARTER_BEAT HALF_BEAT / 2
#define SIXTH_BEAT HALF_BEAT / 3
#define EIGTH_BEAT QUARTER_BEAT/2
#define SIXTEENTH_BEAT EIGTH_BEAT/2
#define DOTTED_HALF_BEAT ONE_BEAT / 1.5
#define REST 0


#define MUSICAL_NOTE_CNT 52
#define END_NOTE_CNT 28

audioToPlay cur_audio = NONE;
audioToPlay new_audio = NONE;

unsigned int changeAudio = 0;

void initPWM(void)
{
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
  
  PORTB->PCR[PTB0_Pin] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[PTB0_Pin] |= PORT_PCR_MUX(3);

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


//int end[28] = {C,C,G,G,A,A,G,F,F,E,E,D,D,C,G,G,F,F,E,E,D,G,G,F,F,E,E,D};
 
#define RUSSIAN_LEN 123
#define RUSSIAN_SPEED 1
int russian_anthem[RUSSIAN_LEN][2] = {
	{G4, HALF_BEAT}, //1
	{C5, ONE_BEAT}, {G4, DOTTED_HALF_BEAT}, {A4, QUARTER_BEAT}, {B4, ONE_BEAT}, {E4, DOTTED_HALF_BEAT}, {E4, QUARTER_BEAT}, // 6
	{A4, ONE_BEAT}, {G4, DOTTED_HALF_BEAT}, {F4, QUARTER_BEAT}, {G4, ONE_BEAT}, {C4, DOTTED_HALF_BEAT}, {C4, QUARTER_BEAT}, //6
	{D4, ONE_BEAT}, {D4, DOTTED_HALF_BEAT}, {E4, QUARTER_BEAT}, {F4, ONE_BEAT}, {F4, DOTTED_HALF_BEAT}, {G4, QUARTER_BEAT},
	{A4, ONE_BEAT}, {B4, HALF_BEAT}, {C5, HALF_BEAT}, {D5,ONE_HALF_BEAT}, {G4, HALF_BEAT},
	{E5, ONE_BEAT}, {D5, DOTTED_HALF_BEAT}, {C5, QUARTER_BEAT}, {D5, ONE_BEAT}, {B4,HALF_BEAT}, {E4, HALF_BEAT},
	{C5, ONE_BEAT}, {B4, DOTTED_HALF_BEAT}, {A4, QUARTER_BEAT}, {B4, ONE_BEAT},  {G4, HALF_BEAT}, {E4, HALF_BEAT},
	{A5, ONE_BEAT}, {G5, DOTTED_HALF_BEAT}, {F4, QUARTER_BEAT}, {G4, ONE_BEAT}, {C4, DOTTED_HALF_BEAT} , {C4, QUARTER_BEAT},
	{C5, ONE_BEAT}, {B4, DOTTED_HALF_BEAT}, {A4, QUARTER_BEAT}, {G4, HALF_BEAT}, {B4, HALF_BEAT}, {C5, HALF_BEAT}, {D5, HALF_BEAT},
	{E5, ONE_HALF_BEAT}, {REST, HALF_BEAT}, {G6, TWO_BEAT}, 
	{REST, HALF_BEAT}, {G6, SIXTH_BEAT},{G6, SIXTH_BEAT},{G6, SIXTH_BEAT},{G6, TWO_BEAT}, {REST, ONE_BEAT},
	{C5, ONE_HALF_BEAT}, {REST,HALF_BEAT}, {E6, TWO_BEAT}, 
	{REST, HALF_BEAT}, {E6, SIXTH_BEAT},{E6, SIXTH_BEAT},{E6, SIXTH_BEAT},{E6, ONE_HALF_BEAT}, {G4, HALF_BEAT}, {A4, HALF_BEAT}, {B4, HALF_BEAT},
	{A4, ONE_BEAT}, {G4, DOTTED_HALF_BEAT}, {F4, QUARTER_BEAT}, {G4, ONE_BEAT}, {C4, DOTTED_HALF_BEAT} , {C4, QUARTER_BEAT},
	{C5, ONE_BEAT}, {B4, DOTTED_HALF_BEAT}, {A4, QUARTER_BEAT}, {G4, ONE_BEAT},{REST, HALF_BEAT}
	
};



#define DEJAVU_LEN 200
#define DEJAVU_SPEED 0.6  // 0.55 is actual, but may need slow down so that it dont affect the autonomous
int dejavu[DEJAVU_LEN][2] = {
	{REST, ONE_BEAT},{As6,HALF_BEAT},{As6,ONE_BEAT},{F6,ONE_BEAT},
	{REST, ONE_BEAT},{As6,HALF_BEAT},{As6,ONE_BEAT},{F6,ONE_BEAT},
	{REST, HALF_BEAT}, {As5, HALF_BEAT}, {D6,HALF_BEAT}, {As5, HALF_BEAT}, {D6, HALF_BEAT}, {As5, HALF_BEAT}, {D6, HALF_BEAT},
	{Ds6, ONE_HALF_BEAT}, {REST, HALF_BEAT}, {G6, ONE_BEAT}, {F6, HALF_BEAT}, {Ds6, HALF_BEAT}, {D6, HALF_BEAT},
	{REST, ONE_BEAT},{As6,HALF_BEAT},{As6,ONE_BEAT},{F6,ONE_BEAT},
	{REST, ONE_BEAT}, {Ds5,HALF_BEAT}, {Ds5,HALF_BEAT}, {D5, HALF_BEAT}, {C5, HALF_BEAT}, {As4, ONE_BEAT},  //6
	{C5, ONE_BEAT},{C5,HALF_BEAT},{F5,QUARTER_BEAT},{G5,QUARTER_BEAT},{C6,QUARTER_BEAT},{C6,QUARTER_BEAT},{F6,QUARTER_BEAT},{G6,QUARTER_BEAT},{C7,QUARTER_BEAT},{C7,QUARTER_BEAT},{F7,QUARTER_BEAT},{G7,QUARTER_BEAT},{C8,HALF_BEAT},
	{REST, ONE_HALF_BEAT}, {REST,HALF_BEAT},{C6, HALF_BEAT}, {D6, HALF_BEAT}, {Ds6, ONE_HALF_BEAT},  //6
	{Ds6, HALF_BEAT},{Ds6, HALF_BEAT},{Ds6, HALF_BEAT}, {As5, HALF_BEAT}, {G5, HALF_BEAT}, {As5, ONE_BEAT},  //6
	{C6, HALF_BEAT}, {C6, ONE_BEAT}, {G6, HALF_BEAT}, {F6, HALF_BEAT}, {Ds6, HALF_BEAT}, {D6, HALF_BEAT}, //6
	{Ds6,ONE_BEAT},{Ds6,HALF_BEAT},{Ds6,HALF_BEAT},{F6,HALF_BEAT},{Ds6,HALF_BEAT},{F6,HALF_BEAT},{F6,ONE_BEAT}, //7
	{G6,HALF_BEAT},{G6,HALF_BEAT},{F6, ONE_BEAT},{C6,HALF_BEAT},{D6,HALF_BEAT},{Ds6,ONE_HALF_BEAT}, //6
	{Ds6, HALF_BEAT},{Ds6, HALF_BEAT},{F6, HALF_BEAT},{D6, HALF_BEAT},{As5, HALF_BEAT},{As5,ONE_BEAT}, //6
	{C6, HALF_BEAT},{C6,ONE_BEAT},{Ds6, HALF_BEAT},{D6, HALF_BEAT},{C6, HALF_BEAT},{As5, HALF_BEAT}, //6
	{C6,ONE_BEAT},{Ds7, HALF_BEAT},{Ds7, HALF_BEAT},{F7, HALF_BEAT},{Ds7, HALF_BEAT},{F7, HALF_BEAT},{F7,ONE_BEAT}, //7
	{G7, HALF_BEAT},{G7, HALF_BEAT},{F7,TWO_BEAT}, //3
	{G7,HALF_BEAT},{REST,HALF_BEAT},{G7,HALF_BEAT},{REST,HALF_BEAT},{G7,HALF_BEAT},{G7,TWO_BEAT}, // 6
	{REST, TWO_BEAT} // 1
};

#define UNTITLED_LEN 1000
#define UNTITLED_SPEED 1

int untitled[UNTITLED_LEN][2] = {
	{Fs6,HALF_BEAT},  //{REST, THREE_BEAT}, {REST, HALF_BEAT},
	{F6, ONE_BEAT},{Fs6, ONE_BEAT},{Cs6, TWO_BEAT}, 
	{REST, HALF_BEAT},{F6,ONE_BEAT}, {Fs6,TWO_BEAT},
	{REST, HALF_BEAT},{Cs6,HALF_BEAT},{Fs6,HALF_BEAT}, {Fs6,QUARTER_BEAT}, {Gs6,DOTTED_HALF_BEAT}, 
	{As6, ONE_BEAT}, {Gs6, ONE_BEAT}, {Fs6, HALF_BEAT}, {Fs6, HALF_BEAT}, {Fs6, HALF_BEAT},
  {Fs6, HALF_BEAT}, {Ds6, ONE_BEAT}, {Cs6, ONE_HALF_BEAT}, {Fs6, HALF_BEAT}, {Fs6, HALF_BEAT},
	{Fs6, HALF_BEAT}, {Gs6, HALF_BEAT}, {As6, HALF_BEAT}, {Gs6, ONE_BEAT}, {F6, ONE_BEAT}, {F6, HALF_BEAT},
	{Fs6, HALF_BEAT},{Fs6, QUARTER_BEAT},{Ds6, QUARTER_BEAT},{Fs6, HALF_BEAT},{Fs6, QUARTER_BEAT},{Ds6, QUARTER_BEAT},{Fs6, HALF_BEAT},{Fs6, HALF_BEAT},{Fs6, QUARTER_BEAT},{Fs6, QUARTER_BEAT},{Gs6, HALF_BEAT},
	{As6, DOTTED_HALF_BEAT},	{Gs6, DOTTED_HALF_BEAT},{Fs6, HALF_BEAT},{F6, HALF_BEAT},{Fs6, HALF_BEAT},{Fs6, HALF_BEAT},{Fs6, HALF_BEAT},
	{Fs6, HALF_BEAT}, {Gs6, HALF_BEAT}, {As6, HALF_BEAT},{Fs6, ONE_HALF_BEAT}, {Fs6, HALF_BEAT}, {Cs7, HALF_BEAT},
	{Cs7, HALF_BEAT},{B6, HALF_BEAT},{As6, HALF_BEAT},{Gs6, ONE_BEAT},{Cs5, EIGTH_BEAT},{Gs6, EIGTH_BEAT},{As6, QUARTER_BEAT},{Gs6, QUARTER_BEAT},{Fs6, DOTTED_HALF_BEAT},
	{Fs6, HALF_BEAT}, {Fs6, QUARTER_BEAT},{Cs6, QUARTER_BEAT}, {Fs6, HALF_BEAT},{Fs6, HALF_BEAT},{Fs6, QUARTER_BEAT},{Cs6, QUARTER_BEAT}, {Fs6, HALF_BEAT},{Fs6, QUARTER_BEAT},{Cs6, QUARTER_BEAT}, {Fs6, QUARTER_BEAT},{Cs6, QUARTER_BEAT},
	{Fs6, QUARTER_BEAT},{Fs6, QUARTER_BEAT},{Cs6, QUARTER_BEAT},{Fs6, HALF_BEAT},{As6, QUARTER_BEAT},{Gs6, QUARTER_BEAT},{REST,HALF_BEAT}, {G6, HALF_BEAT}, {G6, HALF_BEAT}, {G6, HALF_BEAT},
	{G6, HALF_BEAT}, {E6, ONE_BEAT}, {D6, ONE_HALF_BEAT}, {G6, HALF_BEAT}, {G6, HALF_BEAT},
	{G6, HALF_BEAT}, {A6, HALF_BEAT}, {B6, HALF_BEAT}, {A6, ONE_BEAT}, {Fs7, ONE_BEAT}, {Fs7, HALF_BEAT}, 
	{G7, HALF_BEAT}, {G6, QUARTER_BEAT},{D6, QUARTER_BEAT},{G6, HALF_BEAT},{G6, QUARTER_BEAT},{D6, QUARTER_BEAT},{G6, HALF_BEAT},{G6, HALF_BEAT},{G6, QUARTER_BEAT},{G6, QUARTER_BEAT},{A6, HALF_BEAT},
	{B6, DOTTED_HALF_BEAT},	{A6, DOTTED_HALF_BEAT},{G6, HALF_BEAT},{Fs6, HALF_BEAT},{G6, HALF_BEAT},{G6, HALF_BEAT},{G6, HALF_BEAT},
	{G6, HALF_BEAT}, {A6, HALF_BEAT}, {B6, HALF_BEAT},{G6, ONE_HALF_BEAT}, {G6, HALF_BEAT}, {D7, HALF_BEAT},
	{D7, HALF_BEAT},{C6, HALF_BEAT},{B6, HALF_BEAT},{A6, ONE_BEAT},{B6, HALF_BEAT},{A6,QUARTER_BEAT}, {G6, DOTTED_HALF_BEAT},
	{G6, HALF_BEAT},{G6, QUARTER_BEAT},{E6, QUARTER_BEAT},{G6, HALF_BEAT},{G6, QUARTER_BEAT},{E6, QUARTER_BEAT},{G6, HALF_BEAT}, {G6, HALF_BEAT}, {G6, HALF_BEAT}, {D6, HALF_BEAT}, 
	{D6, HALF_BEAT},{C6, HALF_BEAT}, {B5, HALF_BEAT}, {A5, TWO_BEAT}, {REST,ONE_BEAT},
	//{Fs7, ONE_HALF_BEAT},{B7,DOTTED_HALF_BEAT}, {A7, QUARTER_BEAT}, {G7, TWO_BEAT}, {REST, ONE_BEAT},
	{G6, HALF_BEAT}, {A6, HALF_BEAT}, {B6, HALF_BEAT}, {A6, ONE_BEAT}
};

void play_internet_connect(void){
	new_audio = CONNECT;
	//play_audio();
}


void play_run(void){
	new_audio = RUNNING;
	//play_audio();
}

void play_end(void){
	new_audio = END;
	//play_audio();
}

void stop_music(void){
	new_audio = NONE;
}

void play_audio(void) {
	if (new_audio != cur_audio){
			cur_audio = new_audio;
	}
	if (cur_audio == NONE){
		return;
	}
	//unsigned int isFirst = 0;
	//int i = 0;
	double speed;
	int length;
	int (*cur_music)[2];
	//int count = 0;
	switch(cur_audio){
		case(CONNECT):
			cur_music = russian_anthem;
			speed = RUSSIAN_SPEED;
			length = RUSSIAN_LEN;
			break;
		case (RUNNING):
			cur_music = dejavu;
			speed = DEJAVU_SPEED;
			length = DEJAVU_LEN;
			break;
		case (END):
			cur_music = untitled;
			speed = UNTITLED_SPEED;
			length = UNTITLED_LEN;
			break;
		default:
			return;
			break;
	}
	int j = 0;
	while (j < length){			
		if (new_audio != cur_audio){
			cur_audio = new_audio;
			return;
		}
		int duration = speed * cur_music[j][1];
		TPM1->MOD = cur_music[j][0];
		TPM1_C0V = cur_music[j][0] / 2; 
		osDelay(duration);
		TPM1_C0V = 0;
		osDelay(duration);
		j++;
	}
	if (cur_audio == CONNECT){
		stop_music();
	}
		
}
		   