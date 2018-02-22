#include <stdio.h>
#include <stdint.h>
#include <pic32mx.h>
#include "flight.h"
#include "display.h"
#include "analog.h"
#include <stdint.h>

int main(void) {
  /* Init all the important shit. */
  Init();
  MSGEQ_Init();
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
  Display_Clear(); 
  Display_Logo();
  delay(2000);
  Display_Clear();
  ///Display_BarDemo();
  /* Some frequency shit */
  int freq[] = {0, 0, 0, 0, 0, 0, 0};
  for(;;) {
    PORTD |= _MSGEQ7_RESET;
    PORTD &= ~_MSGEQ7_RESET;
    for(uint8_t i = 0; i < 7; i++) {  
      freq[i] = MSGEQ_Read() - 100 < 30 ? 0 : MSGEQ_Read() - 100;
    } 
    for(uint8_t i = 0; i < 7; i++) {
      TX_ByteMulti(itoaconv(freq[i]), 0);
      Display_BarGraph(i, freq[i]); 
      TX_ByteMulti("    ", 0);
      delay(5);;
    }
    TX_ByteMulti("", 1);
    if (freq[0] > 400) {
      OC1RS = 1000; 
    } else {
      OC1RS = 0;
    }
    delay(1); 
  }
}


void Init() {
  /* Set Peripheral clock to 40Mhz (80Mhz / 2) */
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
  
  /* PWM Output. */
  /* PRx = (FPB / PWM_FREQ) - 1 */ 
  OC1CON = 0x6;
  PR2 = (_PB_CLK / _PWM_FREQ) - 1; 
  T2CONSET = 0x8000;
  OC1R = PR2/2;
  OC1RS = 200;
  OC1CONSET = 0x8000;
  Display_Init();
}


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
