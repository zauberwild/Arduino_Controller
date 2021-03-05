#include <Joystick.h>

// PINS
const int pinUp = 15;
const int pinDown = 0;
const int pinLeft = 0;
const int pinRight = 0;

const int pinL1 = 0;
const int pinR1 = 0;

const int pinSel = 0;
const int pinMod = 0;

const int pinPotiX = A0;
const int pinPotiY = A1;
const int pinPotiZ = 10;

const int pinLed = 16;


// OUTPUT STATES
int up = 0;				// values for buttons on the right side
int down = 0;
int left = 0;
int right = 0;

int l1 = 0;				// shoulder buttons
int r1 = 0;

int sel = 0;			// select and mode
int mod = 0;

float potiX = 0;		// analog values for joystick
float potiY = 0;
int potiZ = 0;

int potiDigUp = 0;			// digital values for joystick
int potiDigDown = 0;
int potiDigLeft = 0;
int potiDigRight = 0;


// PREVIOUS OUTPUT STATES
int prevUp = 0;
int prevDown = 0;
int prevLeft = 0;
int prevRight = 0;

int prevL1 = 0;
int prevR1 = 0;

int prevSel = 0;
int prevMod = 0;

float prevPotiX = 0;
float prevPotiY = 0;
int prevPotiZ = 0;

int prevPotiDigUp = 0;
int prevPotiDigDown = 0;
int prevPotiDigLeft = 0;
int prevPotiDigRight = 0;

float potiThreshold = 127/6;    // threshold for change on potis
float dpadThreshold = 127/2;	// threshold for dpad mode on joystick


// OPERATING MODES
enum {controller, serial};
int com_mode = controller;
/* sets the sending mode. in controller mode, the arduino will act as an actual controller,
   in serial mode the arduino will send the values over the serial port.
   change with sending on serial port: "com_mode=controller" or "com_mode=serial"
*/
enum {analog, digital};
int joystick_mode = analog;
/* this sets how the joystick will be treated. analog means the values will be sent as actual analog values,
   digital means, that the joystick will be used as a dpad / hat.
   NOTE: this only applies to controller mode. in serial mode, both is being send (analog between the letters, digital at the end of line)
   change with sending on serial port: "joystick_mode=analog" or "joystick_mode=digital"
*/
enum {continuous, onEvent};
int sending_mode = continuous;
/* this set on how the data should be send. continouos results in a continuous stream of data, onEvent only sends when something has changed.
   NOTE: this only applies to the serial port. In controller mode, the data will be send continuously in either case.
   change with sending on serial port "sending_mode=continuous" or "sending_mode=onEvent"
*/
bool autoSendState = true;

// Functions
void blinkLed(int n){
	/* lets the led blinks the desired amount of time (yes, with the all-forbidden delay())
		1x: controller mode, 2x: serial + continuous; 3x: serial + on event
	*/
	byte d = 200;
	for(int i = 0; i < n; i++){
		digitalWrite(pinLed, HIGH);
		delay(d);
		digitalWrite(pinLed, LOW);
		delay(d);
	}
}

void setup() {
  //communication
  Serial.begin(9600);
  Joystick.begin(autoSendState);
  // pinmodes
  pinMode(pinLed, OUTPUT);

  blinkLed(2);
  delay(350);
  blinkLed(1);
}

