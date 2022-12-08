#include <Adafruit_NeoPixel.h>
#define LED_PIN 4
#define LED_COUNT 58
int pixels[58];
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//int Sized[] = {18, 6, 15, 15, 12, 15, 18, 9, 21, 18};
//int Sizedoff[] = {3, 15, 6, 6, 9, 6, 3, 12, 0, 3};
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP
  strip.clear();
  strip.setBrightness(150);
 strip.setPixelColor(6, strip.Color(255, 255, 0));
        strip.setPixelColor(7, strip.Color(255, 255, 0));
        strip.setPixelColor(20, strip.Color(255, 255, 0));
        strip.setPixelColor(21, strip.Color(255, 255, 0));
        strip.setPixelColor(36, strip.Color(255, 255, 0));
        strip.setPixelColor(37, strip.Color(255, 255, 0));
        strip.setPixelColor(50, strip.Color(255, 255, 0));
        strip.setPixelColor(51, strip.Color(255, 255, 0));

}

void loop() {

         
//  String TestTempUnits = "F";
//
//  //   int Temperature = rtc.getTemperature();
//  int Temperature = 45;
//
//  outputDigitsTemp(Temperature, TestTempUnits);
//  uint16_t i, j;
//
//  for (j = 0; j < 256; j++)
//  {
//    for (i = 0; i < strip.numPixels(); i++)
//    {
//      if (pixels[i] == -1)
//      {
//        strip.setPixelColor(i, Wheel((i * 14 + j) & 255));
//      }
//      else
//      {
//        strip.setPixelColor(i, strip.Color(0, 0, 0));
//      }
//    }
//    strip.show();
//    delay(15);
//  }
}
int numdigits(int i)
{
  char str[20];

  sprintf(str, "%d", i);
  return (strlen(str));
}
//uint32_t Wheel(byte WheelPos) {
//  if (WheelPos < 85)
//  {
//    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
//  }
//  else if (WheelPos < 170)
//  {
//    WheelPos -= 85;
//    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
//  }
//  else
//  {
//    WheelPos -= 170;
//    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
//  }
//}
//void outputDigitsTemp(int Temperature, String units) {
//  String temp1;
//  String temp2;
//  String temp3;
//  int digit2;
//  int digit3;
//  int digit1;
//  int Temp3;
//  String TempUnits;
//  bool ShowThirdDigit = false;
//  if (units == "F") {
//    Serial.println("CaveMan Temperature");
//    Serial.println (numdigits(Temperature));
//    Serial.println(Temperature);
//    int Temperature2 = (Temperature * 1.8) + 32;
//    temp1 = String(Temperature2).substring(0, 1);
//    temp2 = String(Temperature2).substring(1, 2);
//    if (numdigits(Temperature2) > 2) {
//      ShowThirdDigit = true;
//      temp3 = String(Temperature2).substring(2, 3);
//      digit3 = temp3.toInt();
//    } else {
//    }
//    digit1 = temp1.toInt();
//    digit2 = temp2.toInt();
//    TempUnits = "F";
//  } else {
//    temp1 = String(Temperature).substring(0, 1);
//    temp2 = String(Temperature).substring(1, 2);
//    if (numdigits(Temperature) > 2) {
//      ShowThirdDigit = true;
//      temp3 = String(Temperature).substring(2, 3);
//      digit3 = temp3.toInt();
//    } else {
//    }
//    digit1 = temp1.toInt();
//    digit2 = temp2.toInt();
//    TempUnits = "C";
//  }
//  int nums[][15] = {
//
//    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, -1, -1}, //0 //seems ok
//    {  0, 1, -1, -1, -1, -1, 6, 7, 8, 9, 10, 11, 12, 13},       //1 //seems ok
//    { -1, -1, 2, 3, -1, -1, -1, -1, 8, 9, -1, -1, -1, -1},    //2 //seems ok
//    { -1, -1, -1, -1, -1, -1, -1, -1, 8, 9, -1, -1, 12, 13},  //3
//    {  0, 1, -1, -1, -1, -1, 6, 7, -1, -1, -1, -1, 12, 13},     //4
//    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, 12, 13},  //5////numbers represent the pixels that are NOT LIT
//    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1},  //6
//    {  0, 1, -1, -1, -1, -1, -1, -1, 8, 9, 10, 11, 12, 13},     //7 //seems ok
//    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, //8 //seems ok
//    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13}, //9 //seems ok
//    { -1, -1, 2, 3, 4, 5, -1, -1, -1, -1, 10, 11, -1, -1}, //C (for Temp)
//    {  0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1}, // F (for Temp)
//    {  0, 1, 2, 3, 4, 5,  6, 7, 8, 9, -1, -1, 12, 13}, // // - (for Temp)
//    {  0, 1, 2, 3, 4, 5,  6, 7, 8, 9, 10, 11, 12, 13}, // all off
//  };
//
//
//  if ((String)temp1 == "-") {
//    for (int j = 44; j < 58; j++) { //fourth digit... a 1
//      if (nums[12][j - 44] == -1) {
//        pixels[j] = -1;
//      } else {
//        pixels[j] = nums[12][j - 44] + 44;
//      }
//    }
//  } else {
//    for (int j = 44; j < 58; j++) { //fourth digit... a 1
//      if (nums[digit1][j - 44] == -1) {
//        pixels[j] = -1;
//      } else {
//        pixels[j] = nums[digit1][j - 44] + 44;
//      }
//    }
//  }
//
//  for (int j = 30; j < 44; j++) { //third digit.. a 7
//    if (nums[digit2][j - 30] == -1)  {
//      pixels[j] = -1;
//    } else {
//      pixels[j] = nums[digit2][j - 30] + 30;
//    }
//  }
//  pixels[28] = 28; //dots
//  pixels[29] = 29; // dots
//  if (ShowThirdDigit) {
//    for (int j = 14; j < 28; j++)
//    { //second digit... a 3
//      if (nums[digit3][j - 14] == -1)
//      {
//        pixels[j] = -1;
//      }
//      else
//      {
//        pixels[j] = nums[digit3][j - 14] + 14;
//      }
//    }
//  }else{
//    for (int j = 14; j < 28; j++)
//    { //second digit... a 3
//      if (nums[13][j - 14] == -1)
//      {
//        pixels[j] = -1;
//      }
//      else
//      {
//        pixels[j] = nums[13][j - 14] + 14;
//      }
//    }
//  }
//  if (units == "F") {
//    for (int j = 0; j < 14; j++) { //first digit.  a 3
//      pixels[j] = nums[11][j];
//    }
//  } else {
//    for (int j = 0; j < 14; j++) { //first digit.  a 3
//      pixels[j] = nums[10][j];
//    }
//  }
//
//
//}
