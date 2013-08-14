/*
IBEX UK LTD http://www.ibexuk.com
Electronic Product Design Specialists
RELEASED SOFTWARE

The MIT License (MIT)

Copyright (c) 2013, IBEX UK Ltd, http://ibexuk.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//Project Name:		GENERIC HD44780 COMPATIBLE LCD CHARACTER DISPLAY DRIVER



#include "main.h"					//Global data type definitions (see https://github.com/ibexuk/C_Generic_Header_File )
#define LCD_C
#include "lcd-text.h"




//************************************
//************************************
//********** INITIALISE LCD **********
//************************************
//************************************
//Call on powerup
void lcd_initialise (void)
{

	//----- ENSURE DATA PORT IS SET TO OUTPUTS -----
	LCD_DIR4 = 0;
	LCD_DIR5 = 0;
	LCD_DIR6 = 0;
	LCD_DIR7 = 0;

	//----- WAIT AT LEAST 15MS FOR POWER TO STABILISE -----
	lcd_1ms_timer = 16;
	while (lcd_1ms_timer)
		;

	//----- FUNCTION SET (INTERFACE IS 8 BITS LONG) -----
	lcd_write_ctrl_hi_nibble(0x30);
	lcd_1ms_timer = 10;
	while (lcd_1ms_timer)
		;

	//----- FUNCTION SET (INTERFACE IS 8 BITS LONG) -----
	lcd_write_ctrl_hi_nibble(0x30);
	lcd_1ms_timer = 10;
	while (lcd_1ms_timer)
		;

	//----- FUNCTION SET (INTERFACE IS 8 BITS LONG) -----
	lcd_write_ctrl_hi_nibble(0x30);
	lcd_1ms_timer = 10;
	while (lcd_1ms_timer)
		;

	//----- FUNCTION SET (INTERFACE IS 4 BITS LONG) -----
	lcd_write_ctrl_hi_nibble(0x20);
	lcd_1ms_timer = 10;
	while (lcd_1ms_timer)
		;

	//(Do not check the busy flag above here)

	//----- FUNCTION SET (4 BIT INTERFACE, NUMBER OF LINES, FONT) -----
	//Bit 3 clear for 1 line, set for 2 lines
	//Bit 2 clear for 5x8 dot font, set for 5x10 dot font
	#ifdef LCD_FONT_IS_5X10
		if (LCD_NUMBER_OF_LINES > 1)
			lcd_write_control_byte(0x20 | 0x08 | 0x04);
		else
			lcd_write_control_byte(0x20 | 0x04);	
	#else
		if (LCD_NUMBER_OF_LINES > 1)
			lcd_write_control_byte(0x20 | 0x08);
		else
			lcd_write_control_byte(0x20);	
	#endif

	//----- DISPLAY ON/OFF (DISPLAY ON, CURSOR OFF, BLINK OFF) -----
	while (lcd_is_screen_busy())			//Wait if screen is busy from last operation
		;
	lcd_write_control_byte(0x0c);

	//----- CLEAR DISPLAY -----
	while (lcd_is_screen_busy())			//Wait if screen is busy from last operation
		;
	lcd_write_control_byte(0x01);

	//----- ENTRY MODE SET (AUTO-INCREMENT AND NO SHIFT) -----
	while (lcd_is_screen_busy())			//Wait if screen is busy from last operation
		;
	lcd_write_control_byte(0x06);
	lcd_1ms_timer = 10;
	while (lcd_1ms_timer)
		;

}



//****************************************
//****************************************
//********** UPDATE LCD DISPLAY **********
//****************************************
//****************************************
//Call as part of your main loop.  This function outputs characters to the display and exits if the display is currently busy to avoid application stall with slow displays.
void lcd_update_display (void)
{
	BYTE do_next_operation;

	//----- CHECK FOR START UPDATE OF NEW LINE -----
	if ((lcd_outputting_line_number == 0xff) && (lcd_update_line_numbers > 0))
	{
		if (lcd_update_line_numbers & 0x01)
		{
			lcd_outputting_line_number = 0;
			lcd_update_line_numbers &= ~0x01;
		}
		else if (lcd_update_line_numbers & 0x02)
		{
			lcd_outputting_line_number = 1;
			lcd_update_line_numbers &= ~0x02;
		}
		else if (lcd_update_line_numbers & 0x04)
		{
			lcd_outputting_line_number = 2;
			lcd_update_line_numbers &= ~0x04;
		}
		else if (lcd_update_line_numbers & 0x08)
		{
			lcd_outputting_line_number = 3;
			lcd_update_line_numbers &= ~0x08;
		}
		lcd_outputting_line_stage = 0;
	}

	if (lcd_outputting_line_number < 0xff)
	{
		do_next_operation = 1;
		while (do_next_operation)
		{
			//---------------------------------------------------------
			//----- WRITING A LINE TO THE DISPLAY - DO NEXT STAGE -----
			//---------------------------------------------------------
			
			//----- EXIT IF SCREEN IS STILL BUSY FROM LAST OPERATION -----
			if (lcd_is_screen_busy())
				break;						//We will try again the next time this function is called
		
			switch (lcd_outputting_line_stage)
			{
			case 0:
				//----- SET ADDRESS TO START OF LINE -----
				if (lcd_outputting_line_number == 0)
					lcd_write_control_byte(LCD_LINE_0_LEFT_CHAR_ADDRESS | 0x80);		//(Bit 7 is high for set DDRAM address command)
				else if (lcd_outputting_line_number == 1)
					lcd_write_control_byte(LCD_LINE_1_LEFT_CHAR_ADDRESS | 0x80);
				else if (lcd_outputting_line_number == 2)
					lcd_write_control_byte(LCD_LINE_2_LEFT_CHAR_ADDRESS | 0x80);
				else if (lcd_outputting_line_number == 3)
					lcd_write_control_byte(LCD_LINE_3_LEFT_CHAR_ADDRESS | 0x80);
				lcd_outputting_line_stage++;
				lcd_outputting_line_character = 0;
				break;


			case 1:
				//----- DISPLAY FIRST HALF OF LINE -----

				//OUTPUT THE NEXT CHARACTER
				lcd_write_data_byte(lcd_text_buffer[lcd_outputting_line_number][lcd_outputting_line_character]);
				lcd_outputting_line_character++;

				//CHECK FOR FINISHED FIRST HALF OF THE LINE
				if (lcd_outputting_line_character == (LCD_LINE_LENGTH >> 1))
					lcd_outputting_line_stage++;

				break;


			case 2:
				//----- SET ADDRESS TO MIDDLE OF LINE -----
				//(We do this to deal with displays that have a different start address for the second half of each line)
				if (lcd_outputting_line_number == 0)
					lcd_write_control_byte(LCD_LINE_0_MIDDLE_CHAR_ADDRESS | 0x80);
				else if (lcd_outputting_line_number == 1)
					lcd_write_control_byte(LCD_LINE_1_MIDDLE_CHAR_ADDRESS | 0x80);
				else if (lcd_outputting_line_number == 2)
					lcd_write_control_byte(LCD_LINE_2_MIDDLE_CHAR_ADDRESS | 0x80);
				else if (lcd_outputting_line_number == 3)
					lcd_write_control_byte(LCD_LINE_3_MIDDLE_CHAR_ADDRESS | 0x80);

				lcd_outputting_line_stage++;
				break;


			case 3:
				//----- DISPLAY SECOND HALF OF LINE -----

				//OUTPUT THE NEXT CHARACTER
				lcd_write_data_byte(lcd_text_buffer[lcd_outputting_line_number][lcd_outputting_line_character]);
				lcd_outputting_line_character++;

				//CHECK FOR FINISHED THE LINE
				if (lcd_outputting_line_character == LCD_LINE_LENGTH)
				{
					lcd_outputting_line_number = 0xff;			//Change status to not writing to the display
					do_next_operation = 0;
				}

				break;


			} //switch (lcd_outputting_line_stage)

		} //while (do_next_operation)

	} //if (lcd_outputting_line_number < 0xff)



	/* If you want to implement using the cursor, here'show:-
	//Flags:-
	//Bit 0 = Show cursor
	//Bit 1 = Blink cursor

	//----- DISPLAY CURSOR -----
	//(Cursor is already enabled but has fallen off end of screen)
	data = 0x0c;

	//does cursor need displaying?
	if (flags & 0x01)
		data |= 0x02;

	//does blinking need displaying?
	if (flags & 0x02)
		data |= 0x01;

	lcd_write_control_byte(data);

	//If cursor or blink are on then set the cursor position
	if (flags & 0x03)
	{
		//Set cursor to start of line 1
		lcd_write_control_byte(0x80);

		//Move the cursor to the required position
		for (character_count = 0; character_count < cursor_position; character_count++)
		{
			//cursor shift right
			lcd_write_control_byte(0x14);	
		}
	}
	*/


}



