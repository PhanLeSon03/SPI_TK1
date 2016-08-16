#include "stm32f10x.h"
#include "stm32f10x_it.h"
#ifndef SystemClock
     #define SystemClock 72000000
#endif

void tmInit(void);
void tmStart(void);
void tmStop(void);
void tmDelayms(uint32_t nCount);