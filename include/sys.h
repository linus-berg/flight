#include <pic32mx.h>
#ifndef SYS
#define SYS
#define SYS_CLK 80000000
#define PB_CLK (SYS_CLK >> ((OSCCON & 0x180000) >> 19))
#define PWM_FREQ 100 
#endif

