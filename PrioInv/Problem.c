/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

//This code presents a priority inversion blocker

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
	Anita Tino
*----------------------------------------------------------------------------*/

void PA (void  *argument);
void PB (void  *argument);
void PC (void  *argument);

//osThreadDef(PA, osPriorityHigh, 1, 0);
//osThreadDef(PB, osPriorityNormal, 1, 0);
//osThreadDef(PC, osPriorityBelowNormal, 1, 0);

osThreadId_t t_main,t_PA,t_PB,t_PC;

void delay(){ 
	long k, count = 0;
	for(k = 0; k < 100000; k++){
					count++;
			}
	}

__NO_RETURN void PA (void *argument) {
	for (;;) 
	{
		LED_On(0);
		printf("Thread A Exec\n");
		delay(); //execute something, and after requires service from PC
		//osThreadSetPriority(t_PC, osPriorityHigh); //**solution uncomment
	
		osThreadFlagsSet(t_PC, 0x01);
		osThreadFlagsWait(0x02, osFlagsWaitAny,osWaitForever);
		printf("Thread A Exec2\n");
		//osSignalSet(t_PC,0x01);						//call PC to finish the task
		//osSignalWait(0x02,osWaitForever);			//Error => priority inversion, PB will run instead
		
		//osThreadSetPriority(t_PC,osPriorityBelowNormal); //**solution uncomment
		LED_On(6);
		LED_Off(6);
	}
}

__NO_RETURN void PB (void *argument) {
	for (;;) 	{
		printf("Thread B Exec\n");
		LED_On(1); 
		LED_Off(1); 
	}
}

__NO_RETURN void PC (void *argument) {
	for (;;) 	{
		delay(); //do something
		printf("Thread C Exec\n");
		osThreadFlagsWait(0x01, osFlagsWaitAny,osWaitForever);
		printf("Thread C Exec2\n");
		//osSignalWait(0x01,osWaitForever);  
		LED_Off(0); //critical function to be requested by PA
		osThreadFlagsSet(t_PA, 0x02);
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
	
	osThreadTerminate(t_main);
	osKernelStart ();    	// Start thread execution
  for (;;) {}
}
