#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <lidarlite.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include "./GPIO/jetsonGPIO/jetsonGPIO.h"
#include "./PWM/jetsonPWM.h"

using namespace std;

#define LIDAR   0

#define MAIN_PERD    10000000 
#define MAIN_DUTYSTP  100000
#define MAIN_SENSRANGE 68100
#define MOVE_FWD    1
#define MOVE_BACK   2
#define MOVE_LEFT   3
#define MOVE_RIGHT  4

char stKeyPrss,flgMotorRun;
unsigned int numEncoder, numEncoder_Old;
jetsonGPIO PWMMotor,Motor1Dir,Motor2Dir;
jetsonGPIO pushButton;

int getkey() 
{
	
    int character;
    struct termios orig_term_attr;
    struct termios new_term_attr;

    /* set the terminal to raw mode */
    tcgetattr(fileno(stdin), &orig_term_attr);
    memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
    new_term_attr.c_lflag &= ~(ECHO|ICANON);
    new_term_attr.c_cc[VTIME] = 0;
    new_term_attr.c_cc[VMIN] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

    /* read a character from the stdin stream without blocking */
    /*   returns EOF (-1) if no character is available */
    //rewind( stdin );
    //fflush(stdin);
    character = fgetc(stdin);

    /* restore the original terminal attributes */
    tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

    return character;
}

void * Key_Read(void *x_void_ptr)
{
	char *x_ptr = (char *)x_void_ptr;
	
	while (*x_ptr!=27)
	{
		*x_ptr = getkey();
                 //rewind(stdin);
                 //fflush(stdin);
	    //printf("Key Press: %d\n",*x_ptr);
	    usleep(20000);
	}
	
	return NULL;
}

void * Read_Encorder(void *x_void_ptr)
{
	unsigned int *x_ptr = (unsigned int *)x_void_ptr;
	unsigned int stEncoder = low;
	unsigned int stEncoderOld = low;

    while (stKeyPrss!=27)
    {		
		gpioGetValue(pushButton, &stEncoder) ;
		//printf("Pulse status: %d\n",stEncoder);

		if (stEncoder==high && stEncoderOld != high) 
		{
                    *x_ptr=*x_ptr + 1;
	            //printf("Count encoder: %d\n",*x_ptr);
		} 

		usleep(600);
		stEncoderOld = stEncoder;
	}
	
	return NULL;
}

