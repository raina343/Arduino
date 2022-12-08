#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
//#include <DS3231.h>
//#include <RTCZero.h>//re-enable for MKR1010
#include "RTClib.h"
#include <Timezone.h>
#include <Adafruit_NeoPixel.h>
#include <FlashStorage.h>

/////////TODO LIST /////
// 1. need to figure out a way to retry the NTP connection if it fails (which it seems to do alot) - maybe fixed?
// 2. need to figure out why the http server stops responding if say the SSID is wrong. - maybe fixed.  not the best solution in the world
// 3. sort out refreshing the clock twice a day (may be ok with the DS3231 keeping the time, but we'll see.
// 4. Add software reset
// 5. Make sure all debug states are properly displayed via pixels and document those
// 6. Display the temperature on the display properly and revert to time after.. 5 seconds?
// 7. add the Photoresistor

RTC_DS3231 rtc;
#define CLOCK_INTERRUPT_PIN 2
unsigned int localPort = 2390; // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28);
//IPAddress timeServer(128, 138, 141, 172); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48;     // NTP timestamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP Udp;
#define LED_PIN 4
#define LED_COUNT 58
const int pResistor = A0;
int lightvalue;
int pixels[58];
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int Sized[] = {18, 6, 15, 15, 12, 15, 18, 9, 21, 18};
int Sizedoff[] = {3, 15, 6, 6, 9, 6, 3, 12, 0, 3};
char ssid[] = "RainbowClockSetup"; // your network SSID (name)
char pass[] = "";                  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                  // your network key Index number (needed only for WEP)
typedef struct
{
  boolean valid;
  char wifissid[100];
  char Password[100];
  char temp[3];
  char twelvehr[3];
  char timezone[10];
  char tempUnits[3];
} Credentials;

int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
String readString;
int WifiSetup = 1;
FlashStorage(my_flash_store, Credentials);
Credentials owner;

TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240}; // Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};  // Eastern Standard Time = UTC - 5 hours
TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};
TimeChangeRule usMST = {"MST", First, Sun, Nov, 2, -420};
TimeChangeRule usCDT = {"CDT", Second, Sun, Mar, 2, -300};
TimeChangeRule usCST = {"CST", First, Sun, Nov, 2, -360};
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usET(usEDT, usEST);
Timezone usPT(usPDT, usPST);
Timezone usCT(usCDT, usCST);
Timezone usMT(usMDT, usMST);

