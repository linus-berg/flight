#include <stdio.h>
#include <stdint.h>
#include <pic32mx.h>
#include "include/flight.h"
#include "include/display.h"
#include "include/msgeq.h"
#include "include/uart.h"

#define DEBUG 1 
#define _VERSION_ "0.0.1a"

void user_isr() {
  uint8_t rx;
  while((U1STA & 0x1)) {
    rx = U1RXREG & 0xFF;
    if (rx == 0x1) {
      LED_CON.red = LED_CON.red ? 0 : 1;
      uart_String(&U1TXREG, &U1STA, "Red was set to ", 0);
      uart_String(&U1TXREG, &U1STA, itoaconv(LED_CON.red), 1);
    } else if (rx == 0x2) {
      LED_CON.green = LED_CON.green ? 0 : 1;
      uart_String(&U1TXREG, &U1STA, "Green was set to ", 0);
      uart_String(&U1TXREG, &U1STA, itoaconv(LED_CON.green), 1);
    } else if (rx = 0x3) {
      LED_CON.blue = LED_CON.blue ? 0 : 1;
      uart_String(&U1TXREG, &U1STA, "Blue was set to ", 0);
      uart_String(&U1TXREG, &U1STA, itoaconv(LED_CON.blue), 1);
    }
  }
  IFS(0) &= ~(1 << 27);
}

int main(void) {
  /* Init all the important shit. */
  Init();
  uart_Init(&U1STA, &U1MODE, &U1BRG, 0x8000, 0x1400, 9600);
  IPC(6) |= 0x8;
  IEC(0) |= 1 << 27; 
  uart_String(&U1TXREG, &U1STA, "FLIGHT system initialised.", 1);
  uart_String(&U1TXREG, &U1STA, "Version: ", 0);
  uart_String(&U1TXREG, &U1STA, _VERSION_, 1);
  #if DEBUG
    uart_String(&U1TXREG, &U1STA, "__Debug Information__", 1);
    uart_String(&U1TXREG, &U1STA, "_SYS_CLK: ", 0);
    uart_String(&U1TXREG, &U1STA, itoaconv(SYS_CLK), 1);
    uart_String(&U1TXREG, &U1STA, "_PB_CLK: ", 0);
    uart_String(&U1TXREG, &U1STA, itoaconv(PB_CLK), 1);
    uart_String(&U1TXREG, &U1STA, "_PWM_FREQ: ", 0);
    uart_String(&U1TXREG, &U1STA, itoaconv(PWM_FREQ), 1);
    uart_String(&U1TXREG, &U1STA, "PR2: ", 0);
    uart_String(&U1TXREG, &U1STA, itoaconv(PR2), 1);
  #endif

  /* Some frequency shit */
  int freq[] = {0, 0, 0, 0, 0, 0, 0};
  for(;;) {
    PORTD |= _MSGEQ7_RESET;
    PORTD &= ~_MSGEQ7_RESET;
    for (uint8_t i = 0; i < 7; i++) {  
      freq[i] = msgeq_Read() - 100 < 30 ? 0 : msgeq_Read() - 100;
      display_Bar(i, freq[i]);
      delay(1);
    } 
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
  
  /* PWM Output. */
  /* PRx = (FPB / PWM_FREQ) - 1 */ 
  OC1CON = 0x6;
  PR2 = (PB_CLK / PWM_FREQ) - 1; 
  T2CONSET = 0x8000;
  OC1R = PR2/2;
  OC1RS = 200;
  OC1CONSET = 0x8000;

  /* Library inits */ 
  msgeq_Init(&TRISB, 4, 4);
  display_Init();
  display_Clear(); 
  display_Logo();
  _delay(2000);
  display_Clear();
  _enable_interrupt();
}


