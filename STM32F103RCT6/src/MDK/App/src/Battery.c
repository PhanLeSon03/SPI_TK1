#include "Battery.h"
float TableBattery[20] = {
        4.23, 4.22, 4.21, 4.20,
        4.19, 4.18, 4.17, 4.16,
        4.15, 4.14, 4.13, 4.12,
        4.11, 4.10, 4.09, 4.08,
        4.07, 4.06, 4.05, 4.04
};
#if 0
void senReadMONITOR_VBATInit(void)
{
     /* ADCCLK = PCLK2/6 */
   RCC_ADCCLKConfig(RCC_PCLK2_Div6);
   /* Enable ADC1 clock so that we can talk to it */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2|RCC_APB2Periph_GPIOA, ENABLE);

   GPIO_InitTypeDef GPIO_InitStructure;  
   GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
   GPIO_Init(GPIOA, &GPIO_InitStructure);
   
   //config ADC
   ADC_InitTypeDef ADC_InitStructure;
   /*ADC1 and ADC2 operate independencely*/
   ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
   /* Disable the scan conversion soo we do one at the time */
   ADC_InitStructure.ADC_ScanConvMode = DISABLE;
   /* Don't do contimuous conversions - do them on demand */
   ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
   /* Start conversion by software, not an external trigger */
   ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
   /* Conversions are 12 bit - put them in the lower 12 bits of the result */
   ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
   /* Say how many channels would be used by the sequencer */
   ADC_InitStructure.ADC_NbrOfChannel = 1;
   ADC_Init(ADC2, &ADC_InitStructure);
 
   /* ADC1 regular channels configuration */ 
   ADC_RegularChannelConfig(ADC2, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);

   
   /* Enable ADC2*/
   ADC_Cmd(ADC2, ENABLE);
    
   /* Enable ADC1 reset calibaration register */
   ADC_ResetCalibration(ADC2);

   /* Start ADC1 calibaration */	
   ADC_StartCalibration(ADC2);
   
   /* Check the end of ADC1 calibration */
   while(ADC_GetCalibrationStatus(ADC2));
 
}
#endif
void senReadMONITOR_VBATInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//config ADC
	ADC_InitTypeDef ADC_InitStructure;

	 /* ADCCLK = PCLK2/6 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	/* Enable ADC2 clock so that we can talk to it */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2 | RCC_APB2Periph_GPIOC, ENABLE);


	/* gpio init ?*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	ADC_DeInit(ADC2);

	/*ADC1 and ADC2 operate independencely*/
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	/* Disable the scan conversion soo we do one at the time */
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	/* Don't do contimuous conversions - do them on demand */
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	/* Start conversion by software, not an external trigger */
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	/* Conversions are 12 bit - put them in the lower 12 bits of the result */
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	/* Say how many channels would be used by the sequencer */
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC2, &ADC_InitStructure);

	/* ADC2 regular channels configuration */ 
	ADC_RegularChannelConfig(ADC2, ADC_Channel_15, 1, ADC_SampleTime_239Cycles5);


	/* Enable ADC1*/
	ADC_Cmd(ADC2, ENABLE);

	/* Enable ADC1 reset calibaration register */
	ADC_ResetCalibration(ADC2);

	/* Start ADC1 calibaration */	
	ADC_StartCalibration(ADC2);

	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC2));
 
   
   
}
		
float senReadMONITOR_VBAT(void)
{
	uint16_t adc =0; //storage value of ADC
	float volt_bat = 0;    //storage volt value  
	float v_monitor = 0;
	
	/* Start ADC2 Software Conversion */ 
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);

	while (ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC) == RESET)
   		tmDelayms( 10);  /* 10ms */		
	
	adc =  ADC_GetConversionValue(ADC2);	  

	v_monitor = ((float)adc/4096)*MCU_VOL;
	// Calculating the temperature sensor
	volt_bat = (v_monitor * 17.05) /11;//17-11
	return volt_bat;	  
}

float PercentBat(void)
{
	uint8_t i,j;
	float Volt_Bat, Delta_t, Consume_Energy=0, Percent;
	Volt_Bat = senReadMONITOR_VBAT();
	if (Volt_Bat>TableBattery[0])
	{
		return 100;
	}
	else
	{
		for(i=0;i< (20 - 1);i++)
		{
			if ((Volt_Bat<=TableBattery[i])&(Volt_Bat>=TableBattery[i+1]))
			{
				Delta_t = (Volt_Bat - TableBattery[i+1])/(TableBattery[i] - TableBattery[i+1])*3600;
				Consume_Energy += ((TableBattery[i]/2 + Volt_Bat/2)*(TableBattery[i]/2 + Volt_Bat/2)/R)*Delta_t; 
				break;
			}	
			Consume_Energy += ((TableBattery[i]/2 + TableBattery[i+1]/2)*(TableBattery[i]/2 + TableBattery[i+1]/2)/R)*3600;  
		}
		Percent = ((EnergyCapacity - Consume_Energy)/EnergyCapacity)*100;
		return Percent;
	}
	
}