//***********************************************
//***********************************************
//********** WRITE LINE OF LCD DISPLAY **********
//***********************************************
//***********************************************
//Call these functions to display a line of text
//line_number	0 = top line
//alignment		LCD_ALIGN_LEFT, LCD_ALIGN_CENTER or, LCD_ALIGN_RIGHT
//text_line		Null terminated string

//----------------------------------------
//----------------------------------------
//----- VERSION FOR CONSTANT STRINGS -----
//----------------------------------------
//----------------------------------------
void lcd_display_const_string (BYTE line_number, BYTE alignment, CONSTANT BYTE *text_line)
{
	BYTE *character;
	BYTE count;
	BYTE text_length;
	BYTE leading_spaces_to_insert;
	BYTE null_detected;
	
	//----- ENSURE LINE NUMBER IS WITHIN RANGE -----
	if (line_number >= LCD_NUMBER_OF_LINES)
		return;

	//-----  CALCULATE THE TEXT LENGTH -----
	text_length = 0;
	for (count = 0; count < LCD_LINE_LENGTH; count++)
	{
		if (text_line[count] != 0x00)
			text_length++;
		else
			break;
	}

	//----- SETUP ALIGNMENT -----
	if (alignment == LCD_ALIGN_CENTER)
	{
		leading_spaces_to_insert = (LCD_LINE_LENGTH - text_length) >> 1;
	}
	else if (alignment == LCD_ALIGN_RIGHT)
	{
		leading_spaces_to_insert = LCD_LINE_LENGTH - text_length;
	}
	else
	{
		leading_spaces_to_insert = 0;
	}

	//----- COPY THE TEXT TO OUR DISPLAY BUFFER -----
	character = &lcd_text_buffer[0][0];
	character += (line_number * LCD_LINE_LENGTH);

	null_detected = 0;
	for (count = 0; count < LCD_LINE_LENGTH; count++)
	{
		if (leading_spaces_to_insert)
		{
			//Insert leading space to provide the requested alignment
			leading_spaces_to_insert--;
			*character++ = ' ';
		}
		else if (null_detected == 0)
		{
			//Display the next character, checking to see if its the last
			if (*text_line != 0x00)
			{
				*character++ = *text_line++;
			}
			else
			{
				null_detected = 1;
				*character++ = ' ';
			}
		}
		else
		{
			//There are no more characters to display so blank out rest of line
			*character++ = ' ';
		}
	}
	
	//----- TRIGGER THIS LINE TO BE UPDATED -----
	//If we are currently outputting this line then reset to start from the beginning of the line again
	if (lcd_outputting_line_number == line_number)
		lcd_outputting_line_stage = 0;
	
	if (line_number == 0)
		lcd_update_line_numbers |= 0x01;
	else if (line_number == 1)
		lcd_update_line_numbers |= 0x02;
	else if (line_number == 2)
		lcd_update_line_numbers |= 0x04;
	else
		lcd_update_line_numbers |= 0x08;
}



