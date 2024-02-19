/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

//This code solves the priority inversion usign a semaphore

#include "RTE_Components.h"
#include  CMSIS_device_header
#include "cmsis_os2.h"
#define osObjectsPublic                     // define objects in main module
//#include "osObjects.h"                      // RTOS object definitions
//#include "cmsis_os.h"                                           // CMSIS RTOS header file
#include <stdio.h>
#include <math.h>
#include "Board_LED.h"                  // ::Board Support:LED
#include "RTE_Components.h"             // Component selection

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

osMutexId_t mutex_id;  
osSemaphoreId_t sid_Semaphore; 
 
//const osMutexAttr_t Thread_Mutex_attr = {
//  "myThreadMutex",     // human readable mutex name
//  osMutexPrioInherit,  // attr_bits
//  NULL,                // memory for control block   
//  0U                   // size for control block
//};

 const osSemaphoreAttr_t Thread_Semaphore_attr = {
  "myThreadSemaphore",     // human readable mutex name
  osMutexPrioInherit,  // attr_bits
  NULL,                // memory for control block   
  0U                   // size for control block
};

/*----------------------------------------------------------------------------
 * Application main thread
 *---------------------------------------------------------------------------*/
__NO_RETURN static void app_main (void *argument) {
  (void)argument;
  // ...
  for (;;) {}
}
 
const osThreadAttr_t thread1_attr = {
  .stack_size = 1024,
	.priority = osPriorityHigh //osThreadSetPriority(osPriorityHigh)
	// Create the thread stack with a size of 1024 bytes
};

const osThreadAttr_t thread2_attr = {
  .stack_size = 1024,
	.priority = osPriorityNormal //osThreadSetPriority(osPriorityHigh)
	// Create the thread stack with a size of 1024 bytes
};


const osThreadAttr_t thread3_attr = {
  .stack_size = 1024,
	.priority = osPriorityLow //osThreadSetPriority(osPriorityHigh)
	// Create the thread stack with a size of 1024 bytes
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/



  
/*----------------------------------------------------------------------------
	CMSIS RTX Priority Inversion Example
	Priority Inversion = leave commented lines commented
	Priority Elevation = uncomment the 2 commented lines
*----------------------------------------------------------------------------*/

void PA (void  *argument);
void PB (void  *argument);
void PC (void  *argument);

//osThreadDef(PA, osPriorityHigh, 1, 0);
//osThreadDef(PB, osPriorityNormal, 1, 0);
//osThreadDef(PC, osPriorityBelowNormal, 1, 0);

osThreadId_t t_main,t_PA,t_PB,t_PC;

int i;

void delay(){ 
	long k, count = 0;
	for(k = 0; k < 1000; k++){
					count++;
			}
	}

void PA (void *argument) {
	osDelay(25U);
	for (i = 0; i < 10000; i++) 
	{
		LED_On(0);
		printf("Thread A Exec\n");
		 //execute something, and after requires service from PC
		//osThreadSetPriority(t_PC, osPriorityHigh); //**solution uncomment
	  osSemaphoreAcquire(sid_Semaphore, osWaitForever); // try to acquire mutex
		//osThreadFlagsSet(t_PC, 0x01);
		delay();
		//osThreadFlagsWait(0x02, osFlagsWaitAny,osWaitForever);
		printf("Thread A Exec2\n");
		osSemaphoreRelease(sid_Semaphore);
		//osSignalSet(t_PC,0x01);						//call PC to finish the task
		//osSignalWait(0x02,osWaitForever);			//Error => priority inversion, PB will run instead
		
		//osThreadSetPriority(t_PC,osPriorityBelowNormal); //**solution uncomment
		LED_On(6);
		//osDelay(50U);
		LED_Off(6);
		osDelay(10U);
	}
}

void PB (void *argument) {
	osDelay(20U);
	for (;;) 	{
		printf("Thread B Exec\n");
		LED_On(1); 
		LED_Off(1);
	osDelay(10U);		
	}
}

void PC (void *argument) {
	for (i = 0; i < 10000; i++) 	{
		//delay(); //do something
		printf("Thread C Exec\n");
		osSemaphoreAcquire(sid_Semaphore, osWaitForever); // try to acquire mutex
		osDelay(15U);
	//	osThreadFlagsWait(0x01, osFlagsWaitAny,osWaitForever);
		printf("Thread C Exec2\n");
		//osSignalWait(0x01,osWaitForever);  
		LED_Off(0); //critical function to be requested by PA
		osSemaphoreRelease(sid_Semaphore);
		//osDelay(15U);
		//osThreadFlagsSet(t_PA, 0x02);
	//	osSignalSet(t_PA,0x02);	 
	}
}

int main (void) {
 
  // System Initialization
  SystemCoreClockUpdate();
  // ...
  osKernelInitialize();           
	LED_Initialize();	// Initialize CMSIS-RTOS
	t_main= osThreadNew(app_main, NULL, NULL);    // Create application main thread

	
	//app_main = osThreadGetId ();
	osThreadSetPriority(t_main,osPriorityHigh);
	//t_PC = osThreadCreate(osThread(PC), NULL);
	t_PC=osThreadNew(PC, NULL, &thread3_attr);
	osDelay(5);
	//t_PB = osThreadCreate(osThread(PB), NULL);
	t_PB=osThreadNew(PB, NULL, &thread2_attr);
	osDelay(5);
	//t_PA = osThreadCreate(osThread(PA), NULL);
	t_PA=osThreadNew(PA, NULL, &thread1_attr);
	//mutex_id = osMutexNew(&Thread_Mutex_attr);
	sid_Semaphore = osSemaphoreNew(1, 1, &Thread_Semaphore_attr);
	osThreadTerminate(t_main);
	osKernelStart ();    	// Start thread execution
  for (;;) {}
}
