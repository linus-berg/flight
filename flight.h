#ifndef _FLIGHT_
#define _FLIGHT_

#define _DEBUG 1 
#define _VERSION_ "0.0.1a"
#define _SYS_CLK 80000000
#define _PB_CLK (_SYS_CLK >> ((OSCCON & 0x180000) >> 19))
#define _PWM_FREQ 8192
#define _DISPLAY_PORTF_MASK 0x70
#define _DISPLAY_PORTG_MASK 0x200
#define _DISPLAY_PORTD 0x10
#define _DISPLAY_VBAT 1 << 5
#define _DISPLAY_VDD 1 << 6
#define _DISPLAY_DATA 1 << 4
#define _DISPLAY_RESET 1 << 9

void Init();
void TX_Byte(unsigned char byte);
void TX_ByteMulti(register unsigned char *bytes, char cr);
void BRG_SetBaud(int baud_rate);

struct {
  int red;
  int green;
  int blue;
  int enabled;
} LED_CON;

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
