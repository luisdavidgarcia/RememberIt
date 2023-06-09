#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdint.h>

typedef enum {
	NONE, UP, DOWN, LEFT, RIGHT, PRESS
} DIR;

DIR readJoystick(uint16_t, uint16_t);

void startJoystickRead(void);

int8_t checkJoystickComplete(uint16_t, uint16_t);

void setupJoystickButton(void);

int8_t detectButtonPress(void);

#endif
