/*

   Dual Timezone RGB LED Clock
   by Raina

   This uses code from Michael Marolis and Tom Igoe to pull the current time from an NTP server.

   the rest is all mine.  it basically just then works out the timezone of PST, and GMT, using Timezone.h and displays the current time on two 4 digit 7 segment displays.
   in my case those are oversized 3d prints taken from
   https://nerdiy.de/en/howto-elektronik-sieben-segment-ziffer-pxldigit-aus-ws2812-leds-bauen/

   GPL License applies etc etc etc
*/

#include <Adafruit_NeoPixel.h>
#include <RTCZero.h>
#include <SPI.h>
#include <Timezone.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
//#include <FastLED.h>
RTCZero rtc;
int status = WL_IDLE_STATUS;
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "bar";  // your network SSID (name)
char pass[] = "foo";  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;     // your network key index number (needed only for WEP)

unsigned int localPort = 2390;  // local port to listen for UDP packets

IPAddress timeServer(128, 138, 141, 172);  // time.nist.gov NTP server

const int NTP_PACKET_SIZE = 48;  // NTP timestamp is in the first 48 bytes of the message

byte packetBuffer[NTP_PACKET_SIZE];  // buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};  // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};   // Standard Time
Timezone UK(BST, GMT);
// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usPT(usPDT, usPST);
TimeChangeRule utcRule = {"UTC", Last, Sun, Mar, 1, 0};  // UTC
Timezone UTC(utcRule);
#define LED_PIN 4
#define lowerdigitLED_PIN 5
#define LED_COUNT 56
const int pResistor = A0;
int lightvalue;
int pixels[172];
Adafruit_NeoPixel Upperstrip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel Lowerstrip(LED_COUNT, lowerdigitLED_PIN, NEO_GRB + NEO_KHZ800);
// CRGB leds[LED_COUNT];
int Sized[] = {18, 6, 15, 15, 12, 15, 18, 9, 21, 18};
int Sizedoff[] = {3, 15, 6, 6, 9, 6, 3, 12, 0, 3};

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    pinMode(pResistor, INPUT);

    //    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
    //  FastLED.setBrightness(50);
    Upperstrip.begin();  // INITIALIZE NeoPixel Upperstrip object (REQUIRED)
    Upperstrip.show();   // Turn OFF all pixels ASAP
    Upperstrip.clear();
    Upperstrip.setBrightness(150);
    Upperstrip.setPixelColor(42, Upperstrip.Color(255, 0, 0));
    Upperstrip.setPixelColor(43, Upperstrip.Color(255, 0, 0));
    Upperstrip.setPixelColor(128, Upperstrip.Color(255, 0, 0));
    Upperstrip.setPixelColor(129, Upperstrip.Color(255, 0, 0));
    Upperstrip.show();
    //  // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true)
            ;
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
    }

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Upperstrip.setPixelColor(42, Upperstrip.Color(255, 150, 0));
        Upperstrip.setPixelColor(43, Upperstrip.Color(255, 150, 0));
        Upperstrip.setPixelColor(128, Upperstrip.Color(255, 150, 0));
        Upperstrip.setPixelColor(129, Upperstrip.Color(255, 150, 0));
        Upperstrip.show();
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(ssid, pass);

        // wait 10 seconds for connection:
        delay(10000);
    }

    Serial.println("Connected to WiFi");
    Upperstrip.setPixelColor(42, Upperstrip.Color(0, 255, 0));
    Upperstrip.setPixelColor(43, Upperstrip.Color(0, 255, 0));
    Upperstrip.setPixelColor(128, Upperstrip.Color(0, 255, 0));
    Upperstrip.setPixelColor(129, Upperstrip.Color(0, 255, 0));
    Upperstrip.show();

    printWifiStatus();

    Serial.println("\nStarting connection to server...");
    Udp.begin(localPort);
    sendNTPpacket(timeServer);  // send an NTP packet to a time server
    // wait to see if a reply is available
    delay(1000);
    if (Udp.parsePacket()) {
        Serial.println("packet received");
        Upperstrip.setPixelColor(42, Upperstrip.Color(0, 0, 255));
        Upperstrip.setPixelColor(43, Upperstrip.Color(0, 0, 255));
        Upperstrip.setPixelColor(128, Upperstrip.Color(0, 0, 255));
        Upperstrip.setPixelColor(129, Upperstrip.Color(0, 0, 255));
        Upperstrip.show();
        // We've received a packet, read the data from it
        Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer
        // the timestamp starts at byte 40 of the received packet and is four bytes,
        //  or two words, long. First, extract the two words:
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        // combine the four bytes (two words) into a long integer
        // this is NTP time (seconds since Jan 1 1900):
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        const unsigned long seventyYears = 2208988800UL;
        // subtract seventy years:
        unsigned long epoch = secsSince1900 - seventyYears;
        //    Serial.println(epoch);

        rtc.begin();  // initialize RTC
        rtc.setEpoch(epoch + 1);
        rtc.setAlarmTime(0, 0, 59);
        // alarm when seconds match the alarm time:
        rtc.enableAlarm(rtc.MATCH_SS);
        // attach a function to the alarm:
        rtc.attachInterrupt(alarm);
        // print the hour, minute and second:
    }
}

