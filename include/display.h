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
#ifndef DISPLAY_H_
#define DISPLAY_H_
#include <stdio.h>
#include <stdint.h>
#include "math.h"
#include "flight.h"

#define DISPLAY_PORTF_MASK 0x70
#define DISPLAY_PORTG_MASK 0x200
#define DISPLAY_VBAT (1 << 5)
#define DISPLAY_VDD (1 << 6)
#define DISPLAY_DATA (1 << 4)
#define DISPLAY_RESET (1 << 9) 

void display_Init();
void display_SetColumn(uint8_t start_col, uint8_t end_col);
void display_SetPage(uint8_t page, uint8_t end_page);
void display_Clear();
void display_Letter(uint8_t letter);
void display_Bar(uint8_t col, uint16_t freq);
void display_Logo();

static int bars[] = {0, 0, 0, 0, 0, 0, 0};
static const int font[][5] = {
  {0xFF, 0x09, 0x09, 0x09, 0x01}, /* F */
  {0xFF, 0x80, 0x80, 0x80, 0x80}, /* L */
  {0x00, 0x00, 0xFF, 0x00, 0x00}, /* I */
  {0xFF, 0x81, 0x91, 0x91, 0xF1}, /* G */
  {0xFF, 0x18, 0x18, 0x18, 0xFF}, /* H */
  {0x01, 0x01, 0xFF, 0x01, 0x01}  /* T */
};

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
}

inline void display_SetColumn(uint8_t start_col, uint8_t end_col) {
  spi_TX(0x21);
  spi_TX(start_col % 128);
  spi_TX(end_col);
}

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

void display_Letter(uint8_t letter) {
  for (uint8_t col = 0; col < 5; col++) {
    spi_TX(font[letter][col]);
  }
}

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
}
#endif
