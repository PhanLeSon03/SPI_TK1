#include "Accelerometer.h"
#include "Langconfig.h"
void Alarm_On( void );
void Alarm_Off(void);
void SendMassageAlarm(sentence_idx_t Phase);
void NVIC_Configuration_ACC_INT_DIS(void);
void SendMassageAlarm_Detect(void);
void Alarm_Auto(void);