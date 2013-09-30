#include <Keypad_I2C.h>
#include <Keypad.h>
#include <PortsLCD.h>
#include <Wire.h>

//KEYPAD SETUP
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
	{'1','2','3','A'},
	{'4','5','6','B'},
	{'7','8','9','C'},
	{'*','0','#','D'}};

byte rowPins[ROWS] = {0, 1, 2, 3};
byte colPins[COLS] = {4, 5, 6, 7};

int keypadAddress = 0x3A;

Keypad_I2C kpd(makeKeymap(keys),rowPins,colPins, ROWS, COLS, keypadAddress, PCF8574);


//LCD SETUP
PortI2C myI2C (1);
LiquidCrystalI2C lcd (myI2C);
const byte lcdHeight = 4;
const byte lcdWidth = 20;

//DISARMING SETUP
boolean keySet = 0;
char disarmKey;
byte decodingStringPos[] = {0, 3};
byte disarmChars = 20;
byte disarmCharsDone = 0;
boolean disarmed = 0;

void setup()
{
	
	Serial.begin(9600);
	kpd.begin();
	Serial.println("Serial started...");
	
	lcd.begin(lcdWidth, lcdHeight);
	lcd.print("Booting...");
	delay(1000);
	lcd.clear();
	lcd.backlight();
	writeDebuggingTitle();

	randomSeed(analogRead(1));
}

void writeDisarmedScreen(){
	lcd.clear();
	lcd.setCursor(5,1);
	lcd.print("!DISARMED!");
}

void writeDebuggingTitle(){
	lcd.setCursor(0,0);
	lcd.print("Disarming...");
	lcd.setCursor(0,1);
	lcd.print("Enter numbers:");
}

char disarmingCode(){
	byte disarmPos[] = {18,1};
	char randomKey = getRandomKey();
	lcd.setCursor(disarmPos[0], disarmPos[1]);
	lcd.print(randomKey);
	return randomKey;
}

// Writes a new string of random keypad keys to the bottom row of the LCD, 3 digits in.
// This takes the current position as an array so it can set the cursor back to where it was when it
// finishes, and the number of characters to print.
void writeDecodingString(byte lastPos[]){
	lcd.setCursor(decodingStringPos[0] + disarmCharsDone, decodingStringPos[1]);

	for (int i = 0; i < (disarmChars - disarmCharsDone); i++){
		lcd.print(getRandomKey());
	}

	lcd.setCursor(lastPos[0], lastPos[1]);
}

char getRandomKey(){
	int i = random(ROWS);
	int j = random(COLS);

	return keys[i][j];
}
void loop()
{
	if (!disarmed){
		char key = kpd.getKey();

		byte currPos[] = {0,3};
		writeDecodingString(currPos);

		if (!keySet){
			disarmKey = disarmingCode();
			keySet = 1;
		}
		if (key){
			Serial.println(key);
			Serial.println(disarmKey);
		}
		if (key == disarmKey){
			Serial.println(key);
			lcd.setCursor(decodingStringPos[0] + disarmCharsDone, decodingStringPos[1]);
			lcd.print(key);
			disarmCharsDone ++;
			keySet = 0;
		}

		if(disarmCharsDone == disarmChars)
		{
			writeDisarmedScreen();
			disarmed = 1;
		}
	}

}