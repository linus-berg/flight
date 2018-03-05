#include <stdint.h>
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

#endif
