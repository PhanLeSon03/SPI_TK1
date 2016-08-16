#include "Accelerometer.h"
#include "stm32f10x_it.h"
#include "stm32f10x_tim.h"
#include "Common.h"
#include "Gps.h"

#define G 	9.81
#define T       1 // duration of  filter
float GetSpeed(void);
float GetDistance(void);
void DetectTripStart(float Dis, float Velocity);
void DetectTripStop(void);
void ReportTripStart(void);
void ReportTripStop(void);
int8_t DetectIdle(void);
int8_t GetIgnitionStatus(void);
void initTimer3 (void);
void Trip_On(void);
void Trip_Off(void);
void Trip_Now(void);
u8 UpdateVelocity(void);
float *Filter(float Acc, float Pos);
float NewSpeed(void);