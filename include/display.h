/* 
* FILENAME: display.h
* DESCRIPTION:
*   Various display functions for SSD1306 (128x32).
* FUNCTIONS:
*   void Display_Init();
*   void Display_SetColumn(start_col, end_col);
*   void Display_SetPage(page, end_page);
*   void Display_Clear();
*   void Display_Letter(letter);
*   void Display_Bar(col, freq);
*   void Display_Logo();
* NOTES:
*   Display size is hardcoded to 128x32.
*
* AUTHOR: Linus Gunnarsson    
*/
#ifndef DISPLAY_LIB 
#define DISPLAY_LIB
#include <stdio.h>
#include <stdint.h>
#include "math.h"
#include "flight.h"

#define _DISPLAY_PORTF_MASK 0x70
#define _DISPLAY_PORTG_MASK 0x200
#define _DISPLAY_VBAT (1 << 5)
#define _DISPLAY_VDD (1 << 6)
#define _DISPLAY_DATA (1 << 4)
#define _DISPLAY_RESET (1 << 9) 

void Display_Init();
void Display_SetColumn(uint8_t start_col, uint8_t end_col);
void Display_SetPage(uint8_t page, uint8_t end_page);
void Display_Clear();
void Display_Letter(uint8_t letter);
void Display_Bar(uint8_t col, uint16_t freq);
void Display_Logo();

static int bars[] = {0, 0, 0, 0, 0, 0, 0};
static const int font[][5] = {
  {0xFF, 0x09, 0x09, 0x09, 0x01}, /* F */
  {0xFF, 0x80, 0x80, 0x80, 0x80}, /* L */
  {0x00, 0x00, 0xFF, 0x00, 0x00}, /* I */
  {0xFF, 0x81, 0x91, 0x91, 0xF1}, /* G */
  {0xFF, 0x18, 0x18, 0x18, 0xFF}, /* H */
  {0x01, 0x01, 0xFF, 0x01, 0x01}  /* T */
};

void Display_Init() {
  /* Display init */
  PORTF = 0x70;
  PORTG |= (1 << 9);
  TRISFCLR = _DISPLAY_PORTF_MASK;
  TRISGCLR = _DISPLAY_PORTG_MASK;
  
  PORTF &= ~_DISPLAY_DATA;
  delay(10);
  PORTF &= ~_DISPLAY_VDD;
  delay(10);
  SPI_TX(0xAE);
  PORTG &= ~_DISPLAY_RESET;
  delay(10);
  PORTG |= _DISPLAY_RESET;
  delay(10);
  
  SPI_TX(0x8D);
  SPI_TX(0x14);
  SPI_TX(0xD9);
  SPI_TX(0xF1);
  PORTF &= ~_DISPLAY_VBAT;
  delay(10);
  SPI_TX(0xA1);
  SPI_TX(0xC8);

  SPI_TX(0xDA);
  SPI_TX(0x20);

  /* Horizontal addressing mode pls */
  SPI_TX(0x20);
  SPI_TX(0x00);
  /* Start VROOM VROOM */
  SPI_TX(0xAF);
  /* Contrast */
  SPI_TX(0x81);
  SPI_TX(0xFF);
}

inline void Display_SetColumn(uint8_t start_col, uint8_t end_col) {
  SPI_TX(0x21);
  SPI_TX(start_col % 128);
  SPI_TX(end_col);
}

inline void Display_SetPage(uint8_t page, uint8_t end_page) {
  SPI_TX(0x22);
  SPI_TX(page % 4);
  SPI_TX(end_page % 4);
}

void Display_Clear() {
  PORTF &= ~_DISPLAY_DATA;
  Display_SetPage(0, 3);
  Display_SetColumn(0, 0x7F);
  PORTF |= _DISPLAY_DATA;
  for (uint8_t row = 0; row < 4; row++) {
    for (uint8_t col = 0; col <= 127; col++) {
      SPI_TX(0x0);
    }
  }
}

void Display_Letter(uint8_t letter) {
  for (uint8_t col = 0; col < 5; col++) {
    SPI_TX(font[letter][col]);
  }
}

void Display_Bar(uint8_t col, uint16_t freq) {
  if (freq == bars[col]) {
    return;
  } else {
    bars[col] = freq;
  }
  unsigned int intensity = 0xFFFFFFFF;
  intensity = intensity >> (uint8_t)(32 - (32.0 / 1023.0) * freq);
  
  PORTF &= ~_DISPLAY_DATA;
  Display_SetPage(0, 3);
  Display_SetColumn(1 + (col * 18), 19 + (col * 18));
  PORTF |= _DISPLAY_DATA;

  uint8_t bits;
  for (uint8_t page = 0; page < 4; page++) {
    bits = (((0xFF000000 >> (page * 8)) & intensity) >> (8 * (3 - page)));
    if (bits < 0xFF && bits != 0) {
      while ((bits & 0x80) == 0)
        bits <<= 1;
    }
    for (uint8_t col = 0; col <= 18; col++) {
      SPI_TX(col == 18 || col == 0 ? 0x0 : bits);
    }
  }
}

void Display_Logo() {
  /* Top Border. */
  PORTF &= ~_DISPLAY_DATA;
  Display_SetPage(0, 0);
  Display_SetColumn(0, 0x7F); 
  PORTF |= _DISPLAY_DATA;

  for (uint8_t col = 0; col <= 127; col++) {
    SPI_TX(col % 2 == 0 ? 0x81 : 0xBD);
    _delay(8);
  }

  /* Bottom Border. */
  PORTF &= ~_DISPLAY_DATA;
  Display_SetPage(3, 3);
  Display_SetColumn(0, 0x7F);
  PORTF |= _DISPLAY_DATA;

  for (uint8_t col = 0; col <= 127; col++) {
    SPI_TX(col % 2 == 0 ? 0x81 : 0xBD);
    _delay(8);
  }

  /* FLIGHT */
  PORTF &= ~_DISPLAY_DATA;
  Display_SetPage(1, 1);
  Display_SetColumn(46, 0x7F);
  PORTF |= _DISPLAY_DATA;
  
  Display_Letter(0);
  SPI_TX(0x0);
  Display_Letter(1);
  SPI_TX(0x0);
  Display_Letter(2);
  SPI_TX(0x0);
  Display_Letter(3);
  SPI_TX(0x0);
  Display_Letter(4);
  SPI_TX(0x0);
  Display_Letter(5);  
}
#endif
