# include <MKL25Z4.h>
# include "../motor/motor.h"
#define PTB0_Pin 0
#define PTB1_Pin 1
#define PTC1_Pin 1
#define PTC2_Pin 2
//#define PTB2_Pin 2
//#define PTB3_Pin 3

#define MY_TPM_MOD 3750 //3750
#define MAX_TPM_VAL MY_TPM_MOD    //7500// 5626//3750
#define MIN_TPM_UNIT MAX_TPM_VAL/5  //750  3750
#define BASE_TPM MAX_TPM_VAL/2
#define MIN_TPM_TURN MAX_TPM_VAL/5 // 10




volatile uint8_t straight_level = 0; // volatile
volatile uint8_t side_level = 0;
volatile dir f_b_dir = STOP;
volatile dir l_r_dir = STRAIGHT;

volatile unsigned int CUR_TPM1_C0V = 0;
volatile unsigned int CUR_TPM1_C1V = 0;
volatile unsigned int CUR_TPM0_C1V = 0;
volatile unsigned int CUR_TPM0_C0V = 0;

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
	
	
	TPM1->MOD = MY_TPM_MOD;
	TPM0->MOD = MY_TPM_MOD;

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

void just_forward(uint8_t level){
		CUR_TPM1_C0V = MIN_TPM_UNIT  * level;//MAX_TPM_VAL; //BASE_TPM + cmd.level * MIN_TPM_UNIT;
		CUR_TPM0_C0V = MIN_TPM_UNIT  * level; // BASE_TPM + cmd.level * MIN_TPM_UNIT;
		CUR_TPM1_C1V = 0;
		CUR_TPM0_C1V = 0;
		f_b_dir = FORWARD;
}

void just_backward(uint8_t level){
		CUR_TPM1_C1V = MIN_TPM_UNIT  * level; // BASE_TPM + cmd.level * MIN_TPM_UNIT;
		CUR_TPM0_C1V = MIN_TPM_UNIT  * level; //BASE_TPM + cmd.level * MIN_TPM_UNIT;
		CUR_TPM1_C0V = 0;
		CUR_TPM0_C0V = 0;
		f_b_dir = BACKWARD;
}

void just_stop(){
		CUR_TPM1_C0V = 0;
		CUR_TPM0_C0V = 0;
		CUR_TPM1_C1V = 0;
		CUR_TPM0_C1V = 0;
		f_b_dir = STOP;
}

void set_motors(motor_cmd cmd) {
	if (cmd.direction == FORWARD){
		just_forward(cmd.level);
		straight_level = cmd.level;
	} else if (cmd.direction == BACKWARD) {
		just_backward(cmd.level);
		straight_level = cmd.level;
	} else if (cmd.direction == STOP){
		//stop
		just_stop();
		straight_level = 0;
	} else {
		if (f_b_dir == FORWARD){
			just_forward(straight_level);
		} else if(f_b_dir == BACKWARD){
			just_backward(straight_level);
		} else {
			just_stop();
		}
	}
	
	if (cmd.direction == LEFT){
		if (f_b_dir == BACKWARD){
			calc_left_back(cmd.level);
			//CUR_TPM1_C1V += MAX_TPM_VAL;
			//CUR_TPM0_C1V -= MAX_TPM_VAL;
		} else {
			calc_left(cmd.level);
			//CUR_TPM1_C0V += MAX_TPM_VAL;
			//CUR_TPM0_C0V -= MAX_TPM_VAL;
		}
	} else if (cmd.direction == RIGHT){
		if (f_b_dir == BACKWARD){
			calc_right_back(cmd.level);
			//CUR_TPM0_C1V += MAX_TPM_VAL;
			//CUR_TPM1_C1V -= MAX_TPM_VAL;
		} else {
			calc_right(cmd.level);
			//CUR_TPM0_C0V += MAX_TPM_VAL;
			//CUR_TPM1_C0V -= MAX_TPM_VAL;
		}
	} else if (cmd.direction != STRAIGHT) {   // forward and backwards
		if (l_r_dir == LEFT){
			if (f_b_dir == BACKWARD){
				calc_left_back(side_level);
			} else {
				calc_left(side_level);
			}
		} else if (l_r_dir == RIGHT){
			if (f_b_dir == BACKWARD){
				calc_right_back(side_level);
			} else {
				calc_right(side_level);
			}
		}
	} else {
		l_r_dir = STRAIGHT;
	}
	
	// now set registers
	TPM0_C0V = CUR_TPM0_C0V;
	TPM0_C1V = CUR_TPM0_C1V;
	TPM1_C0V = CUR_TPM1_C0V;
	TPM1_C1V = CUR_TPM1_C1V;
}