//----------------------------------------
//----------------------------------------
//----- VERSION FOR VARIABLE STRINGS -----
//----------------------------------------
//----------------------------------------
void lcd_display_variable_string (BYTE line_number, BYTE alignment, BYTE *text_line)
{
	BYTE *character;
	BYTE count;
	BYTE text_length;
	BYTE leading_spaces_to_insert;
	BYTE null_detected;
	
	//----- ENSURE LINE NUMBER IS WITHIN RANGE -----
	if (line_number >= LCD_NUMBER_OF_LINES)
		return;

	//-----  CALCULATE THE TEXT LENGTH -----
	text_length = 0;
	for (count = 0; count < LCD_LINE_LENGTH; count++)
	{
		if (text_line[count] != 0x00)
			text_length++;
		else
			break;
	}

	//----- SETUP ALIGNMENT -----
	if (alignment == LCD_ALIGN_CENTER)
	{
		leading_spaces_to_insert = (LCD_LINE_LENGTH - text_length) >> 1;
	}
	else if (alignment == LCD_ALIGN_RIGHT)
	{
		leading_spaces_to_insert = LCD_LINE_LENGTH - text_length;
	}
	else
	{
		leading_spaces_to_insert = 0;
	}

	//----- COPY THE TEXT TO OUR DISPLAY BUFFER -----
	character = &lcd_text_buffer[0][0];
	character += (line_number * LCD_LINE_LENGTH);
	
	null_detected = 0;
	for (count = 0; count < LCD_LINE_LENGTH; count++)
	{
		if (leading_spaces_to_insert)
		{
			//Insert leading space to provide the requested alignment
			leading_spaces_to_insert--;
			*character++ = ' ';
		}
		else if (null_detected == 0)
		{
			//Display the next character, checking to see if its the last
			if (*text_line != 0x00)
			{
				*character++ = *text_line++;
			}
			else
			{
				null_detected = 1;
				*character++ = ' ';
			}
		}
		else
		{
			//There are no more characters to display so blank out rest of line
			*character++ = ' ';
		}
	}
	
	//----- TRIGGER THIS LINE TO BE UPDATED -----
	//If we are currently outputting this line then reset to start from the beginning of the line again
	if (lcd_outputting_line_number == line_number)
		lcd_outputting_line_stage = 0;
	
	if (line_number == 0)
		lcd_update_line_numbers |= 0x01;
	else if (line_number == 1)
		lcd_update_line_numbers |= 0x02;
	else if (line_number == 2)
		lcd_update_line_numbers |= 0x04;
	else
		lcd_update_line_numbers |= 0x08;
}



