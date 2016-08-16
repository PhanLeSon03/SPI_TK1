#include "stm32f10x_i2c.h"
#include "EXTI_ACCE.h"
#include "Delayms.h"
#include <math.h>
#include "stm32f10x_it.h"

 
 
#define ClockSpeed              3000
#define MODE_8G  0x00
#define MODE_4G  0x08
#define MODE_2G  0x04
#define MODE_STAND  0x00
#define MODE_MEAS   0x01
#define MODE_LEVEL   0x02
#define MODE_PULSE  0x03
#define  I2C1_ACC_ADDRESS7  0x90   
#define  I2C2_ACC_ADDRESS7  0x3A  //0x3A 

#define READ   0x01
#define WRITE 0x00

#define XOUTL 0x00
#define XOUTH 0x01

#define YOUTL 0x02
#define YOUTH 0x03

#define ZOUTL 0x04
#define ZOUTH 0x05

#define XOUT 0x06
#define YOUT 0x07
#define ZOUT 0x08

#define STATUS         0x09
#define DETSRC         0x0A
#define TOUT             0x0B
#define I2CAD           0x0D
#define USRINF         0x0E
#define WHOAMI       0x0F
#define XOFFL          0x10
#define XOFFH          0x11
#define YOFFL           0x12
#define YOFFH          0x13
#define ZOFFL           0x14
#define ZOFFH          0x15
#define MCTL            0x16
#define INTRST          0x17
#define CTL1             0x18
#define CTL2             0x19
#define LDTH            0x1A
#define PDTH            0x1B
#define PW               0x1C
#define LT                0x1D
#define TH               0x1E

void I2CInit(void);
void SetAccRange (u8 ModeStatus);
void SetAccThresholdINT(s8 ValueThres);
float ReadAccValueX (void);
float ReadAccValueY (void);
float ReadAccValueZ (void);

float ReadAccValueZ10Bit (void);
float ReadAccValueY10Bit (void);
float ReadAccValueX10Bit (void);

void ClearIntLatch(void);
void IIC_ByteWrite(u8 Address, s8 data);
s8 IIC_ByteRead(u8 Address);
void SetStandByACC(void);

void NVIC_Configuration_I2C( void);
void DetectACC(s8 ValueThreshold, u8 TimeDetect,float *maxX,float  *minX, float *maxY, float *minY, float *maxZ, float *minZ);

void SetOffsetX (void);
void SetOffsetY (void);
void SetOffsetZ (void);
s8 ConvertTypeNumber(u8 Input);
s8 ReConvertTypeNumber(u8 Input);
float Round(float Input);
void SelfTestZAxis(FunctionalState Switch);

float CalculateAngleZACC ( float x, float y, float z);
float CalculateAngleYACC ( float x, float y, float z);
float CalculateAngleXACC ( float x, float y, float z);
u8 DetectStop(void);