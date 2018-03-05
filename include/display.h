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

#endif
