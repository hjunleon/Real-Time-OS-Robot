# include <MKL25Z4.h>
#ifndef DELAY
#define DELAY
static void delay(volatile uint32_t nof){
  while(nof != 0) {
    __asm("NOP");
    nof--;
  }
}
#endif
