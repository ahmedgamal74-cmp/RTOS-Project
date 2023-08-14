/*
*********************************************************
* @file: Project Program
* @author: Ahmed Gamal Ismail Noureddine
* @brief: A program consists of 4 tasks communicating via queue,
		  3 Senders and a Receiver with different Priorities
*********************************************************
 */

/* !! ((( OUR OUTPUTS ARE INSERTED AS A COMMENT BELOW ))) !! */

/* ************ Includes start ************************** */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "diag/trace.h"
#include "math.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

#define CCM_RAM __attribute__((section(".ccmram")))

/* ************ Includes end **************************** */



/* ************ Macro start ***************************** */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

#define QUEUE_LENGTH          3 // 10 in the second requirement
#define QUEUE_SIZE            (50*sizeof(char))

/* ************ Macro end ******************************* */


/* ************ Declerations start ********************** */

QueueHandle_t Queue;

TaskHandle_t Sender_1,
 	 	 	 Sender_2,
			 Sender_3,
			 Receiver;

TimerHandle_t Sender_1_Timer,
			  Sender_2_Timer,
			  Sender_3_Timer,
			  Receiver_Timer;

SemaphoreHandle_t Sender_1_Semaphore,
				  Sender_2_Semaphore,
				  Sender_3_Semaphore,
				  Receiver_Semaphore;

BaseType_t isTimer1_Started,
		   isTimer2_Started,
		   isTimer3_Started,
		   isTimer4_Started;



/* ************ Declerations end ************************ */



/* ************ Global Variables start ****************** */

int number_of_received_message = 0;

int iteration_number = 0;

// for the 3 senders
int blocked_messages[3] = {0, 0, 0};
int transmitted_messages[3] = {0, 0, 0};

/* ************ Global Variables end ****************** */



/* ************ Sub Programs start ********************** */

void Timers_Destroy(){
	xTimerDelete(Sender_1_Timer, 0);
	xTimerDelete(Sender_2_Timer, 0);
	xTimerDelete(Sender_3_Timer, 0);
	xTimerDelete(Receiver_Timer, 0);
}

void Reset_Function(){

	printf("This is iteration %i \n", 1+iteration_number);

	// 1 (printing total)
	int total_blocked =  blocked_messages[0] + blocked_messages[1] + blocked_messages[2];
	int total_transmitted =  transmitted_messages[0] + transmitted_messages[1] + transmitted_messages[2];
	printf("Total number of successfully sent messages = %i \n", total_transmitted);
	printf("Total number of blocked messages = %i \n", total_blocked);
	// 2 (statistics)
	for (int counter = 0; counter < 3; counter++) {
		// int total_blocked_transmitted = *(transmitted + counter) + *(blocked + counter);
		// int blocked_percentage = *(blocked + counter) / total_blocked_transmitted * 100;
		// int transmitted_percentage = *(transmitted + counter) / total_blocked_transmitted * 100;
		printf("Sender Task %i :\n", (1 + counter));
		printf("Number of successfully sent messages = %i \n", transmitted_messages[counter]);
		printf("Number of blocked messages = %i \n", blocked_messages[counter]);
	}

	// checking number of iterations

	iteration_number++;

	if(6 == iteration_number){
		printf("Game Over \n");
		// destroy timers then stop execution
		Timers_Destroy();
		vTaskEndScheduler();
		exit(0); // stop execution
		return ;
	}

	// 3 (setting values)
	number_of_received_message = 0;
	for (int counter = 0; counter < 3; counter++) {
		blocked_messages[counter] = 0;
	}
	for (int counter = 0; counter < 3; counter++) {
		transmitted_messages[counter] = 0;
	}
	// 4 (clearing queue)
	xQueueReset(Queue);



}

TickType_t Random_Period_Generator(){
	TickType_t Tsender;
	// srand(time(0));
	int min = (50 + 30*iteration_number);
	int diff = (100 + 20*iteration_number + 1);
	int random_value = rand()%diff + min;
	Tsender =  pdMS_TO_TICKS ( random_value );
	return Tsender;
}

