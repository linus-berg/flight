#include <stdio.h>
#include <pic32mx.h>
#include "flight.h"

int main(void) {
  /* Init all the important shit. */
  Init();
  TX_ByteMulti("FLIGHT system initialised.", 1);
  TX_ByteMulti("Version: ", 0);
  TX_ByteMulti(_VERSION_, 1);
  #if _DEBUG
    TX_ByteMulti("__Debug Information__", 1);
    TX_ByteMulti("_SYS_CLK: ", 0);
    TX_ByteMulti(itoaconv(_SYS_CLK), 1);
    TX_ByteMulti("_PB_CLK: ", 0);
    TX_ByteMulti(itoaconv(_PB_CLK), 1);
    TX_ByteMulti("_PWM_FREQ: ", 0);
    TX_ByteMulti(itoaconv(_PWM_FREQ), 1);
  #endif 
  while(1);
}

void Init() {
  /* Set Peripheral clock to 40Mhz (80Mhz / 2) */
  /*
  OSCCON &= ~0x180000;
  OSCCON |= 0x080000;
  */

  /* Default setup, RGB and enabled. */
  LED_CON.red = 1;
  LED_CON.green = 1;
  LED_CON.blue = 1;
  LED_CON.enabled = 1;

  /* UART initialisation */
  BRG_SetBaud(9600);
  U1STA = 0;
  U1MODE = 0x8000;
  U1STASET = 0x1400;

  /* PWM Output. */
  /* PRx = (FPB / PWM_FREQ) - 1 */ 
  OC1CON = 0x6;
  PR2 = (_PB_CLK / _PWM_FREQ) - 1; 
  T2CONSET = 0x8000;
  //OC1CONSET = 0x8000;
  /* TODO: */ 
  //OC1RS = ;

}

/* Transmit one byte. */
void TX_Byte(unsigned char byte) {
  /* Write buffer check. */
  while(U1STA & (1 << 9)); 
  U1TXREG = byte;
  /* Transmitting check. */
  while(!(U1STA & (1 << 8)));
}

/* Transmit multiple bytes. */
void TX_ByteMulti(register unsigned char *bytes, char cr) {
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
