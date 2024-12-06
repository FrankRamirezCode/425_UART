#include "UART1.h"

void UART1_Init(void)
{
	// Enable the clock to UART1 by setting the 
	// R1 bit (Bit 1) in the RCGCUART register
	SYSCTL->RCGCUART |= 0x02;
	
	// Enable the clock to Port B by setting the
	// R2 bit (Bit 1) in the RCGCGPIO register
	SYSCTL->RCGCGPIO |= 0x02;
	
	// Configure the PB1 (U1TX) and PB0 (U1RX) pins to use the alternate function
	// by setting Bits 0 to 1 in the AFSEL register
	GPIOB->AFSEL |= 0x03;
	
	// Clear the PMC7 (Bits 31 to 28) and PMC6 (Bits 27 to 24) fields in the PCTL register before configuration
	GPIOB->PCTL &= ~0xF0;
	
	// Configure the PC7 pin to operate as a U3TX pin by writing 0x1 to the
	// PMC7 field (Bits 31 to 28) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOB->PCTL |= 0x00000001;
	
	// Configure the PC6 pin to operate as a U3RX pin by writing 0x1 to the
	// PMC6 field (Bits 27 to 24) in the PCTL register
	// The 0x1 value is derived from Table 23-5 in the TM4C123G Microcontroller Datasheet
	GPIOB->PCTL |= 0x000010;
	
	// Enable the digital functionality for the PB0 and PB1 pins
	// by setting Bits 0 to 1 in the DEN register
	GPIOB->DEN |= 0x03;
	
	// Disable the UART1 module before configuration by clearing
	// the UARTEN bit (Bit 0) in the CTL register
	UART1->CTL &= ~0x0001;
	
	// Specify the UART clock source to use the system clock by
	// writing a value of 0x0 to the CS field (Bits 3 to 0) in the CC register
	//UART1->CC &= ~0xF;
	
	// Configure the UART1 module to use the system clock (50 MHz)
	// divided by 16 by clearing the HSE bit (Bit 5) in the CTL register
	UART1->CTL &= ~0x0020;
	
	// Set the baud rate by writing to the DIVINT field (Bits 15 to 0)
	// and the DIVFRAC field (Bits 5 to 0) in the IBRD and FBRD registers, respectively.
	// The integer part of the calculated constant will be written to the IBRD register,
	// while the fractional part will be written to the FBRD register.
	// BRD = (System Clock Frequency) / (16 * Baud Rate)
	// BRDI = (50,000,000) / (16 * 9600) = 325.5208333 (IBRD = 325)
	// BRDF = ((0.5208333 * 64) + 0.5) = 33.8333 (FBRD = 33)
	UART1->IBRD = 325;
	UART1->FBRD = 33;
	
	// Configure the data word length of the UART packet to be 8 bits by 
	// writing a value of 0x3 to the WLEN field (Bits 6 to 5) in the LCRH register
	UART1->LCRH |= 0x60;
	
	// Enable the transmit and receive FIFOs by setting the FEN bit (Bit 4) in the LCRH register
	UART1->LCRH |= 0x10;
	
	// Select one stop bit to be transmitted at the end of a UART frame by
	// clearing the STP2 bit (Bit 3) in the LCRH register
	UART1->LCRH &= ~0x08;
	
	// Disable the parity bit by clearing the PEN bit (Bit 1) in the LCRH register
	UART1->LCRH &= ~0x02;
	
	// Enable the UART1 module after configuration by setting
	// the UARTEN bit (Bit 0) in the CTL register
	UART1->CTL |= 0x01;
}

char UART1_Input_Character(void)
{
	while((UART1->FR & UART1_RECEIVE_FIFO_EMPTY_BIT_MASK) != 0);
	
	return (char)(UART1->DR & 0xFF);
}

void UART1_Output_Character(char data)
{
	while((UART1->FR & UART1_TRANSMIT_FIFO_FULL_BIT_MASK) != 0);
	
	UART1->DR = data;
}

