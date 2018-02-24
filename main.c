#include <stdio.h>
#include <stdint.h>
#include <pic32mx.h>
#include "include/flight.h"
#include "include/display.h"
#include "include/msgeq.h"
#include "include/UART.h"

void user_isr() {
  uint8_t rx;
  while((U1STA & 0x1)) {
    rx = U1RXREG & 0xFF;
    if (rx == 0x1) {
      LED_CON.red = LED_CON.red ? 0 : 1;
      TX_ByteMulti("Red was set to ", 0);
      TX_ByteMulti(itoaconv(LED_CON.red), 1);
    } else if (rx == 0x2) {
      LED_CON.green = LED_CON.green ? 0 : 1;
      TX_ByteMulti("Green was set to ", 0);
      TX_ByteMulti(itoaconv(LED_CON.green), 1);
    } else if (rx = 0x3) {
      LED_CON.blue = LED_CON.blue ? 0 : 1;
      TX_ByteMulti("Blue was set to ", 0);
      TX_ByteMulti(itoaconv(LED_CON.blue), 1);
    }
  }
  IFS(0) &= ~(1 << 27);
}

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
    TX_ByteMulti("PR2: ", 0);
    TX_ByteMulti(itoaconv(PR2), 1);
  #endif

  /* Some frequency shit */
  int freq[] = {0, 0, 0, 0, 0, 0, 0};
  for(;;) {
    PORTD |= _MSGEQ7_RESET;
    PORTD &= ~_MSGEQ7_RESET;
    for (uint8_t i = 0; i < 7; i++) {  
      freq[i] = MSGEQ_Read() - 100 < 30 ? 0 : MSGEQ_Read() - 100;
      Display_Bar(i, freq[i]);
      //TX_ByteMulti(itoaconv(freq[i]), 0);
      //TX_ByteMulti("    ", 0);
      delay(1);
    } 
    //TX_ByteMulti("", 1);
    OC1RS = (400.0 / 950) * (freq[0] + freq[1]) / 2;
  }
}


void Init() {
	/* Set up SPI as master */
	SPI2CON = 0;
	SPI2BRG = 4;
	SPI2STATCLR &= ~0x40;
  SPI2CON |= 0x60;
  SPI2CONSET = 0x8000;
  
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
  IPC(6) = 0x8;
  IEC(0) = 1 << 27; 
  
  /* PWM Output. */
  /* PRx = (FPB / PWM_FREQ) - 1 */ 
  OC1CON = 0x6;
  PR2 = (_PB_CLK / _PWM_FREQ) - 1; 
  T2CONSET = 0x8000;
  OC1R = PR2/2;
  OC1RS = 200;
  OC1CONSET = 0x8000;

  /* Library inits */ 
  MSGEQ_Init();
  Display_Init();
  Display_Clear(); 
  Display_Logo();
  delay(2000);
  Display_Clear();
  enable_interrupt();
}