//************************************
//************************************
//********** IS SCREEN BUSY **********
//************************************
//************************************
//Returns 1 if screen is still busy from last operation, 0 if not
BYTE lcd_is_screen_busy (void)
{
	BYTE busy_bit;
	
	//SET DATA PORT TO INPUTS
	LCD_DIR7 = 1;
	LCD_DIR6 = 1;
	LCD_DIR5 = 1;
	LCD_DIR4 = 1;

	LCD_RW = 1;
	LCD_ENABLE = 0;
	LCD_ID = 0;
	LCD_ENABLE = 1;
	LCD_BUS_ACCESS_DELAY;
	
	//GET THE BUSY BIT
	busy_bit = 0;
	if (LCD_DATA7_IP)
		busy_bit = 1;
	
	LCD_ENABLE = 0;
	LCD_BUS_ACCESS_DELAY;
	
	//DUMP SECOND NIBBLE
	LCD_ENABLE = 1;
	LCD_ENABLE_PIN_DELAY;
	LCD_ENABLE = 0;
	LCD_ENABLE_PIN_DELAY;


	//LCD TO READ
	LCD_RW = 0;

	//SET DATA PORT BACK TO OUTPUTS
	LCD_DIR7 = 0;
	LCD_DIR6 = 0;
	LCD_DIR5 = 0;
	LCD_DIR4 = 0;

	return(busy_bit);
}



