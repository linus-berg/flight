#ifndef FLIGHT_H_
#define FLIGHT_H_
#include <pic32mx.h>
#include <stdint.h>
#include "uart.h"
#include "sys.h"
#define ITOA_BUFSIZ ( 24 )
void Init();
void delay();
int spi_TX(uint8_t data);
char * itoaconv(int num);
extern void _enable_interrupt();
extern void _delay();
struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} LED_CON;

#endif
