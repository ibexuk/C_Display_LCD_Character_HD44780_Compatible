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





//*************************************
//*************************************
//********** DRIVER REVISION **********
//*************************************
//*************************************
//
//V1.00
//- Original release



//*********************************
//*********************************
//********** USAGE NOTES **********
//*********************************
//*********************************

//Include this header file in any .c files within your project from which you wish to use it's functions.

//##################################
//##### APPLICATION INITIALISE #####
//##################################
/*
	//----- INITIALISE LCD SCREEN -----
	lcd_initialise();
*/

//#################################
//##### APPLICATION MAIN LOOP #####
//#################################
/*
	//----- CHECK FOR UPDATE LCD SCREEN -----
	lcd_update_display();
*/


//################################################
//##### ADD THIS TO YOUR HEARTBEAT INTERRUPT #####
//################################################
/*
	//----- 1mS HEARTBEAT TIMER -----
	if (lcd_1ms_timer)
		lcd_1ms_timer--;
*/

//###############################################
//##### DISPLAY STRINGS FROM PROGRAM MEMORY #####
//###############################################
/*
CONSTANT BYTE text_demo_l1[] = {"This is line 1"};
CONSTANT BYTE text_demo_l2[] = {"This is line 2"};
CONSTANT BYTE text_demo_l3[] = {"This is line 3"};
CONSTANT BYTE text_demo_l4[] = {"This is line 4"};


	//DISPLAY TEST MESSAGE
	lcd_display_const_string (0, LCD_ALIGN_LEFT, &text_demo_l1[0]);			//Line 0
	lcd_display_const_string (1, LCD_ALIGN_CENTER, &text_demo_l2[0]);		//Line 1
	lcd_display_const_string (2, LCD_ALIGN_RIGHT, &text_demo_l3[0]);		//Line 2
	lcd_display_const_string (3, LCD_ALIGN_LEFT, &text_demo_l4[0]);			//Line 3

*/

//###########################################
//##### DISPLAY STRINGS FROM RAM MEMORY #####
//###########################################
/*
	BYTE text_demo_l1[] = {"This is line 1"};
	BYTE text_demo_l2[] = {"This is line 2"};
	BYTE text_demo_l3[] = {"This is line 3"};
	BYTE text_demo_l4[] = {"This is line 4"};


	//DISPLAY TEST MESSAGE
	lcd_display_variable_string (0, LCD_ALIGN_LEFT, &text_demo_l1[0]);			//Line 0
	lcd_display_variable_string (1, LCD_ALIGN_CENTER, &text_demo_l2[0]);		//Line 1
	lcd_display_variable_string (2, LCD_ALIGN_RIGHT, &text_demo_l3[0]);			//Line 2
	lcd_display_variable_string (3, LCD_ALIGN_LEFT, &text_demo_l4[0]);			//Line 3

*/




//*****************************
//*****************************
//********** DEFINES **********
//*****************************
//*****************************
#ifndef LCD_C_INIT		//(Do only once)
#define LCD_C_INIT



//----------------
//----- PINS -----									//<<<<< CHECK FOR A NEW APPLICATION <<<<<
//----------------
#define	LCD_ENABLE			LATCbits.LATC2			//Enable = 0
#define	LCD_RW				LATCbits.LATC1			//Write = 0, read = 1
#define	LCD_ID				LATCbits.LATC0			//Instruction = 0, data = 1 (sometimes called RS instead of ID)
#define	LCD_DATA4_OP		LATBbits.LATB0
#define	LCD_DATA5_OP		LATBbits.LATB1
#define	LCD_DATA6_OP		LATBbits.LATB2
#define	LCD_DATA7_OP		LATBbits.LATB3
#define	LCD_DATA4_IP		PORTBbits.RB0
#define	LCD_DATA5_IP		PORTBbits.RB1
#define	LCD_DATA6_IP		PORTBbits.RB2
#define	LCD_DATA7_IP		PORTBbits.RB3
#define	LCD_DIR4			TRISBbits.TRISB0
#define	LCD_DIR5			TRISBbits.TRISB1
#define	LCD_DIR6			TRISBbits.TRISB2
#define	LCD_DIR7			TRISBbits.TRISB3

#define	LCD_ENABLE_PIN_DELAY		Nop(); Nop();
#define	LCD_BUS_ACCESS_DELAY		Nop(); Nop();


