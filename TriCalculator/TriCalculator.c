#include <stdio.h>
#include <stdint.h>


#define NUM_LEDS 28
uint8_t Vect[NUM_LEDS];
uint8_t i;


int main()
{
    Vect[0] = 255/NUM_LEDS*2; 
    for(i=1; i<NUM_LEDS/2; i++)    {
        Vect[i] = Vect[i-1] + 255/NUM_LEDS*2;
    }
    for(;i<NUM_LEDS;i++)    {
        Vect[i] = Vect[i-1] - 255/NUM_LEDS*2;
    }

    Vect[NUM_LEDS -1] = Vect[NUM_LEDS -2];

    for(i=0; i<NUM_LEDS; i++)    {
        printf("%d,", Vect[i]);
    }

    return 0;
}
