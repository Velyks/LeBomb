#include <math.h>

const byte pinData = 5;
const byte pinLatch = 6;
const byte pinClock = 7;
byte digits[10] = {192,249,164,176,153,146,130,248,128,144};
const byte decimalP = -128;
boolean countdown = 1;
int seconds = 300;
long ticks = seconds * 24;
boolean fixTime = 0;
boolean lightRingFill = 0;



void setup() {
  pinMode (pinData, OUTPUT);
  pinMode (pinLatch, OUTPUT);
  pinMode (pinClock, OUTPUT);
  pinMode (13, OUTPUT);
  pinMode (9, INPUT);
  cli();
  
  //Timer
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 650; //15624
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1<< CS10);
  TIMSK1 |= (1 << OCIE1A);
  
  sei();
}

ISR(TIMER1_COMPA_vect){


  if (countdown && (seconds > 0)){
    ticks--;
  }
  else if (!countdown){
    ticks++;
  }

   if (ticks % 24 == 0){
	  lightRingFill = !lightRingFill;
  }

  //The timer gains/loses an extra second every 24 seconds. So this is to sort this out.
 // if (fixTime){
	//  if (countdown){
	//	  ticks++;
	//  }
	//  else if (!countdown){
	//	  ticks--;
	//  }
	//  fixTime = 0;
 // }
 // else if ((ticks % 24) == 0){  
	//fixTime = 1;
 // }
  // End timer fix
}

void startTimer(){
  sei();
}

void stopTimer(){
  cli();
}

void updateRotator(){
	long position = (long)(0x800000) >> (ticks % 24);

	//long expo = ticks % 24;
	//long position = 0;

	//if (!lightRingFill){
	//	position = (long)0xffffff >> expo;
	//	
	//}
	//else if (lightRingFill){
	//	position = (long)(0xffffff >> expo) ^ 0xffffff;
	//}


	if (seconds == 0){
		position = 0xFFFFFF;
	}
	byte first = position >> 16;
	byte second = position >> 8;
	byte third = (byte) position;

	shiftOut(pinData, pinClock, MSBFIRST, first);
	shiftOut(pinData, pinClock, MSBFIRST, second);
	shiftOut(pinData, pinClock, MSBFIRST, third);
}

void update7Seg(int seconds, int minutes){
    int digit[4];
    digit[0] = minutes / 10;
    digit[1] = minutes % 10;
    digit[2] = seconds / 10;
    digit[3] = seconds % 10;
    
    for(int i = 0; i<4; i++){
      byte section = 1 << i;
      digitalWrite(pinLatch, LOW);
	  updateRotator();
      shiftOut(pinData, pinClock, MSBFIRST, section);
      shiftOut(pinData, pinClock, MSBFIRST, digits[digit[i]]);
      digitalWrite(pinLatch, HIGH);
    }
}



void loop() {
	seconds = ticks / 24;
	update7Seg((seconds % 60), (seconds / 60));
}
