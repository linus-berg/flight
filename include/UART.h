/*
* FILENAME: UART.h
* DESCRIPTION:
*   UART interfacing.
* FUNCTIONS:
*   void UART_TX(TXREG, STA, byte)
*   void UART_String(TXREG, STA, bytes, carriage return)
*   void UART_Baud(BRG, baudrate)
*
* AUTHOR: Linus Gunnarsson    
*/
#ifndef UART_H
#define UART_H
#include <stdint.h>
#include "sys.h"
void UART_TX(volatile unsigned *tx, volatile unsigned *sta, uint8_t byte);
void UART_String(volatile unsigned *tx, volatile unsigned *sta,
                 register uint8_t *bytes, uint8_t cr);
void UART_Baud(volatile unsigned *brg, int baud_rate);

void UART_Init(volatile unsigned *sta, volatile unsigned *mode,
               volatile unsigned *brg, unsigned mode_con,
               unsigned sta_con, int baud_rate) {
  UART_Baud(brg, baud_rate);
  *sta = 0;
  *mode = mode_con;
  *sta |= sta_con;
}
/* Transmit one byte. */
void UART_TX(volatile unsigned *tx, volatile unsigned *sta, uint8_t byte) {
  /* Write buffer check. */
  while (*sta & (1 << 9)); 
  *tx = byte;
  while (!(*sta & (1 << 8)));
}

/* Transmit multiple bytes. */
void UART_String(volatile unsigned *tx, volatile unsigned *sta,
                 register uint8_t  *bytes, uint8_t cr) {
  while (*bytes) {
    UART_TX(tx, sta, *bytes);
    bytes++;
  }
  if (cr) {
    UART_TX(tx, sta, 0xA);
  }
}

void UART_Baud(volatile unsigned *brg, int baud_rate) {
  /* Peripheral clock */
  *brg = (_PB_CLK / (16 * baud_rate)) - 1;
}
#endif
