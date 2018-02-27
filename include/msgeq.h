/*
* FILENAME: msgeq.h
* DESCRIPTION:
*   Analog port setup and read functions for MSGEQ7 microchip.
* FUNCTIONS:
*   void MSGEQ_Init(port, port_bit, port_an)
*   int MSGEQ_Read()
* NOTES:
*   Strobe and Reset are hardcoded to port D(1 & 2).
*   Strobe: D1
*   Reset:  D2 
*
* AUTHOR: Linus Gunnarsson    
*/
#ifndef MSGEQ7_H_
#define MSGEQ7_H_
#define _MSGEQ7_STROBE 0x100
#define _MSGEQ7_RESET 0x400

void msgeq_Init(volatile unsigned *port, uint8_t port_bit, uint8_t port_an);
uint16_t msgeq_Read();

void msgeq_Init(volatile unsigned *port, uint8_t port_bit, uint8_t port_an) {

  AD1PCFG &= ~(0x1 << port_an);
  *port |= (0x1 << port_bit);
  AD1CON1 = (0x4 << 8) | (0x7 << 5);

	AD1CON2 = 0x0;
  AD1CON3 |= (0x1 << 15);
  AD1CON1 |= (0x1 << 15);
  AD1CHS = (port_an << 16);

  /* Strobe & Reset */
  TRISBCLR = 0x500;
  PORTB |= _MSGEQ7_STROBE;
  PORTB &= ~_MSGEQ7_RESET;
}

uint16_t msgeq_Read() {
  uint16_t freq = 0;
  PORTB &= ~_MSGEQ7_STROBE;
  delay(8000);
  AD1CON1 |= (0x1 << 1);
  while(!(AD1CON1 & (0x1 << 1)));
  while(!(AD1CON1 & 0x1));
  /* Get the analog value */
  freq = ADC1BUF0;
  PORTB |= _MSGEQ7_STROBE;
  return freq;
}
#endif