void UART1_Input_String(char *buffer_pointer, uint16_t buffer_size) 
{
	int length = 0;
	char character = UART1_Input_Character();
	
	while(character != UART1_CR)
	{
		if (character == UART1_BS)
		{
			if (length)
			{
				buffer_pointer--;
				length--;
				UART1_Output_Character(UART1_BS);
			}
		}
		else if(length < buffer_size)
		{
			*buffer_pointer = character;
			buffer_pointer++;
			length++;
			UART1_Output_Character(character);
		}
		character = UART1_Input_Character();
	}
	*buffer_pointer = 0;
}

void UART1_Output_String(char *pt)
{
	while(*pt)
	{
		UART1_Output_Character(*pt);
		pt++;
	}
}

uint32_t UART1_Input_Unsigned_Decimal(void)
{
	uint32_t number = 0;
	uint32_t length = 0;
  char character = UART1_Input_Character();
	
	// Accepts until <enter> is typed
	// The next line checks that the input is a digit, 0-9.
	// If the character is not 0-9, it is ignored and not echoed
  while(character != UART1_CR)
	{ 
		if ((character >= '0') && (character <= '9')) 
		{
			// "number" will overflow if it is above 4,294,967,295
			number = (10 * number) + (character - '0');
			length++;
			UART1_Output_Character(character);
		}
		
		// If the input is a backspace, then the return number is
		// changed and a backspace is outputted to the screen
		else if ((character == UART1_BS) && length)
		{
			number /= 10;
			length--;
			UART1_Output_Character(character);
		}
		
		character = UART1_Input_Character();
	}
	
	return number;
}

void UART1_Output_Unsigned_Decimal(uint32_t n)
{
	// Use recursion to convert decimal number
	// of unspecified length as an ASCII string
  if (n >= 10)
	{
    UART1_Output_Unsigned_Decimal(n / 10);
    n = n % 10;
  }
	
	// n is between 0 and 9
  UART1_Output_Character(n + '0');
}

uint32_t UART1_Input_Unsigned_Hexadecimal(void)
{
	uint32_t number = 0;
	uint32_t digit = 0;
	uint32_t length = 0;
  char character = UART1_Input_Character();
	
	while(character != UART1_CR)
	{
		// Initialize digit and assume that the hexadecimal character is invalid
		digit = 0x10;
		
		if ((character >= '0') && (character <= '9'))
		{
			digit = character - '0';
		}
		else if ((character>='A') && (character <= 'F'))
		{
			digit = (character - 'A') + 0xA;
		}
		else if ((character >= 'a') && (character <= 'f'))
		{
			digit = (character - 'a') + 0xA;
		}
	
		// If the character is not 0-9 or A-F, it is ignored and not echoed
    if (digit <= 0xF)
		{
      number = (number * 0x10) + digit;
      length++;
      UART1_Output_Character(character);
    }
		
		// Backspace outputted and return value changed if a backspace is inputted
		else if((character == UART1_BS) && length)
		{
			number /= 0x10;
			length--;
			UART1_Output_Character(character);
		}
		
		character = UART1_Input_Character();
  }
  return number;
}

void UART1_Output_Unsigned_Hexadecimal(uint32_t number)
{
	// Use recursion to convert the number of
	// unspecified length as an ASCII string
	if (number >= 0x10)
	{
		UART1_Output_Unsigned_Hexadecimal(number / 0x10);
		UART1_Output_Unsigned_Hexadecimal(number % 0x10);
	}
	else
	{
		if (number < 0xA)
		{
			UART1_Output_Character(number + '0');
		}
		else
		{
			UART1_Output_Character((number - 0x0A) + 'A');
		}
	}
}

void UART1_Output_Newline(void)
{
	UART1_Output_Character(UART1_CR);
	UART1_Output_Character(UART1_LF);
}
