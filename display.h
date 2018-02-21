/* Written by Linus Gunnarsson */
#ifndef display_flight
#define display_flight
#define _DISPLAY_PORTF_MASK 0x70
#define _DISPLAY_PORTG_MASK 0x200
#define _DISPLAY_PORTD 0x10
#define _DISPLAY_VBAT 1 << 5
#define _DISPLAY_VDD 1 << 6
#define _DISPLAY_DATA 1 << 4
#define _DISPLAY_RESET 1 << 9

void Display_SetColumn(char column);
void Display_SetPage(char page);
void Display_Init();
void Display_Reset();
void Display_Logo();

int font[][5] = {
  {0xFF, 0x09, 0x09, 0x09, 0x01}, /* F */
  {0xFF, 0x80, 0x80, 0x80, 0x80}, /* L */
  {0x00, 0x00, 0xFF, 0x00, 0x00}, /* I */
  {0xFF, 0x81, 0x91, 0x91, 0xF1}, /* G */
  {0xFF, 0x18, 0x18, 0x18, 0xFF}, /* H */
  {0x01, 0x01, 0xFF, 0x01, 0x01}, /* T */
  {0x7F, 0x0D, 0x15, 0x25, 0x47}  /* R */
};

void Display_SetColumn(char column) {
  SPI_TX(0x21);
  SPI_TX(column % 128);
  SPI_TX(0x7F);
}

void Display_SetPage(char page) {
  SPI_TX(0x22);
  SPI_TX(page % 4);
  SPI_TX(page % 4);
}

void Display_Reset() {
  int i, j;
  for (i = 0; i < 9; i++) {
    PORTF &= ~(_DISPLAY_DATA);
    Display_SetPage(i);
    Display_SetColumn(0);
    PORTF |= _DISPLAY_DATA;
    for (j = 0; j <= 127; j++) {
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
  /* Start VROOM VROOM */
	SPI_TX(0xAF);
  /* Contrast */
  SPI_TX(0x81);
  SPI_TX(0xFF);
}

void Display_Logo(){
  /* Top Border. */
  PORTF &= ~(_DISPLAY_DATA);
  Display_SetPage(0);
  Display_SetColumn(0); 
  PORTF |= _DISPLAY_DATA;
  int j = 0;
  for (j = 0; j <= 127; j++) {
    if (j % 2 == 0) {
      SPI_TX(0x81);
    } else {
      SPI_TX(0xBD);
    }
    delay(10);
  }
  /* Bottom Border. */
  PORTF &= ~(_DISPLAY_DATA);
  Display_SetPage(3);
  Display_SetColumn(0);
  PORTF |= _DISPLAY_DATA;
  for (j = 0; j <= 127; j++) {
    if (j % 2 == 0) {
      SPI_TX(0x81);
    } else {
      SPI_TX(0xBD);
    }
    delay(10);
  }
  /* R G B */
  PORTF &= ~(_DISPLAY_DATA);
  Display_SetPage(1);
  Display_SetColumn(46);
  PORTF |= _DISPLAY_DATA;
  for (j = 0; j < 5; j++) {
    SPI_TX(font[0][j]);
    delay(100);
  }
  SPI_TX(0x0);
  for (j = 0; j < 5; j++) {
    SPI_TX(font[1][j]);
    delay(100);
  }
  SPI_TX(0x0);
  for (j = 0; j < 5; j++) {
    SPI_TX(font[2][j]);
    delay(100);
  }
  SPI_TX(0x0);
  for (j = 0; j < 5; j++) {
    SPI_TX(font[3][j]);
    delay(100);
  }
  SPI_TX(0x0);
  for (j = 0; j < 5; j++) {
    SPI_TX(font[4][j]);
    delay(100);
  }
  SPI_TX(0x0);
  for (j = 0; j < 5; j++) {
    SPI_TX(font[5][j]);
    delay(100);
  }
}
#endif
