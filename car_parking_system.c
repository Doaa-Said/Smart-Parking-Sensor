/*
 * car_parking_system.c
 *
 *  Created on: Oct 9, 2024
 *
 *   Author:Doaa Said
 */

#include "../HAL/LCD.h"
#include "../HAL/BUZZER.h"
#include "../HAL/LED.h"
#include "../HAL/Ultrasonic.h"
#include "../MCAL/std_types.h"
#include <avr/io.h>            // Include AVR I/O definitions to set I bit in SREG
#include <util/delay.h>       // Include delay functions

uint8 is_stop_dispalayed=0; // Flag to track whether "STOP" is displayed on the LCD

// Function to display the distance on the LCD
void display_distance(uint16 distance ){
	LCD_moveCursor(0,11); // Move cursor to the specified position on the LCD
	if(distance>=100){ // Check if the distance is 100 or more
		LCD_intgerToString(distance); // Display the distance
	}
	else{
		LCD_intgerToString(distance); // Display the distance
		LCD_displayCharacter(' '); // Add a space for alignment
	}

	// Check if the distance is less than or equal to 5 cm and "STOP" is not already displayed
	if(distance<=5 && !is_stop_dispalayed){
		LCD_displayStringRowColumn(1,6,"STOP"); // Display "STOP" on the second row
		is_stop_dispalayed=1; // Set the flag to indicate "STOP" is displayed
	}

	// Check if "STOP" was displayed and distance is now greater than 5 cm
	if(is_stop_dispalayed && distance>5){
		LCD_displayStringRowColumn(1,6,"    "); // Clear the "STOP" message
		is_stop_dispalayed=0; // Reset the flag
	}
}

// Main function
int main(void){
	SREG|=1<<7;       // Enable global interrupts
	LCD_init();        // Initialize the LCD
	Buzzer_init();     // Initialize the Buzzer
	LEDS_init();       // Initialize the LEDs
	Ultrasonic_init(); // Initialize the ultrasonic sensor

	uint16 distance;   // Variable to hold the measured distance
	LCD_displayStringRowColumn(0,1,"Distance =   Cm"); // Display initial message on the LCD

	for(;;){ // Infinite loop
		distance=Ultrasonic_readDistance(); // Read the distance from the ultrasonic sensor

		// Check the measured distance and control LEDs and Buzzer accordingly
		if(distance>20){
			LED_off(RED_ID); // Turn off all LEDs
			LED_off(GREEN_ID);
			LED_off(BLUE_ID);
			Buzzer_off(); // Turn off the buzzer
			display_distance(distance); // Display the distance
		}
		else if((distance>=16) && (distance<=20)){
			LED_on(RED_ID); // Turn on the Red LED
			LED_off(GREEN_ID); // Turn off Green and Blue LEDs
			LED_off(BLUE_ID);
			Buzzer_off(); // Turn off the buzzer
			display_distance(distance); // Display the distance
		}
		else if((distance>=11) && (distance<=15)){
			LED_on(RED_ID); // Turn on Red and Green LEDs
			LED_on(GREEN_ID);
			LED_off(BLUE_ID); // Turn off the Blue LED
			Buzzer_off(); // Turn off the buzzer
			display_distance(distance); // Display the distance
		}
		else if((distance>=6) && (distance<=10)){
			LED_on(RED_ID); // Turn on Red and Green LEDs
			LED_on(GREEN_ID);
			LED_on(BLUE_ID); // Turn on the Blue LED
			Buzzer_off(); // Turn off the buzzer
			display_distance(distance); // Display the distance
		}
		else if(distance<=5){ // Check if distance is less than or equal to 5 cm
			display_distance(distance); // Display the distance

			LED_on(RED_ID); // Turn on all LEDs
			LED_on(GREEN_ID);
			LED_on(BLUE_ID);
			Buzzer_on(); // Activate the buzzer
			_delay_ms(500); // On for 500 ms
			LED_off(RED_ID); // Turn off all LEDs
			LED_off(GREEN_ID);
			LED_off(BLUE_ID);
			Buzzer_off(); // Turn off the buzzer
			_delay_ms(500); // Off for 500 ms
		}
	}
}