void setup()
{
  Serial.begin(9600);

  while (!Serial)
  {
  }
  owner = my_flash_store.read();

  strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();  // Turn OFF all pixels ASAP
  strip.clear();
  strip.setBrightness(150);
  if (owner.valid == false)
  {
    status = WiFi.beginAP(ssid);
    if (status != WL_AP_LISTENING)
    {
      strip.setPixelColor(50, strip.Color(255, 150, 0));
      strip.setPixelColor(51, strip.Color(255, 150, 0));
      strip.show();
      //48 and 49
      Serial.println("Creating access point failed");
      // don't continue
      while (true)
        ;
    }
    delay(10000);
    server.begin();
    printWiFiStatus();
    Serial.println("Access Point Web Server");
    if (WiFi.status() == WL_NO_MODULE)
    {
      Serial.println("Communication with WiFi module failed!");
      strip.setPixelColor(50, strip.Color(255, 0, 0));
      strip.setPixelColor(51, strip.Color(255, 0, 0));
      strip.show();
      while (true)
        ;
    }
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
      Serial.println("Please upgrade the firmware");
    }
    Serial.print("Creating access point named: ");
    Serial.println(ssid);
    strip.setPixelColor(50, strip.Color(255, 255, 0));
    strip.setPixelColor(51, strip.Color(255, 255, 0));
    strip.setPixelColor(36, strip.Color(255, 255, 0));
    strip.setPixelColor(37, strip.Color(255, 255, 0));
    strip.show();
  }
  else
  {
    Serial.print("Stored SSID:");
    Serial.println((String)owner.wifissid);
    WifiSetup = 0;
    validateSSID(owner);
    delay(10000);
    server.begin();
    strip.setPixelColor(28, strip.Color(255, 0, 0));
    strip.setPixelColor(29, strip.Color(255, 0, 0));
    strip.show();
    if (WiFi.status() == WL_NO_MODULE)
    {
      strip.setPixelColor(50, strip.Color(255, 0, 0));
      strip.setPixelColor(51, strip.Color(255, 0, 0));
      strip.show();
      Serial.println("Communication with WiFi module failed!");
      while (true)
        ;
    }
    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    {
      Serial.println("Please upgrade the firmware");
    }
    while (status != WL_CONNECTED)
    {
      strip.setPixelColor(28, strip.Color(255, 150, 0));
      strip.setPixelColor(29, strip.Color(255, 150, 0));
      strip.show();
      Serial.print("Attempting to connect to SSID: ");
      Serial.println(ssid);
      //Connect to WPA/WPA2 network. Change this line if using open or WEP network:
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

    delay(3000);
    if (Udp.parsePacket())
    {
      Serial.println("packet received");
      strip.setPixelColor(28, strip.Color(0, 0, 255));
      strip.setPixelColor(29, strip.Color(0, 0, 255));
      strip.show();
      // We've received a packet, read the data from it
      Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;
      rtc.begin(); // initialize RTC
      rtc.disable32K();
      rtc.adjust(DateTime(epoch + 1));
      rtc.clearAlarm(1);
      rtc.disable32K();
      pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), alarm, FALLING);
      rtc.clearAlarm(1);
      rtc.clearAlarm(2);
      rtc.writeSqwPinMode(DS3231_OFF);
      rtc.setAlarm1(DateTime(0, 0, 0, 0, 0, 30), DS3231_A1_Second);
      rtc.setAlarm2(rtc.now(), DS3231_A2_PerMinute);
    }
    else
    {
      strip.setPixelColor(28, strip.Color(255, 255, 255));
      strip.setPixelColor(29, strip.Color(255, 255, 255));
      strip.show();
    }
  }
}

void printWiFiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void alarm()
{
  if (rtc.alarmFired(1))
  {
    Serial.println("Alarm1 Triggered");
    if ((String)owner.temp == "on")
    {
      //this is for the temperature
      Serial.println(owner.temp);
      Serial.print("Temperature: ");
      Serial.print(rtc.getTemperature());
      Serial.println(" C");
      Serial.println(owner.tempUnits);
      String temp1;
      String temp2;
      String TempUnits;
      if ((String)owner.tempUnits == "F")
      {
        Serial.println("CaveMan Temperature");
        int Temperature = rtc.getTemperature();
        int Temperature2 = (Temperature * 1.8) + 32;
        temp1 = String(Temperature2).substring(0, 1);
        temp2 = String(Temperature2).substring(1, 2);
        Serial.println(temp1);
        Serial.println(temp2);
        TempUnits = "F";
      }
      else
      {
        temp1 = String(rtc.getTemperature()).substring(0, 1);
        temp2 = String(rtc.getTemperature()).substring(1, 2);
        Serial.println(temp1);
        Serial.println(temp2);
        TempUnits = "C";
      }
      Serial.println("in temp loop");
      outputDigitsTemp(temp1.toInt(), temp2.toInt(), TempUnits);
      //delay(5000);
      SetTime();
      rtc.clearAlarm(1);
      rtc.clearAlarm(2);
    }
  }
  else
  {
    SetTime();
  }
}
void SetTime()
{
  /*
     this is what handles the actual updating of the time.  it's an Alarm interrupt on the RTC.
  */
  Serial.println("Alarm2 Triggered");
  DateTime now = rtc.now();

  int alarmepoch = now.unixtime();
  char bufPST[40];
  char mPST[4];
  char mEST[4];
  char mMST[4];
  char mCST[4];
  TimeChangeRule *tcr;
  //    time_t tPST = usPT.toLocal(alarmepoch, &tcr);

  char PSTHour[3];
  char PSTMin[3];
  char PSTSec[3];
  if ((String)owner.timezone == "PST")
  {
    Serial.println("PST Time Zone...");
    time_t tPST = usPT.toLocal(alarmepoch, &tcr);
    strcpy(mPST, monthShortStr(month(tPST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tPST), minute(tPST), second(tPST), dayShortStr(weekday(tPST)), day(tPST), mPST, year(tPST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tPST));
    sprintf(PSTMin, "%.2d", minute(tPST));
    sprintf(PSTSec, "%.2d", second(tPST));
  }

  if ((String)owner.timezone == "EST")
  {
    Serial.println("EST Time Zone...");
    time_t tEST = usET.toLocal(alarmepoch, &tcr);
    strcpy(mEST, monthShortStr(month(tEST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tEST), minute(tEST), second(tEST), dayShortStr(weekday(tEST)), day(tEST), mEST, year(tEST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tEST));
    sprintf(PSTMin, "%.2d", minute(tEST));
    sprintf(PSTSec, "%.2d", second(tEST));
  }
  if ((String)owner.timezone == "CST")
  {
    Serial.println("CST Time Zone...");
    time_t tCST = usCT.toLocal(alarmepoch, &tcr);

    strcpy(mCST, monthShortStr(month(tCST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tCST), minute(tCST), second(tCST), dayShortStr(weekday(tCST)), day(tCST), mCST, year(tCST), tcr->abbrev);

    Serial.println(bufPST);

    sprintf(PSTHour, "%.2d", hour(tCST));
    sprintf(PSTMin, "%.2d", minute(tCST));
    sprintf(PSTSec, "%.2d", second(tCST));
  }
  if ((String)owner.timezone == "MST")
  {
    Serial.println("MST Time Zone...");
    time_t tMST = usMT.toLocal(alarmepoch, &tcr);

    strcpy(mMST, monthShortStr(month(tMST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tMST), minute(tMST), second(tMST), dayShortStr(weekday(tMST)), day(tMST), mMST, year(tMST), tcr->abbrev);

    Serial.println(bufPST);

    sprintf(PSTHour, "%.2d", hour(tMST));
    sprintf(PSTMin, "%.2d", minute(tMST));
    sprintf(PSTSec, "%.2d", second(tMST));
  }
  //There may be a better way to do all this, but I don't know what it is.  but since we're driving the digits in different colours, I figured we needed
  //to drive the digits entirely independently.  so in order to do this, I need to break the time down into separate digits.

  String PSTHours = String(PSTHour);
  if ((String)owner.twelvehr == "on")
  {
    Serial.println("12 hour Time!!!");
    if ((int)PSTHour > 12)
    {
      String foo = (String)PSTHour;
      int bar = foo.toInt();
      int www = bar - 12;
      if (www < 10)
      {
        String foo2 = (String)www;
        String prefix = "0";
        String bar2 = prefix + foo2;

        bar2.toCharArray(PSTHour, 3);
        //www = foo3.toInt();
      }
      else
      {
        String bar2 = (String)www;
        bar2.toCharArray(PSTHour, 3);
      }
    }
  }
  String PSTMins = String(PSTMin);
  String PSTSeconds = String(PSTSec);
  String PSTMinute1 = String(PSTMin).substring(0, 1);
  String PSTMinute2 = String(PSTMin).substring(1);
  String PSTHour1 = String(PSTHour).substring(0, 1);
  String PSTHour2 = String(PSTHour).substring(1);
  rtc.clearAlarm(1);
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

void (*resetFunc)(void) = 0; //declare reset function @ address 0

void outputDigitsTemp(int digit1, int digit2, String units)
{

  int nums[][14] = {

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
      {-1, -1, 2, 3, 4, 5, -1, -1, -1, -1, 10, 11, -1, -1},     //C (for Temp)
      {0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1},       // F (for Temp)
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},           // all off
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

  pixels[28] = 28; //dots
  pixels[29] = 29; // dots
  for (int j = 0; j < 14; j++)
  { //first digit.  a 3
    pixels[j] = nums[12][j];
  }
  if (units == "F")
  {
    for (int j = 14; j < 28; j++)
    { //second digit... a 3
      if (nums[11][j - 14] == -1)
      {
        pixels[j] = -1;
      }
      else
      {
        pixels[j] = nums[11][j - 14] + 14;
      }
    }
  }
  else
  {
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
      {-1, -1, 2, 3, 4, 5, -1, -1, -1, -1, 10, 11, -1, -1},     //C (for Temp)
      {0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1},       // F (for Temp)
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},           // all off
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
  if (WifiSetup == 0)
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

    //    lightvalue = analogRead(pResistor);
    //    Serial.println(lightvalue);
    //    if (lightvalue > 350) {
    //      strip.setBrightness(255);
    //      strip.show();
    //    }
    //    if ((lightvalue > 150) && (lightvalue < 350)) {
    //      strip.setBrightness(150);
    //      strip.show();
    //    }
    //    if (lightvalue < 150) {
    //      strip.setBrightness(20);
    //      strip.show();
    //    }
  }

  WiFiClient client = server.available();
  if (client)
  {
    //Serial.println("In Client Loop");
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();
        Serial.println(readString.length());
        //read char by char HTTP request
        if (readString.length() < 200)
        {

          //store characters to string
          readString += c;
          //Serial.print(c); //uncomment to see in serial monitor
        }
        //Serial.println(readString);
        //if HTTP request has ended
        if (c == '\n')
        {
          if (readString.indexOf("Submit=Subm") > 0)
          {
            Serial.println(readString);
            char Buf[250];
            readString.toCharArray(Buf, 150);
            char *token = strtok(Buf, "/?"); // Get everything up to the /if(token) // If we got something
            {
              char *name = strtok(NULL, "="); // Get the first name. Use NULL as first argument to keep parsing same string
              while (name)
              {
                char *valu = strtok(NULL, "&");
                if (valu)
                {
                  if (String(name) == "?ssid")
                  {
                    String ssidname = valu;
                    ssidname.toCharArray(owner.wifissid, 100);
                  }
                  if (String(name) == "password")
                  {
                    String pass = valu;
                    pass.toCharArray(owner.Password, 100);
                  }
                  if (String(name) == "timezone")
                  {
                    String pass = valu;
                    pass.toCharArray(owner.timezone, 10);
                  }
                  if (String(name) == "12hr")
                  {
                    String twelvehr = valu;
                    twelvehr.toCharArray(owner.twelvehr, 3);
                  }
                  if (String(name) == "ShowTemp")
                  {
                    String temp = valu;
                    temp.toCharArray(owner.temp, 3);
                  }
                  if (String(name) == "TempUnits")
                  {
                    String temp = valu;
                    temp.toCharArray(owner.tempUnits, 3);
                  }
                  name = strtok(NULL, "="); // Get the next name
                }
              }
            }
            String returnedip = validateSSID(owner);
            if (returnedip == "0.0.0.0")
            {
              Serial.println("Wifi Failed");
              status = WiFi.beginAP(ssid);
              strip.setPixelColor(50, strip.Color(255, 0, 0));
              strip.setPixelColor(51, strip.Color(255, 0, 0));

              strip.setPixelColor(36, strip.Color(255, 0, 0));
              strip.setPixelColor(37, strip.Color(255, 0, 0));
              strip.show();
              //              delay (2000);
              printWiFiStatus();
            }
            else
            {
              strip.setPixelColor(50, strip.Color(0, 0, 255));
              strip.setPixelColor(51, strip.Color(0, 0, 255));
              strip.setPixelColor(37, strip.Color(0, 0, 255));
              strip.setPixelColor(36, strip.Color(0, 0, 255));
              strip.show();
              //              delay (2000);
              owner.valid = true;

              my_flash_store.write(owner);
              resetFunc(); //call reset
            }
          }
          String twelvehour = "";
          if ((String)owner.twelvehr == "on")
          {
            twelvehour = " selected='selected' ";
          }
          String ShowTempUnitsSetting = "";
          if ((String)owner.tempUnits == "F")
          {
            ShowTempUnitsSetting = " selected ";
          }

          String ShowTempSetting = "";
          if ((String)owner.temp == "on")
          {
            ShowTempSetting = " selected ";
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.print("<HTML><HEAD><TITLE>Rainbow Clock Setup</TITLE></HEAD><script><BODY><style type=\"text/css\">*,*::before,*::after{box-sizing: border-box;}:root{--select-border: \#777;--select-focus: blue;--select-arrow: var(--select-border);}select{-webkit-appearance: none;-moz-appearance: none;appearance: none;background-color: transparent;border: none;padding: 0 1em 0 0;margin: 0;width: 100%;font-family: inherit;font-size: inherit;cursor: inherit;line-height: inherit;z-index: 1;outline: none;}select::-ms-expand{display: none;}.select{display: grid;grid-template-areas: \"select\";align-items: center;position: relative;min-width: 15ch;max-width: 30ch;border: 1px solid var(--select-border);border-radius: 0.25em;padding: 0.25em 0.5em;font-size: 1.25rem;cursor: pointer;line-height: 1.1;background-color: \#fff;background-image: linear-gradient(to top, \#f9f9f9, \#fff 33%);}.select select, .select::after{grid-area: select;}.select:not(.select--multiple)::after{content: "                       ";justify-self: end;width: 0.8em;height: 0.5em;background-color: var(--select-arrow);-webkit-clip-path: polygon(100% 0%, 0 0%, 50% 100%);clip-path: polygon(100% 0%, 0 0%, 50% 100%);}select:focus + .focus{position: absolute;top: -1px;left: -1px;right: -1px;bottom: -1px;border: 2px solid var(--select-focus);border-radius: inherit;}select[multiple]{padding-right: 0;height: 6rem;}select[multiple] option{white-space: normal;outline-color: var(--select-focus);}.select--disabled{cursor: not-allowed;background-color: \#eee;background-image: linear-gradient(to top, \#ddd, \#eee 33%);}label{font-size: 1.125rem;font-weight: 500;}.select + label{margin-top: 2rem;}body{min-height: 100vh;display: grid;place-content: center;grid-gap: 0.5rem;font-family: \"Baloo 2\", sans-serif;padding: 1rem;}.formcontent label{display: block;}.formcontent{text-align: center;width: 30ch;margin: 0 auto;}.formcontent li{list-style: none;padding-top: 10px;width: 30ch;}.formcontent input{display: grid;grid-template-areas: \"select\";align-items: center;position: relative;min-width: 15ch;max-width: 24ch;border: 1px solid var(--select-border);border-radius: 0.25em;padding: 0.25em 0.5em;font-size: 1.25rem;cursor: pointer;line-height: 1.1;background-color: \#fff;text-align: center;background-image: linear-gradient(to top, \#f9f9f9, \#fff 33%);}.formcontent button{display: grid;grid-template-areas: \"select\";align-items: center;position: relative;min-width: 15ch;max-width: 24ch;border: 1px solid var(--select-border);border-radius: 0.25em;padding: 0.25em 0.5em;font-size: 1.25rem;cursor: pointer;line-height: 1.1;background-color: \#fff;text-align: center;background-image: linear-gradient(to top, \#f9f9f9, \#fff 33%);}html{/*background: rgb(43,9,82);background: linear-gradient(75deg, \#ee7752, \#e73c7e, \#23a6d5, \#23d5ab);*/background: linear-gradient(145deg,rgba(43, 9, 82, 1) 9%,rgba(2, 2, 255, 1) 18%,rgba(2, 255, 2, 1) 27%,rgba(196, 181, 0, 1) 36%,rgba(255, 165, 0, 1) 45%,rgba(162, 19, 19, 1) 63%,rgba(255, 165, 0, 1) 72%,rgba(196, 181, 0, 1) 81%,rgba(2, 255, 2, 1) 90%,rgba(2, 2, 255, 1) 99%,rgba(43, 9, 82, 1) 100%);background-size: 400% 400%;animation: gradient 5s ease infinite;height: 100vh;width: 100vw;align-items: center;justify-content: center;}@keyframes gradient{0%{background-position: 0% 50%;}50%{background-position: 100% 50%;}100%{background-position: 0% 50%;}}.loader{border: 16px solid \#f3f3f3; /* Light grey */border-top: 16px solid \#3498db; /* Blue */border-radius: 50%;width: 120px;height: 120px;animation: spin 2s linear infinite;}@keyframes spin{0%{transform: rotate(0deg);}100%{transform: rotate(360deg);}}</style><header class=\"container-col\"><H1>Enter your SSID, Password below. if I haven't fucked up, after you hit Save, it should reconnect to your mainWifiand this Wireless Access Point will vanish (if the info entered is correct)<br>You can revisit this setup pagebychecking your router or other tools for the clocks new ip address</H1><div class=\"formcontent\"><ul><Li><label for='ssid'>SSID</label><input id='ssid' required type='text' name='ssid' value='Tuppence' placeholder='ssid'></Li><li><label for='password'>Password</label><input id='password' type='password' name='password' value='nevermindthebuzzcocks22' placeholder='password' required></li><li><label for='ShowTemp'>Show Temperature every 1 minute</label><div class=\"select\"><select name='ShowTemp' id='ShowTemp'><option value='off'>off</option><option value='on' selected>on</option></select><span class=\"focus\"></span></div></li><li><label for='TempUnits'>Temperature Units</label><div class=\"select\"><select name='TempUnits' id='TempUnits'><option value='C'>C</option><option value='F' selected>F</option></select><span class=\"focus\"></span></div></li><li><label for='12hr'>Show 12hr Time</label><div class=\"select\"><select name='12hr' id='12hr'><option value='off'>off</option><option value='on'>on</option></select><span class=\"focus\"></span></div></li><li><label for='timezone'>Timezone</label><div class=\"select\"><select name='timezone' id='timezone'><option value='EST' selected='selected'>Eastern</option><option value='CST'>Central</option><option value='MST'>Mountain</option><option value='PST'>Pacific</option><option value='AKST'>Alaska</option><option value='HST'>Hawaii</option></select><span class=\"focus\"></span></div></li><li><button id='Submit' name='Submit' value='Submit' type='submit'>Save</button></li></ul></div><script>$(document).ready(function (){$("\#Submit ").click(function (){var data=[];$(\".formcontent input\").each(function (){data[$(this).attr(\"id\")]=$(this).val();})$(\".formcontent select\").each(function (){})console.log(data);$.ajax({type: \"POST\",async: true,dataType: \"json\",url: \"/\",data:{\"checkinformation\": true,},success: function (json){processnotice(json);}});})})</script></BODY></HTML>");
          //          client.println("<HTML>");
          //          client.println("<HEAD>");
          //          client.println("<TITLE>Rainbow Clock Setup</TITLE>");
          //          client.println("</HEAD>");
          //          client.println("<BODY>");
          //          client.println("<H1>Enter your SSID, Password below.  if I haven't fucked up, after you hit Save, it should reconnect to your main Wifi and this Wireless Access Point will vanish (if the info entered is correct)<br>You can revisit this setup page by checking your router or other tools for the clocks new ip address</H1>");
          //          client.print("<form method='get' action=''>");
          //          client.print("<input id='ssid' required type='text' name='ssid' ");
          //          client.print("value='");
          //          client.print(owner.wifissid);
          //          client.print("' placeholder='ssid'><label for='ssid'>SSID</label><br>");
          //          client.print("<input id='password' type='password' name='password'");
          //          client.print("value='");
          //          client.print(owner.Password);
          //          client.print("' placeholder='password' required><label for='password'>Password</label><br>");
          //          client.print("<select name='ShowTemp' id='ShowTemp'>");
          //          client.print("<option value='off'>off</option>");
          //          client.print("<option value='on' ");
          //          client.print (ShowTempSetting);
          //          client.print(">on</option>");
          //          client.print("</select>");
          //          client.print("<label for='ShowTemp'>Show Temperature every 1 minute</label><br>");
          //          client.print("<select name='TempUnits' id='TempUnits'>");
          //          client.print("<option value='C'>C</option>");
          //          client.print("<option value='F' ");
          //          client.print (ShowTempUnitsSetting);
          //          client.print(">F</option>");
          //          client.print("</select>");
          //          client.print("<label for='TempUnits'>Temperature Units</label><br>");
          //
          //          client.print("<select name='12hr' id='12hr'>");
          //          client.print("<option value='off'>off</option>");
          //          client.print("<option value='on' ");
          //          client.print (twelvehour);
          //          client.print (">on</option>");
          //          client.print("</select>");
          //          client.print("<label for='12hr'>Show 12hr Time</label><br>");
          //
          //          client.print("<select name='timezone' id='timezone'>");
          //          client.print("<option value='EST' ");
          //          if ((String)owner.timezone == "EST") {
          //            client.print(" selected='selected' ");
          //          }
          //          client.print (">Eastern</option>");
          //          client.print("<option value='CST' ");
          //          if ((String)owner.timezone == "CST") {
          //            client.print(" selected='selected' ");
          //          }
          //          client.print (">Central</option>");
          //          client.print("<option value='MST' ");
          //          if ((String)owner.timezone == "MST") {
          //            client.print(" selected='selected' ");
          //          }
          //          client.print (">Mountain</option>");
          //          client.print("<option value='PST' ");
          //          if ((String)owner.timezone == "PST") {
          //            client.print(" selected='selected' ");
          //          }
          //          client.print (">Pacific</option>");
          //          client.print("<option value='AKST' ");
          //          if ((String)owner.timezone == "AKST") {
          //            client.print(" selected='selected' ");
          //          }
          //          client.print (">Alaska</option>");
          //          client.print("<option value='HST' ");
          //          if ((String)owner.timezone == "HST") {
          //            client.print(" selected='selected' ");
          //          }
          //          client.print (">Hawaii</option>");
          //
          //          client.print (twelvehour);
          //
          //          client.print("</select>");
          //          client.print("<label for='timezone'>Timezone</label><br>");
          //
          //
          //
          //          client.print("<button id='Submit' name='Submit' value='Submit'type='submit'>Save</button>");
          //          client.print("</form>");
          //          client.println("");
          //          client.println("</BODY>");
          //          client.println("</HTML>");
          delay(1);
          client.stop();
          readString = "";
        }
      }
    }
  }
}
String validateSSID(Credentials owner)
{
  status = WiFi.begin(owner.wifissid, owner.Password);
  delay(10000);
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  String LocalIP = String() + WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  return (LocalIP);
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress &address)
{
  Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  Serial.println("2");
  packetBuffer[0] = 0b11100011; // LI, Version, Mode
  packetBuffer[1] = 0;          // Stratum, or type of clock
  packetBuffer[2] = 6;          // Polling Interval
  packetBuffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;

  Serial.println("3");

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Serial.println("4");
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Serial.println("5");
  Udp.endPacket();
  Serial.println("6");
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

void reboot()
{
  //  wdt_disable();
  //  wdt_enable(WDTO_15MS);
  while (1)
  {
    ;
  }
}
