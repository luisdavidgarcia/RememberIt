#include "LCD_Methods.h"
#include "ILI9341.h"
#include "main.h"
#include <stdint.h>

//grid
extern const uint8_t defaultData_white[320 * 240 * 2];
extern const uint8_t defaultData_green[320 * 240 * 2];
extern const uint8_t defaultData_red[320 * 240 * 2];
extern const uint8_t defaultData_blue[320 * 240 * 2];


void displayDir(DIR dir) {
	/*
	 Displays direction on LCD in BLUE
	 :param p: LCD grid
	 :param dir: direction to display
	 */

	switch (dir) {
	case UP: {
		ILI9341_setColumnAddress(160, 320);
		ILI9341_setPageAddress(0, 240);
		ILI9341_MemoryWrite(defaultData_blue, 320 * 240 * 2);

		ILI9341_setColumnAddress(0, 160);
		ILI9341_setPageAddress(0, 240);
		ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);

		break;
	}
	case DOWN: {
		ILI9341_setColumnAddress(0, 160);
		ILI9341_setPageAddress(0, 240);
		ILI9341_MemoryWrite(defaultData_blue, 320 * 240 * 2);

		ILI9341_setColumnAddress(160, 320);
		ILI9341_setPageAddress(0, 240);
		ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);

		break;
	}
	case LEFT: {
		ILI9341_setColumnAddress(0, 320);
		ILI9341_setPageAddress(0, 120);
		ILI9341_MemoryWrite(defaultData_blue, 320 * 240 * 2);

		ILI9341_setColumnAddress(0, 320);
		ILI9341_setPageAddress(120, 240);
		ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);

		break;
	}
	case RIGHT: {
		ILI9341_setColumnAddress(0, 320);
		ILI9341_setPageAddress(120, 240);
		ILI9341_MemoryWrite(defaultData_blue, 320 * 240 * 2);

		ILI9341_setColumnAddress(0, 320);
		ILI9341_setPageAddress(0, 120);
		ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);
		break;
	}
	default: {
		break;
	}
	}
}

void displayEmpty(void) {
	/*
	 Displays Green Screen to indicate success
	 :param dir: direction to display
	 :
	 */
	ILI9341_setColumnAddress(0, 320);
	ILI9341_setPageAddress(0, 240);
	ILI9341_MemoryWrite(defaultData_white, 320 * 240 * 2);
}

void displayPass(void) {
	/*
	 Displays Green Screen to indicate success
	 :param dir: direction to display
	 */

	ILI9341_setColumnAddress(0, 320);
	ILI9341_setPageAddress(0, 240);
	ILI9341_MemoryWrite(defaultData_green, 320 * 240 * 2);
}

void displayFail(void) {
	/*
	 Displays Red Screen to indicate success
	 :param p: LCD grid
	 :param dir: direction to display
	 */

	ILI9341_setColumnAddress(0, 320);
	ILI9341_setPageAddress(0, 240);
	ILI9341_MemoryWrite(defaultData_red, 320 * 240 * 2);
}
