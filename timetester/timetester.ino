
#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
//#include <RTCZero.h>
#include <FlashStorage.h>
#include <Timezone.h>
#include <Adafruit_NeoPixel.h>


#include "RTClib.h"
//RTCZero rtc;
RTC_DS3231 rtc;
#define CLOCK_INTERRUPT_PIN 5
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServer(128, 138, 141, 172); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP timestamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE];
WiFiUDP Udp;
#define LED_PIN    6
#define SecondRowLED_PIN    7
#define LED_COUNT 172
const int pResistor = A0;
int checktime = 0;
int Reset = 6;
int clearsettings = 3;
int lightvalue;
int pixels[172];
int pixelsLower[172];


Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripSecondRow(LED_COUNT, SecondRowLED_PIN, NEO_GRB + NEO_KHZ800);

//int status = WL_IDLE_STATUS;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "Tuppence";        // your network SSID (name)
char pass[] = "nevermindthebuzzcocks22";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key index number (needed only for WEP)
typedef struct {
  boolean valid;
  char wifissid[100];
  char Password[100];
  char brightness[5];
  char temp[3];
  char twelvehr[3];
  char timezone[10];
  char secondtimezone[10];
  char tempUnits[3];
  char dimmer[3];
} Credentials;


int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;

String readString;
int WifiSetup = 1;
//FlashStorage(my_flash_store, Credentials);
//Credentials owner;
//Credentials owner2;
TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};  // Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};   // Eastern Standard Time = UTC - 5 hours
TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};
TimeChangeRule usMST = {"MST", First, Sun, Nov, 2, -420};
TimeChangeRule usCDT = {"CDT", Second, Sun, Mar, 2, -300};
TimeChangeRule usCST = {"CST", First, Sun, Nov, 2, -360};
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
TimeChangeRule usAKDT = {"AKDT", Second, Sun, Mar, 2, -480};
TimeChangeRule usAKST = {"AKST", First, Sun, Nov, 2, -540};
TimeChangeRule usHST = {"HST", Second, Sun, Mar, 2, -600};
TimeChangeRule AST = {"AST", First, Sun, Nov, 2, -180};
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};  // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};    // Central European Standard Time
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};     // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};      // Standard Time
Timezone UK(BST, GMT);
Timezone CE(CEST, CET);
Timezone usET(usEDT, usEST);
Timezone usPT(usPDT, usPST);
Timezone usCT(usCDT, usCST);
Timezone usMT(usMDT, usMST);
Timezone usAZ(usMST);
Timezone usHI(usHST);
Timezone usAK(usAKDT, usAKST);
Timezone AAST(AST);




void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
////  owner = my_flash_store.read();

  //  pinMode(pResistor, INPUT);
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.clear();


  stripSecondRow.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  stripSecondRow.show();            // Turn OFF all pixels ASAP
  stripSecondRow.clear();
  strip.setBrightness(150);
  stripSecondRow.setBrightness(150);
  delay(200);

//  if (owner.valid == false) {
//
//    status = WiFi.beginAP(ssid);
//    if (status != WL_AP_LISTENING) {
//      ////debugoutput(1, 0, 0, 0, 255, 75, 250);
//      // WAP couldn't start.  Top row pinkish
//      while (true);
//    }
//    delay(10000);
//   // server.begin();
//
//    Serial.println("Im here");
//    Serial.println("--------------------------------------------");
//    Serial.print("Stored SSID:");
//    Serial.println((String)owner.wifissid);
//    Serial.println((String)owner.valid);
//    Serial.println(timezone);
//    Serial.println(secondtimezone);
//    printWifiStatus();
//    Serial.println("Access Point Web Server");
//    Serial.print("connection status: ");
////    Serial.println(server.status());
//
//    //  // check for the WiFi module:
//    if (WiFi.status() == WL_NO_MODULE) {
//      Serial.println("Communication with WiFi module failed!");
//      ////debugoutput(1, 0, 0, 0, 255, 0, 0); // no wifi module top row red
//      while (true);
//    }
//    String fv = WiFi.firmwareVersion();
//    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
//      Serial.println("Please upgrade the firmware");
//    }
//    Serial.print("Creating access point named: ");
//    Serial.println(ssid);
//    // WAP Started - top row yellow
//    ////debugoutput(1, 0, 0, 0, 255, 255, 0);
//
//  } else {
    // we have saved credentials, so we start up normally. middle row green
   // ////debugoutput(0, 1, 0, 0, 0, 150, 0);

    Serial.print("Stored SSID:");