void alarm() {
    /*
       this is what handles the actual updating of the time.  it's an Alarm interrupt on the RTC.
    */
    int alarmepoch = rtc.getEpoch();
    char bufPST[40];
    char mPST[4];
    char bufUK[40];
    char mUK[4];
    TimeChangeRule *tcr;
    time_t tPST = usPT.toLocal(alarmepoch, &tcr);
    time_t tUK = UK.toLocal(alarmepoch, &tcr);
    char PSTHour[3];
    char PSTMin[3];
    char PSTSec[3];
    char UKHour[3];
    char UKMin[3];
    char UKSec[3];
    // There may be a better way to do all this, but I don't know what it is.  but since we're driving the digits in different colours, I figured we needed
    // to drive the digits entirely independently.  so in order to do this, I need to break the time down into separate digits.
    strcpy(mUK, monthShortStr(month(tUK)));
    sprintf(bufUK, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tUK), minute(tUK), second(tUK), dayShortStr(weekday(tUK)), day(tUK), mUK, year(tUK), tcr->abbrev);
    strcpy(mPST, monthShortStr(month(tPST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tPST), minute(tPST), second(tPST), dayShortStr(weekday(tPST)), day(tPST), mPST, year(tPST), tcr->abbrev);

    Serial.println(bufPST);
    Serial.println(bufUK);

    sprintf(PSTHour, "%.2d", hour(tPST));
    sprintf(PSTMin, "%.2d", minute(tPST));
    sprintf(PSTSec, "%.2d", second(tPST));
    sprintf(UKHour, "%.2d", hour(tUK));
    sprintf(UKMin, "%.2d", minute(tUK));
    sprintf(UKSec, "%.2d", second(tUK));

    String PSTHours = String(PSTHour);
    String PSTMins = String(PSTMin);
    String PSTSeconds = String(PSTSec);
    String UKHours = String(UKHour);
    String UKMins = String(UKMin);
    String UKSeconds = String(UKSec);

    String PSTMinute1 = String(PSTMin).substring(0, 1);
    String PSTMinute2 = String(PSTMin).substring(1);
    String UKMinute1 = String(UKMin).substring(0, 1);
    String UKMinute2 = String(UKMin).substring(1);

    String PSTHour1 = String(PSTHour).substring(0, 1);
    String PSTHour2 = String(PSTHour).substring(1);
    String UKHour1 = String(UKHour).substring(0, 1);
    String UKHour2 = String(UKHour).substring(1);

    // now that the time's been split up, we can output it all to the digits of the clock.
    // I'm doing each digit separately.  again as with before, there's probably way better ways of doing this.
    outputDigits(PSTMinute2.toInt(), PSTMinute1.toInt(), PSTHour2.toInt(), PSTHour1.toInt(), UKMinute2.toInt(), UKMinute1.toInt(), UKHour2.toInt(), UKHour1.toInt());

    if ((PSTHour1 == "0") && (PSTHour2 == "0") && (PSTMinute1 == "0") && (PSTMinute2 == "0")) {
        // it's midnight, so re-run the time sync
        GetTime();
    }
    if ((PSTHour1 == "1") && (PSTHour2 == "2") && (PSTMinute1 == "0") && (PSTMinute2 == "0")) {
        // it's Noon, so re-run the time sync

        GetTime();
    }
}