void Sender_1_timerCallback(TimerHandle_t xTimer){
	xSemaphoreGive(Sender_1_Semaphore);
}

void Sender_2_timerCallback(TimerHandle_t xTimer){
	xSemaphoreGive(Sender_2_Semaphore);
}

void Sender_3_timerCallback(TimerHandle_t xTimer){
	xSemaphoreGive(Sender_3_Semaphore);
}

void Receiver_timerCallback(TimerHandle_t xTimer){
	xSemaphoreGive(Receiver_Semaphore);
}

void Sender_1_function(void *ptr){

	unsigned long int current_time_in_ticks;
	char sent_message[50];

	while(1){

		xSemaphoreTake(Sender_1_Semaphore, portMAX_DELAY);

		current_time_in_ticks = (unsigned long int)xTaskGetTickCount();

		sprintf(sent_message, "Time is %lu", current_time_in_ticks);

		BaseType_t isSent = xQueueSend(Queue, &sent_message, 0);

		if (isSent == pdPASS){
			transmitted_messages[0]++;
		}
		else{
			blocked_messages[0]++;
		}

		TickType_t random_period = Random_Period_Generator();

		xTimerChangePeriod(Sender_1_Timer, random_period, 0);

	}
}

void Sender_2_function(void *ptr){


	unsigned long int current_time_in_ticks;
	char sent_message[50];

	while(1){

		xSemaphoreTake(Sender_2_Semaphore, portMAX_DELAY);

		current_time_in_ticks = (unsigned long int)xTaskGetTickCount();

		sprintf(sent_message, "Time is %lu", current_time_in_ticks);

		BaseType_t isSent = xQueueSend(Queue, &sent_message, 0);

		if (isSent == pdPASS){
			transmitted_messages[1]++;
		}
		else{
			blocked_messages[1]++;
		}

		TickType_t random_period = Random_Period_Generator();

		xTimerChangePeriod(Sender_2_Timer, random_period, 0);

	}
}

void Sender_3_function(void *ptr){

	unsigned long int current_time_in_ticks;
	char sent_message[50];

	while(1){

		xSemaphoreTake(Sender_3_Semaphore, portMAX_DELAY);

		current_time_in_ticks = (unsigned long int)xTaskGetTickCount();

		sprintf(sent_message, "Time is %lu", current_time_in_ticks);


		BaseType_t isSent = xQueueSend(Queue, &sent_message, 0);

		if (isSent == pdPASS){
			transmitted_messages[2]++;
		}
		else{
			blocked_messages[2]++;
		}


		TickType_t random_period = Random_Period_Generator();

		xTimerChangePeriod(Sender_3_Timer, random_period, 0);

	}
}

void Receiverfunction(void *ptr){

	char received_message[50];

	while(1){

		xSemaphoreTake(Receiver_Semaphore, portMAX_DELAY);

		if (uxQueueMessagesWaiting(Queue) == 0){
			// do nothing -- there is no messages to be received
		}
		else{
			xQueueReceive(Queue, received_message, 0);
			printf("%s \n", received_message);
			number_of_received_message++;
		}
		if(1000 == number_of_received_message){
			Reset_Function();
		}

	}
}

/* ************ Sub Programs end ************************ */



/* ************ Main start ****************************** */