void calc_right(uint8_t level){

	//CUR_TPM0_C0V += MAX_TPM_VAL;
	//CUR_TPM1_C0V -= MAX_TPM_VAL;
	unsigned int turn_amt = MAX_TPM_VAL;//level * MIN_TPM_TURN;
		CUR_TPM0_C0V += turn_amt; //left forward
		CUR_TPM0_C1V = 0; // left backward
		if (turn_amt > CUR_TPM1_C0V){ //right forward 
			CUR_TPM1_C1V += (turn_amt - CUR_TPM1_C0V);
			CUR_TPM1_C0V = 0;
		} else {
			CUR_TPM1_C0V -= turn_amt;
			CUR_TPM1_C1V = 0;
		}
		l_r_dir = RIGHT;
		side_level = level;
}

void calc_right_back(uint8_t level){
	unsigned int turn_amt = MAX_TPM_VAL;//level * MIN_TPM_TURN;
		CUR_TPM0_C1V += turn_amt;
		CUR_TPM0_C0V = 0;
		if (turn_amt > CUR_TPM1_C1V){
			CUR_TPM1_C0V += (turn_amt - CUR_TPM1_C1V);
			CUR_TPM1_C1V = 0;
		} else {
			CUR_TPM1_C1V -= turn_amt;
			CUR_TPM1_C0V = 0;
		}
		l_r_dir = RIGHT;
		side_level = level;
}

void calc_left(uint8_t level){
	unsigned int turn_amt = MAX_TPM_VAL;//level * MIN_TPM_TURN;
	CUR_TPM1_C0V += turn_amt;
	CUR_TPM1_C1V = 0;
	if (turn_amt > CUR_TPM0_C0V){
		CUR_TPM0_C1V += (turn_amt - CUR_TPM0_C0V);
		CUR_TPM0_C0V = 0;
	} else {
		CUR_TPM0_C0V -= turn_amt;
		CUR_TPM0_C1V = 0;
	}
	//swap if backward
	l_r_dir = LEFT;
	side_level = level;

}
void calc_left_back(uint8_t level){
	unsigned int turn_amt = MAX_TPM_VAL;//level * MIN_TPM_TURN;
	CUR_TPM1_C1V += turn_amt;
	CUR_TPM1_C0V = 0;
	if (turn_amt > CUR_TPM0_C1V){
		CUR_TPM0_C0V += (turn_amt - CUR_TPM0_C0V);
		CUR_TPM0_C1V = 0;
	} else {
		CUR_TPM0_C1V -= turn_amt;
		CUR_TPM0_C0V = 0;
	}
	l_r_dir = LEFT;
	side_level = level;
}


void disable_motor(void){
		TPM1->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM1_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	
	TPM1_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	
	
	TPM0->SC &= ~((TPM_SC_CMOD_MASK) | (TPM_SC_PS_MASK));
	TPM0_C0SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
	
	TPM0_C1SC &= ~((TPM_CnSC_ELSB_MASK) | (TPM_CnSC_ELSA_MASK) | (TPM_CnSC_MSB_MASK) | (TPM_CnSC_MSA_MASK));
}

motor_cmd create_motor_cmd(uint8_t level, dir direction){
	motor_cmd motorMsg;
	motorMsg.level = level;
	if (level == 0){
		if (direction == LEFT || direction == RIGHT || direction == STRAIGHT){
			motorMsg.direction = STRAIGHT;
		} else {
			motorMsg.direction = STOP;
		}
		
	} else {
		motorMsg.direction = direction;
	}
	return motorMsg;
}

unsigned int get_move_state(){
	if (f_b_dir == STOP && l_r_dir == STRAIGHT){
		return 0;
	}
	return 1;
}