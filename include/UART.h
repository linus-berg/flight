/*
* FILENAME: UART.h
* DESCRIPTION:
*   UART interfacing with computer via USB (via U1STA).
* FUNCTIONS:
*   void TX_Byte(byte)
*   void TX_ByteMulti(bytes, carriage return)
* NOTES:
*   Only communicates over UART1.
*
* AUTHOR: Linus Gunnarsson    
*/
#ifndef UART_H
#define UART_H
#include <stdint.h>

void TX_Byte(uint8_t byte);
void TX_ByteMulti(register uint8_t *bytes, uint8_t cr);
void BRG_SetBaud(int baud_rate);

/* Transmit one byte. */
void TX_Byte(uint8_t byte) {
  /* Write buffer check. */
  while (U1STA & (1 << 9)); 
  U1TXREG = byte;
  /* Transmitting check. */
  while (!(U1STA & (1 << 8)));
}

/* Transmit multiple bytes. */
void TX_ByteMulti(register uint8_t  *bytes, uint8_t cr) {
  while (*bytes) {
    TX_Byte(*bytes);
    bytes++;
  }
  if (cr) {
    TX_Byte(0xA);
  }
}

void BRG_SetBaud(int baud_rate) {
  /* Peripheral clock */
  U1BRG = (_PB_CLK / (16 * baud_rate)) - 1;
}
#endif
