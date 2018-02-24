#ifndef SYS
#define SYS
#define _SYS_CLK 80000000
#define _PB_CLK (_SYS_CLK >> ((OSCCON & 0x180000) >> 19))
#define _PWM_FREQ 100 
#endif

