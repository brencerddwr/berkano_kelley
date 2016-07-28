//cycles entire set through the rainbow

#include <FastLED.h>
// Variable definitions start here

// How many leds are in the strip?
#define NUM_LEDS 12

// Data pin that led data will be written out over
#define DATA_PIN 11

// This is an array of leds.  One item for each led in your strip.
CRGB leds[NUM_LEDS];

int F16pos = 0;
byte F16delta = 1;
byte Width = 3; // width of line
unsigned int InterframeDelay = 30; //ms

// starting hue
uint16_t Fhue16 = 64;

// logical order arrays

// start of logical array one
int logical_array_one_start = 2; //the range of this variable is 0 to NUM_LEDS-1

//int logical_array_one[NUM_LEDS/2] = {2,3,4,5,6,7};
int logical_array_one[NUM_LEDS/2];
// int logical_array_two[NUM_LEDS/2] = {1,0,11,10,9,8};
int logical_array_two[NUM_LEDS/2];

// ***********************************************************************************************************
// *
// *                            Power Up Init.
// *
// *
// ***********************************************************************************************************
void setup() {

	// sanity check delay - allows reprogramming if accidently blowing power w/leds
	delay(3000);

	// Change this line as needed to match the LED string type, control chip and color sequence
	FastLED.addLeds<TM1803, DATA_PIN, GBR>(leds, NUM_LEDS); // RadioShack LED String

	// turn off all leds
	FastLED.setBrightness(96);
	fill_solid ( &(leds[0]), NUM_LEDS, CRGB::Black);
	FastLED.show();
	delay (1000);
	build_logical_arrays(logical_array_one_start);

	
}

// ***********************************************************************************************************
// *
// *                            Main Loop
// *
// *
// ***********************************************************************************************************
void loop()
{
	// Update the "Fraction Bar" by 1/16th pixel every time
	F16pos += F16delta;
	
	// wrap around at end
	// remember that F16pos contains position in "16ths of a pixel"
	// so the 'end of the strip' is ((NUM_LEDS/2) * 16)
	if( F16pos >= ((NUM_LEDS/2) * 16)) {
		F16pos -= ((NUM_LEDS/2) * 16);
	}
	
	
	// Draw everything:
	// clear the pixel buffer
	memset8( leds, 0, NUM_LEDS * sizeof(CRGB));
	
	// advance to the next hue
	Fhue16 = Fhue16 + 19;
	
	// draw the Fractional Bar, length=4px
	drawFractionalBar( F16pos, Width, Fhue16 / 256);
	
	FastLED.show();
	#if defined(FASTLED_VERSION) && (FASTLED_VERSION >= 2001000)
	FastLED.delay(InterframeDelay);
	#else
	delay(InterframeDelay);
	#endif
}


void drawFractionalBar( int pos16, int width, uint8_t hue)
{
	int i = pos16 / 16; // convert from pos to raw pixel number
	uint8_t frac = pos16 & 0x0F; // extract the 'factional' part of the position
	
	// brightness of the first pixel in the bar is 1.0 - (fractional part of position)
	// e.g., if the light bar starts drawing at pixel "57.9", then
	// pixel #57 should only be lit at 10% brightness, because only 1/10th of it
	// is "in" the light bar:
	//
	//                       57.9 . . . . . . . . . . . . . . . . . 61.9
	//                        v                                      v
	//  ---+---56----+---57----+---58----+---59----+---60----+---61----+---62---->
	//     |         |        X|XXXXXXXXX|XXXXXXXXX|XXXXXXXXX|XXXXXXXX |
	//  ---+---------+---------+---------+---------+---------+---------+--------->
	//                   10%       100%      100%      100%      90%
	//
	// the fraction we get is in 16ths and needs to be converted to 256ths,
	// so we multiply by 16.  We subtract from 255 because we want a high
	// fraction (e.g. 0.9) to turn into a low brightness (e.g. 0.1)
	uint8_t firstpixelbrightness = 255 - (frac * 16);
	
	// if the bar is of integer length, the last pixel's brightness is the
	// reverse of the first pixel's; see illustration above.
	uint8_t lastpixelbrightness  = 255 - firstpixelbrightness;
	
	// For a bar of width "N", the code has to consider "N+1" pixel positions,
	// which is why the "<= width" below instead of "< width".
	
	uint8_t bright;
	for( int n = 0; n <= width; n++) {
		if( n == 0) {
			// first pixel in the bar
			bright = firstpixelbrightness;
			} else if( n == width ) {
			// last pixel in the bar
			bright = lastpixelbrightness;
			} else {
			// middle pixels
			bright = 255;
		}
		
		leds[logical_array_one[i]] += CHSV( hue, 255, bright);
		leds[logical_array_two[i]] += CHSV( hue, 255, bright);
		//    leds[i] += CHSV( hue, 255, bright);
		i++;
		if( i == NUM_LEDS/2) i = 0; // wrap around
	}
}

void build_logical_arrays(int _startPosition){
	// build logical arrays
	int logical_array_one_position = _startPosition;
	int logical_array_two_position = _startPosition-1;
	for (int a = 0;a <= (NUM_LEDS/2); a++){
		if (logical_array_one_position > NUM_LEDS-1)
		{logical_array_one_position=logical_array_one_position-NUM_LEDS;
		}
		if (logical_array_two_position < 0)
		{logical_array_two_position=logical_array_two_position+NUM_LEDS;
		}
		logical_array_one[a]=logical_array_one_position;
		logical_array_two[a]=logical_array_two_position;
		logical_array_one_position++;
		logical_array_two_position--;
	}
	logical_array_one[0]=_startPosition; //without this line, position 0 in array 1 does not display anything.
	//there is some sort of a problem in the for loop above that must be writing bad data to logical_array_one[0].

}
