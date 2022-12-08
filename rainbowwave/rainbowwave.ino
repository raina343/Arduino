#include <FastLED.h>

#define NUM_LEDS  172
#define LED_PIN   6

CRGB leds[NUM_LEDS];

//has to be uint8_t so it starts at 0 after it reached 256
uint8_t hue = 0;

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(50);
}

void loop() {
  
  for (int i = 0; i < NUM_LEDS; ++i) {
    leds[i] = CHSV(hue + (i * 10), 255, 255);
  }

  //You can change the pattern speed here
  EVERY_N_MILLISECONDS(15){
    hue++;
  }
  
  FastLED.show();
}
