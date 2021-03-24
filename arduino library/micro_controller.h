/*
	micro_controller.h - Library for communicating with my custom snes-controller
	Created by zauberwild, March 2021
	license: Released under the MIT license

	explanations in ReadMe-file for this library
*/

#ifndef micro_controller_h
#define micro_controller_h

#include "Arduino.h"

class Controller
{
	public:
		Controller(int txPin, int rxPin);							// constructor
		void changeSettings(int setting, int mode);					// function for settings
		enum settings {com_mode, controller, serial, sending_mode, continuous, onEvent, joystick_mode, analog, digital};
		void update();												// updates and reads incoming data
		int up, down, left, right;
		int mode, select, l1, r1;
		int potiX, potiY, potiZ;
		int potiUp, potiDown, potiLeft, potiRight;
	private:
		int _txPin, _rxPin;
};

#endif