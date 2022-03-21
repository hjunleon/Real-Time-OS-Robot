# include <MKL25Z4.h>

static void delay(volatile uint32_t nof){
  while(nof != 0) {
    __asm("NOP");
    nof--;
  }
}