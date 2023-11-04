#include <FastLED.h> 
#define NUM_LEDS 10
#define DATA_PIN 11 			//SPI MOSI
#define COLOR_ORDER GRB 	//Green (G), Red (R), Blue (B)
#define CHIPSET WS2812B
#define BRIGHTNESS 60 
#define VOLTS 5 
#define MAX_AMPS 500 		//value in milliamps

CRGB leds[NUM_LEDS];

void setup() {

FastLED.addLeds<CHIPSET,DATA_PIN,COLOR_ORDER>(leds,NUM_LEDS);
//FastLED.setMaxPowerInVoltsAndMilliamps(VOLTS,MAX_AMPS);
//FastLED.setBrightness(BRIGHTNESS);
//FastLED.clear();
//FastLED.show(); 

}

void loop() { //Breathing effect!
  // Turn the LED on, then pause
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);
  // Now turn the LED off, then pause
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
}
