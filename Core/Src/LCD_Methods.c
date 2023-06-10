#include "LCD_Methods.h"
#include "ILI9341.h"
#include "main.h"
#include <stdint.h>

//lookup tables from library representing LCD screen grid
extern const uint8_t defaultData_white[320 * 240 * 2];
extern const uint8_t defaultData_green[320 * 240 * 2];
extern const uint8_t defaultData_red[320 * 240 * 2];
extern const uint8_t defaultData_blue[320 * 240 * 2];


void displayDir(DIR dir) {
	/*
	 Displays direction on LCD in BLUE using library functions.
	 Pixel grid mapped 0 to 320 (bottom to top) on y axis and 0 to 240 on x-axis (left to right)
	 :param dir: direction to display
	 */

	switch (dir) {
		//display blue on top half of LCD
		case UP: {
			//write blue to top half
			ILI9341_setColumnAddress(160, 320);
			ILI9341_setPageAddress(0, 240);
			ILI9341_MemoryWrite(defaultData_blue, 320 * 240 * 2);

			//write white to bottom half
			ILI9341_setColumnAddress(0, 160);
			ILI9341_setPageAddress(0, 240);
			ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);
			break;
		}
	
		//displays blue on bottom half of LCD
		case DOWN: {
			//set blue on bottom half
			ILI9341_setColumnAddress(0, 160);
			ILI9341_setPageAddress(0, 240);
			ILI9341_MemoryWrite(defaultData_blue, 320 * 240 * 2);

			//display white on top half
			ILI9341_setColumnAddress(160, 320);
			ILI9341_setPageAddress(0, 240);
			ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);
			break;
		}

		//displays blue on left half
		case LEFT: {
			//display blue on left half
			ILI9341_setColumnAddress(0, 320);
			ILI9341_setPageAddress(0, 120);
			ILI9341_MemoryWrite(defaultData_blue, 320 * 240 * 2);

			//displays white on right half
			ILI9341_setColumnAddress(0, 320);
			ILI9341_setPageAddress(120, 240);
			ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);
			break;
		}

		//displays blue on right half
		case RIGHT: {
			//display blue on right half
			ILI9341_setColumnAddress(0, 320);
			ILI9341_setPageAddress(120, 240);
			ILI9341_MemoryWrite(defaultData_blue, 320 * 240 * 2);

			//displays white on left half
			ILI9341_setColumnAddress(0, 320);
			ILI9341_setPageAddress(0, 120);
			ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);
			break;
		}
		default: break;
	}//end switch
}

void displayEmpty(void) {
	/*
	 Displays Green Screen to indicate success
	*/
	//display white screen on LCD
	ILI9341_setColumnAddress(0, 320);
	ILI9341_setPageAddress(0, 240);
	ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);
}

void displayPass(void) {
	/*
	 Displays Green Screen to indicate success
	*/
	//display full screen of green on LCD
	ILI9341_setColumnAddress(0, 320);
	ILI9341_setPageAddress(0, 240);
	ILI9341_MemoryWrite(defaultData_green, 320 * 240 * 2);
}

void displayFail(void) {
	/*
	 Displays Red Screen to indicate success
	*/
	//display full screen of red on LCD
	ILI9341_setColumnAddress(0, 320);
	ILI9341_setPageAddress(0, 240);
	ILI9341_MemoryWrite(defaultData_red, 320 * 240 * 2);
}
