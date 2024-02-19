// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// Released under the GPLv3 license to match the rest of the
// Adafruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
int pixels[16];
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        4 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 16 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel strip(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels
   char PSTHour[3]="15";
  char PSTMin[3]="53";
  char PSTSec[3]="22";
void setup() {
   Serial.begin(9600);
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving it):
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)

  Serial.println('-----------------');
//      sprintf(PSTHour,"15");
//    sprintf(PSTMin, "%.2d", "36");
//    sprintf(PSTSec, "%.2d", "22");
//  Serial.println((String)PSTHour);
  String PSTMins = String(PSTMin);
  String PSTSeconds = String(PSTSec);
  String PSTMinute1 = String(PSTMin).substring(0, 1);
  String PSTMinute2 = String(PSTMin).substring(1);
  String PSTHour1 = String(PSTHour).substring(0, 1);
  String PSTHour2 = String(PSTHour).substring(1);
  Serial.println((String)PSTHour1);
  Serial.println((String)PSTHour2);
   outputDigits(PSTMinute2.toInt(), PSTMinute1.toInt(), PSTHour2.toInt(), PSTHour1.toInt());

}
 
  // now that the time's been split up, we can output it all to the digits of the clock.
  // I'm doing each digit separately.  again as with before, there's probably way better ways of doing this.


 


