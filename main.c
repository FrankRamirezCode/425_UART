/*
 * @file main.c
 *
 * @brief Main source code for the UART program.
 *
 * This file contains the main entry point and function definitions for the UART program.
 *
 * @note For more information regarding the UART module, refer to the
 * Universal Asynchronous Receivers / Transmitters (UARTs) section
 * of the TM4C123GH6PM Microcontroller Datasheet.
 *   - Link: https://www.ti.com/lit/gpn/TM4C123GH6PM
 *
 * @author Frank Ramirez
 */

#include "TM4C123GH6PM.h"
#include "SysTick_Delay.h"
#include "UART0.h"
#include "UART1.h"
#include "GPIO.h"
#include "string.h"


int main(void)
{
	SysTick_Delay_Init();
	UART0_Init();
	UART1_Init();
	RGB_LED_Init();
	
	int buffer = 32;
	
	char input[32];
	
	while(1)
	{
		/*
		UART0_Output_String("Hello World");
		UART0_Output_Newline();
		SysTick_Delay1ms(500);
		*/
		
		/*
		UART0_Output_String("Enter Decimal Number: ");
		uint32_t input_decimal = UART0_Input_Unsigned_Decimal();
		UART0_Output_Newline();
		UART0_Output_String("Output Decimal: ");
		UART0_Output_Unsigned_Decimal(input_decimal);
		UART0_Output_Newline();
		SysTick_Delay1ms(500);
		*/
		
		/*
		UART0_Output_String("Enter Command: ");
		UART0_Input_String(input,buffer);
		UART0_Output_Newline();
		
		if(strcmp(input,"rgb led red") == 0 )
		{
			RGB_LED_Output(RGB_LED_RED);
		}
		else if(strcmp(input, "rgb led green") == 0)
		{
			RGB_LED_Output(RGB_LED_GREEN);
		}
		else if(strcmp(input,"rgb led blue") == 0)
		{
			RGB_LED_Output(RGB_LED_BLUE);
		}
		else if(strcmp(input,"rgb led cycle") == 0)
		{
			while(1)
			{
				RGB_LED_Output(RGB_LED_RED);
				SysTick_Delay1ms(500);
				RGB_LED_Output(RGB_LED_GREEN);
				SysTick_Delay1ms(500);
				RGB_LED_Output(RGB_LED_BLUE);
				SysTick_Delay1ms(500);
			}
		}
		else
		UART0_Output_String("Invalid Command. Try again");	
		*/	
		
		
		UART1_Output_Character(0x55);
		SysTick_Delay1ms(500);
		
//		UART1_Output_String("Hello World");
//		UART1_Output_Newline();
//		SysTick_Delay1ms(500);
	}
}
