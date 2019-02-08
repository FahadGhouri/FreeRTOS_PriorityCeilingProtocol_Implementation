/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * NOTE: Windows will not be running the FreeRTOS demo threads continuously, so
 * do not expect to get real time behaviour from the FreeRTOS Windows port, or
 * this demo application.  Also, the timing information in the FreeRTOS+Trace
 * logs have no meaningful units.  See the documentation page for the Windows
 * port for further information:
 * http://www.freertos.org/FreeRTOS-Windows-Simulator-Emulator-for-Visual-Studio-and-Eclipse-MingW.html
 *
 ******************************************************************************
 *
 * NOTE:  Console input and output relies on Windows system calls, which can
 * interfere with the execution of the FreeRTOS Windows port.  This demo only
 * uses Windows system call occasionally.  Heavier use of Windows system calls
 * can crash the port.
 */

/* Standard includes. */
#include <stdio.h>
#include <conio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

#define mainNUMBER_OF_SEMAPHORS					( 3 )

#define Task1_Priortiy	5
#define Task2_Priortiy	4
#define Task3_Priortiy	3
#define Task4_Priortiy	2

#define Task_Stack_Size	50

#define workersUSELESS_CYCLES_PER_TIME_UNIT		( 1000000UL)
/*-----------------------------------------------------------*/

/* Type Definations */
typedef struct _CustomStruct_t {
	SemaphoreHandle_t Resource_Semaphore;
	char Resource_Identifier;
	TaskHandle_t* Using_Task;
	uint8_t Ceiling_Priority;
	uint8_t Using_Task_Priorty;
}NewSemaphore_t;
/*-----------------------------------------------------------*/

/* Global Variable */
NewSemaphore_t A,B,C;		//Resources
TaskHandle_t Tasks_Handle[4];
/*-----------------------------------------------------------*/

/* Wrapper Function Prototypes */
BaseType_t usPrioritySemaphoreWait(NewSemaphore_t* Resource, TaskHandle_t* Calling_Task, uint32_t maxWait);
BaseType_t usPrioritySemaphoreSignal(NewSemaphore_t* Resource, TaskHandle_t* Calling_Task);
/*-----------------------------------------------------------*/

static void vUselessLoad(uint32_t ulCycles);
static void prvTask1(void *pvParameters);
static void prvTask2(void *pvParameters);
static void prvTask3(void *pvParameters);
static void prvTask4(void *pvParameters);

/*-----------------------------------------------------------*/

void main_exercise( void )
{
	/* Initialize Data Structures */
	A.Resource_Semaphore = xSemaphoreCreateMutex();
	A.Ceiling_Priority = 4;
	A.Resource_Identifier = 'A';
	B.Resource_Semaphore = xSemaphoreCreateMutex();
	B.Ceiling_Priority = 5;
	B.Resource_Identifier = 'B';
	C.Resource_Semaphore = xSemaphoreCreateMutex();
	C.Ceiling_Priority = 5;
	C.Resource_Identifier = 'C';

	/* Create Tasks */
	xTaskCreate(prvTask1,"Task1", Task_Stack_Size,NULL,Task1_Priortiy,&Tasks_Handle[0]);
	xTaskCreate(prvTask2, "Task2", Task_Stack_Size, NULL, Task2_Priortiy, &Tasks_Handle[1]);
	xTaskCreate(prvTask3, "Task3", Task_Stack_Size, NULL, Task3_Priortiy, &Tasks_Handle[2]);
	xTaskCreate(prvTask4, "Task4", Task_Stack_Size, NULL, Task4_Priortiy, &Tasks_Handle[3]);

	/* Start Schedular */
	vTaskStartScheduler();

	for( ;; );
}
/*-----------------------------------------------------------*/

static void vUselessLoad(uint32_t ulTimeUnits) {
uint32_t ulUselessVariable = 0;

	for (uint32_t i = 0; i < ulTimeUnits*workersUSELESS_CYCLES_PER_TIME_UNIT; i++)
	{
		ulUselessVariable = 0;
		ulUselessVariable = ulUselessVariable + 1;
	}
}

/* The Initial Delay in Release Times is modeled by vTaskDelayUntil() */

