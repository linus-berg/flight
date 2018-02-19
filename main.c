#include <pic32mx.h>
#define _SYS_CLK 80000000
#define _PB_CLK (_SYS_CLK >> ((OSCCON & 0x180000) >> 19));
#define _PWM_FREQ 8192

void Init();
void TX_Byte(unsigned char byte);
void TX_ByteMulti(unsigned char *bytes);
void BRG_SetBaud(int baud_rate);

int main(void) {
  /* Init all the important shit. */
  Init();
  while(1);
}

void Init() {
  /* Set Peripheral clock to 40Mhz (80Mhz / 2) 010 */
  OSCCON &= ~0x180000;
  OSCCON |= 0x080000;

  /* UART initialisation */
  BRG_SetBaud(80000000, 9600);
	U1STA = 0;
  U1MODE = 0x8000;
	U1STASET = 0x1400;

  /* PWM Output. */
  OC1CON = 0x6;
  /* PRx = (FPB / PWM_FREQ) - 1 */ 
  PR2 = (_PB_CLK / _PWM_FREQ) - 1; 
  T2CONSET = 0x8000;
  OC1CONSET = 0x8000;
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
void TX_ByteMulti(unsigned char *bytes) {
  while (*bytes) {
    TX_byte(*bytes);
    bytes++;
  }
}

void BRG_SetBaud(int baud_rate) {
  /* Peripheral clock */
  U1BRG = (_PB_CLK / (16 * baud_rate)) - 1;

