#include <FastLED.h> 
#define NUM_LEDS 		16
#define DATA_PIN 		11 			//SPI MOSI
#define COLOR_ORDER 	GRB 	//Green (G), Red (R), Blue (B)
#define CHIPSET 		WS2812B
#define BRIGHTNESS 		90 
#define VOLTS 			5 
#define MAX_AMPS 		500 		//value in milliamps
// #define BUTTON_PIN 		14 		
#define BUTTON_PIN 		5 		
#define CountUp 		0 		
#define CountDown 		1 		
#define RedDD 			0	 
#define GreenDD 		1
#define BlueDD 			2
#define PurpleBlueDD 	3
#define GreenYellowDD 	4

CRGB leds[NUM_LEDS];

uint8_t 	Status;
uint8_t 	iFrame;
uint8_t 	xFrame;
uint8_t 	CountDir;
uint8_t 	WriteStatusRom = 0;
uint8_t 	FrameDelay;
uint8_t 	Vect[NUM_LEDS];
uint8_t* 	LedPointer;
unsigned long ResetTimeStamp;


#if 	NUM_LEDS == 16
	uint8_t 	Tri[NUM_LEDS] = {30,60,90,120,150,180,210,240,210,180,150,120,90,60,30,20};
#elif 	NUM_LEDS == 100
	uint8_t 	Tri[NUM_LEDS] = {4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,124,128,132,136,140,144,148,152,156,160,164,168,172,176,180,184,188,192,196,200,196,192,188,184,180,176,172,168,164,160,156,152,148,144,140,136,132,128,124,120,116,112,108,104,100,96,92,88,84,80,76,72,68,64,60,56,52,48,44,40,36,32,28,24,20,16,12,8,4,4};
#elif 	NUM_LEDS == 28
	uint8_t 	Tri[NUM_LEDS] = {18,36,54,72,90,108,126,144,162,180,198,216,234,252,234,216,198,180,162,144,126,108,90,72,54,36,18,18};
#elif 	NUM_LEDS == 9
	uint8_t 	Tri[NUM_LEDS] = {50,100,150,200,250,200,150,100,50};
#endif


void setup() {
	pinMode(BUTTON_PIN, INPUT_PULLUP);

	FastLED.addLeds<CHIPSET,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
	//FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS,MAX_AMPS);
	FastLED.setBrightness(BRIGHTNESS);
	//FastLED.clear();
	//FastLED.show(); 

	EEAR = 0x00;
	//asm volatile ("sbi EECR,EERE");		//Start read.
	asm volatile ("sbi 0x1C,0");			//Read enable on;
	Status = EEDR;

	iFrame = 0;
	xFrame = 0;
	LedPointer = &Vect[0];
				

}