//    Serial.println((String)owner.wifissid);
//    Serial.println((String)owner.brightness);
    WifiSetup = 0;
//    String returnedip = validateSSID(owner);
//    if (returnedip == "0.0.0.0") {
//    //  ////debugoutput(1, 1, 1, 0, 255, 0, 0); //no IP on bootup.  Wifi issue.  Top middle and bottom red
//      Serial.println("Wifi Failed");
//      status = WiFi.beginAP(ssid);
//
//      printWifiStatus();
//      delay(5000);
//    } else {
      // validateSSID(owner);
      delay(5000);
//      server.begin();

      if (WiFi.status() == WL_NO_MODULE) {  // module failed.  middle row red
        ////debugoutput(0, 1, 0, 0, 255, 0, 0);
        Serial.println("Communication with WiFi module failed!");
        while (true)
          ;
      }
      String fv = WiFi.firmwareVersion();
      if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
      }
      while (status != WL_CONNECTED) {  // Wifi is disconnected, so start the connection process
        // lets turn the bottom row green too
        ////debugoutput(0, 0, 1, 0, 0, 150, 0);
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);
        // wait 10 seconds for connection:
        delay(10000);
      }
      Serial.println("Connected to WiFi");
      strip.setPixelColor(42, strip.Color(0, 255, 0));
      strip.setPixelColor(43, strip.Color(0, 255, 0));
      strip.show();
      printWifiStatus();
      Serial.println("\nStarting connection to server (first run)...");
      GetTime();
   // }
  //}
}
void SetTime() {
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
  char mHST[4];
  char mCET[4];
  char mGMT[4];
  char mAKDT[4];
  char mAZDT[4];
  char mAST[4];

  TimeChangeRule *tcr;
  char PSTHour[3];
  char PSTMin[3];
  char PSTSec[3];
  char PSTDOW[3];
  char PSTYear[3];
  char PSTDay[3];
   char PSTMon[3];
  char OtherHour[3];
  char OtherMin[3];
  char OtherSec[3];
String timezone="EST";
String secondtimezone="PST";
  if (timezone == "PST") {
    // Serial.println("PST Time Zone...");
    time_t tPST = usPT.toLocal(alarmepoch, &tcr);
    strcpy(mPST, monthShortStr(month(tPST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tPST), minute(tPST), second(tPST), dayShortStr(weekday(tPST)), day(tPST), mPST, year(tPST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tPST));
    sprintf(PSTMin, "%.2d", minute(tPST));
    sprintf(PSTSec, "%.2d", second(tPST));
  }

  if (timezone == "EST") {
    Serial.println("EST Time Zone...");
    time_t tEST = usET.toLocal(alarmepoch, &tcr);
    strcpy(mEST, monthShortStr(month(tEST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tEST), minute(tEST), second(tEST), dayShortStr(weekday(tEST)), day(tEST), mEST, year(tEST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tEST));
    sprintf(PSTMin, "%.2d", minute(tEST));
    
    sprintf(PSTDOW, "%s", dayShortStr(weekday(tEST)));
    sprintf(PSTMon, "%d", month(tEST));
    sprintf(PSTYear, "%d", year(tEST));
    sprintf(PSTDay, "%u", day(tEST));



//Serial.println("------------------------------------------");
    
//    Serial.println(PSTHour);
//    Serial.println(PSTMin);
//
//
//    
//    Serial.println(dayShortStr(weekday(tEST)));
//    Serial.println(month(tEST));
//     Serial.println(year(tEST));
//      Serial.println(day(tEST));
  }
  if (timezone == "CST") {
    Serial.println("CST Time Zone...");
    time_t tCST = usCT.toLocal(alarmepoch, &tcr);
    strcpy(mCST, monthShortStr(month(tCST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tCST), minute(tCST), second(tCST), dayShortStr(weekday(tCST)), day(tCST), mCST, year(tCST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tCST));
    sprintf(PSTMin, "%.2d", minute(tCST));
    sprintf(PSTSec, "%.2d", second(tCST));
  }
  if (timezone == "MST") {
    Serial.println("MST Time Zone...");
    time_t tMST = usMT.toLocal(alarmepoch, &tcr);
    strcpy(mMST, monthShortStr(month(tMST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tMST), minute(tMST), second(tMST), dayShortStr(weekday(tMST)), day(tMST), mMST, year(tMST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tMST));
    sprintf(PSTMin, "%.2d", minute(tMST));
    sprintf(PSTSec, "%.2d", second(tMST));
  }
  if (timezone == "GMT") {
    Serial.println("GMT Time Zone...");
    time_t tGMT = UK.toLocal(alarmepoch, &tcr);
    strcpy(mGMT, monthShortStr(month(tGMT)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tGMT), minute(tGMT), second(tGMT), dayShortStr(weekday(tGMT)), day(tGMT), mGMT, year(tGMT), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tGMT));
    sprintf(PSTMin, "%.2d", minute(tGMT));
    sprintf(PSTSec, "%.2d", second(tGMT));
  }
  if (timezone == "CET") {
    Serial.println("CET Time Zone...");
    time_t tCET = CE.toLocal(alarmepoch, &tcr);
    strcpy(mCET, monthShortStr(month(tCET)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tCET), minute(tCET), second(tCET), dayShortStr(weekday(tCET)), day(tCET), mCET, year(tCET), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tCET));
    sprintf(PSTMin, "%.2d", minute(tCET));
    sprintf(PSTSec, "%.2d", second(tCET));
  }
  if (timezone == "AST") {
    Serial.println("AST Time Zone...");
    time_t tAST = AAST.toLocal(alarmepoch, &tcr);
    strcpy(mAST, monthShortStr(month(tAST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tAST), minute(tAST), second(tAST), dayShortStr(weekday(tAST)), day(tAST), mAST, year(tAST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tAST));
    sprintf(PSTMin, "%.2d", minute(tAST));
    sprintf(PSTSec, "%.2d", second(tAST));
  }
  if (timezone == "ZST") {
    Serial.println("AZST Time Zone...");
    time_t tAZST = usAZ.toLocal(alarmepoch, &tcr);
    strcpy(mAZDT, monthShortStr(month(tAZST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tAZST), minute(tAZST), second(tAZST), dayShortStr(weekday(tAZST)), day(tAZST), mAZDT, year(tAZST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tAZST));
    sprintf(PSTMin, "%.2d", minute(tAZST));
    sprintf(PSTSec, "%.2d", second(tAZST));
  }
  if (timezone == "KST") {
    Serial.println("AKST Time Zone...");
    time_t tAKDT = usAK.toLocal(alarmepoch, &tcr);
    strcpy(mAKDT, monthShortStr(month(tAKDT)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tAKDT), minute(tAKDT), second(tAKDT), dayShortStr(weekday(tAKDT)), day(tAKDT), mAKDT, year(tAKDT), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tAKDT));
    sprintf(PSTMin, "%.2d", minute(tAKDT));
    sprintf(PSTSec, "%.2d", second(tAKDT));
  }
  if (timezone == "HST") {
    Serial.println("HST Time Zone...");
    time_t tHST = usHI.toLocal(alarmepoch, &tcr);
    strcpy(mHST, monthShortStr(month(tHST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tHST), minute(tHST), second(tHST), dayShortStr(weekday(tHST)), day(tHST), mHST, year(tHST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tHST));
    sprintf(PSTMin, "%.2d", minute(tHST));
    sprintf(PSTSec, "%.2d", second(tHST));
  }

  if (secondtimezone == "PST") {
    Serial.println("PST Other Time Zone...");
    time_t tPST = usPT.toLocal(alarmepoch, &tcr);
    strcpy(mPST, monthShortStr(month(tPST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tPST), minute(tPST), second(tPST), dayShortStr(weekday(tPST)), day(tPST), mPST, year(tPST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tPST));
    sprintf(OtherMin, "%.2d", minute(tPST));
    sprintf(OtherSec, "%.2d", second(tPST));
  }

  if (secondtimezone == "EST") {
    Serial.println("EST  Other Time Zone...");
    time_t tEST = usET.toLocal(alarmepoch, &tcr);
    strcpy(mEST, monthShortStr(month(tEST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tEST), minute(tEST), second(tEST), dayShortStr(weekday(tEST)), day(tEST), mEST, year(tEST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tEST));
    sprintf(OtherMin, "%.2d", minute(tEST));
    sprintf(OtherSec, "%.2d", second(tEST));
  }
  if (secondtimezone == "CST") {
    Serial.println("CST Other Time Zone...");
    time_t tCST = usCT.toLocal(alarmepoch, &tcr);
    strcpy(mCST, monthShortStr(month(tCST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tCST), minute(tCST), second(tCST), dayShortStr(weekday(tCST)), day(tCST), mCST, year(tCST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tCST));
    sprintf(OtherMin, "%.2d", minute(tCST));
    sprintf(OtherSec, "%.2d", second(tCST));
  }
  if (secondtimezone == "MST") {
    Serial.println("MST Other Time Zone...");
    time_t tMST = usMT.toLocal(alarmepoch, &tcr);
    strcpy(mMST, monthShortStr(month(tMST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tMST), minute(tMST), second(tMST), dayShortStr(weekday(tMST)), day(tMST), mMST, year(tMST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tMST));
    sprintf(OtherMin, "%.2d", minute(tMST));
    sprintf(OtherSec, "%.2d", second(tMST));
  }
  if (secondtimezone == "GMT") {
    Serial.println("GMT Other Time Zone...");
    time_t tGMT = UK.toLocal(alarmepoch, &tcr);
    strcpy(mGMT, monthShortStr(month(tGMT)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tGMT), minute(tGMT), second(tGMT), dayShortStr(weekday(tGMT)), day(tGMT), mGMT, year(tGMT), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tGMT));
    sprintf(OtherMin, "%.2d", minute(tGMT));
    sprintf(OtherSec, "%.2d", second(tGMT));
  }
  if (secondtimezone == "CET") {
    Serial.println("CET Other Time Zone...");
    time_t tCET = CE.toLocal(alarmepoch, &tcr);
    strcpy(mCET, monthShortStr(month(tCET)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tCET), minute(tCET), second(tCET), dayShortStr(weekday(tCET)), day(tCET), mCET, year(tCET), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tCET));
    sprintf(OtherMin, "%.2d", minute(tCET));
    sprintf(OtherSec, "%.2d", second(tCET));
  }
  if (secondtimezone == "AST") {
    Serial.println("AST Other Time Zone...");
    time_t tAST = AAST.toLocal(alarmepoch, &tcr);
    strcpy(mAST, monthShortStr(month(tAST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tAST), minute(tAST), second(tAST), dayShortStr(weekday(tAST)), day(tAST), mAST, year(tAST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tAST));
    sprintf(OtherMin, "%.2d", minute(tAST));
    sprintf(OtherSec, "%.2d", second(tAST));
  }
  if (secondtimezone == "ZST") {
    Serial.println("AZST Other Time Zone...");
    time_t tAZST = usAZ.toLocal(alarmepoch, &tcr);
    strcpy(mAZDT, monthShortStr(month(tAZST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tAZST), minute(tAZST), second(tAZST), dayShortStr(weekday(tAZST)), day(tAZST), mAZDT, year(tAZST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tAZST));
    sprintf(OtherMin, "%.2d", minute(tAZST));
    sprintf(OtherSec, "%.2d", second(tAZST));
  }
  if (secondtimezone == "KST") {
    Serial.println("AKST Other Time Zone...");
    time_t tAKDT = usAK.toLocal(alarmepoch, &tcr);
    strcpy(mAKDT, monthShortStr(month(tAKDT)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tAKDT), minute(tAKDT), second(tAKDT), dayShortStr(weekday(tAKDT)), day(tAKDT), mAKDT, year(tAKDT), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tAKDT));
    sprintf(OtherMin, "%.2d", minute(tAKDT));
    sprintf(OtherSec, "%.2d", second(tAKDT));
  }
  if (secondtimezone == "HST") {
    Serial.println("HST Other Time Zone...");
    time_t tHST = usHI.toLocal(alarmepoch, &tcr);
    strcpy(mHST, monthShortStr(month(tHST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tHST), minute(tHST), second(tHST), dayShortStr(weekday(tHST)), day(tHST), mHST, year(tHST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(OtherHour, "%.2d", hour(tHST));
    sprintf(OtherMin, "%.2d", minute(tHST));
    sprintf(OtherSec, "%.2d", second(tHST));
  }
//
String foo = (String)PSTMon;
      int bar = foo.toInt();
      int www = bar;
      if (www < 10) {
        String foo2 = (String)www;
        String prefix = "0";
        String bar2 = prefix + foo2;
        bar2.toCharArray(PSTMon, 3);
      } else {
        String bar2 = (String)www;
        bar2.toCharArray(PSTMon, 3);
      }

String fooD = (String)PSTDay;
      int barD = fooD.toInt();
      int wwwD = barD;
      if (wwwD < 10) {
        String foo2D = (String)wwwD;
        String prefixD = "0";
        String bar2D = prefixD + foo2D;
        bar2D.toCharArray(PSTDay, 3);
      } else {
        String bar2D = (String)wwwD;
        bar2D.toCharArray(PSTDay, 3);
      }

  // There may be a better way to do all this, but I don't know what it is.  but since we're driving the digits in different colours, I figured we needed
  // to drive the digits entirely independently.  so in order to do this, I need to break the time down into separate digits.

  String PSTHours = String(PSTHour);
//  if ((String)owner.twelvehr == "on") {
//    //  Serial.println("12 hour Time!!!");
//    String fooA = (String)PSTHour;
//    int barA = fooA.toInt();
//    if ((int)barA > 12) {
//      //  Serial.println ("I shouldnt be here for 12 hr time");
//      String foo = (String)PSTHour;
//      int bar = foo.toInt();
//      int www = bar - 12;
//      if (www < 10) {
//        String foo2 = (String)www;
//        String prefix = "0";
//        String bar2 = prefix + foo2;
//        bar2.toCharArray(PSTHour, 3);
//      } else {
//        String bar2 = (String)www;
//        bar2.toCharArray(PSTHour, 3);
//      }
//    } else {
//      //  Serial.println ("I should be here for 24 hr time");
//      String foo = (String)PSTHour;
//      int bar = foo.toInt();
//      int www = bar;
//      if (www < 10) {
//        String foo2 = (String)www;
//        String prefix = "0";
//        String bar2 = prefix + foo2;
//        bar2.toCharArray(PSTHour, 3);
//      } else {
//        String bar2 = (String)www;
//        bar2.toCharArray(PSTHour, 3);
//      }
//    }
//  }
Serial.println("---------------------------------------------");
  Serial.println((String)PSTHour);
  Serial.println(PSTDOW);
Serial.println(PSTYear);
Serial.println(PSTMon);


Serial.println(PSTDay);
Serial.println("---------------------------------------------");
  String PSTMins = String(PSTMin);
  String PSTSeconds = String(PSTSec);
  String PSTMinute1 = String(PSTMin).substring(0, 1);
  String PSTMinute2 = String(PSTMin).substring(1);
  String PSTHour1 = String(PSTHour).substring(0, 1);
  String PSTHour2 = String(PSTHour).substring(1);
String PSTDOW1 = String(PSTDOW).substring(0, 1);
String PSTDOW2 = String(PSTDOW).substring(1, 2);
String PSTDOW3 = String(PSTDOW).substring(2);
String PSTDay1 = String(PSTDay).substring(0, 1);
String PSTDay2 = String(PSTDay).substring(1);
String PSTMon1 = String(PSTDay).substring(0, 1);
String PSTMon2 = String(PSTDay).substring(1);


  String OtherMins = String(OtherMin);
  String OtherSeconds = String(OtherSec);
  String OtherMinute1 = String(OtherMin).substring(0, 1);
  String OtherMinute2 = String(OtherMin).substring(1);
  String OtherHour1 = String(OtherHour).substring(0, 1);
  String OtherHour2 = String(OtherHour).substring(1);


  Serial.println((String)PSTHour1);
  Serial.println((String)PSTHour2);
  Serial.println((String)OtherHour1);
  Serial.println((String)OtherHour2);
  rtc.clearAlarm(1);
  rtc.clearAlarm(2);
  // now that the time's been split up, we can output it all to the digits of the clock.
  // I'm doing each digit separately.  again as with before, there's probably way better ways of doing this.

  if ((PSTHour1 == "1") && (PSTHour2 == "2") && (PSTMinute1 == "0") && (PSTMinute2 == "0")) {
    // it's midnight, so re-run the time sync
    checktime = 1;
  }

 // outputDigits(PSTHour1.toInt(), PSTHour2.toInt(), PSTMinute1.toInt(), PSTMinute2.toInt());
 // outputDigitsRow2 (OtherHour1.toInt(), OtherHour2.toInt(), OtherMinute1.toInt(), OtherMinute2.toInt());
}

void loop(){
  
}
void GetTime() {
  //ok we're going to try and do DNS Lookups
  IPAddress result;
  int err = WiFi.hostByName("0.us.pool.ntp.org", result) ;
  Serial.print("Getting Time from IP : ");
  Serial.println(result);
  if (err == 1) {
    Serial.println("--------------------------------------");
    Serial.println(result);
    Udp.begin(localPort);
    sendNTPpacket(result);  // send an NTP packet to a time server
    delay(3000);
    if (Udp.parsePacket()) {
      Serial.println("packet received");
      //debugoutput(0, 0, 0, 1, 0, 0, 255); // turn the dots blue
      // We've received a packet, read the data from it
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer
      delay(1000);
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      unsigned long epoch = secsSince1900 - seventyYears;
      rtc.begin();  // initialize RTC
      rtc.disable32K();
      rtc.adjust(DateTime(epoch + 4));
      rtc.clearAlarm(1);
      rtc.disable32K();
      pinMode(CLOCK_INTERRUPT_PIN, INPUT_PULLUP);
      attachInterrupt(digitalPinToInterrupt(CLOCK_INTERRUPT_PIN), alarm, FALLING);
      rtc.clearAlarm(1);
      rtc.clearAlarm(2);
      rtc.writeSqwPinMode(DS3231_OFF);
      rtc.setAlarm2(rtc.now(), DS3231_A2_PerMinute);
      rtc.setAlarm1(DateTime(0, 0, 0, 0, 0, 55), DS3231_A1_Second);

      if (checktime == 1) {
        checktime = 0;
      } else {
        SetTime();
      }
    } else {
      //debugoutput(1, 1, 1, 0, 255, 75, 250);
      //didn't get NTP Packet
      delay(10000);
    }
  } else {
    Serial.print("Error code: ");
    Serial.println(err);
    //debugoutput(1, 1, 1, 0, 150, 150, 150); // failed on dns lookups
    delay(10000);
  }
}

void alarm() {
  if (rtc.alarmFired(1)) {
    Serial.println("Alarm1 Triggered");
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
//    if ((String)owner.temp == "on") {
//      String TempUnits = "C";
//      int Temperature = 45;
//      if ((String)owner.tempUnits == "F") {
//        TempUnits = "F";
//      } else {
//        TempUnits = "C";
//      }
//      ////   outputDigitsTemp(rtc.getTemperature(), TempUnits);
//    }
  } else {
    Serial.println("Alarm1 Triggered2");
//    Serial.println(timezone);
//    Serial.println(secondtimezone);
    SetTime();
  }
}

unsigned long sendNTPpacket(IPAddress & address) {
  Serial.println("1");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  Serial.println("2");
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

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
void printWifiStatus() {
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