int main()
{
    pthread_t Key_Thread, Encoder_Thread;
    float facCur;
    int vDuty = MAIN_PERD;
    int vDutyPre = MAIN_PERD-MAIN_DUTYSTP;
    int vDutyLidar;
    int distance, distance_Old1, distance_Old2, distance_Old3,distance_Flt;
    int stManCtrl=0;
    int cntPWM = 0;
    unsigned int value = low;
    unsigned int nRPM, nRPM_Old;
    char flgTest=0;
    char flgRun=0;
    jetsonGPIO pinPWM0 = gpio165 ;
    jetsonGPIO pinPWM1 = gpio166 ;
    jetsonPWM PWM0 = 2; // pin 55 of J3A2 - gpio165
	                    // pwm0 (on pin 49 of J3A2) 
						// pwm1 (on pin 50 of J3A1) is enabled by default in rel21 but is being used for the eDP display's backlight.
                        // pwm2 (on pin 55 of J3A2) is disabled by default in rel21.
                        // pwm3 (on pin 58 of J3A2) is disabled by default in rel21
	
    jetsonPWM  PWM1 = 3; // PWM0 on pin 49 of J3A2 - 49
                        // PWM3 on pin 58 of J3A2) - gpio166	
    
    PWMMotor = gpio57; 
    Motor1Dir = gpio161; //pin 43
    Motor2Dir = gpio162; //pin 46
    pushButton = gpio164; //pin 52 of J3A2
	
    gpioExport(pushButton);
    gpioExport(Motor1Dir);
    gpioExport(Motor2Dir);
    gpioExport(PWMMotor);
	
	/* use for PWM intialize */
	gpioExport(pinPWM0);
	gpioExport(pinPWM1);
	gpioSetDirection(pinPWM0,outputPin);
	gpioSetDirection(pinPWM1,outputPin) ;
	
	gpioSetValue(pinPWM0, on);
	gpioSetValue(pinPWM1, on);
	usleep(200000);         // on for 200ms
	gpioSetValue(pinPWM0, off);
	gpioSetValue(pinPWM1, off);
	usleep(200000);         // off for 200ms
	
	gpioSetValue(pinPWM0,low) ;
	gpioSetValue(pinPWM1,low) ;
	gpioUnexport(pinPWM0);   
	gpioUnexport(pinPWM1);		
	
	/* -----------------------------*/

    gpioSetDirection(pushButton,inputPin);
    gpioSetDirection(PWMMotor,outputPin);
	gpioSetDirection(Motor1Dir,outputPin);
	gpioSetDirection(Motor2Dir,outputPin);
    //gpioActiveLow(pushButton,1);

#if LIDAR	
    LidarLite *lidarLite = new LidarLite() ;

    int err = lidarLite->openLidarLite();    
#endif
    distance_Old1 = 0;
    distance_Old2 = 0;
    distance_Old3 = 0;
	
	if(pthread_create(&Key_Thread, NULL, Key_Read, (void *)&stKeyPrss)) 
	{
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    
	if(pthread_create(&Encoder_Thread, NULL, Read_Encorder, (void *)&numEncoder)) 
	{
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

	gpioSetValue(PWMMotor, on);
	gpioSetValue(Motor1Dir, on);
	gpioSetValue(Motor2Dir, on);
    //PWM out
    PWMExport(PWM0);
    PWMSetPeriod(PWM0,MAIN_PERD);
    PWMSetDuty(PWM0,MAIN_PERD);//vDuty
    PWMEnable(PWM0);
	
	PWMExport(PWM1);
    PWMSetPeriod(PWM1,MAIN_PERD);
    PWMSetDuty(PWM1,vDuty);
    PWMEnable(PWM1);

	// 27 is the ESC key
	while (stKeyPrss!=27)
	{
		  
		//err = lidarLite->openLidarLite();
#if LIDAR
		distance = lidarLite->getDistance();
		if ((distance < 0)) 
		{
			int llError ;
			llError = lidarLite->getError() ;
			printf("Lidar-Lite error: %d\n",llError) ;
			printf("Distance: %d cm\n", distance);
			usleep(10000);
			lidarLite-> closeLidarLite();
			err = lidarLite->openLidarLite();
			
		} 
	    else 
	    {    
		    printf("Distance: %d cm\n", distance);              
	    }
		
	   //facCur = (float)(((((long long)distance_Flt*distance_Flt)/48100)/48100));
	   facCur = ((float)distance_Flt/MAIN_SENSRANGE);
	   //facCur = facCur*facCur;
	   //printf("factor: %f\n",facCur);
	   //vDutyLidar =(int)(MAIN_PERD*facCur) + 30*MAIN_DUTYSTP;
	   if ((distance_Flt > MAIN_SENSRANGE/16)&&(distance>0))
	   {
		   vDutyLidar = MAIN_PERD;
		   vDutyPre = vDuty;
	   }
	   else
	   {
		  vDutyLidar = MAIN_PERD;//0
		  vDuty = vDutyPre;
	   }
	   //printf("Duty Cycle: %d\n",(int)(facCur*100));
	   
	   /* Filter */
	   distance_Flt = (distance_Old1 + distance_Old2 + distance_Old3 + distance)/4;
#endif
	   

	   
	   if (stKeyPrss==105) // i
	   {
		   stManCtrl = MOVE_FWD;
	   }
	   else if (stKeyPrss ==107) //'k' pressed
	   {
		   stManCtrl = MOVE_BACK;
	   }
	   else if (stKeyPrss ==106)   // 'j' pressed
	   {
		   stManCtrl = MOVE_LEFT;
	   }
	   else if (stKeyPrss ==108)  // 'l' pressed
	   {
		    stManCtrl = MOVE_RIGHT;
	   }
	   else
	   {
		   stManCtrl = 0;
	   }

	   
	   if (stKeyPrss==97)//a key pressed
	   {
		   if(vDuty >= 5*MAIN_DUTYSTP) 
		   {
			   vDuty = vDuty -  5*MAIN_DUTYSTP;
			   printf("Duty Cycle: %d\n",vDuty);
		   }
	   }
	   
	   if (stKeyPrss==100) //d key pressed          
	   {
		  if (vDuty <= MAIN_PERD-5*MAIN_DUTYSTP) 
		  { 
			  vDuty = vDuty + 5*MAIN_DUTYSTP;
			  printf("Duty Cycle: %d\n",vDuty);
		  }
	   }

	   
	   if (flgRun == 1)
	   {
		   //PWMSetDuty(PWM0,vDuty);
	   }
	   
	   if (vDuty <= 0)
	   {
		   vDuty = 0;
		   //PWMSetDuty(PWM0,MAIN_PERD);
		   //PWMSetDuty(PWM1,MAIN_PERD);
	   }
	   else if (vDuty >= MAIN_PERD)     
	   {
		   vDuty = MAIN_PERD;
		   //PWMSetDuty(PWM0,0);
		   //PWMSetDuty(PWM1,0);
	   }
	   else
	   {
		   //system("./output_gpio.sh");
		   //PWMSetDuty(PWM0,MAIN_PERD - vDuty);//since there is a BJT invert the logic of ON-OFF PWM
		   //PWMSetDuty(PWM1,MAIN_PERD - vDuty);
	   }

       nRPM = (numEncoder - numEncoder_Old)*10*60/12;
	   nRPM_Old = nRPM;
	   
	   int nRPMFlt;
	   
	   nRPMFlt = nRPM_Old + (nRPM - nRPM_Old)*0.1;
	   
	   numEncoder_Old = numEncoder;
	  
       printf("---------------Rotation Speed: %d\n",nRPMFlt);


		   
	   switch (stManCtrl)
	   {
            case MOVE_FWD:
			   gpioSetValue(Motor1Dir, on);
	                   gpioSetValue(Motor2Dir, on);
			   
			   /* MIN between vDuty and vDutyLidar */
#if LIDAR
			   vDuty = (vDuty<vDutyLidar)?vDuty:vDutyLidar;
#endif
			   
			   PWMSetDuty(PWM0,MAIN_PERD - vDuty);
			   cntPWM = 0;
			   flgRun = 1;
			   //usleep(800000);
			   printf("go forward \n");
			   
		       break;
		    case MOVE_BACK:
			   gpioSetValue(Motor1Dir, off);
	                   gpioSetValue(Motor2Dir, off);
			   PWMSetDuty(PWM0,MAIN_PERD - vDuty);
			   cntPWM = 0;
			   //usleep(800000);
			   printf("go backward \n");
		       break;
			case MOVE_LEFT:
			    gpioSetValue(Motor1Dir, on);
	                    gpioSetValue(Motor2Dir, off);
				PWMSetDuty(PWM0,MAIN_PERD - vDuty);
				cntPWM = 0;
				//usleep(800000);
				 printf("Turn Left \n");
			    break;
			case MOVE_RIGHT:
			    gpioSetValue(Motor1Dir, off);
	                    gpioSetValue(Motor2Dir, on);
				PWMSetDuty(PWM0,MAIN_PERD - vDuty);
				cntPWM = 0;
				//usleep(800000);
				printf("Turn Right \n");
			    break;
		    default:
			    if (cntPWM >5)
				{
				    PWMSetDuty(PWM0,MAIN_PERD);
					cntPWM = 0;
					flgRun = 0;
				}
					
			    
	   }
	   cntPWM++;
	   
	   flgTest ^=0x01;
	   if (flgTest==1)
	   {

	   }
	   else
	   {

	   }
	   distance_Old3 = distance_Old2;
	   distance_Old2 = distance_Old1;
	   distance_Old1 = distance;
	   //stKeyPrss = getkey();
	   usleep(20000);
	}//while
#if LIDAR
	lidarLite-> closeLidarLite();
#endif
	gpioUnexport(PWMMotor);
	gpioUnexport(Motor1Dir);
	gpioUnexport(Motor2Dir);
	gpioUnexport(pushButton); 
	//PWMDisable(PWM0);
	//PWMUnexport(PWM0);
	PWMSetDuty(PWM0,MAIN_PERD);
	PWMSetDuty(PWM1,MAIN_PERD);
	pthread_exit(NULL);
}
