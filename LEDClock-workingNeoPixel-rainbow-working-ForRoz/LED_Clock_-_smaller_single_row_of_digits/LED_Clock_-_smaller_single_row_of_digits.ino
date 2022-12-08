/*

   Dual Timezone RGB LED Clock
   by Raina

   This uses code from Michael Marolis and Tom Igoe to pull the current time from an NTP server.

   the rest is all mine.  it basically just then works out the timezone of PST, and GMT, using Timezone.h and displays the current time on two 4 digit 7 segment displays.
   in my case those are oversized 3d prints taken from
   https://nerdiy.de/en/howto-elektronik-sieben-segment-ziffer-pxldigit-aus-ws2812-leds-bauen/

   GPL License applies etc etc etc
*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
//#include <DS3231.h>
//#include <RTCZero.h>//re-enable for MKR1010
#include "RTClib.h"
#include <Timezone.h>
#include <Adafruit_NeoPixel.h>
//#include <FastLED.h>
//RTCZero rtc; //re-enable for MKR1010
RTC_DS3231 rtc;
int status = WL_IDLE_STATUS;
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;          // your network key index number (needed only for WEP)
#define CLOCK_INTERRUPT_PIN 2
unsigned int localPort = 2390; // local port to listen for UDP packets

IPAddress timeServer(128, 138, 141, 172); // time.nist.gov NTP server

const int NTP_PACKET_SIZE = 48; // NTP timestamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
//TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        // British Summer Time
//TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         // Standard Time
//Timezone UK(BST, GMT);
// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240}; // Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};  // Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);

//TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
//TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
//Timezone usPT(usPDT, usPST);
TimeChangeRule utcRule = {"UTC", Last, Sun, Mar, 1, 0}; // UTC
Timezone UTC(utcRule);
#define LED_PIN 4
#define LED_COUNT 58
const int pResistor = A0;
int lightvalue;
int pixels[172];
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//CRGB leds[LED_COUNT];
int Sized[] = {18, 6, 15, 15, 12, 15, 18, 9, 21, 18};
int Sizedoff[] = {3, 15, 6, 6, 9, 6, 3, 12, 0, 3};

void setup()
{
  Wire.begin();
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  //pinMode(pResistor, INPUT);

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP
  strip.clear();
  strip.setBrightness(150);
  strip.setPixelColor(28, strip.Color(255, 0, 0));
  strip.setPixelColor(29, strip.Color(255, 0, 0));

  strip.show();
  //  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {

    strip.setPixelColor(28, strip.Color(255, 150, 0));
    strip.setPixelColor(29, strip.Color(255, 150, 0));
    strip.show();
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  strip.setPixelColor(28, strip.Color(0, 255, 0));
  strip.setPixelColor(29, strip.Color(0, 255, 0));

  strip.show();

  printWifiStatus();

  Serial.println("\nStarting connection to server (first run)...");
  Udp.begin(localPort);
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  if (Udp.parsePacket())
  {
    Serial.println("packet received");
    strip.setPixelColor(28, strip.Color(0, 0, 255));
    strip.setPixelColor(29, strip.Color(0, 0, 255));
    strip.show();
    // We've received a packet, read the data from it
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, extract the two words:
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    //    Serial.println(epoch);

    rtc.begin(); // initialize RTC
    rtc.disable32K();
    rtc.adjust(DateTime(epoch + 1));
    // rtc.setEpoch(epoch + 1);
    rtc.clearAlarm(1);
    rtc.disable32K();

    pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), alarm, FALLING);

    rtc.clearAlarm(1);
    rtc.clearAlarm(2);

    // stop oscillating signals at SQW Pin
    // otherwise setAlarm1 will fail
    rtc.writeSqwPinMode(DS3231_OFF);

    rtc.setAlarm2(rtc.now(), DS3231_A2_PerMinute);
    // rtc.setAlarmTime(0, 0, 59);
    // alarm when seconds match the alarm time:
    //  rtc.enableAlarm(rtc.MATCH_SS);
    // attach a function to the alarm:
    //  rtc.attachInterrupt(alarm);
    // print the hour, minute and second:
  }else{
    strip.setPixelColor(28, strip.Color(255, 255, 255));
    strip.setPixelColor(29, strip.Color(255, 255, 255));
    strip.show();
  }
}

void alarm(){

  /*
     this is what handles the actual updating of the time.  it's an Alarm interrupt on the RTC.
  */
  Serial.println("Alarm Triggered");
  DateTime now = rtc.now();

  int alarmepoch = now.unixtime();
  char bufPST[40];
  char mPST[4];

  TimeChangeRule *tcr;
  time_t tPST = usET.toLocal(alarmepoch, &tcr);

  char PSTHour[3];
  char PSTMin[3];
  char PSTSec[3];

  //There may be a better way to do all this, but I don't know what it is.  but since we're driving the digits in different colours, I figured we needed
  //to drive the digits entirely independently.  so in order to do this, I need to break the time down into separate digits.

  strcpy(mPST, monthShortStr(month(tPST)));
  sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tPST), minute(tPST), second(tPST), dayShortStr(weekday(tPST)), day(tPST), mPST, year(tPST), tcr->abbrev);

  Serial.println(bufPST);

  sprintf(PSTHour, "%.2d", hour(tPST));
  sprintf(PSTMin, "%.2d", minute(tPST));
  sprintf(PSTSec, "%.2d", second(tPST));

  String PSTHours = String(PSTHour);
  String PSTMins = String(PSTMin);
  String PSTSeconds = String(PSTSec);

  String PSTMinute1 = String(PSTMin).substring(0, 1);
  String PSTMinute2 = String(PSTMin).substring(1);

  String PSTHour1 = String(PSTHour).substring(0, 1);
  String PSTHour2 = String(PSTHour).substring(1);

  rtc.clearAlarm(2);
  //now that the time's been split up, we can output it all to the digits of the clock.
  //I'm doing each digit separately.  again as with before, there's probably way better ways of doing this.

  if ((PSTHour1 == "0") && (PSTHour2 == "0") && (PSTMinute1 == "0") && (PSTMinute2 == "0"))
  {
    //it's midnight, so re-run the time sync
    GetTime();
  }
  if ((PSTHour1 == "1") && (PSTHour2 == "2") && (PSTMinute1 == "0") && (PSTMinute2 == "0"))
  {
    //it's Noon, so re-run the time sync

    GetTime();
  }
  outputDigits(PSTMinute2.toInt(), PSTMinute1.toInt(), PSTHour2.toInt(), PSTHour1.toInt());
  //outputDigits (1, 1, 1, 1);
  //outputDigits (2, 2, 2, 2);
  //outputDigits (3, 3, 3, 3);
  //outputDigits (4, 4, 4, 4);
  //outputDigits (5, 5, 5, 5);
  //outputDigits (6, 6, 6, 6);
  //outputDigits (7, 7, 7, 7);
  //outputDigits (8, 8, 8, 8);
  //outputDigits (9, 9, 9, 9);
}

