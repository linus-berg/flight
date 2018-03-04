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
  if ((IFS(0) >> 11) & 0x1) {
    uart_TX(&U1TXREG, &U1STA, 0x52);
    IFS(0) &= ~(0x00000800);
  }
  if ((IFS(0) >> 15) & 0x1) {
    uart_TX(&U1TXREG, &U1STA, 0x51);
    IFS(0) &= ~(0x00008000);
  }
  if ((IFS(0) >> 19) & 0x1) {
    uart_TX(&U1TXREG, &U1STA, 0x50);
    IFS(0) &= ~(0x00080000);
  }
  /* UART RX Interrupt */
  if ((IFS(0) >> 27) & 0x1) {
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

  int freq[] = {0, 0, 0, 0, 0, 0, 0};
  uint16_t pwm = 0;
  for (;;) {
    PORTB |= _MSGEQ7_RESET;
    PORTB &= ~_MSGEQ7_RESET;
    for (uint8_t i = 0; i < 7; i++) {
      freq[i] = msgeq_Read();
      display_Bar(i, freq[i]);
    }
    PORTE = 0xFF >> (8 - (uint8_t)((8.0 / 980) * freq[0]));
    pwm = (freq[0] + freq[1]) / 2;
    OC1RS = LED_CON.red ? pwm : 0;
    OC2RS = LED_CON.green ? pwm : 0;
    OC3RS = LED_CON.blue ? pwm : 0;
    //OC3RS = pwm;
  }
}


void Init() {
  SPI2CON = 0;
  SPI2BRG = 4;
  SPI2STATCLR &= ~0x40;
  SPI2CON |= 0x60;
  SPI2CONSET = 0x8000;

  TRISD |= 0xE00;
  TRISD &= ~(0x7);
  
  IEC(0) |= 0x88800;
  IEC(0) |= 1 << 27; 
  IPC(2) |= 0x1A000000;
  IPC(3) |= 0x1B000000;
  IPC(4) |= 0x1C000000;
  IPC(6) |= 0x8;

  /* Default setup, RGB and enabled. */
  LED_CON.red = 1;
  LED_CON.green = 1;
  LED_CON.blue = 1;
  LED_CON.enabled = 1;
  TRISE &= ~0xFF;
  PORTE &= ~0xFF; 

  /* PWM Output. */
  PR2 = 1023;
  T2CONSET = 0x8000;
  
  OC1CON = 0x6;
  OC1R = PR2/2;
  OC1RS = 200;
  OC1CONSET = 0x8000;
  
  OC2CON = 0x6;
  OC2R = PR2/2;
  OC2RS = 0;
  OC2CONSET = 0x8000;
  
  OC3CON = 0x6;
  OC3R = PR2/2;
  OC3RS = 0;
  OC3CONSET = 0x8000;

  msgeq_Init(&TRISB, 4, 4);
  uart_Init(&U1STA, &U1MODE, &U1BRG, 0x8000, 0x1400, 9600);
  display_Init();
  display_Logo();
  _enable_interrupt();
}