void loop() {
	uint8_t i;
	uint8_t iLed;
	// uint8_t ResetStatus = 0;


	for(i = 0; i < FrameDelay; i++)	{
		if(digitalRead(BUTTON_PIN) == 0) {
			delay(30);											//Debounce.
			if(digitalRead(BUTTON_PIN) == 0)	{
				Status += 1;
				iFrame = 0;
				xFrame = 0;
				CountDir = CountUp;
				LedPointer = &Vect[0];
				FastLED.setBrightness(BRIGHTNESS);
				// ResetStatus = 1;
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
		FrameDelay = 15;
	}
  	//////////////////// STATUS 9 ///////////////////////
  	else if(Status == 9)  {
		ColorLeds(CRGB::Blue);
		for (iLed = iFrame; iLed < (iFrame + 5); iLed++) {
			if(iLed < NUM_LEDS) {leds[iLed] = CRGB::Yellow;}
			else                {leds[iLed - NUM_LEDS] = CRGB::Yellow;}
		}
		if  (iFrame == NUM_LEDS - 1)        { iFrame = 0;}
		else                            	{ iFrame++; }
		FrameDelay = 15;
  	}	
  	//////////////////// STATUS 10 ///////////////////////
  	else if(Status == 10)  {
    	if(iFrame == 0) {
			ColorLeds(CRGB::Black);
		}
		leds[random(NUM_LEDS)] = CRGB(0, random(255), 0); 
		FrameDelay = 10;
  	}
  	//////////////////// STATUS 11 ///////////////////////
  	else if(Status == 11)  {
    	if(iFrame == 0) {
			ColorLeds(CRGB::Black);
		}
		iFrame = 1;
		leds[random(NUM_LEDS)] = CRGB(0, random(255), 0); 
		FrameDelay = 10;
  	}
	//////////////////// STATUS 12 ///////////////////////
	else if(Status == 12)  {
		for (iLed = 0; iLed<NUM_LEDS; iLed++) { leds[iLed] = CRGB::Black; }
		for (iLed = iFrame; iLed < (iFrame + 2); iLed++) { leds[iLed] = CRGB::Green;}
		for (iLed = NUM_LEDS - 1 - iFrame; iLed > (NUM_LEDS - 3 - iFrame); iLed--) { leds[iLed] = CRGB::Green;}
		if		(iFrame + 2 == NUM_LEDS/2)	{CountDir = CountDown;}
		else if	(iFrame == 0)				{CountDir = CountUp;}
		if		(CountDir == CountUp)		{ iFrame ++;}
		else								{ iFrame --;}
		FrameDelay = 20;
  	}
	//////////////////// STATUS 13 ///////////////////////
	else if(Status == 13)  {
		// if(ResetStatus == 1) {	
		// 	ColorLeds(CRGB::Blue);
		// 	xFrame = NUM_LEDS;
		// 	iFrame = 0;
		// 	CountDir = CountUp;
		// 	ResetStatus = 0;
		// }
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
		FrameDelay = 10;
  	}	
  	//////////////////// STATUS 14 ///////////////////////
  	else if(Status == 14)  {
		LedPointer = &Tri[0];
		ShiftLed(GreenDD);
		if  (iFrame == NUM_LEDS - 1)    { iFrame = 0;}
		else                            { iFrame++; }
		FrameDelay = 15;
  	}
  	//////////////////// STATUS 15 ///////////////////////
  	else if(Status == 15)  {
		// if(ResetStatus)	{
			RampInit();
			// ResetStatus = 0;
		// }
		ShiftLed(GreenDD);
		if  (iFrame == NUM_LEDS - 1)    { iFrame = 0;}
		else                            { iFrame++; }
		FrameDelay = 15;
  	}
  	//////////////////// STATUS 16 ///////////////////////
  	else if(Status == 16)  {
		// if(ResetStatus)	{
			RampInit();
			// ResetStatus = 0;
		// }
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
	
		FrameDelay = 25;
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
		ShiftLed(GreenDD);
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
		ShiftLed(GreenDD);
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
  	//////////////////// STATUS 21 ///////////////////////
  	else if(Status == 21)  {
		LedPointer = &Tri[0];
		ShiftLed(PurpleBlueDD);
		if  (iFrame == NUM_LEDS - 1)    { iFrame = 0;}
		else                            { iFrame++; }
		FrameDelay = 25;
  	}
  	//////////////////// STATUS 22 ///////////////////////
  	else if(Status == 22)  {
		LedPointer = &Tri[0];
		ShiftLed(GreenYellowDD);
		if  (iFrame == NUM_LEDS - 1)    { iFrame = 0;}
		else                            { iFrame++; }
		FrameDelay = 25;
  	}
	//////////////////// STATUS 23 ///////////////////////
  	else if(Status == 23)  {
		for(iLed = 0; iLed < NUM_LEDS - 6; iLed = iLed + 7)	{
			Vect[iLed + 0] = 32;
			Vect[iLed + 1] = 64;
			Vect[iLed + 2] = 96;
			Vect[iLed + 3] = 128;
			Vect[iLed + 4] = 160;
			Vect[iLed + 5] = 192;
			Vect[iLed + 6] = 224;
		} 
		ShiftLed(GreenDD);
		if  (iFrame == NUM_LEDS - 1)    { iFrame = 0;}
		else                            { iFrame++; }
		FrameDelay = 15;
  	}
	
	//////////////////// STATUS OTHERS ///////////////////////
	else	{
		Status = 0;
	}
	FastLED.show();
		

}

void ShiftLed(uint8_t ColorSwitch)	{
	uint8_t iLed;
	CRGB TempColor;
	uint8_t TempIndex;
	for (iLed=0; iLed<NUM_LEDS; iLed++) {
		TempIndex = iLed + iFrame;
		while(TempIndex >= NUM_LEDS)	{ 
			TempIndex = TempIndex - NUM_LEDS;
		}
		if		(ColorSwitch == RedDD)			{TempColor = CRGB(LedPointer[TempIndex], 0, 0);}
		else if	(ColorSwitch == GreenDD)		{TempColor = CRGB(0, LedPointer[TempIndex], 0);}
		else if	(ColorSwitch == BlueDD)			{TempColor = CRGB(0, 0, LedPointer[TempIndex]);}
		else if	(ColorSwitch == PurpleBlueDD)	{TempColor = CRGB(LedPointer[TempIndex], 0, 255);}
		else if	(ColorSwitch == GreenYellowDD)	{TempColor = CRGB(LedPointer[TempIndex], 255, 0);}
		leds[iLed] = TempColor;
	}
}

void ColorLeds(CRGB color)	{
	uint8_t iLed;
	for (iLed=0; iLed<NUM_LEDS; iLed++) {
		leds[iLed] = color; 
	}
}

void RampInit()	{
	uint8_t i;
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
