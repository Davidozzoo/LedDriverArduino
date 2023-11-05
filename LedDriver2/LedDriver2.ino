#include <FastLED.h> 
#define NUM_LEDS 28
#define DATA_PIN 11 			//SPI MOSI
#define COLOR_ORDER GRB 	//Green (G), Red (R), Blue (B)
#define CHIPSET WS2812B
#define BRIGHTNESS 50 
#define VOLTS 5 
#define MAX_AMPS 500 		//value in milliamps
#define BUTTON_PIN 14 		
#define CountUp 0 		
#define CountDown 1 		
#define RedDD 	0 
#define GreenDD 1
#define BlueDD 	2

CRGB leds[NUM_LEDS];

uint8_t Status;
uint8_t i;
uint8_t iLed;
uint8_t iFrame;
uint8_t xFrame;
uint8_t ResetStatus;
uint8_t WriteStatusRom = 0;
uint8_t CountDir;
uint8_t FrameDelay;
uint8_t Tri[NUM_LEDS] = {18,36,54,72,90,108,126,144,162,180,198,216,234,252,234,216,198,180,162,144,126,108,90,72,54,36,18,18};
uint8_t Vect[NUM_LEDS];
unsigned long ResetTimeStamp;

void setup() {
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	FastLED.addLeds<CHIPSET,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
	//FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS,MAX_AMPS);
	FastLED.setBrightness(BRIGHTNESS);
	//FastLED.clear();
	//FastLED.show(); 

	EEAR = 0x00;
	//asm volatile ("sbi EECR,EERE");		//Start read.
	asm volatile ("sbi 0x1C,0");			//Write enable on;
	Status = EEDR;


}