void outputDigits(int digit1, int digit2, int digit3, int digit4)
{

  int nums[][14] = {
      // 0   1   2   3   4   5   6   7   8   9  10  11  12  13
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, -1, -1}, //0 //seems ok
      {0, 1, -1, -1, -1, -1, 6, 7, 8, 9, 10, 11, 12, 13},       //1 //seems ok
      {-1, -1, 2, 3, -1, -1, -1, -1, 8, 9, -1, -1, -1, -1},     //2 //seems ok
      {-1, -1, -1, -1, -1, -1, -1, -1, 8, 9, -1, -1, 12, 13},   //3
      {0, 1, -1, -1, -1, -1, 6, 7, -1, -1, -1, -1, 12, 13},     //4
      {-1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, 12, 13},   //5////numbers represent the pixels that are NOT LIT
      {-1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1},   //6
      {0, 1, -1, -1, -1, -1, -1, -1, 8, 9, 10, 11, 12, 13},     //7 //seems ok
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, //8 //seems ok
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13}, //9 //seems ok

  };

  for (int j = 0; j < 14; j++)
  { //first digit.  a 3
    pixels[j] = nums[digit1][j];
  }

  for (int j = 14; j < 28; j++)
  { //second digit... a 3
    if (nums[digit2][j - 14] == -1)
    {
      pixels[j] = -1;
    }
    else
    {
      pixels[j] = nums[digit2][j - 14] + 14;
    }
  }
  pixels[28] = -1; //dots
  pixels[29] = -1; // dots

  for (int j = 30; j < 44; j++)
  { //third digit.. a 7
    if (nums[digit3][j - 30] == -1)
    {
      pixels[j] = -1;
    }
    else
    {

      pixels[j] = nums[digit3][j - 30] + 30;
    }
  }
  for (int j = 44; j < 58; j++)
  { //fourth digit... a 1
    if (nums[digit4][j - 44] == -1)
    {
      pixels[j] = -1;
    }
    else
    {
      pixels[j] = nums[digit4][j - 44] + 44;
    }
  }
}
uint32_t Wheel(byte WheelPos)
{
  if (WheelPos < 85)
  {
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void loop()
{

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
  /*
     This is for the photoresistor I put in, so the brightness of the clock can auto adjust based on ambient light levels.  needs some tweaking for my
     environment.  ie right now at 9:00am with the kitchen light on and one of the curtains opened, this value is around 290-311, which would put it in the medium range.

     at 15:32, both curtains open, kitchen light on.. value is about 270-280

  */
  //
    lightvalue = analogRead(pResistor);
   //// Serial.println(lightvalue);
    if (lightvalue > 350) {
       strip.setBrightness(255);
      strip.show();
    }
    if ((lightvalue > 150) && (lightvalue < 350)) {
       strip.setBrightness(150);
       strip.show();
    }
    if (lightvalue < 150) {
       strip.setBrightness(20);
      strip.show();
    }
  //  delay(1000);
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress &address)
{
  //Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  //Serial.println("2");
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  //Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  //Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  //Serial.println("5");
  Udp.endPacket();
  //Serial.println("6");
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

void GetTime()
{
  strip.setPixelColor(28, strip.Color(255, 0, 0));
  strip.setPixelColor(29, strip.Color(255, 0, 0));

  strip.show();
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
  {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED)
  {
    strip.setPixelColor(28, strip.Color(255, 150, 0));
    strip.setPixelColor(29, strip.Color(255, 150, 0));

    strip.show();
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  Serial.println("Connected to WiFi");
  strip.setPixelColor(28, strip.Color(0, 255, 0));
  strip.setPixelColor(29, strip.Color(0, 255, 0));

  strip.show();
  printWifiStatus();

  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  delay(1000);
  if (Udp.parsePacket())
  {
    Serial.println("packet received");
    strip.setPixelColor(28, strip.Color(0, 0, 255));
    strip.setPixelColor(29, strip.Color(0, 0, 255));

    strip.show();
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
    //   rtc.setEpoch(epoch + 1);
  }
}
