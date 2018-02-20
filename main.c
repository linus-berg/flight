#include <stdio.h>
#include <pic32mx.h>
#include "flight.h"
#include <stdint.h>

int spi_send_recv(int data);
void test_OLED( ){
  spi_send_recv(0xA5);
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
  #endif 
  test_OLED();
  int i = 0;
  while(1) {
    for(i = 0; i <= PR2/2; i++) {
      OC1RS = i;
      delay(1);
    }
    for(i = i; i >= 0; i--) {
      OC1RS = i;
      delay(1);
    }
    delay(1000);
    OC1RS = PR2;
    delay(10000);
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
  OC1R = PR2;
  OC1RS = PR2/2;
  OC1CONSET = 0x8000;
  
  /* Display init */
	PORTF = 0x70;
	PORTG |= (1 << 9);
	TRISFCLR = _DISPLAY_PORTF_MASK;
	TRISGCLR = _DISPLAY_PORTG_MASK;
  
  PORTF &= ~_DISPLAY_DATA;
	delay(10);
	PORTF &= ~_DISPLAY_VDD;
	delay(10);
  spi_send_recv(0xAE);
	PORTG &= ~_DISPLAY_RESET;
	delay(10);
	PORTG |= _DISPLAY_RESET;
	delay(10);
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	PORTF &= ~_DISPLAY_VBAT;
	delay(10);
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	spi_send_recv(0xAF);
  spi_send_recv(0x81);
  spi_send_recv(0xFF);
}

int spi_send_recv(int data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 0x01));
	return SPI2BUF;
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