void loop() {
	for(i = 0; i < FrameDelay; i++)	{
		if(digitalRead(BUTTON_PIN) == 0) {
			delay(30);											//Debounce.
			if(digitalRead(BUTTON_PIN) == 0)	{
				Status += 1;
				iFrame = 0;
				xFrame = 0;
				CountDir = CountUp;
				FastLED.setBrightness(BRIGHTNESS);
				ResetStatus = 1;
				WriteStatusRom = 1;
				ResetTimeStamp = millis();
				while(digitalRead(BUTTON_PIN) == 0)	{}		//Wait button release.
				break;
			}
		}
		delay(5);
	}
	if(WriteStatusRom == 1)	{
		if(millis()-ResetTimeStamp >= 30000)	{
			//Write status to rom
			EEAR = 0x00;
			EEDR = Status;
			//asm volatile ("sbi EECR,EEMWE");			//Write enable on;
			asm volatile ("sbi 0x1C,2");			//Write enable on;
			//asm volatile ("sbi EECR,EEWE");			//Start write;
			asm volatile ("sbi 0x1C,1");			//Start write;
			WriteStatusRom = 0;	
		}
	}


	//////////////////// STATUS 0, 1, 2, 3 ///////////////////////
	if((Status == 0x00) || (Status == 0x01) || (Status == 0x02) || (Status == 0x03))	{
		if		(Status == 0)	{ColorLeds(CRGB::Red);}
		else if	(Status == 1)	{ColorLeds(CRGB::Green);}
		else if	(Status == 2)	{ColorLeds(CRGB::Blue);}
		else 					{ColorLeds(CRGB::Purple);}
		FastLED.setBrightness(iFrame);
		if		(iFrame == BRIGHTNESS)		{ CountDir = CountDown;}
		else if	(iFrame == 0)             	{ CountDir = CountUp;}
		if		(CountDir == CountUp)		{ iFrame ++;}
		else								{ iFrame --;}
		FrameDelay = 5;
	}
	//////////////////// STATUS 4 ///////////////////////
	else if(Status == 4)	{
		ColorLeds(CRGB::Red);
		FrameDelay = 255;
	}
	//////////////////// STATUS 5 ///////////////////////
	else if(Status == 5)	{
		ColorLeds(CRGB::Green);
		FrameDelay = 255;
	}
	//////////////////// STATUS 6 ///////////////////////
	else if(Status == 6)	{
		ColorLeds(CRGB::Blue);
		FrameDelay = 255;
	}
	//////////////////// STATUS 7 ///////////////////////
	else if(Status == 7)	{
		ColorLeds(CRGB::Purple);
		FrameDelay = 255;
	}
	//////////////////// STATUS 8 ///////////////////////
  	else if(Status == 8)  {
		fill_rainbow_circular(leds,NUM_LEDS,iFrame);
		//FastLED.setBrightness(BRIGHTNESS);
		iFrame += 10;
		FrameDelay = 5;
	}
  	//////////////////// STATUS 9 ///////////////////////
  	else if(Status == 9)  {
		ColorLeds(CRGB::Blue);
		for (iLed = iFrame; iLed < (iFrame + 5); iLed++) {
			if(iLed < NUM_LEDS) {leds[iLed] = CRGB::Yellow;}
			else                {leds[iLed - NUM_LEDS] = CRGB::Yellow;}
		}
		if  (iFrame == NUM_LEDS)        { iFrame = 0;}
		else                            { iFrame++; }
		FrameDelay = 15;
  	}	
  	//////////////////// STATUS 10 ///////////////////////
  	else if(Status == 10)  {
    	if(iFrame == 0) {
			ColorLeds(CRGB::Black);
		}
		leds[random(NUM_LEDS)] = CRGB(0, 0, random(255)); 
		FrameDelay = 10;
  	}
  	//////////////////// STATUS 11 ///////////////////////
  	else if(Status == 11)  {
    	if(iFrame == 0) {
			ColorLeds(CRGB::Black);
		}
		iFrame = 1;
		leds[random(NUM_LEDS)] = CRGB(0, 0, random(255)); 
		FrameDelay = 10;
  	}
	//////////////////// STATUS 12 ///////////////////////
	else if(Status == 12)  {
		for (iLed = 0; iLed<NUM_LEDS; iLed++) { leds[iLed] = CRGB::Black; }
		for (iLed = iFrame; iLed < (iFrame + 2); iLed++) { leds[iLed] = CRGB::Blue;}
		for (iLed = NUM_LEDS - 1 - iFrame; iLed > (NUM_LEDS - 3 - iFrame); iLed--) { leds[iLed] = CRGB::Blue;}
		if		(iFrame + 2 == NUM_LEDS/2)	{CountDir = CountDown;}
		else if	(iFrame == 0)				{CountDir = CountUp;}
		if		(CountDir == CountUp)		{ iFrame ++;}
		else								{ iFrame --;}
		FrameDelay = 20;
  	}
	//////////////////// STATUS 13 ///////////////////////
	else if(Status == 13)  {
		if(ResetStatus == 1) {	
			ColorLeds(CRGB::Blue);
			xFrame = NUM_LEDS;
			iFrame = 0;
			CountDir = CountUp;
			ResetStatus = 0;
		}
		if(xFrame == 255)	{
			for (iLed=0; iLed < NUM_LEDS; iLed++) 	{ leds[iLed] = CRGB::Yellow;}
			CountDir = CountDown;
			xFrame = 0;
			iFrame = NUM_LEDS - 1;
		}
		else if(xFrame == NUM_LEDS)	{
			for (iLed=0; iLed < NUM_LEDS; iLed++) 	{ leds[iLed] = CRGB::Blue;}
			CountDir = CountUp;
			xFrame = NUM_LEDS - 1;
			iFrame = 0;
		}	
		else if(iFrame == xFrame)	{
			for (iLed=0; iLed < iFrame; iLed++) 	{ leds[iLed] = CRGB::Blue;}
			for (; iLed < NUM_LEDS; iLed++) 		{ leds[iLed] = CRGB::Yellow;}	
			if(CountDir == CountUp)	{
				xFrame = xFrame - 1;
				iFrame = 0;
			}
			else{
				leds[iFrame] = CRGB::Blue;
				xFrame = xFrame + 1;
				iFrame = NUM_LEDS - 1;
			}
		}
		else if(CountDir == CountUp)	{
			for (iLed=0; iLed < iFrame; iLed++) { leds[iLed] = CRGB::Blue;	}
			leds[iFrame] = CRGB::Yellow;	
			iFrame++;
		}
		else	{
			for (iLed=NUM_LEDS - 1; iLed > iFrame; iLed--) { leds[iLed] = CRGB::Yellow; }
			leds[iFrame] = CRGB::Blue;
			iFrame--;
		}
		FrameDelay = 5;
  	}	
  	//////////////////// STATUS 14 ///////////////////////
  	else if(Status == 14)  {
		for (iLed=0; iLed<NUM_LEDS; iLed++) {
			if(iLed + iFrame >= NUM_LEDS)	{ leds[iLed] = CRGB(0, 0, Tri[iLed + iFrame - NUM_LEDS]);}
			else									{ leds[iLed] = CRGB(0, 0, Tri[iLed + iFrame]);} 
		}
		if  (iFrame == NUM_LEDS - 1)    { iFrame = 0;}
		else                            { iFrame++; }
		FrameDelay = 15;
  	}
  	//////////////////// STATUS 15 ///////////////////////
  	else if(Status == 15)  {
		if(ResetStatus)	{
			RampInit();
			ResetStatus = 0;
		}
		ShiftLed(BlueDD);
		if  (iFrame == NUM_LEDS - 1)    { iFrame = 0;}
		else                            { iFrame++; }
		FrameDelay = 15;
  	}
  	//////////////////// STATUS 16 ///////////////////////
  	else if(Status == 16)  {
		if(ResetStatus)	{
			RampInit();
			ResetStatus = 0;
		}
		if(xFrame == 0)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) { leds[iLed] = CRGB(0,255,0); }
			xFrame++;
			iFrame = 0;
		}
		//Fade Blue
		else if(xFrame ==1)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(0,255,0);
			}		
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(0, 255, Vect[iFrame - iLed]);
			}			
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}
		//Fill Blue
		else if(xFrame == 2)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(0, 255, Vect[NUM_LEDS - 1 - iLed + iFrame]);
			}
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(0, 255, 255);
			}
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}
		//Fade Green out
		else if(xFrame == 3)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(0, 255, 255);
			}
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(0, Vect[NUM_LEDS - 1 - iFrame + iLed], 255);
			}
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}
		//Turn off green
		else if(xFrame == 4)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(0, Vect[iLed - iFrame], 255);
			}
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(0, 0, 255);
			}
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}	
		//Fade red
		else if(xFrame == 5)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(0,0,255);
			}		
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(Vect[iFrame - iLed], 0, 255);
			}			
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}
		//Fill red
		else if(xFrame == 6)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(Vect[NUM_LEDS - 1 - iLed + iFrame], 0, 255);
			}
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(255, 0, 255);
			}
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}
		//Fade blue out		
		else if(xFrame == 7)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(255, 0, 255);
			}
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(255, 0, Vect[NUM_LEDS - 1 - iFrame + iLed]);
			}
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}
		//Turn off blue
		else if(xFrame == 8)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(255, 0, Vect[iLed - iFrame]);
			}
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(255, 0, 0);
			}
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}		
		//Fade in Green
		else if(xFrame == 9)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(255,0,0);
			}		
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(255,Vect[iFrame - iLed], 0);
			}			
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}			
		}		
		//Fill green
		else if(xFrame == 10)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(255,Vect[NUM_LEDS - 1 - iLed + iFrame], 0);
			}
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(255, 255, 0);
			}
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}
		//Fade red out		
		else if(xFrame == 11)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(255, 255, 0);
			}
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(Vect[NUM_LEDS - 1 - iFrame + iLed], 255, 0);
			}
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame++;}
			else						{iFrame++;}		
		}
		//Turn off red
		else if(xFrame == 12)	{
			for (iLed = NUM_LEDS-1; iLed > iFrame; iLed--) {
				leds[iLed] = CRGB(Vect[iLed - iFrame], 255, 0);
			}
			for (;iLed != 255; iLed--) {
				leds[iLed] = CRGB(0, 255, 0);
			}
			if(iFrame == NUM_LEDS-1)	{iFrame = 0; xFrame = 1;}
			else						{iFrame++;}		
		}		
	
		FrameDelay = 15;
  	}

  	//////////////////// STATUS 17 ///////////////////////
  	else if(Status == 17)  {
		for (iLed=0; 			iLed<NUM_LEDS; 		iLed++)	{ Vect[iLed] = 0; }
		for (iLed=0; 			iLed<NUM_LEDS/4; 	iLed++) { Vect[iLed] = 255; }
		for (iLed=NUM_LEDS/2; 	iLed<NUM_LEDS*3/4;	iLed++) { Vect[iLed] = 255; }
		ShiftLed(GreenDD);
		if(iFrame) {
			iFrame = 0;
		}
		else{
			iFrame += NUM_LEDS/4;
		}
		FrameDelay = 80;
  	}
  	//////////////////// STATUS 18 ///////////////////////
  	else if(Status == 18)  {
		for (iLed=0; iLed<NUM_LEDS; iLed++) { Vect[iLed] = 0; }
		Vect[0] = 255;
		Vect[1] = 255;
		Vect[2] = 255;
		Vect[3] = 255;
		Vect[NUM_LEDS/2] = 255;
		Vect[NUM_LEDS/2 + 1] = 255;
		Vect[NUM_LEDS/2 + 2] = 255;
		Vect[NUM_LEDS/2 + 3] = 255;
		ShiftLed(BlueDD);
		if  (iFrame == NUM_LEDS - 1)    { iFrame = 0;}
		else                            { iFrame++; }
		FrameDelay = 15;
  	}
  	//////////////////// STATUS 19 ///////////////////////
  	else if(Status == 19)  {
		for (iLed=0; iLed<NUM_LEDS; iLed++) { Vect[iLed] = 0; }
		Vect[NUM_LEDS - 1] = 255;
		Vect[NUM_LEDS - 2] = 255;
		Vect[NUM_LEDS - 3] = 255;
		Vect[NUM_LEDS - 4] = 255;
		ShiftLed(GreenDD);
		if		(iFrame == NUM_LEDS - 4)	{ CountDir = CountDown;}
		else if	(iFrame == 0)             	{ CountDir = CountUp;}
		if		(CountDir == CountUp)		{ iFrame ++;}
		else								{ iFrame --;}
		FrameDelay = 8;
  	}
  	//////////////////// STATUS 20 ///////////////////////
  	else if(Status == 20)  {
		for (iLed=0; iLed<NUM_LEDS; iLed++) { Vect[iLed] = 0; }
		Vect[NUM_LEDS - 1] = 255;
		Vect[NUM_LEDS - 2] = 255;
		Vect[NUM_LEDS - 3] = 255;
		Vect[NUM_LEDS - 4] = 255;
		ShiftLed(RedDD);
		if  (iFrame == NUM_LEDS - 1)    { 
			iFrame = 0;
			if		(xFrame == 8)			{ CountDir = CountDown;}
			else if	(xFrame == 1)       	{ CountDir = CountUp;}
			if		(CountDir == CountUp)	{ xFrame++;}
			else							{ xFrame--;}
		}
		else                            { 
			iFrame++; 
		}
		FrameDelay = xFrame;
  	}
	//////////////////// STATUS OTHERS ///////////////////////
	else	{
		Status = 0;
	}
	FastLED.show();
		

}

