# include <MKL25Z4.h>
#ifndef UART_ME
#define UART_ME

//uint8_t UART2_Receive_Poll(void);
uint8_t UART2_IRQHandler(void);

void initUART2(uint32_t baud_rate);
#endif
#define BAUD_RATE 9600
//#define UART_TX_PORTE22 22
#define UART_RX_PORTE23 23
#define UART2_INT_PRIO 128
