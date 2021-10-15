#include "stdlib.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "uart.h"
#include "LCD.h"
#include "inputUtil.h"

// Time fields indexes
#define INDEX_HOURS   0
#define INDEX_MINUTES 3
#define INDEX_SECONDS 6

// Buffers size
#define BUFFER_SIZE 20
#define CITIES_NUM  10

/* The tasks to be created */
static void initTask(void *pvParameters);
static void timeTask(void *pvParameters);
static void lcdTask(void *pvParameters);
static void uartTask(void *pvParameters);

// Function prototypes
void readTime(char buf[]);

// Message structure
typedef struct
{
	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;
} Time;


// Cities and their time differences from London
static char * cities[] = {"London", "Paris", "Madrid", "Rome", "Athens", "Ankara", "Istanbul", "Cairo", "Moscow", "Tehran"};
static char timediff[] = {0, 1, 1, 1, 2, 2, 2, 2, 3, 4};
static unsigned int selection;

// Queues and semaphores
static xQueueHandle xQueue1;
static xQueueHandle xQueue2;
static xSemaphoreHandle uart_semaphore;


int main( void )
{	
	
	// Create the queues
	xQueue1 = xQueueCreate(1, sizeof(Time));
  xQueue2 = xQueueCreate(1, sizeof(Time));
	vSemaphoreCreateBinary( uart_semaphore )
	
	if( xQueue1 != NULL && xQueue2 != NULL && uart_semaphore != NULL){
		// Create the tasks
		xTaskCreate(initTask, "Init", 200, NULL, 11, NULL);
		xTaskCreate(timeTask, "Time Controller", 250, NULL, 10, NULL);
		xTaskCreate(lcdTask, "LCD Controller", 250, NULL, 10, NULL);
		xTaskCreate(uartTask, "UART Controller", 250, NULL, 10, NULL);
		
		vTaskStartScheduler();
	}
}

static void initTask(void *pvParameters){
	// Initializing ports
	UART0_init();
	LCD_init();
	
	vTaskSuspend(NULL);
}

// Time Controller
static void timeTask(void *pvParameters)
{
	Time time;
	
	xQueueReceive(xQueue2, &time, portMAX_DELAY);	// Receive initial time from Task 3

	for( ;; )
	{
		vTaskDelay(pdMS_TO_TICKS(1000)); // Wait for 1 second
		time.seconds++; 								 // Increment seconds
		
		if(time.seconds == 60) 	// If 60
		{
			time.seconds = 0; 			// Reset to 0
			time.minutes++; 			// Increment minutes
			if(time.minutes == 60) // If 60
			{
				time.minutes = 0; 		// Reset to 0
				time.hours++; 			// Increment hours
				if(time.hours == 24) // If 24
					time.hours = 0; 		// Reset to 0
			}
		}
		
		xQueueSendToBack(xQueue1, &time, 0);	// Send to Task 2
	}
}

// LCD controller
static void lcdTask(void *pvParameters)
{
	char buffer[BUFFER_SIZE];
	Time time;

	selection = 0;
	
	while(1){
		// Waiting for time update
		xQueueReceive(xQueue1, &time, portMAX_DELAY);
		
		LCD_clearScreen();
		
		// Displaying the city
		LCD_writeWord(cities[selection]);
		
		// Applying time difference
		time.hours = (time.hours + timediff[selection]) % 24; 	
		
		// Displaying Time
		sprintf(buffer, "%02d:%02d:%02d", time.hours, time.minutes, time.seconds);
		LCD_secondLine();
		LCD_writeWord(buffer);
	}
}

// UART controller
static void uartTask(void *pvParameters){
	char buffer[BUFFER_SIZE], hours, minutes, seconds, inputSelection;
	Time time;
	
	// Initial prompt to get london time
	UART0_writeText("Time in Different Countries\n\r");
	UART0_writeText("===========================\n\r");
	UART0_writeText("Enter the time in London (hh::mm::ss): \r\n");
	
	// Getting valid london time
	while(1){
		readTime(buffer);
		hours =   getHour(buffer + INDEX_HOURS);
		minutes = getSecMin(buffer + INDEX_MINUTES);
		seconds = getSecMin(buffer + INDEX_SECONDS);
		
		if( hours != (char)-1 && minutes != (char)-1 && seconds != (char)-1)
				break;
					
		UART0_writeText("Invalid input, try again\r\n");
	}
	
	time.hours =   hours;
	time.minutes = minutes;
	time.seconds = seconds;
	
	xQueueSendToBack( xQueue2, &time, 0 );	  // Send to task 1
	xSemaphoreTake( uart_semaphore , 0);		// Empty semaphore
	
	for(;;)
	{
		// Printing the list of countries
		UART0_writeText("\n\r\n\rSelect a City:");
		for(int k = 0; k < CITIES_NUM;k++)
		{
			sprintf(buffer, "\n\r%d. %s", k, cities[k]);
			UART0_writeText(buffer);
		}
		
		UART0_writeText("\n\rSelection: ");
		
		// Getting valid country selection
		while(1){
			xSemaphoreTake( uart_semaphore , portMAX_DELAY);
			
			inputSelection = UART0_read() - '0';
			if(checkRange(inputSelection, 0, CITIES_NUM-1 ) != 0){
				// Valid selection
				selection = inputSelection;
				break;
			}
			
			UART0_writeText("\r\nChoose a number between 0 and 9: ");
		}
	}
}


// This function is used to read the time in London from the user
// Read time from the keyboard. The time is entered as hh:mm:ss
void readTime(char buf[])
{
	unsigned char c, k = 0;
	
	while(1)
	{
		c = UART0_read();				// Read a char
		if (c == '\r') break;		// If Enter
		buf[k] = c;							// Save char
		k++;										// Increment pointer
	}
	buf[k] = '\0';						// NULL terminator
}


void UART0_Handler(void){
		portBASE_TYPE xHighestPriorityTaskWoken = pdFALSE;
	
		// Giving uart task the semaphore to read
		xSemaphoreGiveFromISR(uart_semaphore , &xHighestPriorityTaskWoken);
		
		// Clearing the interrupt
		UART0_clearInt();
		
		portEND_SWITCHING_ISR(xHighestPriorityTaskWoken)

}