void loop() {
	// watch the serial port
  	if(Serial.available() > 0){
		String in = Serial.readString();
		in.trim();
		in.replace(" ", "");

		//com_mode
		if(in == "com_mode=controller"){
			com_mode = controller;
			Serial.println("com_mode=controller");
			Serial.println("Communication mode is set to controller");
			blinkLed(1);
		}
		else if(in == "com_mode=serial"){
			com_mode = serial;
			Serial.println("com_mode=serial");
			Serial.println("Communication mode is set to serial");
			if(sending_mode=continuous) blinkLed(2);
			if(sending_mode=onEvent) blinkLed(3);
		}

		// joystick mode
		else if (in == "joystick_mode=analog"){
			joystick_mode = analog;
			Serial.println("joystick_mode=analog");
			Serial.println("sending analog values");
		}
		else if(in == "joystick_mode=digital"){
			joystick_mode = digital;
			Serial.println("joystick_mode=digital");
			Serial.println("sending digital values");
		}
		// sending_mode
		else if(in == "sending_mode=continuous") {
			sending_mode = continuous;
			Serial.println("sending_mode=continuous");
			Serial.println("sending continuous data stream");
			blinkLed(2);
		}
		else if (in == "sending_mode=onEvent"){
			sending_mode = onEvent;
			Serial.println("sending_mode=onEvent");
			Serial.println("sending only on event");
			blinkLed(3);
		}
		// error message
		else{
			Serial.println("ERROR: '" + in + "' doesn't match with any commands");
		}
		  
  	}

	// read in data
	// buttons on the right
	up = !digitalRead(pinUp);
	down = !digitalRead(pinDown);
	left = !digitalRead(pinLeft);
	right = !digitalRead(pinRight);
	
	mod = !digitalRead(pinMod);
	sel = !digitalRead(pinSel);

	// shoulder buttons
	l1 = !digitalRead(pinL1);
	r1 = !digitalRead(pinR1);

	// joystick
	potiX = analogRead(pinPotiX);
	potiY = analogRead(pinPotiY);
	potiX = map(potiX, 0, 1023, -127, 127);
	potiY = map(potiY, 0, 1023, -127, 127);
	potiZ = !digitalRead(pinPotiZ);

	potiDigUp 	= potiY < -dpadThreshold;
	potiDigLeft = potiX < -dpadThreshold;
	potiDigRight = potiX > dpadThreshold;
	potiDigDown = potiY > dpadThreshold;

	// controller mode
	if(com_mode == controller){
		Joystick.setButton(0, down);
		Joystick.setButton(1, right);
		Joystick.setButton(2, left);
		Joystick.setButton(3, up);

		Joystick.setButton(4, l1);
		Joystick.setButton(5, r1);

		Joystick.setButton(8, sel);
		Joystick.setButton(9, mod);

		if(joystick_mode == analog){
			Joystick.setXAxis(potiX);
			Joystick.setYAxis(potiY);
			Joystick.setButton(10, potiZ);
		}else{
			Joystick.setButton(11, potiDigDown);
			Joystick.setButton(12, potiDigRight);
			Joystick.setButton(13, potiDigLeft);
			Joystick.setButton(14, potiDigUp);		
		}
	}
	// serial mode
	else if (com_mode == serial){
		// continouos stream
		if(sending_mode == continuous){
			Serial.print(up);
			Serial.print(down);
			Serial.print(left);
			Serial.print(right);
			Serial.print(mod);
			Serial.print(sel);
			Serial.print(l1);
			Serial.print(r1);
			Serial.print('x');
			Serial.print(potiX);
			Serial.print('y');
			Serial.print(potiY);
			Serial.print('z');
			Serial.print(potiZ);
			Serial.print(potiDigUp);
			Serial.print(potiDigDown);
			Serial.print(potiDigLeft);
			Serial.print(potiDigRight);
			Serial.print('\n');
		}
		else if (sending_mode == onEvent){			// TODO: change on event algorithm from for-loop to a lot of ifs
			int curState[] = {up, down, left, right, sel, mod, l1, r1, potiX, potiY, potiZ, potiDigUp, potiDigDown, potiDigLeft, potiDigRight};
			int prevState[] = {prevUp, prevDown, prevLeft, prevRight, prevSel, prevMod, prevL1, prevR1, prevPotiX, prevPotiY, prevPotiZ, prevPotiDigUp, prevPotiDigDown, prevPotiDigLeft, prevPotiDigRight};

			for(int i = 0; i < 11; i++){
				if(curState[i] != prevState[i]){			// compare states with previous states

					if (i == 8 || i == 9){		// if comparing the analog poti results
						
						if(!(curState[i] + potiThreshold > prevState[i]			// if poti value is NOT bigger or smaller than previous value by a certain amount
							|| curState[i] - potiThreshold < prevState[i])){
							continue;												// than break and don't send changes
							}
					}

					switch (i){
					case 0:
						Serial.print("up");
						break;
					case 1:
						Serial.print("down");
						break;
					case 2:
						Serial.print("left");
						break;
					case 3:
						Serial.print("right");
						break;
					case 4:
						Serial.print("sel");
						break;
					case 5:
						Serial.print("mod");
						break;
					case 6:
						Serial.print("l1");
						break;
					case 7:
						Serial.print("r1");
						break;
					case 8:
						Serial.print("potiX");
						break;
					case 9:
						Serial.print("potiY");
						break;
					case 10:
						Serial.print("potiZ");
						break;
					case 11:
						Serial.print("potiDigUp");
						break;
					case 12:
						Serial.print("potiDigDown");
						break;
					case 13:
						Serial.print("potiDigLeft");
						break;
					case 14:
						Serial.print("potiDigRight");
						break;
					default:
						Serial.print("ERR overflow");
						break;
					}

					Serial.println(curState[i]);
				}
			}
		}
		
	}
	
	// save previous states
	prevUp = up;
	prevDown = down;
	prevLeft = left;
	prevRight = right;

	prevL1 = l1;
	prevR1 = r1;

	prevSel = sel;
	prevMod = mod;

	prevPotiX = potiX;
	prevPotiY = potiY;
	prevPotiZ = potiZ;

	prevPotiDigUp = potiDigUp;
	prevPotiDigLeft = potiDigLeft;
	prevPotiDigRight = potiDigRight;
	prevPotiDigDown = potiDigDown;

  	delay(5);
}
