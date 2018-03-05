#include "include/display.h"
#include "include/flight.h"
#include <pic32mx.h>
void display_Init() {
  /* Display init */
  PORTF = 0x70;
  PORTG |= (1 << 9);
  TRISFCLR = DISPLAY_PORTF_MASK;
  TRISGCLR = DISPLAY_PORTG_MASK;
  
  PORTF &= ~DISPLAY_DATA;
  delay(10);
  PORTF &= ~DISPLAY_VDD;
  delay(10);
  spi_TX(0xAE);
  PORTG &= ~DISPLAY_RESET;
  delay(10);
  PORTG |= DISPLAY_RESET;
  delay(10);
  
  spi_TX(0x8D);
  spi_TX(0x14);
  spi_TX(0xD9);
  spi_TX(0xF1);
  PORTF &= ~DISPLAY_VBAT;
  delay(10);
  spi_TX(0xA1);
  spi_TX(0xC8);

  spi_TX(0xDA);
  spi_TX(0x20);

  /* Horizontal addressing mode pls */
  spi_TX(0x20);
  spi_TX(0x00);
  /* Start VROOM VROOM */
  spi_TX(0xAF);
  /* Contrast */
  spi_TX(0x81);
  spi_TX(0xFF);
  display_Clear(); 
}

/*
  Set active columns on display.
  ARGS:
    uint8_t Starting column (0:127).
    uint8_t End column (0:127).
  NOTE:
    The display automatically increments columns on data write.
*/
inline void display_SetColumn(uint8_t start_col, uint8_t end_col) {
  spi_TX(0x21);
  spi_TX(start_col % 128);
  spi_TX(end_col);
}

/*
  Set active pages on display.
  ARGS:
    uint8_t Starting page (0:3).
    uint8_t End page (0:3).
  NOTE:
    The display automatically switches to next page on column overflow.
*/
inline void display_SetPage(uint8_t page, uint8_t end_page) {
  spi_TX(0x22);
  spi_TX(page % 4);
  spi_TX(end_page % 4);
}

void display_Clear() {
  PORTF &= ~DISPLAY_DATA;
  display_SetPage(0, 3);
  display_SetColumn(0, 0x7F);
  PORTF |= DISPLAY_DATA;
  for (uint8_t row = 0; row < 4; row++) {
    for (uint8_t col = 0; col <= 127; col++) {
      spi_TX(0x0);
    }
  }
}

/*
  Print a letter to the display.
  ARGS:
    uint8_t Letter to print (from font[]).
*/
void display_Letter(uint8_t letter) {
  for (uint8_t col = 0; col < 5; col++) {
    spi_TX(font[letter][col]);
  }
}

/*
  Display a bar graph with the intensity of the given frequency.
  ARGS:
    uint8_t Column (0:7).
    uint8_t Frequency (0:1023).
*/
void display_Bar(uint8_t col, uint16_t freq) {
  if (freq == bars[col]) {
    return;
  } else {
    bars[col] = freq;
  }
  /*
  * IMPORTANT: This part is bad, shifting by 32 is undefined behaviour.
  * In our case it works, so fuck it.
  * P.S: Really fucking bad.
  */
  unsigned int intensity = 0xFFFFFFFF;
  intensity = intensity >> (uint8_t)(32 - (32.0 / 1023.0) * freq);
  /* Quick hacky fix for the overflow. TODO: Make this code correct.*/
  intensity = intensity == -1 ? 0 : intensity;

  PORTF &= ~DISPLAY_DATA;
  display_SetPage(0, 3);
  display_SetColumn(1 + (col * 18), 19 + (col * 18));
  PORTF |= DISPLAY_DATA;

  uint8_t bits;
  for (uint8_t page = 0; page < 4; page++) {
    bits = (((0xFF000000 >> (page * 8)) & intensity) >> (8 * (3 - page)));
    if (bits < 0xFF && bits != 0) {
      while ((bits & 0x80) == 0)
        bits <<= 1;
    }
    for (uint8_t col = 0; col <= 18; col++) {
      spi_TX(col == 18 || col == 0 ? 0x0 : bits);
    }
  }
}

/* Just some fancy stuff. */
void display_Logo() {
  /* Top Border. */
  PORTF &= ~DISPLAY_DATA;
  display_SetPage(0, 0);
  display_SetColumn(0, 0x7F); 
  PORTF |= DISPLAY_DATA;

  for (uint8_t col = 0; col <= 127; col++) {
    spi_TX(col % 2 == 0 ? 0x81 : 0xBD);
    _delay(8);
  }

  /* Bottom Border. */
  PORTF &= ~DISPLAY_DATA;
  display_SetPage(3, 3);
  display_SetColumn(0, 0x7F);
  PORTF |= DISPLAY_DATA;

  for (uint8_t col = 0; col <= 127; col++) {
    spi_TX(col % 2 == 0 ? 0x81 : 0xBD);
    _delay(8);
  }

  /* FLIGHT */
  PORTF &= ~DISPLAY_DATA;
  display_SetPage(1, 1);
  display_SetColumn(46, 0x7F);
  PORTF |= DISPLAY_DATA;
  
  display_Letter(0);
  spi_TX(0x0);
  display_Letter(1);
  spi_TX(0x0);
  display_Letter(2);
  spi_TX(0x0);
  display_Letter(3);
  spi_TX(0x0);
  display_Letter(4);
  spi_TX(0x0);
  display_Letter(5);  
  _delay(2000);
  display_Clear();
}
