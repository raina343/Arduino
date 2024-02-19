
#include <Adafruit_NeoPixel.h>
#define LED_PIN 4
#define LED_COUNT 18
int pixels[18];
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
  char PSTHour[3];
  char PSTMin[3];
  char PSTSec[3];


uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void setup() {
   Serial.begin(9600);
   Serial.println("initializing");
    strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.clear();
// PSTMin=22;
// PSTHour="18";
  String PSTMins = String(PSTMin);
  String PSTSeconds = String(PSTSec);
  String PSTMinute1 = String(PSTMin).substring(0, 1);
  String PSTMinute2 = String(PSTMin).substring(1);
  String PSTHour1 = String(PSTHour).substring(0, 1);
  String PSTHour2 = String(PSTHour).substring(1);


}
void outputDigits(int hr, int min, int sec) {

  Serial.println("here");
  int Row1 [6] = {
    12,13,14,15,16,17
  };
  int Row2 [6] = {
    11, 10, 9, 8,7,6
  };
  int Row3 [6] = {
      0, 1, 2, 3,4,5
  };

  byte someValue = hr; //For this example, lets convert the number 20
  char binary[9] = {0}; //This is where the binary representation will be stored
  someValue += 128; //Adding 128 so that there will always be 8 digits in the string
  itoa(someValue, binary, 2); //Conver someValue to a string using base 2 and save it in the array named binary
  char* string = binary + 1; //get rid of the most significant digit as you only want 7 bits


  byte someValue2 = min; //For this example, lets convert the number 20
  char binary2[9] = {0}; //This is where the binary representation will be stored
  someValue2 += 128; //Adding 128 so that there will always be 8 digits in the string
  itoa(someValue2, binary2, 2); //Conver someValue to a string using base 2 and save it in the array named binary
  char* string2 = binary2 + 1; //get rid of the most significant digit as you only want 7 bits

  byte someValue3 = sec; //For this example, lets convert the number 20
  char binary3[9] = {0}; //This is where the binary representation will be stored
  someValue3 += 128; //Adding 128 so that there will always be 8 digits in the string
  itoa(someValue3, binary3, 2); //Conver someValue to a string using base 2 and save it in the array named binary
  char* string3 = binary3 + 1; //get rid of the most significant digit as you only want 7 bits
  // Serial.println(String(string3));
  for (int j = 0; j < 6; j++) {  // first digit.  a 3
    if (String(string).substring(1).charAt(j) == '1') {
      pixels[Row1[j]] = -1;
    } else {
      pixels[Row1[j]] = j;
    }
  }
  for (int j = 0; j < 6; j++) {  // first digit.  a 3
    if (String(string2).substring(1).charAt(j) == '1') {
      pixels[Row2[j]] = -1;
    } else {
      pixels[Row2[j]] = j;
    }
  }
  for (int j = 0; j < 6; j++) {  // first digit.  a 3
    if (String(string3).substring(1).charAt(j) == '1') {
      pixels[Row3[j]] = -1;
    } else {
      pixels[Row3[j]] = j;
    }
  }
}
void loop() {
  // put your main code here, to run repeatedly:
 outputDigits(0,0,23);
uint16_t i, j;

    for (j = 0; j < 256; j++) {
      for (i = 0; i < strip.numPixels(); i++) {
        if (pixels[i] == -1) {
          strip.setPixelColor(i, Wheel((i * 14 + j) & 255));
        } else {
          strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
      }
      strip.show();
      delay(15);
    }
}
