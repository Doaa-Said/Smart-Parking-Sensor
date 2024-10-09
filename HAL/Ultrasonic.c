/******************************************************************************
 *
 * Module: Ultrasonic
 *
 * File Name: Ultrasonic.c
 *
 * Description: Source file for the Ultrasonic driver
 *
 * Author: Doaa Said
 *
 *******************************************************************************/

#include "Ultrasonic.h"        // Include the header file for Ultrasonic functions
#include "../MCAL/GPIO.h"      // Include the header file for GPIO functions
#include "../MCAL/ICU.h"       // Include the header file for ICU functions
#include <util/delay.h>        // Include delay functions

// Configuration structure for ICU
static ICU_ConfigType configuration = { F_CPU_8, RAISING };
static uint8 g_edgeCount = 0;  // Global variable to count edges detected
static uint16 g_timeHigh;      // Global variable to store the time high value

// Function to initialize the ultrasonic sensor
void Ultrasonic_init(void) {
	ICU_init(&configuration); // Initialize the ICU with the specified configuration
	ICU_setCallBack(Ultrasonic_edgeProcessing); // Set the callback function for edge processing

	// Set the direction of the ultrasonic trigger pin to output
	GPIO_setupPinDirection(ULTRASONIC_Trigger_PORT_ID,
	ULTRASONIC_Trigger_PIN_ID, PIN_OUTPUT);
}

// Function to trigger the ultrasonic sensor
void Ultrasonic_Trigger(void) {
	GPIO_writePin(ULTRASONIC_Trigger_PORT_ID, ULTRASONIC_Trigger_PIN_ID,
	LOGIC_HIGH); // Set the trigger pin high

	_delay_us(10); // Wait for 10 microseconds

	GPIO_writePin(ULTRASONIC_Trigger_PORT_ID, ULTRASONIC_Trigger_PIN_ID,
	LOGIC_LOW); // Set the trigger pin low
}

// Function to read the distance from the ultrasonic sensor
uint16 Ultrasonic_readDistance(void) {
	uint16 distance = 0; // Variable to store the calculated distance
	g_timeHigh = 0; // Reset time high
	g_edgeCount = 0; // Reset edge count

	ICU_clearTimerValue(); // Clear the timer value

	Ultrasonic_Trigger(); // Trigger the ultrasonic sensor

	// Wait for two edges to be detected
	while (g_edgeCount != 2)
		;

	// Calculate the distance based on the time high
	distance = (g_timeHigh) * (0.008575);
	distance += 1; // Adjust distance value
	g_edgeCount = 0; // Reset edge count for the next measurement

	return distance; // Return the calculated distance
}

// Callback function to process edges detected by the ICU
void Ultrasonic_edgeProcessing(void) {
	g_edgeCount++; // Increment edge count
	if (g_edgeCount == 1) {
		ICU_clearTimerValue(); // Clear timer for the first edge
		ICU_setEdgeDetectionType(FALLING); // Set edge detection to falling edge
	} else if (g_edgeCount == 2) {
		g_timeHigh = ICU_getInputCaptureValue(); // Get the time high value for the second edge

		ICU_setEdgeDetectionType(RAISING); // Reset edge detection to rising edge
	}
}
