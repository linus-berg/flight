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

struct layer_Object{
  uint8_t w;
  uint8_t h;
  uint8_t x;
  uint8_t y;
  unsigned *bitmap;
};

typedef struct layer_Object layer_Object;

void display_Init();
void display_SetColumn(uint8_t start_col, uint8_t end_col);
void display_SetPage(uint8_t page, uint8_t end_page);
void display_Clear();
void display_Letter(uint8_t letter);
void display_Bar(uint8_t col, uint16_t freq);
void display_Logo();
void display_AddXY(uint8_t x, uint8_t y, uint8_t *buffer);
void display_AddObject(layer_Object obj, uint8_t *buffer);
void display_RenderBuffer(uint8_t *buffer);
void display_ClearBuffer(uint8_t *buffer);
#endif