void outputDigits(int digit1, int digit2, int digit3, int digit4) {
//this whole thing is going to be completely different on the binary clock compared to others.
//we're literally only lighting up at most 4leds per digit.
//so the last column for minutes would be  3, 4,11 and 12
// second from the left column for tens of minutes would be 2,5,10 and 13
//third column for hours would be 1,6,9,14
//and 4th column for tens of hours would be 0,7,8,15



//while (digit1>0){
//  
//}
  
//  int nums[][14] = {
//    // 0   1   2   3   4   5   6   7   8   9  10  11  12  13
////    {  3,4,11,12},
////    { -1,4,11,12},
////    { -1,-1,11,12},
//    {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13}, // 0 //seems ok
//    {  0,  1,  2, -1,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13},        // 1 //seems ok
//    {  0,  1,  2, 3, -1,  5,  6,  7,  8,  9, 10, 11, 12, 13},     // 2 //seems ok
//     {  0,  1,  2, -1, -1,  5,  6,  7,  8,  9, 10, 11, 12, 13},    // 3
////    {  0, 1, -1, -1, -1, -1, 6, 7, -1, -1, -1, -1, 12, 13},      // 4
////    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, 12, 13},   // 5////numbers represent the pixels that are NOT LIT
////    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1},   // 6
////    {0, 1, -1, -1, -1, -1, -1, -1, 8, 9, 10, 11, 12, 13},      // 7 //seems ok
////    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, // 8 //seems ok
////    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13}, // 9 //seems ok
////    { -1, -1, 2, 3, 4, 5, -1, -1, -1, -1, 10, 11, -1, -1},     // C (for Temp)
////    {0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1},        // F (for Temp)
////    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},            // all off
//  };

//int numsb[][4] = {
//       {  2,5,10,13},
//    { -1,5,10,13},
//    { -1,-1,10,13},
//  };

int Row1 [4]={
//  3, 4,11 ,12
  12,11,4,3
};
int Row2 [4]={
//  2,5,10 , 13
  13,10,5,2
};
int Row3 [4]={
//  1,6,9,14
  14,9,6,1
};
int Row4 [4]={
  //0,7,8,15
  15,8,7,0
};
//pixels[13] = -1;

  byte someValue = digit1; //For this example, lets convert the number 20
  
  
  char binary[9] = {0}; //This is where the binary representation will be stored
  someValue += 128; //Adding 128 so that there will always be 8 digits in the string
  itoa(someValue,binary,2); //Conver someValue to a string using base 2 and save it in the array named binary
  char* string = binary + 1; //get rid of the most significant digit as you only want 7 bits
  byte someValue2 = digit2; //For this example, lets convert the number 20
  
  
  char binary2[9] = {0}; //This is where the binary representation will be stored
  someValue2 += 128; //Adding 128 so that there will always be 8 digits in the string
  itoa(someValue2,binary2,2); //Conver someValue to a string using base 2 and save it in the array named binary
  char* string2 = binary2 + 1; //get rid of the most significant digit as you only want 7 bits

 byte someValue3 = digit3; //For this example, lets convert the number 20
  
  
  char binary3[9] = {0}; //This is where the binary representation will be stored
  someValue3 += 128; //Adding 128 so that there will always be 8 digits in the string
  itoa(someValue3,binary3,2); //Conver someValue to a string using base 2 and save it in the array named binary
  char* string3 = binary3 + 1; //get rid of the most significant digit as you only want 7 bits

   byte someValue4 = digit4; //For this example, lets convert the number 20
  
  
  char binary4[9] = {0}; //This is where the binary representation will be stored
  someValue4 += 128; //Adding 128 so that there will always be 8 digits in the string
  itoa(someValue4,binary4,2); //Conver someValue to a string using base 2 and save it in the array named binary
  char* string4 = binary4 + 1; //get rid of the most significant digit as you only want 7 bits
  
  for (int j = 0; j < 4; j++) {  // first digit.  a 3

  if (String(string).substring(3,7).charAt(j)=='1'){
    pixels[Row1[j]] = -1;
  }else{
    pixels[Row1[j]] = j;
  }

  }
  for (int j = 0; j < 4; j++) {  // first digit.  a 3
    if (String(string2).substring(3,7).charAt(j)=='1'){
       pixels[Row2[j]] = -1;
    }else{
    pixels[Row2[j]] = j;
    }

  }
    for (int j = 0; j < 4; j++) {  // first digit.  a 3
      if (String(string3).substring(3,7).charAt(j)=='1'){
        pixels[Row3[j]] = -1;
      }else{
    pixels[Row3[j]] = j;
      }

  }
    for (int j = 0; j < 4; j++) {  // first digit.  a 3
       if (String(string4).substring(3,7).charAt(j)=='1'){
        pixels[Row4[j]] =-1;
       }else{
    pixels[Row4[j]] = j;
       }

  }
//  for (int j = 0; j < 14; j++) {  // first digit.  a 3
//
//  }
//  for (int j = 14; j < 28; j++) {  // second digit... a 3
//    if (nums[digit2][j - 14] == -1) {
//      pixels[j] = -1;
//    } else {
//      pixels[j] = nums[digit2][j - 14] + 14;
//    }
//  }
//  pixels[28] = -1;  // dots
//  pixels[29] = -1;  // dots
//
//  for (int j = 30; j < 44; j++) {  // third digit.. a 7
//    if (nums[digit3][j - 30] == -1) {
//      pixels[j] = -1;
//    } else {
//      pixels[j] = nums[digit3][j - 30] + 30;
//    }
//  }
//  for (int j = 44; j < 58; j++) {  // fourth digit... a 1
//    if (nums[digit4][j - 44] == -1) {
//      pixels[j] = -1;
//    } else {
//      pixels[j] = nums[digit4][j - 44] + 44;
//    }
//  }
}
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
void loop() {
    Serial.println("--------");
      String PSTMins = String(PSTMin);
  String PSTSeconds = String(PSTSec);
  String PSTMinute1 = String(PSTMin).substring(0, 1);
  String PSTMinute2 = String(PSTMin).substring(1);
  String PSTHour1 = String(PSTHour).substring(0, 1);
  String PSTHour2 = String(PSTHour).substring(1);
      Serial.println((String)PSTHour1);
  Serial.println((String)PSTHour2);
        Serial.println((String)PSTMinute1);
  Serial.println((String)PSTMinute2);

    byte someValue = 3; //For this example, lets convert the number 20
  
  
  char binary[9] = {0}; //This is where the binary representation will be stored
  someValue += 128; //Adding 128 so that there will always be 8 digits in the string
  itoa(someValue,binary,2); //Conver someValue to a string using base 2 and save it in the array named binary
  char* string = binary + 1; //get rid of the most significant digit as you only want 7 bits


  Serial.println(String(string).substring(3,7).charAt(1));
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
//  strip.clear(); // Set all pixel colors to 'off'
//
//  // The first NeoPixel in a strand is #0, second is 1, all the way up
//  // to the count of pixels minus one.
//  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...
//
//    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
//    // Here we're using a moderately bright green color:
//    strip.setPixelColor(i, pixels.Color(0, 150, 0));
//
//    strip.show();   // Send the updated pixel colors to the hardware.
//
//    delay(DELAYVAL); // Pause before next pass through loop
//  }
}
