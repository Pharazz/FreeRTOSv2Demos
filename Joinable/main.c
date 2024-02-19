/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/
 
#define osObjectsPublic    

//#include "osObjects.h"   
#include "RTE_Components.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Board_LED.h"
#include <string.h>
#include "Board_Joystick.h"
#include "RTE_Components.h" 

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;



int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}

// extern int Init_Thread (void);
/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
osThreadId_t thread1;                        // thread id
osThreadId_t thread2;                        // thread id
osThreadId_t thread3;                        // thread id
osThreadId_t mainThread;                        // thread id

osMutexId_t mutex1;  //mutex id

int i, c;
int a = 10;
int b = 15;
int Diff = 0;
int Sum = 0;
int Pyth = 0;


static const osThreadAttr_t threadAtt1 = {
	
	.priority=osPriorityHigh,
	.attr_bits=osThreadJoinable,
};
 
static const osThreadAttr_t threadAtt2 = {
	.priority=osPriorityNormal,
	.attr_bits=osThreadJoinable,
};

static const osThreadAttr_t threadAtt3 = {
	.priority=osPriorityBelowNormal,
	.attr_bits=osThreadJoinable,
};

static const osMutexAttr_t mutexAttr1 = {
  .attr_bits=osMutexPrioInherit,  // attr_bits
};

void delay(){ 
	long k, count = 0;
	for(k = 0; k < 100000; k++){
					count++;
		}
}

void createMutex (void)  {
  mutex1 = osMutexNew(&mutexAttr1);
	//mutex1 = osMutexNew(NULL);
}

void highThread1 (void *argument);                   // thread function
void normalThread2 (void *argument);                  // thread function
void lowThread3 (void *argument);                   // thread function
void mainT (void *argument);

void highThread1 (void *argument) {
	while (1) {
		printf("Thread 1 running\n");
		//osMutexAcquire(mutex1, osWaitForever);
		osThreadJoin(thread2);
		osThreadJoin(thread3);
		printf("Sum of Cubes: %d\n", Sum);
		printf("Difference of Cubes: %d\n", Diff);
		
		if(Sum%2 > Diff%2){
			LED_On(1);
			LED_Off(0);
			LED_Off(6);
		}
		else if(Sum%2 < Diff%2){
			LED_On(0);
			LED_Off(1);
			LED_Off(6);
		}
		else{
			LED_On(6);
			LED_Off(1);
			LED_Off(0);
		}
		a+= 10;
		b+= 10;
		//avgWordLen = wordLength/550;
		//osDelay(30U);
		a= (rand() % 400)+100;
		b= (rand() % 400)+100;
		printf("a = %d, b = %d\n", a, b);
		thread2 = osThreadNew(normalThread2, NULL, &threadAtt2);
		thread3 = osThreadNew(lowThread3, NULL, &threadAtt2);
		osKernelStart();
		//osMutexRelease(mutex1);
	}
}

void normalThread2 (void *argument) {
	osDelay(10U);
	while (1) {
		printf("Thread 2 running\n");
		for (i=0; i<550; i++){
//			if (strlen(ch_arr[i]) > largestLen) {
//				largestLen = strlen(ch_arr[i]);
			Diff = (a-b)*(a*a + - 2*a*b + b*b);
			a = Diff;
		}
		osThreadExit();
	}
}

void lowThread3 (void *argument) {
	osDelay(10U);
	while (1) {
		printf("Thread 3 running\n");
		//osMutexAcquire(mutex1, osWaitForever);
		for (i=0; i<550; i++){
			Sum = (a+b)*(a*a + 2*a*b + b*b);
			//wordLength += strlen(ch_arr[i]);
		}
		osThreadExit();
	}
}


int main (void) {
 
  // System Initialization
  //SystemCoreClockUpdate();
  // ...
 
  osKernelInitialize();                 // Initialize CMSIS-RTOS
	//createMutex();
	srand(39);
  //osThreadNew(app_main, NULL, NULL);    // Create application main thread
	//mainThread = osThreadNew(mainT, NULL, NULL);
                       // Start thread execution
  for (;;) {
							thread1 = osThreadNew(highThread1, NULL, &threadAtt1);
							thread2 = osThreadNew(normalThread2, NULL, &threadAtt2);
							thread3 = osThreadNew(lowThread3, NULL, &threadAtt2);
		
							a= (rand() % 400)+100;
							b= (rand() % 400)+100;
							printf("a = %d, b = %d\n", a, b);
							osKernelStart();
						//printf("Joystick Center\n");


						//printf("Joystick Up\n");
						//switchCase = 1;



//						printf("Pythagorean Result: %d\n", Pyth);

	}
}
