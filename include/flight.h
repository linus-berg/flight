#ifndef FLIGHT_H_
#define FLIGHT_H_

#include <stdint.h>
#include "uart.h"
#include "sys.h"
#define             SYS_FREQ (80000000L)
#define             COUNTS_PER_MICRO_SEC ((SYS_FREQ/2L)/1000000L)
#define             COUNTS_PER_MILLI_SEC ((SYS_FREQ/2L)/1000L)

void Init();
extern void _enable_interrupt();
extern void _delay();
struct {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
  uint8_t enabled;
} LED_CON;

void delay(int delay) {
  for(int i = 0; i < delay; i++);
}

int spi_TX(uint8_t data) {
	while (!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while (!(SPI2STAT & 0x01));
	return SPI2BUF;
}

#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the first occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
#endif