int
main(int argc, char* argv[])
{

	srand(time(0));

	// creation of a queue
	Queue = xQueueCreate(QUEUE_LENGTH, QUEUE_SIZE);

	if(NULL == Queue){
		trace_puts("Queue is not created! \n");
		// exit(0);
	}


	// creation of 3 senders and 1 receiver
	xTaskCreate(Sender_1_function, "Sender_1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(Sender_2_function, "Sender_2", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(Sender_3_function, "Sender_3", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(Receiverfunction, "Receiver",  configMINIMAL_STACK_SIZE, NULL, 3, NULL);


	// creation of 4 semphs
	Sender_1_Semaphore = xSemaphoreCreateBinary();
	Sender_2_Semaphore = xSemaphoreCreateBinary();
	Sender_3_Semaphore = xSemaphoreCreateBinary();
	Receiver_Semaphore = xSemaphoreCreateBinary();

	// creation of 4 timers

	Sender_1_Timer = xTimerCreate("Sender_Timer_1", pdMS_TO_TICKS(100), pdTRUE, NULL, Sender_1_timerCallback);


	Sender_2_Timer = xTimerCreate("Sender_Timer_2", pdMS_TO_TICKS(100), pdTRUE, NULL, Sender_2_timerCallback);


	Sender_3_Timer = xTimerCreate("Sender_Timer_3", pdMS_TO_TICKS(100), pdTRUE, NULL, Sender_3_timerCallback);


	Receiver_Timer = xTimerCreate("Receiver_Timer", pdMS_TO_TICKS(100), pdTRUE, NULL, Receiver_timerCallback);



	// changing the 3 senders periods with random periods

	TickType_t random_period_1 = Random_Period_Generator();

	xTimerChangePeriod(Sender_1_Timer, random_period_1, 0);

	TickType_t random_period_2 = Random_Period_Generator();

	xTimerChangePeriod(Sender_2_Timer, random_period_2, 0);

	TickType_t random_period_3 = Random_Period_Generator();

	xTimerChangePeriod(Sender_3_Timer, random_period_3, 0);



	// vTaskStartScheduler();

	if((Sender_1_Timer != NULL) && (Sender_2_Timer != NULL) && (Sender_3_Timer != NULL) && (Receiver_Timer != NULL)){
		isTimer1_Started = xTimerStart(Sender_1_Timer, 0);
		isTimer2_Started = xTimerStart(Sender_2_Timer, 0);
		isTimer3_Started = xTimerStart(Sender_3_Timer, 0);
		isTimer4_Started = xTimerStart(Receiver_Timer, 0);
		// trace_puts("timers are ok not null");
	}
	else{
		trace_puts("timers are null");
	}

	// check if timers started successfully
	if((isTimer1_Started == pdPASS) && (isTimer2_Started == pdPASS) && (isTimer3_Started == pdPASS) && (isTimer4_Started == pdPASS)){
		// trace_puts("sched. is started");
		vTaskStartScheduler();
	}
	else
	{
		trace_puts("Creation of queue has failed");
	}

	// trace_puts("ALL is OK");
	return 0;
}


/* ************ Main end ******************************** */








/* ************ Demo main file functions ******************************** */


#pragma GCC diagnostic pop



void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amout of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

void vApplicationTickHook(void) {
}

StaticTask_t xIdleTaskTCB CCM_RAM;
StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE] CCM_RAM;

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
  state will be stored. */
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

  /* Pass out the array that will be used as the Idle task's stack. */
  *ppxIdleTaskStackBuffer = uxIdleTaskStack;

  /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
  Note that, as the array is necessarily of type StackType_t,
  configMINIMAL_STACK_SIZE is specified in words, not bytes. */
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCB CCM_RAM;
static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH] CCM_RAM;

