#include "stm32f10x.h"
#include "stm32f10x_adc.h"
#define EnergyCapacity 1*3600*4
#define R                      10*1000
#define MCU_VOL          3.0
float senReadMONITOR_VBAT(void);
float PercentBat(void);
void senReadMONITOR_VBATInit(void);