static void prvTask1(void *pvParameters)
{
	pvParameters = NULL; //For Complier Warnings
	TickType_t xLastWakeTime;
	/* Run Every 10 Sec */
	const TickType_t xRelease = pdMS_TO_TICKS(10);
	const TickType_t xFrequency = pdMS_TO_TICKS(10000);
	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
	
	for (;;) {
		/* Initial Release Delay */
		vTaskDelayUntil(&xLastWakeTime, xRelease);
		
		/* Task Timing Model */
		vUselessLoad(1);
		usPrioritySemaphoreWait(&B, &Tasks_Handle[0], portMAX_DELAY);
		vUselessLoad(1);
		usPrioritySemaphoreSignal(&B, &Tasks_Handle[0]);
		vUselessLoad(1);
		usPrioritySemaphoreWait(&C, &Tasks_Handle[0], portMAX_DELAY);
		vUselessLoad(1);
		usPrioritySemaphoreSignal(&C, &Tasks_Handle[0]);
		vUselessLoad(1);

		/* Wake Up in Next Period */
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

static void prvTask2(void *pvParameters)
{
	pvParameters = NULL; //For Complier Warnings
	TickType_t xLastWakeTime;
	/* Run Every 10 Sec */
	const TickType_t xRelease = pdMS_TO_TICKS(3);
	const TickType_t xFrequency = pdMS_TO_TICKS(10000);
	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for (;;) {
		/* Initial Release Delay */
		vTaskDelayUntil(&xLastWakeTime, xRelease);

		/* Task Timing Model */
		vUselessLoad(1);
		usPrioritySemaphoreWait(&C, &Tasks_Handle[1], portMAX_DELAY);
		vUselessLoad(2);
		usPrioritySemaphoreSignal(&C, &Tasks_Handle[1]);
		vUselessLoad(2);
		usPrioritySemaphoreWait(&A, &Tasks_Handle[1], portMAX_DELAY);
		vUselessLoad(1);
		usPrioritySemaphoreSignal(&A, &Tasks_Handle[1]);
		vUselessLoad(1);

		/* Wake Up in Next Period */
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

static void prvTask3(void *pvParameters)
{
	pvParameters = NULL; //For Complier Warnings
	TickType_t xLastWakeTime;
	/* Run Every 10 Sec */
	const TickType_t xRelease = pdMS_TO_TICKS(5);
	const TickType_t xFrequency = pdMS_TO_TICKS(10000);
	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for (;;) {
		/* Initial Release Delay */
		vTaskDelayUntil(&xLastWakeTime, xRelease);

		/* Task Timing Model */
		vUselessLoad(2);
		usPrioritySemaphoreWait(&B, &Tasks_Handle[2], portMAX_DELAY);
		vUselessLoad(1);
		usPrioritySemaphoreWait(&A, &Tasks_Handle[2], portMAX_DELAY);
		vUselessLoad(2);
		usPrioritySemaphoreSignal(&A, &Tasks_Handle[2]);
		vUselessLoad(2);
		usPrioritySemaphoreSignal(&B, &Tasks_Handle[2]);
		vUselessLoad(1);

		/* Wake Up in Next Period */
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

static void prvTask4(void *pvParameters)
{
	pvParameters = NULL; //For Complier Warnings
	TickType_t xLastWakeTime;
	/* Run Every 10 Sec */
	const TickType_t xFrequency = pdMS_TO_TICKS(10000);
	// Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();

	for (;;) {

		/* Task Timing Model */
		vUselessLoad(2);
		usPrioritySemaphoreWait(&A, &Tasks_Handle[3], portMAX_DELAY);
		vUselessLoad(2);
		usPrioritySemaphoreWait(&B, &Tasks_Handle[3], portMAX_DELAY);
		vUselessLoad(2);
		usPrioritySemaphoreSignal(&B, &Tasks_Handle[3]);
		vUselessLoad(2);
		usPrioritySemaphoreSignal(&A, &Tasks_Handle[3]);
		vUselessLoad(1);

		/* Wake Up in Next Period */
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

BaseType_t usPrioritySemaphoreWait(NewSemaphore_t* Resource, TaskHandle_t* Calling_Task, uint32_t maxWait) {
	/* Semaphore Availablity Check */
	if (NULL == Resource->Resource_Semaphore)
		return pdFALSE;
	/* Semaphore Take */
	if (pdFALSE == xSemaphoreTake(Resource->Resource_Semaphore, pdMS_TO_TICKS(maxWait)))
		return pdFALSE; //TimeOUT
    /* Change Task Priority to Semaphore Priority Ceiling */
	if (Resource->Ceiling_Priority < (uint8_t)uxTaskPriorityGet(*Calling_Task)) {
		Resource->Using_Task_Priorty = (uint8_t)uxTaskPriorityGet(*Calling_Task);
		/* Print to Console */
		printf("Task <%s> acquired resource <%c> and keeps its priority of <%d> instead of changing to <%d>\n",
			pcTaskGetName(*Calling_Task), Resource->Resource_Identifier, Resource->Using_Task_Priorty,
			Resource->Ceiling_Priority);
		/* Save the Task Handle */
		Resource->Using_Task = Calling_Task;
	}
	else {
		Resource->Using_Task_Priorty = (uint8_t)uxTaskPriorityGet(*Calling_Task);
		vTaskPrioritySet(*Calling_Task, Resource->Ceiling_Priority);
		/* Save the Task Handle */
		Resource->Using_Task = Calling_Task;
		/* Print to console */
		printf("Task <%s> acquired resource <%c> and changed its priority from <%d> to <%d>\n",
			pcTaskGetName(*Calling_Task), Resource->Resource_Identifier, Resource->Using_Task_Priorty,
			Resource->Ceiling_Priority);
	}
	return pdTRUE;
}
/*********************************************************************/

BaseType_t usPrioritySemaphoreSignal(NewSemaphore_t* Resource, TaskHandle_t* Calling_Task) {
	/* Semaphore Availablity Check */
	if (NULL == Resource->Resource_Semaphore)
		return pdFALSE;
	/* Check if some other task is trying to give semaphore other than the task which has taken it */
	if (Resource->Using_Task != Calling_Task)
		return pdFALSE;
	/* Print to console */
	printf("Task <%s> released resource <%c> and changed its priority from <%d> to <%d>\n",
		pcTaskGetName(*Calling_Task), Resource->Resource_Identifier, (uint8_t)uxTaskPriorityGet(*Calling_Task),
		Resource->Using_Task_Priorty);
	/* Semaphore Give and Task Priority is changed in FreeRTOS Critical Section to prevent context switch in between */
	taskENTER_CRITICAL();
	/* Semaphore Give */
	xSemaphoreGive(Resource->Resource_Semaphore);
	/* Change Task Priority to default */
	vTaskPrioritySet(*Calling_Task, Resource->Using_Task_Priorty);
	taskEXIT_CRITICAL();

	return pdTRUE;
}
/*********************************************************************/