/* configUSE_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize) {
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
  *ppxTimerTaskStackBuffer = uxTimerTaskStack;
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}



/*

At Queue Size = 3
-------------------------------

This is iteration 1
Total number of successfully sent messages = 1002
Total number of blocked messages = 2006
Sender Task 1 :
Number of successfully sent messages = 334
Number of blocked messages = 663
Sender Task 2 :
Number of successfully sent messages = 355
Number of blocked messages = 653
Sender Task 3 :
Number of successfully sent messages = 313
Number of blocked messages = 690

This is iteration 2
Total number of successfully sent messages = 1002
Total number of blocked messages = 1142
Sender Task 1 :
Number of successfully sent messages = 347
Number of blocked messages = 369
Sender Task 2 :
Number of successfully sent messages = 349
Number of blocked messages = 366
Sender Task 3 :
Number of successfully sent messages = 306
Number of blocked messages = 407

This is iteration 3
Total number of successfully sent messages = 1002
Total number of blocked messages = 678
Sender Task 1 :
Number of successfully sent messages = 331
Number of blocked messages = 224
Sender Task 2 :
Number of successfully sent messages = 332
Number of blocked messages = 232
Sender Task 3 :
Number of successfully sent messages = 339
Number of blocked messages = 222

This is iteration 4
Total number of successfully sent messages = 1002
Total number of blocked messages = 353
Sender Task 1 :
Number of successfully sent messages = 333
Number of blocked messages = 119
Sender Task 2 :
Number of successfully sent messages = 337
Number of blocked messages = 114
Sender Task 3 :
Number of successfully sent messages = 332
Number of blocked messages = 120

This is iteration 5
Total number of successfully sent messages = 1002
Total number of blocked messages = 153
Sender Task 1 :
Number of successfully sent messages = 331
Number of blocked messages = 54
Sender Task 2 :
Number of successfully sent messages = 323
Number of blocked messages = 57
Sender Task 3 :
Number of successfully sent messages = 348
Number of blocked messages = 42

This is iteration 6
Total number of successfully sent messages = 1002
Total number of blocked messages = 14
Sender Task 1 :
Number of successfully sent messages = 333
Number of blocked messages = 5
Sender Task 2 :
Number of successfully sent messages = 330
Number of blocked messages = 5
Sender Task 3 :
Number of successfully sent messages = 339
Number of blocked messages = 4
Game Over

 */

/*

 At Queue Size = 10
-------------------------------

This is iteration 1
Total number of successfully sent messages = 1009
Total number of blocked messages = 1988
Sender Task 1 :
Number of successfully sent messages = 322
Number of blocked messages = 678
Sender Task 2 :
Number of successfully sent messages = 358
Number of blocked messages = 631
Sender Task 3 :
Number of successfully sent messages = 329
Number of blocked messages = 679

This is iteration 2
Total number of successfully sent messages = 1009
Total number of blocked messages = 1123
Sender Task 1 :
Number of successfully sent messages = 338
Number of blocked messages = 372
Sender Task 2 :
Number of successfully sent messages = 338
Number of blocked messages = 381
Sender Task 3 :
Number of successfully sent messages = 333
Number of blocked messages = 370

This is iteration 3
Total number of successfully sent messages = 1009
Total number of blocked messages = 649
Sender Task 1 :
Number of successfully sent messages = 349
Number of blocked messages = 204
Sender Task 2 :
Number of successfully sent messages = 325
Number of blocked messages = 225
Sender Task 3 :
Number of successfully sent messages = 335
Number of blocked messages = 220

This is iteration 4
Total number of successfully sent messages = 1009
Total number of blocked messages = 357
Sender Task 1 :
Number of successfully sent messages = 335
Number of blocked messages = 118
Sender Task 2 :
Number of successfully sent messages = 335
Number of blocked messages = 128
Sender Task 3 :
Number of successfully sent messages = 339
Number of blocked messages = 111

This is iteration 5
Total number of successfully sent messages = 1009
Total number of blocked messages = 150
Sender Task 1 :
Number of successfully sent messages = 335
Number of blocked messages = 53
Sender Task 2 :
Number of successfully sent messages = 336
Number of blocked messages = 52
Sender Task 3 :
Number of successfully sent messages = 338
Number of blocked messages = 45

This is iteration 6
Total number of successfully sent messages = 1004
Total number of blocked messages = 2
Sender Task 1 :
Number of successfully sent messages = 331
Number of blocked messages = 2
Sender Task 2 :
Number of successfully sent messages = 335
Number of blocked messages = 0
Sender Task 3 :
Number of successfully sent messages = 338
Number of blocked messages = 0
Game Over

 */












