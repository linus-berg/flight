#ifndef MSGEQ7 
#define MSGEQ7 

#define _MSGEQ7_READ 0x1
#define _MSGEQ7_STROBE 0x2
#define _MSGEQ7_RESET 0x4

void MSGEQ_Init();

void MSGEQ_Init() {
  AD1PCFG &= ~_MSGEQ7_READ;
  TRISBSET = _MSGEQ7_READ;
  AD1CON1 = (0x4 << 8) | (0x7 << 5);

	AD1CON2 = 0x0;
  AD1CON3 |= (0x1 << 15);
  AD1CON1 |= (0x1 << 15);
  /* Strobe & Reset */
  TRISDCLR = 0x6;
  PORTD |= _MSGEQ7_STROBE;
  PORTD &= ~_MSGEQ7_RESET;
}
#endif