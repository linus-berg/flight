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
  if ((((IFS(0) >> 19) & 0x1) == 1)) {
    uart_TX(&U1TXREG, &U1STA, 0x50);
    IFS(0) &= ~(0x00080000);  // reset INT4 flagg till 0 (bit 19)
  }
  if ((((IFS(0) >> 15) & 0x1) == 1)) {
    uart_TX(&U1TXREG, &U1STA, 0x51);
    IFS(0) &= ~(0x00008000);  // reset INT3 flagg till 0 (bit 15)
  }
  if ((((IFS(0) >> 11) & 0x1) == 1)) {
    uart_TX(&U1TXREG, &U1STA, 0x52);
    IFS(0) &= ~(0x00000800);  // reset INT3 flagg till 0 (bit 15)
  }
  if (IFS(0) >> 27) {
    uint8_t rx;
    while((U1STA & 0x1)) {
      rx = U1RXREG & 0xFF;
      if (rx == 0x1) {
        LED_CON.red = LED_CON.red ? 0 : 1;
        uart_TX(&U1TXREG, &U1STA, LED_CON.red ? 0x11 : 0x01);
      } else if (rx == 0x2) {
        LED_CON.green = LED_CON.green ? 0 : 1;
        uart_TX(&U1TXREG, &U1STA, LED_CON.green ? 0x12 : 0x02);
      } else if (rx = 0x3) {
        LED_CON.blue = LED_CON.blue ? 0 : 1;
        uart_TX(&U1TXREG, &U1STA, LED_CON.blue ? 0x13 : 0x03);
      }
    }
    IFS(0) &= ~(1 << 27);
  }
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
  for (;;) {
    PORTB |= _MSGEQ7_RESET;
    PORTB &= ~_MSGEQ7_RESET;
    for (uint8_t i = 0; i < 7; i++) {
      /*
        Do not combine the ternary operator and the first freq[i] statement
        msgeq_Read automatically advances the MSGEQ7 multiplexor to next
        frequency.
      */
      freq[i] = msgeq_Read();
      display_Bar(i, freq[i]);
    }
    PORTE = 0xFF >> (8 - (uint8_t)((8.0 / 980) * freq[0])); 
    OC1RS = (200.0 / 1023.0) * ((freq[0] + freq[1]) / 2);
  }
}


void Init() {
	/* Set up SPI as master */
  SPI2CON = 0;
  SPI2BRG = 4;
  SPI2STATCLR &= ~0x40;
  SPI2CON |= 0x60;
  SPI2CONSET = 0x8000;

  TRISD |= 0x800;   // switch 4(längst vänster) påslagen
  TRISD |= 0x400;   // switch 3(näst längst vänster) påslagen
  TRISD |= 0x200;
  IEC(0) |= 0x88800;   // interrupt enable control (bit 15=INT3 och 19=INT4)
  IPC(2) |= 0x1A000000;
  IPC(3) |= 0x1B000000; // INT3 priority
  IPC(4) |= 0x1C000000; // INT4 priority
  /* Default setup, RGB and enabled. */
  LED_CON.red = 1;
  LED_CON.green = 1;
  LED_CON.blue = 1;
  LED_CON.inverted = 0;
  LED_CON.enabled = 1;
  TRISE &= ~0xFF;
  PORTE &= ~0xFF; 

  /* PWM Output. */
  /* PRx = (FPB / PWM_FREQ) - 1 */ 
  OC1CON = 0x6;
  PR2 = 1023;
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


