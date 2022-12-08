#include "RTClib.h"
RTC_DS3231 rtc;
#define CLOCK_INTERRUPT_PIN 2
#include <Adafruit_NeoPixel.h>

#define LED_PIN 4
#define LED_COUNT 58
const int pResistor = A0;
int lightvalue;
int pixels[58];
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int Sized[] = {18, 6, 15, 15, 12, 15, 18, 9, 21, 18};
int Sizedoff[] = {3, 15, 6, 6, 9, 6, 3, 12, 0, 3};


void setup() {

  Serial.begin(9600);

  while (!Serial) {}

        rtc.begin(); // initialize RTC
      rtc.disable32K();
//      rtc.adjust(DateTime(epoch + 1));
      rtc.clearAlarm(1);
      rtc.disable32K();
      pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), alarm, FALLING);
      rtc.clearAlarm(1);
      rtc.clearAlarm(2);
      rtc.writeSqwPinMode(DS3231_OFF);
      rtc.setAlarm1(DateTime(0, 0, 0, 0, 0, 30), DS3231_A1_Second);
  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP
  strip.clear();
  // put your setup code here, to run once:

}
void alarm() {
  if (rtc.alarmFired(1)) {
    Serial.println("Alarm1 Triggered");
    //this is for the temperature
//    Serial.println(owner.temp);
    Serial.print("Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");
    String temp1 = String(rtc.getTemperature()).substring(0, 1);
    String temp2 = String(rtc.getTemperature()).substring(1, 2);
    Serial.println(temp1);
    Serial.println(temp2);
//    if ((String)owner.temp == "on") {
      Serial.println("in temp loop");
outputDigitsTemp(temp1.toInt(), temp2.toInt(),"C");
      //outputDigits(temp1.toInt(), temp2.toInt(), PSTHour2.toInt(), PSTHour1.toInt());
//    }
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
  }
}
void outputDigitsTemp(int digit1, int digit2, String units) {

  int nums[][14] = {
 
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, -1, -1}, //0 //seems ok
    {0, 1, -1, -1, -1, -1, 6, 7, 8, 9, 10, 11, 12, 13},       //1 //seems ok
    { -1, -1, 2, 3, -1, -1, -1, -1, 8, 9, -1, -1, -1, -1},    //2 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, 8, 9, -1, -1, 12, 13},  //3
    {0, 1, -1, -1, -1, -1, 6, 7, -1, -1, -1, -1, 12, 13},     //4
    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, 12, 13},  //5////numbers represent the pixels that are NOT LIT
    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1},  //6
    {0, 1, -1, -1, -1, -1, -1, -1, 8, 9, 10, 11, 12, 13},     //7 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, //8 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13}, //9 //seems ok
    { -1, -1, 2, 3, 4, 5, -1, -1, -1, -1, 10, 11, -1, -1}, //C (for Temp)
    { 0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1}, // F (for Temp)
    { 0, 1, 2, 3, 4, 5,  6, 7, 8, 9, 10, 11, 12, 13}, // all off
  };

  for (int j = 30; j < 44; j++)
  { //third digit.. a 7
    if (nums[digit2][j - 30] == -1)
    {
      pixels[j] = -1;
    }
    else
    {

      pixels[j] = nums[digit2][j - 30] + 30;
    }
  }
  for (int j = 44; j < 58; j++)
  { //fourth digit... a 1
    if (nums[digit1][j - 44] == -1)
    {
      pixels[j] = -1;
    }
    else
    {
      pixels[j] = nums[digit1][j - 44] + 44;
    }
  }

   for (int j = 14; j < 28; j++)
  { //second digit... a 3
    if (nums[10][j - 14] == -1)
    {
      pixels[j] = -1;
    }
    else
    {
      pixels[j] = nums[10][j - 14] + 14;
    }
  }
}
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85){
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170){
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }  else  {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
void loop() {
  //  // put your main code here, to run repeatedly:
//  String temp1 = "2";
//  String temp2 = "5";
//  Serial.println(temp1);
//  Serial.println(temp2);
  ////    if ((String)owner.temp == "on") {
  //      Serial.println("in temp loop");
  //outputDigitsTemp(temp1.toInt(), temp2.toInt(),"C");
  //delay (1000);
  //}
  //  delay(10000);

  //   lightvalue = analogRead(pResistor);
  uint16_t i, j;

  for (j = 0; j < 256; j++)
  {
    for (i = 0; i < strip.numPixels(); i++)
    {
      if (pixels[i] == -1)
      {
        strip.setPixelColor(i, Wheel((i * 14 + j) & 255));
      }
      else
      {
        strip.setPixelColor(i, strip.Color(0, 0, 0));
      }
    }
    strip.show();
    delay(15);
  }

}
