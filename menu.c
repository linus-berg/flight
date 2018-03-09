#include "include/menu.h"
#include "include/display.h"
#include "include/flight.h"
#include "include/math.h"
#include "include/uart.h"
uint8_t menu[4 * 128] = {0};
unsigned mouse[] = {
  0b00001000,
  0b00011100,
  0b00111110,
  0b01111111,
  0b00011000,
  0b00011000
};

unsigned play_btn[] = {
  0b1100000000010110011,
  0b1111000000100110011,
  0b1111110001000110011,
  0b1111111110000110011,
  0b1111110100000110011, 
  0b1111001000000110011,
  0b1100010000000110011
};

unsigned next_btn[] = {
  0b11000001,
  0b11110001,
  0b11111101,
  0b11111111,
  0b11111101,
  0b11110001,
  0b11000001
};

unsigned prev_btn[] = {
  0b10000011,
  0b10001111,
  0b10111111,
  0b11111111,
  0b10111111,
  0b10001111,
  0b11000011
};
int menu_Buttons() {
  return (((PORTD >> 5) & 0x7) | ((PORTF & 0x2) << 2));
}
void menu_Init() {
  TRISD |= 0xE0;
  TRISF |= 0x2;
}
void menu_Display() {
  
  layer_Object mouse_obj;
  mouse_obj.w = 8;
  mouse_obj.h = 6;
  mouse_obj.x = 63;
  mouse_obj.y = 14;
  mouse_obj.bitmap = mouse;
  
  layer_Object play_btn_obj;
  play_btn_obj.w = 20;
  play_btn_obj.h = sizeof(play_btn) / sizeof(unsigned);
  play_btn_obj.x = 28;
  play_btn_obj.y = 2;
  play_btn_obj.bitmap = play_btn;
  
  layer_Object next_btn_obj;
  next_btn_obj.w = 8;
  next_btn_obj.h = sizeof(next_btn) / sizeof(unsigned);
  next_btn_obj.x = 70;
  next_btn_obj.y = 2;
  next_btn_obj.bitmap = next_btn;

  layer_Object prev_btn_obj;
  prev_btn_obj.w = 8;
  prev_btn_obj.h = sizeof(prev_btn) / sizeof(unsigned);
  prev_btn_obj.x = 50;
  prev_btn_obj.y = 2;
  prev_btn_obj.bitmap = prev_btn;
  for(;;) {
    uint8_t btns = menu_Buttons(); 
    if (btns & 0x1) {
      mouse_obj.y--;
    }
    if (btns & 0x2) {
      mouse_obj.y++;
    }
    if (btns & 0x4) {
      mouse_obj.x--;
    }
    if (btns & 0x8) {
      mouse_obj.x++;
    }
    mouse_obj.y = mouse_obj.y >= 31 ? 31 : mouse_obj.y;
    mouse_obj.y = mouse_obj.y <= 1  ? 1  : mouse_obj.y;
    mouse_obj.x = mouse_obj.x >= 119 ? 119 : mouse_obj.x;
    mouse_obj.x = mouse_obj.x <= 4  ? 4  : mouse_obj.x;
    display_AddObject(mouse_obj, menu);
    display_AddObject(play_btn_obj, menu);
    display_AddObject(next_btn_obj, menu);
    display_AddObject(prev_btn_obj, menu);
    display_RenderBuffer(menu);
    _delay(10);
    display_ClearBuffer(menu);
    if (abs(mouse_obj.x - next_btn_obj.x) < 6 &&
        abs(mouse_obj.y - next_btn_obj.y) < 6) {
      uart_TX(&U1TXREG, &U1STA, 0x52);
      return;
    }
    if (abs(mouse_obj.x - prev_btn_obj.x) < 6 &&
        abs(mouse_obj.y - prev_btn_obj.y) < 6) {
      uart_TX(&U1TXREG, &U1STA, 0x51);
      return;
    }
    if (abs(mouse_obj.x - play_btn_obj.x) < 6 &&
        abs(mouse_obj.y - play_btn_obj.y) < 6) {
      uart_TX(&U1TXREG, &U1STA, 0x50);
      return;
    }
  }
}

