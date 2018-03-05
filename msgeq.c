#include "include/msgeq.h"
#include <pic32mx.h>
/*
  Initialise the MSGEQ7 equalizer.
  ARGS:
    volatile unsigned Port for input.
    uint8_t Port #(bit).
    uint8_t Analog port # (0:15). 
*/
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

/*
  Read frequencies from the MSGEQ7.
  RETURN:
    uint16_t
  NOTE:
    The function automatically moves the multiplexor to the next frequency.
*/
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
  return ((freq - 100) < 30 ? 0 : (freq - 100));
}