//********************************************************************
//********************************************************************
//********** SEND HIGH AND LOW NIBBLES OF W TO LCD DATA REG **********
//********************************************************************
//********************************************************************
void lcd_write_data_byte (BYTE data)
{

	//----- SEND HIGH NIBBLE TO LCD DATA REG -----
	LCD_ENABLE = 0;
	LCD_ID = 1;				//set lcd to data input

	if (data & 0x80)
		LCD_DATA7_OP = 1;
	else
		LCD_DATA7_OP = 0;

	if (data & 0x40)
		LCD_DATA6_OP = 1;
	else
		LCD_DATA6_OP = 0;

	if (data & 0x20)
		LCD_DATA5_OP = 1;
	else
		LCD_DATA5_OP = 0;

	if (data & 0x10)
		LCD_DATA4_OP = 1;
	else
		LCD_DATA4_OP = 0;

	LCD_RW = 0;

	LCD_ENABLE = 1;
	LCD_ENABLE_PIN_DELAY;
	LCD_ENABLE = 0;
	LCD_ENABLE_PIN_DELAY;


	//----- SEND LOW NIBBLE TO LCD DATA REG -----
	LCD_ENABLE = 0;
	LCD_ID = 1;				//set lcd to data input

	if (data & 0x08)
		LCD_DATA7_OP = 1;
	else
		LCD_DATA7_OP = 0;

	if (data & 0x04)
		LCD_DATA6_OP = 1;
	else
		LCD_DATA6_OP = 0;

	if (data & 0x02)
		LCD_DATA5_OP = 1;
	else
		LCD_DATA5_OP = 0;

	if (data & 0x01)
		LCD_DATA4_OP = 1;
	else
		LCD_DATA4_OP = 0;

	LCD_RW = 0;

	LCD_ENABLE = 1;
	LCD_ENABLE_PIN_DELAY;
	LCD_ENABLE = 0;
	LCD_ENABLE_PIN_DELAY;
}



//***************************************************************
//***************************************************************
//********** SEND HIGH AND LOW NIBBLES TO LCD CTRL REG **********
//***************************************************************
//***************************************************************
void lcd_write_control_byte (BYTE data)
{
	//----- SEND HIGH NIBBLE TO LCD CONTROL REG -----
	lcd_write_ctrl_hi_nibble(data);

	//----- SEND LOW NIBBLE TO LCD CONTROL REG -----
	lcd_write_ctrl_lo_nibble(data);
}



//*********************************************************
//*********************************************************
//********** SEND HI NIBBLE OF W TO LCD CTRL REG **********
//*********************************************************
//*********************************************************
void lcd_write_ctrl_hi_nibble (BYTE data)
{
	LCD_ENABLE = 0;
	LCD_ID = 0;				//set lcd to instruction input


	if (data & 0x80)
		LCD_DATA7_OP = 1;
	else
		LCD_DATA7_OP = 0;

	if (data & 0x40)
		LCD_DATA6_OP = 1;
	else
		LCD_DATA6_OP = 0;

	if (data & 0x20)
		LCD_DATA5_OP = 1;
	else
		LCD_DATA5_OP = 0;

	if (data & 0x10)
		LCD_DATA4_OP = 1;
	else
		LCD_DATA4_OP = 0;

	LCD_RW = 0;

	LCD_ENABLE = 1;
	LCD_ENABLE_PIN_DELAY;
	LCD_ENABLE = 0;
	LCD_ENABLE_PIN_DELAY;
}



//*********************************************************
//*********************************************************
//********** SEND LO NIBBLE OF W TO LCD CTRL REG **********
//*********************************************************
//*********************************************************
void lcd_write_ctrl_lo_nibble (BYTE data)
{

	LCD_ENABLE = 0;
	LCD_ID = 0;				//set lcd to instruction input

	if (data & 0x08)
		LCD_DATA7_OP = 1;
	else
		LCD_DATA7_OP = 0;

	if (data & 0x04)
		LCD_DATA6_OP = 1;
	else
		LCD_DATA6_OP = 0;

	if (data & 0x02)
		LCD_DATA5_OP = 1;
	else
		LCD_DATA5_OP = 0;

	if (data & 0x01)
		LCD_DATA4_OP = 1;
	else
		LCD_DATA4_OP = 0;

	LCD_RW = 0;

	LCD_ENABLE = 1;
	LCD_ENABLE_PIN_DELAY;
	LCD_ENABLE = 0;
	LCD_ENABLE_PIN_DELAY;
}