//--------------------------------
//----- SCREEN CONFIGURATION -----							//<<<<< CHECK FOR A NEW APPLICATION <<<<<
//--------------------------------
#define	LCD_NUMBER_OF_LINES		4		//1, 2 or 4 permitted (if your 1 line display does not work try configuring this as 2 lines - some single line displays are actually internally wired as 2 lines)
#define	LCD_LINE_LENGTH			20		//Number of characters per line of the display (must be even - usually <= 20 as displays that are more than 20 characters wide are typically configured as 2 lines per actual display line)

//SCREEN FONT
//#define LCD_FONT_IS_5X10				//Comment for 5x8 font (usually font is 5x8)

//----- CHARACTER ADDRESSES IN LCD SCREEN RAM -----
//(values for lines not implemented in this application can be ignored)
#define	LCD_LINE_0_LEFT_CHAR_ADDRESS		0x00
#define	LCD_LINE_0_MIDDLE_CHAR_ADDRESS		0x0a			//(Some displays have a different RAM start address for the second half of each line)
#define	LCD_LINE_1_LEFT_CHAR_ADDRESS		0x14
#define	LCD_LINE_1_MIDDLE_CHAR_ADDRESS		0x1e
#define	LCD_LINE_2_LEFT_CHAR_ADDRESS		0x40
#define	LCD_LINE_2_MIDDLE_CHAR_ADDRESS		0x4a
#define	LCD_LINE_3_LEFT_CHAR_ADDRESS		0x54
#define	LCD_LINE_3_MIDDLE_CHAR_ADDRESS		0x5e





//-------------------------
//----- DISPLAY FLAGS -----
//-------------------------

//CURSOR CONTROL
#define	LCD_CURSOR_OFF			0x00
#define	LCD_CURSOR_ON			0x01
#define	LCD_CURSOR_BLINKING		0x02

//TEXT ALIGNMENT
#define	LCD_ALIGN_LEFT			0
#define	LCD_ALIGN_CENTER		1
#define	LCD_ALIGN_CENTRE		1
#define	LCD_ALIGN_RIGHT			2

#endif



//*******************************
//*******************************
//********** FUNCTIONS **********
//*******************************
//*******************************
#ifdef LCD_C
//-----------------------------------
//----- INTERNAL ONLY FUNCTIONS -----
//-----------------------------------
BYTE lcd_is_screen_busy (void);
void lcd_write_data_byte (BYTE data);
void lcd_write_control_byte (BYTE data);
void lcd_write_ctrl_hi_nibble (BYTE data);
void lcd_write_ctrl_lo_nibble (BYTE data);


//-----------------------------------------
//----- INTERNAL & EXTERNAL FUNCTIONS -----
//-----------------------------------------
//(Also defined below as extern)
void lcd_initialise (void);
void lcd_update_display (void);
void lcd_display_variable_string (BYTE line_number, BYTE alignment, BYTE *text_line);
void lcd_display_const_string (BYTE line_number, BYTE alignment, CONSTANT BYTE *text_line);


#else
//------------------------------
//----- EXTERNAL FUNCTIONS -----
//------------------------------
extern void lcd_initialise (void);
extern void lcd_update_display (void);
extern void lcd_display_variable_string (BYTE line_number, BYTE alignment, BYTE *text_line);
extern void lcd_display_const_string (BYTE line_number, BYTE alignment, CONSTANT BYTE *text_line);


#endif




//****************************
//****************************
//********** MEMORY **********
//****************************
//****************************
#ifdef LCD_C
//--------------------------------------------
//----- INTERNAL ONLY MEMORY DEFINITIONS -----
//--------------------------------------------
BYTE lcd_outputting_line_number = 0xff;
BYTE lcd_update_line_numbers = 0;
BYTE lcd_text_buffer[LCD_NUMBER_OF_LINES][LCD_LINE_LENGTH];
BYTE lcd_outputting_line_stage;
BYTE lcd_outputting_line_character;


//--------------------------------------------------
//----- INTERNAL & EXTERNAL MEMORY DEFINITIONS -----
//--------------------------------------------------
//(Also defined below as extern)
volatile BYTE lcd_1ms_timer;


#else
//---------------------------------------
//----- EXTERNAL MEMORY DEFINITIONS -----
//---------------------------------------
extern volatile BYTE lcd_1ms_timer;


#endif