void ShiftLed(uint8_t ColorSwitch)	{
	CRGB TempColor;
	uint8_t TempIndex;
	for (iLed=0; iLed<NUM_LEDS; iLed++) {
		TempIndex = iLed + iFrame;
		while(TempIndex >= NUM_LEDS)	{ 
			TempIndex = TempIndex - NUM_LEDS;
		}
		if		(ColorSwitch == RedDD)		{TempColor = CRGB(Vect[TempIndex], 0, 0);}
		else if	(ColorSwitch == GreenDD)	{TempColor = CRGB(0, Vect[TempIndex], 0);}
		else if	(ColorSwitch == BlueDD)		{TempColor = CRGB(0, 0, Vect[TempIndex]);}
		leds[iLed] = TempColor;
	}
}

void ColorLeds(CRGB color)	{
	for (iLed=0; iLed<NUM_LEDS; iLed++) {
		leds[iLed] = color; 
	}
}

void RampInit()	{
	for(Vect[0] = 0, i=1; i < NUM_LEDS; i++)	{ Vect[i] = Vect[i-1]+255/NUM_LEDS;};
}

/*
void TriInit()		{
    Vect[0] = 255/NUM_LEDS*2; 
    for(i=1; i<NUM_LEDS; i++)    {
		if(i < NUM_LEDS/2)	{
	    	Vect[i] = Vect[i-1] + 255/NUM_LEDS*2;
		}
		else	{
		    Vect[i] = Vect[i-1] - 255/NUM_LEDS*2;
		}
    }
    Vect[NUM_LEDS -1] = Vect[NUM_LEDS -2];
}
*/