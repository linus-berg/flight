/* Written by Linus Gunnarsson */
#ifndef DISPLAY_LIB 
#define DISPLAY_LIB
#include <stdint.h>
#define _DISPLAY_PORTF_MASK 0x70
#define _DISPLAY_PORTG_MASK 0x200
#define _DISPLAY_PORTD 0x10
#define _DISPLAY_VBAT (1 << 5)
#define _DISPLAY_VDD (1 << 6)
#define _DISPLAY_DATA (1 << 4)
#define _DISPLAY_RESET (1 << 9) 

/* Bar graph intensity numbers */
#define INTENSITY_12  0x000000F0
#define INTENSITY_25  0x000000FF
#define INTENSITY_37  0x0000F0FF
#define INTENSITY_50  0x0000FFFF
#define INTENSITY_62  0x00F0FFFF
#define INTENSITY_75  0x00FFFFFF
#define INTENSITY_87  0xF0FFFFFF
#define INTENSITY_100 0xFFFFFFFF

void Display_SetColumn(uint8_t start_col, uint8_t end_col);
void Display_SetPage(uint8_t page);
void Display_Init();
void Display_Clear();
void Display_Logo();
void Display_Letter(uint8_t letter);
void Display_BarGraph(uint8_t col, int intensity);

static int bars[] = {0, 0, 0, 0, 0, 0, 0};
static const int font[][5] = {
  {0xFF, 0x09, 0x09, 0x09, 0x01}, /* F */
  {0xFF, 0x80, 0x80, 0x80, 0x80}, /* L */
  {0x00, 0x00, 0xFF, 0x00, 0x00}, /* I */
  {0xFF, 0x81, 0x91, 0x91, 0xF1}, /* G */
  {0xFF, 0x18, 0x18, 0x18, 0xFF}, /* H */
  {0x01, 0x01, 0xFF, 0x01, 0x01}  /* T */
};

void Display_SetColumn(uint8_t start_col, uint8_t end_col) {
  SPI_TX(0x21);
  SPI_TX(start_col % 128);
  SPI_TX(end_col);
}

void Display_SetPage(uint8_t page) {
  SPI_TX(0x22);
  SPI_TX(page % 4);
  SPI_TX(3);
}

void Display_Clear() {
  for (uint8_t row = 0; row < 9; row++) {
    PORTF &= ~_DISPLAY_DATA;
    Display_SetPage(row);
    Display_SetColumn(0, 0x7F);
    PORTF |= _DISPLAY_DATA;
    for (uint8_t col = 0; col <= 127; col++) {
      SPI_TX(0x0);
    }
  }
}

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

void Display_Logo(){
  /* Top Border. */
  PORTF &= ~_DISPLAY_DATA;
  Display_SetPage(0);
  Display_SetColumn(0, 0x7F); 
  PORTF |= _DISPLAY_DATA;
  for (uint8_t col = 0; col <= 127; col++) {
    if (col % 2 == 0) {
      SPI_TX(0x81);
    } else {
      SPI_TX(0xBD);
    }
    delay(10);
  }
  /* Bottom Border. */
  PORTF &= ~_DISPLAY_DATA;
  Display_SetPage(3);
  Display_SetColumn(0, 0x7F);
  PORTF |= _DISPLAY_DATA;
  for (uint8_t col = 0; col <= 127; col++) {
    if (col % 2 == 0) {
      SPI_TX(0x81);
    } else {
      SPI_TX(0xBD);
    }
    delay(10);
  }
  /* R G B */
  PORTF &= ~_DISPLAY_DATA;
  Display_SetPage(1);
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

void Display_Letter(uint8_t letter) {
  for (int col = 0; col < 5; col++) {
    SPI_TX(font[letter][col]);
    delay(50);
  }
}
/* Col 0 - 7  */
void Display_BarGraph(uint8_t col, int intensity) {
  PORTF &= ~_DISPLAY_DATA;
  Display_SetPage(0);
  Display_SetColumn(1 + (col * 18), 19 + (col * 18));
  PORTF |= _DISPLAY_DATA;
  uint8_t bits = 0;
  for (uint8_t page = 0; page < 4; page++) {
    bits = (((0xFF000000 >> (page*8)) & intensity) >> (8 * (3 - page)));
    for (uint8_t col = 0; col <= 18; col++) {
      if (col == 18 || col == 0) {
        SPI_TX(0x0);
      } else {
        SPI_TX(bits);
      }
    }
  }
}

void Display_BarDemo() {
  int intense[] = {INTENSITY_100, INTENSITY_75, INTENSITY_50, INTENSITY_12};
  int i = 0;
  for (;;) {
    Display_BarGraph(i % 4, intense[(i + i / 2) % 4]);
    delay(100);
    i++;
  }
}
#endif