void outputDigits(int digit1, int digit2, int digit3, int digit4, int digit5, int digit6, int digit7, int digit8) {
    int nums[][21] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, 16, 17, -1, -1, -1},  // 0
        {0, 1, 2, -1, -1, -1, -1, -1, -1, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},      // 1
        {-1, -1, -1, 3, 4, 5, -1, -1, -1, -1, -1, -1, 12, 13, 14, -1, -1, -1, -1, -1, -1},     // 2
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, -1, -1, -1, 18, 19, 20},  // 3
        {0, 1, 2, -1, -1, -1, -1, -1, -1, 9, 10, 11, -1, -1, -1, -1, -1, -1, 18, 19, 20},      // 4
        {-1, -1, -1, -1, -1, -1, 6, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, 19, 20},     // 5
        {-1, -1, -1, -1, -1, -1, 6, -7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},    // 6
        {0, 1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, 18, 19, 20},     // 7
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 8
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, 19, 20},  // 9

    };
    int numsUK[][21] = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, 16, 17, -1, -1, -1},  // 0
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1, 15, 16, 17, -1, -1, -1},            // 1
        {-1, -1, -1, 3, 4, 5, -1, -1, -1, -1, -1, -1, 12, 13, 14, -1, -1, -1, -1, -1, -1},     // 2
        {-1, -1, -1, 3, 4, 5, 6, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},        // 3
        {0, 1, 2, -1, -1, -1, 6, 7, 8, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1},         // 4
        {-1, -1, -1, -1, -1, -1, 6, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, 19, 20},     // 5
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, 19, 20},  // 6 ////numbers represent the pixels that are NOT LIT
        {-1, -1, -1, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1, -1, -1, 15, 16, 17, -1, -1, -1},         // 7
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},  // 8
        {-1, -1, -1, -1, -1, -1, 6, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},     // 9

    };

    for (int j = 0; j < 21; j++) {  // first digit.  a 3
        pixels[j] = nums[digit1][j];
    }

    for (int j = 21; j < 42; j++) {  // second digit... a 3
        if (nums[digit2][j - 21] == -1) {
            pixels[j] = -1;
        } else {
            pixels[j] = nums[digit2][j - 21] + 21;
        }
    }
    pixels[42] = -1;  // dots
    pixels[43] = -1;  // dots

    for (int j = 44; j < 65; j++) {  // third digit.. a 7
        if (nums[digit3][j - 44] == -1) {
            pixels[j] = -1;
        } else {
            pixels[j] = nums[digit3][j - 44] + 44;
        }
    }
    for (int j = 65; j < 86; j++) {  // fourth digit... a 1
        if (nums[digit4][j - 65] == -1) {
            pixels[j] = -1;
        } else {
            pixels[j] = nums[digit4][j - 65] + 65;
        }
    }

    for (int j = 86; j < 107; j++) {  // fifth digit... a 3
        if (numsUK[digit5][j - 86] == -1) {
            pixels[j] = -1;
        } else {
            pixels[j] = nums[digit5][j - 86] + 86;
        }
    }
    for (int j = 107; j < 128; j++) {  // sixth digit.. a 3
        if (numsUK[digit6][j - 107] == -1) {
            pixels[j] = -1;
        } else {
            pixels[j] = nums[digit6][j - 107] + 107;
        }
    }
    pixels[128] = -1;  // dots
    pixels[129] = -1;  // dots

    for (int j = 130; j < 151; j++) {  // sevenths digit... a 4
        if (numsUK[digit7][j - 130] == -1) {
            pixels[j] = -1;
        } else {
            pixels[j] = nums[digit7][j - 130] + 130;
        }
    }
    for (int j = 151; j < 172; j++) {  // eighth digit.. a 0
        if (numsUK[digit8][j - 151] == -1) {
            pixels[j] = -1;
        } else {
            pixels[j] = nums[digit8][j - 151] + 151;
        }
    }
}
uint32_t Wheel(byte WheelPos) {
    if (WheelPos < 85) {
        return Upperstrip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        return Upperstrip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    } else {
        WheelPos -= 170;
        return Upperstrip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
}

void loop() {
    //   lightvalue = analogRead(pResistor);
    uint16_t i, j;

    for (j = 0; j < 256; j++) {
        for (i = 0; i < Upperstrip.numPixels(); i++) {
            if (pixels[i] == -1) {
                Upperstrip.setPixelColor(i, Wheel((i * 21 + j) & 255));
            } else {
                Upperstrip.setPixelColor(i, Upperstrip.Color(0, 0, 0));
            }
        }
        Upperstrip.show();
        //      Serial.println(lightvalue);
        //    if (lightvalue < 150) {
        //       Upperstrip.setBrightness(20);
        //      Upperstrip.show();
        //    }else{
        //       Upperstrip.setBrightness(150);
        //    }
        // Serial.println(lightvalue);
        delay(15);
    }
    /*
       This is for the photoresistor I put in, so the brightness of the clock can auto adjust based on ambient light levels.  needs some tweaking for my
       environment.  ie right now at 9:00am with the kitchen light on and one of the curtains opened, this value is around 290-311, which would put it in the medium range.

       at 15:32, both curtains open, kitchen light on.. value is about 270-280

    */
    //
    //  lightvalue = analogRead(pResistor);
    //  Serial.println(lightvalue);
    //  if (lightvalue > 350) {
    //     Upperstrip.setBrightness(255);
    //    Upperstrip.show();
    //  }
    //  if ((lightvalue > 250) && (lightvalue < 350)) {
    //     Upperstrip.setBrightness(150);
    // Upperstrip.show();
    //  }
    //  if (lightvalue < 250) {
    //     Upperstrip.setBrightness(20);
    //    Upperstrip.show();
    //  }
    //  delay(1000);
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress &address) {
    // Serial.println("1");
    //  set all bytes in the buffer to 0
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    // Initialize values needed to form NTP request
    // (see URL above for details on the packets)
    // Serial.println("2");
    packetBuffer[0] = 0b11100011;  // LI, Version, Mode
    packetBuffer[1] = 0;           // Stratum, or type of clock
    packetBuffer[2] = 6;           // Polling Interval
    packetBuffer[3] = 0xEC;        // Peer Clock Precision
    // 8 bytes of zero for Root Delay & Root Dispersion
    packetBuffer[12] = 49;
    packetBuffer[13] = 0x4E;
    packetBuffer[14] = 49;
    packetBuffer[15] = 52;

    // Serial.println("3");

    // all NTP fields have been given values, now
    // you can send a packet requesting a timestamp:
    Udp.beginPacket(address, 123);  // NTP requests are to port 123
    // Serial.println("4");
    Udp.write(packetBuffer, NTP_PACKET_SIZE);
    // Serial.println("5");
    Udp.endPacket();
    // Serial.println("6");
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

void GetTime() {
    Upperstrip.setPixelColor(42, Upperstrip.Color(255, 0, 0));
    Upperstrip.setPixelColor(43, Upperstrip.Color(255, 0, 0));
    Upperstrip.setPixelColor(128, Upperstrip.Color(255, 0, 0));
    Upperstrip.setPixelColor(129, Upperstrip.Color(255, 0, 0));
    Upperstrip.show();
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Communication with WiFi module failed!");
        // don't continue
        while (true)
            ;
    }

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
        Serial.println("Please upgrade the firmware");
    }

    // attempt to connect to WiFi network:
    while (status != WL_CONNECTED) {
        Upperstrip.setPixelColor(42, Upperstrip.Color(255, 150, 0));
        Upperstrip.setPixelColor(43, Upperstrip.Color(255, 150, 0));
        Upperstrip.setPixelColor(128, Upperstrip.Color(255, 150, 0));
        Upperstrip.setPixelColor(129, Upperstrip.Color(255, 150, 0));
        Upperstrip.show();
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        status = WiFi.begin(ssid, pass);
        delay(10000);
    }

    Serial.println("Connected to WiFi");
    Upperstrip.setPixelColor(42, Upperstrip.Color(0, 255, 0));
    Upperstrip.setPixelColor(43, Upperstrip.Color(0, 255, 0));
    Upperstrip.setPixelColor(128, Upperstrip.Color(0, 255, 0));
    Upperstrip.setPixelColor(129, Upperstrip.Color(0, 255, 0));
    Upperstrip.show();
    printWifiStatus();

    Serial.println("\nStarting connection to server...");
    Udp.begin(localPort);
    sendNTPpacket(timeServer);  // send an NTP packet to a time server
    delay(1000);
    if (Udp.parsePacket()) {
        Serial.println("packet received");
        Upperstrip.setPixelColor(42, Upperstrip.Color(0, 0, 255));
        Upperstrip.setPixelColor(43, Upperstrip.Color(0, 0, 255));
        Upperstrip.setPixelColor(128, Upperstrip.Color(0, 0, 255));
        Upperstrip.setPixelColor(129, Upperstrip.Color(0, 0, 255));
        Upperstrip.show();
        Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read the packet into the buffer
        unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
        unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
        unsigned long secsSince1900 = highWord << 16 | lowWord;
        const unsigned long seventyYears = 2208988800UL;
        unsigned long epoch = secsSince1900 - seventyYears;
        //   rtc.setEpoch(epoch + 1);
    }
}
