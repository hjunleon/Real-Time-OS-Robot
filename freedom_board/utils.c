# include <MKL25Z4.h>
//#ifndef UTILS
//#define UTILS
void util_delay(volatile uint32_t nof){
  while(nof != 0) {
    __asm("NOP");
    nof--;
  }
}
//#endif
