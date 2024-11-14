/*
what's left to do.


check for variations on settings values (case)

5. external RTC
6. fix debugging and error handling


*/


#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "hardware/watchdog.h"
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h>
#include <RP2040_RTC.h>
#include <Timezone.h>
#include <NTPClient.h>
#include <microDS18B20.h>
// void handle_OnConnect();
// void handle_update();
// void handle_restart();
// void handle_NotFound();
// void software_reset();

int clearsettings = 6;  //pin the reset button is on
String HTML();
String status = "Connected";
const char* ap_ssid = "RainbowClockSetup";  //Access Point SSID
const char* ap_password = "123456789";      //Access Point Password
uint8_t max_connections = 1;                //Maximum Connection Limit for AP
int current_stations = 0, new_stations = 0;
const char* filename = "/config.json";
int checktime = 0;
datetime_t alarmT;
volatile bool alarmTriggered = false;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
#define LED_PIN 1
#define LED_COUNT 58
MicroDS18B20<22> tempsensor;
const int pResistor = 26;
int lightvalue;
int buttonState = 0;
//photoresistor on 19
int temperature;
int pixels[58];
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
IPAddress local_IP(192, 168, 4, 1);  // Set your desired static IP address
IPAddress gateway(192, 168, 4, 1);   // Usually the same as the IP address
IPAddress subnet(255, 255, 255, 0);
IPAddress IP;
JsonDocument doc;
int WifiSetup = 1;
WebServer server(80);
TimeChangeRule usEDT = { "EDT", Second, Sun, Mar, 2, -240 };  // Eastern Daylight Time = UTC - 4 hours
TimeChangeRule usEST = { "EST", First, Sun, Nov, 2, -300 };   // Eastern Standard Time = UTC - 5 hours
Timezone usET(usEDT, usEST);
TimeChangeRule usPDT = { "PDT", Second, Sun, Mar, 2, -420 };
TimeChangeRule usPST = { "PST", First, Sun, Nov, 2, -480 };
Timezone usPT(usPDT, usPST);
TimeChangeRule usMDT = { "MDT", Second, Sun, Mar, 2, -360 };
TimeChangeRule usMST = { "MST", First, Sun, Nov, 2, -420 };
Timezone usMT(usMDT, usMST);
Timezone usAZ(usMST);
TimeChangeRule usCDT = { "CDT", Second, Sun, Mar, 2, -300 };
TimeChangeRule usCST = { "CST", First, Sun, Nov, 2, -360 };
Timezone usCT(usCDT, usCST);
TimeChangeRule usAKDT = { "AKDT", Second, Sun, Mar, 2, -480 };
TimeChangeRule usAKST = { "AKST", First, Sun, Nov, 2, -540 };
Timezone usAK(usAKDT, usAKST);
TimeChangeRule usHST = { "HST", Second, Sun, Mar, 2, -600 };
Timezone usHI(usHST);
TimeChangeRule IST = { "IST", Last, Sun, Mar, 1, 330 };
Timezone IIST(IST);
TimeChangeRule AST = { "AST", First, Sun, Nov, 2, -180 };
Timezone AAST(AST);
TimeChangeRule CEST = { "CEST", Last, Sun, Mar, 2, 120 };  // Central European Summer Time
TimeChangeRule CET = { "CET ", Last, Sun, Oct, 3, 60 };    // Central European Standard Time
Timezone CE(CEST, CET);
TimeChangeRule BST = { "BST", Last, Sun, Mar, 1, 60 };  // British Summer Time
TimeChangeRule GMT = { "GMT", Last, Sun, Oct, 2, 0 };   // Standard Time
Timezone UK(BST, GMT);
void setup() {
  //Start the serial communication channel
  Serial.begin(9600);
  // while (!Serial)
  //  ;  // Wait untill serial is available
  Serial.println("in Setup");
  pinMode(clearsettings, INPUT_PULLUP);
 analogReadResolution(12);

  // pinMode(pResistor, INPUT);
  // digitalWrite(clearsettings, HIGH);
  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.clear();
  strip.setBrightness(150);
  LittleFS.begin();
  Serial.println("in Setup");
  // File file2 = LittleFS.open(filename, "r");
  // if (!file2) {
  //   Serial.println(F("Failed to read file"));
  //   return;
  // }
  // while (file2.available()) {
  //   Serial.print((char)file2.read());
  // }
  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println(F("Failed to read file  - setting defaults"));
    doc["unsaved"] = "true";
    doc["ssid"] = "";
    doc["password"] = "";
    doc["ShowTemp"] = "on";
    doc["TempUnits"] = "C";
    doc["twelvehr"] = "off";
    doc["timezone"] = "PST";
    doc["dimmer"] = "on";
    doc["range"] = "";
    doc["rangevalue"] = "125";
    //return;
  }
  while (file.available()) {
    DeserializationError error = deserializeJson(doc, file);
    Serial.println((String)doc["ssid"]);
  }
  file.close();
  if ((String)doc["unsaved"] == "true") {  //here if no settings are saved (well if the unsaved flag is true)
    Serial.println();
    Serial.println("Im not here when saved settings are present");
    Serial.println("-----------------------------");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);  // Configure static IP

    //Setting the AP Mode with SSID, Password, and Max Connection Limit
    if (WiFi.softAP(ap_ssid, NULL, 1, false, max_connections) == true) {
      Serial.print("Access Point is Created with SSID: ");
      Serial.println(ap_ssid);
      Serial.print("Max Connections Allowed: ");
      Serial.println(max_connections);
      Serial.print("Access Point IP: ");
      Serial.println(WiFi.softAPIP());
      debugoutput(1, 0, 0, 0, 255, 255, 0, 1);  //WAP Started - top row yellow
    } else {
      debugoutput(1, 0, 0, 0, 255, 75, 250, 1);
      Serial.println("Unable to Create Access Point");
    }

    //Specifying the functions which will be executed upon corresponding GET request from the client
    server.on("/", HTTP_GET, handle_OnConnect);
    server.on("/update", handle_update);
    server.on("/restart", handle_restart);
    server.on("/scan", scannetwork);
    server.onNotFound(handle_NotFound);

    //Starting the Server
    server.begin();
    Serial.println("HTTP Server Started");
  } else {

    Serial.println("Normal bootup starting");
    debugoutput(0, 1, 0, 0, 0, 255, 0, 1);
    //we have credentials.  lets try and connect to the wifi
    String returnedip = validateSSID(doc);
    if (returnedip == "0.0.0.0") {  //no IP returned.  probably wifi issues
      // WiFi.end();
      // WiFi.begin();
      debugoutput(1, 0, 1, 0, 255, 0, 0, 1);  //Top and bottom row red - no IP Address Returned.  This is bad.  Your DHCP Server is broken
      Serial.println("Wifi Failed :(");
      WiFi.mode(WIFI_AP);
      Serial.println(local_IP);
      WiFi.softAPdisconnect();
      WiFi.softAPConfig(local_IP, gateway, subnet);  // Configure static IP
      if (WiFi.softAP(ap_ssid, NULL, 1, false, max_connections) == true) {
        Serial.print("Access Point is Created with SSID: ");
        Serial.println(ap_ssid);
        Serial.print("Max Connections Allowed: ");
        Serial.println(max_connections);
        Serial.print("Access Point IP: ");
        Serial.println(WiFi.softAPIP());
        debugoutput(1, 0, 0, 0, 255, 255, 0, 1);  //WAP Started - top row yellow
      } else {
        debugoutput(1, 0, 0, 0, 255, 75, 250, 1);
        Serial.println("Unable to Create Access Point");
      }

      //Specifying the functions which will be executed upon corresponding GET request from the client
      server.on("/", HTTP_GET, handle_OnConnect);
      server.on("/update", handle_update);
      server.on("/restart", handle_restart);
      server.on("/scan", scannetwork);
      server.onNotFound(handle_NotFound);

      //Starting the Server
      server.begin();
      Serial.println("HTTP Server Started");
      printWiFiStatus();
      delay(5000);
    } else {
      //so we got an IP.  yay!
      delay(5000);
      WifiSetup = 0;
      // server.begin();
      debugoutput(0, 0, 0, 1, 0, 255, 0, 0);
      server.on("/", HTTP_GET, handle_OnConnect);
      server.on("/update", handle_update);
      server.on("/restart", handle_restart);
      server.on("/scan", scannetwork);
      server.onNotFound(handle_NotFound);
      server.begin();
      Serial.println("HTTP Server Started");
      getNTPTime();
      // GetTime();
    }
  }
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
const char* macToString(uint8_t mac[6]) {
  static char s[20];
  sprintf(s, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return s;
}
const char* encToString(uint8_t enc) {
  switch (enc) {
    case ENC_TYPE_NONE: return "NONE";
    case ENC_TYPE_TKIP: return "WPA";
    case ENC_TYPE_CCMP: return "WPA2";
    case ENC_TYPE_AUTO: return "AUTO";
  }
  return "UNKN";
}
void printDateTime(time_t t, const char* tz) {
  char buf[32];
  char m[4];  // temporary storage for month string (DateStrings.cpp uses shared buffer)
  strcpy(m, monthShortStr(month(t)));
  sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",
          hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tz);
  Serial.println(buf);
}
void displayRTCTime() {
  //   static unsigned long displayRTCTime_timeout = 0;

  // #define DISPLAY_RTC_INTERVAL     60000L

  //   // Send status report every STATUS_REPORT_INTERVAL (60) seconds: we don't need to display frequently.
  //   if ((millis() > displayRTCTime_timeout) || (displayRTCTime_timeout == 0))
  //   {
  //     Serial.println("============================");
  //     displayTime();

  //     displayRTCTime_timeout = millis() + DISPLAY_RTC_INTERVAL;
  //   }
}
void loop() {


  if (digitalRead(clearsettings) == HIGH) {

  } else {
    Serial.println("on");
    LittleFS.remove(filename);
    delay(2000);
    software_reset();
  }

  server.handleClient();

  //Continuously check how many stations are connected to Soft AP and notify whenever a new station is connected or disconnected
  new_stations = WiFi.softAPgetStationNum();

  if (current_stations < new_stations) {  //Device is Connected
    current_stations = new_stations;
    Serial.print("New Device Connected to SoftAP... Total Connections: ");
    Serial.println(current_stations);
  }

  if (current_stations > new_stations) {  //Device is Disconnected
    current_stations = new_stations;
    Serial.print("Device disconnected from SoftAP... Total Connections: ");
    Serial.println(current_stations);
  }
  uint16_t i, j;
  tempsensor.requestTemp();

  // delay(1000);
  temperature = round(tempsensor.getTemp());

  if (WifiSetup == 1) {
  } else {
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
      Serial.println((String)doc["ssid"]);
       Serial.println((String)doc["ShowTemp"]);
        Serial.println((String)doc["dimmer"]);
         Serial.println((String)doc["rangevalue"]);
    // Serial.println(doc["ShowTemp"]);
 if (doc["dimmer"] == "off") {
      String BrightnessLevel = (String)doc["rangevalue"];
     BrightnessLevel = (String)doc["rangevalue"];
       Serial.println("Setting Brightness to");
      Serial.println((String)BrightnessLevel.toInt());
      strip.setBrightness(BrightnessLevel.toInt());
      strip.show();
    } else {
      lightvalue = analogRead(pResistor);
      Serial.println(lightvalue);
      if (lightvalue > 800) {

        strip.setBrightness(200);
        strip.show();
      }
      if ((lightvalue > 250) && (lightvalue < 700)) {
        strip.setBrightness(150);
        strip.show();
      }
      if ((lightvalue > 100) && (lightvalue < 250)) {
        strip.setBrightness(50);
        strip.show();
      }
      if (lightvalue < 100) {
        strip.setBrightness(5);
        strip.show();
      }
    }


  }

}
void scannetwork() {
  Serial.println("Scanning Network");
  auto cnt = WiFi.scanNetworks();
  if (!cnt) {
    Serial.printf("No networks found\n");
  } else {
    JsonDocument doc;
    Serial.printf("Found %d networks\n\n", cnt);
    Serial.printf("%32s %5s %17s %2s %4s\n", "SSID", "ENC", "BSSID        ", "CH", "RSSI");
    for (auto i = 0; i < cnt; i++) {
      if (WiFi.SSID(i) == "") {
      } else {
        doc[i] = WiFi.SSID(i);
      }
      uint8_t bssid[6];
      WiFi.BSSID(i, bssid);
      Serial.printf("%32s %5s %17s %2d %4ld\n", WiFi.SSID(i), encToString(WiFi.encryptionType(i)), macToString(bssid), WiFi.channel(i), WiFi.RSSI(i));
    }
    //  serializeJsonPretty(doc, server);
    String output;
    serializeJson(doc, output);
    server.send(200, "application/json", output);
  }
}
void handle_OnConnect() {
  status = "Connected to AP";
  Serial.println("Client Connected");
  server.send(200, "text/html", HTML());
}
void handle_update() {
  debugoutput(1, 0, 0, 0, 150, 0, 150, 1);  //top row Purple.  Checking and valdiating settings
  status = "Update initiated...";
  Serial.println("Update initiated...");
  String ssid_text = server.arg("ssid");
  String pwd_text = server.arg("password");
  String ShowTemp = server.arg("ShowTemp");
  String TempUnits = server.arg("TempUnits");
  String twelvehr = server.arg("12hr");
  String timezone = server.arg("timezone");
  String dimmer = server.arg("dimmer");
  String range = server.arg("range");
  String rangevalue = server.arg("rangevalue");
  LittleFS.remove(filename);
  File file = LittleFS.open(filename, "w");
  if (!file) {
    Serial.println(F("Failed to create file"));
    return;
  }

  JsonDocument doc;
  // Set the values in the document
  doc["ssid"] = ssid_text;
  doc["password"] = pwd_text;
  doc["ShowTemp"] = ShowTemp;
  doc["TempUnits"] = TempUnits;
  doc["twelvehr"] = twelvehr;
  doc["timezone"] = timezone;
  doc["dimmer"] = dimmer;
  doc["range"] = range;
  doc["rangevalue"] = rangevalue;

  String returnedip = validateSSID(doc);
  if (returnedip == "0.0.0.0") {
    debugoutput(1, 0, 1, 0, 255, 0, 0, 1);  //Top and bottom row red - no IP Address Returned.  This is bad.  Your DHCP Server is broken
    Serial.println("Wifi Failed");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);  // Configure static IP
    doc["unsaved"] = "true";
    printWiFiStatus();
    delay(2000);
  } else {
    //so we got an IP.  yay!
    delay(2000);
    server.begin();
    debugoutput(1, 0, 1, 0, 0, 0, 255, 1);
    // GetTime();
  }

  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
  Serial.printf("SSID: %s\n", ssid_text.c_str());
  Serial.printf("Password: %s\n", pwd_text.c_str());
  status = "Update complete!";
  delay(1000);
  software_reset();
}
void handle_restart() {
  status = "Restart initiated...";
  Serial.println("Restart initiated...");
  server.send(200, "text/html", HTML());
  delay(700);
  software_reset();
}
void handle_NotFound() {
  server.send(404, "text/plain", "Not found");
}
String HTML() {
  String msg = "<HTML>";
  msg += "<HEAD>";
  msg += "<TITLE>Rainbow Clock Setup</TITLE>";
  msg += "<meta http-equiv='Cache-Control' content='no-cache, no-store, must-revalidate' />";
  msg += "<meta http-equiv='Pragma' content='no-cache' />";
  msg += "<meta http-equiv='Expires' content='0' />";
  msg += "</HEAD>";
  msg += "<BODY>";
  msg += "<style type='text/css'>";
  msg += "*,";
  msg += "*::before,";
  msg += "*::after {";
  msg += "box-sizing: border-box;";
  msg += "}";
  msg += ":root {";
  msg += "--select-border: #777;";
  msg += "--select-focus: blue;";
  msg += "--select-arrow: var(--select-border);";
  msg += "}";
  msg += ".loader_website {";
  msg += "position: fixed;";
  msg += "top: 0;";
  msg += "left: 0px;";
  msg += "z-index: 1100;";
  msg += "width: 100%;";
  msg += "height: 100%;";
  msg += "background-color: rgba(0, 0, 0, 0.5);";
  msg += "display: block;";
  msg += "-webkit-transition: ease-in-out 0.1s;";
  msg += "-moz-transition: ease-in-out 0.1s;";
  msg += "-o-transition: ease-in-out 0.1s;";
  msg += "-ms-transition: ease-in-out 0.1s;";
  msg += "transition: ease-in-out 0.1s;";
  msg += "-webkit-box-sizing: border-box;";
  msg += "-moz-box-sizing: border-box;";
  msg += "-o-box-sizing: border-box;";
  msg += "-ms-box-sizing: border-box;";
  msg += "box-sizing: border-box;";
  msg += "}";
  msg += ".loader_website * {";
  msg += "-webkit-box-sizing: border-box;";
  msg += "-moz-box-sizing: border-box;";
  msg += "-o-box-sizing: border-box;";
  msg += "-ms-box-sizing: border-box;";
  msg += "box-sizing: border-box;";
  msg += "}";

  msg += "body.loader .loader_website span {";
  msg += "top: 18%;";
  msg += "}";

  msg += ".loader_website > span {";
  msg += "display: block;";
  msg += "width: 48px;";
  msg += "height: 48px;";
  msg += "padding: 4px;";
  msg += "background-color: #ffffff;";
  msg += "-webkit-border-radius: 100%;";
  msg += "-moz-border-radius: 100%;";
  msg += "-o-border-radius: 100%;";
  msg += "-ms-border-radius: 100%;";
  msg += "border-radius: 100%;";
  msg += "position: absolute;";
  msg += "left: 50%;";
  msg += "margin-left: -24px;";
  msg += "top: -50px;";

  msg += "-webkit-transition: ease-in-out 0.1s;";
  msg += "-moz-transition: ease-in-out 0.1s;";
  msg += "-o-transition: ease-in-out 0.1s;";
  msg += "-ms-transition: ease-in-out 0.1s;";
  msg += "transition: ease-in-out 0.1s;";

  msg += "-webkit-box-shadow: #000 0px 5px 10px -5px;";
  msg += "-moz-box-shadow: #000 0px 5px 10px -5px;";
  msg += "-o-box-shadow: #000 0px 5px 10px -5px;";
  msg += "-ms-box-shadow: #000 0px 5px 10px -5px;";
  msg += "box-shadow: #000 0px 5px 10px -5px;";
  msg += "}";

  msg += ".loader_website > span > svg {";
  msg += "fill: transparent;";
  msg += "stroke: #563d7c;";
  msg += "stroke-width: 5;";
  msg += "animation: loader_dash 2s ease infinite, loader_rotate 2s linear infinite;";
  msg += "}";

  msg += "@keyframes loader_dash {";
  msg += "0% {";
  msg += "stroke-dasharray: 1, 95;";
  msg += "stroke-dashoffset: 0;";
  msg += "}";
  msg += "50% {";
  msg += "stroke-dasharray: 85, 95;";
  msg += "stroke-dashoffset: -25;";
  msg += "}";
  msg += "100% {";
  msg += "stroke-dasharray: 85, 95;";
  msg += "stroke-dashoffset: -93;";
  msg += "}";
  msg += "}";

  msg += "@keyframes loader_rotate {";
  msg += "0% {";
  msg += "transform: rotate(0deg);";
  msg += "}";
  msg += "100% {";
  msg += "transform: rotate(360deg);";
  msg += "}";
  msg += "}";

  msg += "select {";
  msg += "color: black;";
  msg += "-webkit-appearance: none;";
  msg += "-moz-appearance: none;";
  msg += "appearance: none;";
  msg += "background-color: transparent;";
  msg += "border: none;";
  msg += "padding: 0 1em 0 0;";
  msg += "margin: 0;";
  msg += "width: 100%;";
  msg += "font-family: inherit;";
  msg += "font-size: inherit;";
  msg += "cursor: inherit;";
  msg += "line-height: inherit;";
  msg += "z-index: 1;";
  msg += "outline: none;";
  msg += "}";

  msg += "select::-ms-expand {";
  msg += "display: none;";
  msg += "}";

  msg += ".select {";
  msg += "color: black;";
  msg += "display: grid;";
  msg += "grid-template-areas: 'select';";
  msg += "align-items: center;";
  msg += "position: relative;";
  msg += "min-width: 15ch;";
  msg += "max-width: 30ch;";
  msg += "border: 1px solid var(--select-border);";
  msg += "border-radius: 0.25em;";
  msg += "padding: 0.25em 0.5em;";
  msg += "font-size: 1.25rem;";
  msg += "cursor: pointer;";
  msg += "line-height: 1.1;";
  msg += "background-color: #fff;";
  msg += "background-image: linear-gradient(to top, #f9f9f9, #fff 33%);";
  msg += "}";

  msg += ".select select, .select::after {";
  msg += "grid-area: select;";
  msg += "}";

  msg += ".select:not(.select--multiple)::after {";
  msg += "content: '';";
  msg += "justify-self: end;";
  msg += "width: 0.8em;";
  msg += "height: 0.5em;";
  msg += "background-color: var(--select-arrow);";
  msg += "-webkit-clip-path: polygon(100% 0%, 0 0%, 50% 100%);";
  msg += "clip-path: polygon(100% 0%, 0 0%, 50% 100%);";
  msg += "}";

  msg += "select:focus + .focus {";
  msg += "position: absolute;";
  msg += "top: -1px;";
  msg += "left: -1px;";
  msg += "right: -1px;";
  msg += "bottom: -1px;";
  msg += "border: 2px solid var(--select-focus);";
  msg += "border-radius: inherit;";
  msg += "}";

  msg += "select[multiple] {";

  msg += "height: 6rem;";
  msg += "}";

  msg += "select[multiple] option {";
  msg += "white-space: normal;";
  msg += "outline-color: var(--select-focus);";
  msg += "}";

  msg += ".select--disabled {";
  msg += "cursor: not-allowed;";
  msg += "background-color: #eee;";
  msg += "background-image: linear-gradient(to top, #ddd, #eee 33%);";
  msg += "}";

  msg += "label {";
  msg += "font-size: 1.125rem;";
  msg += "font-weight: 500;";
  msg += "}";

  msg += ".select + label {";
  msg += "margin-top: 2rem;";
  msg += "}";

  msg += "body {";
  msg += "min-height: 100vh;";
  msg += "display: grid;";
  msg += "place-content: center;";
  msg += "grid-gap: 0.5rem;";
  msg += "font-family: 'Baloo 2', sans-serif;";
  msg += "/*background-color: #e9f2fd;*/";
  msg += "padding: 1rem;";
  msg += "}";

  msg += ".formcontent label {";
  msg += "/*color: red;*/";
  msg += "display: block;";
  msg += "}";

  msg += ".formcontent {";
  msg += "text-align: center;";
  msg += "width: 30ch;";
  msg += "margin: 0 auto;";
  msg += "}";

  msg += ".formcontent li {";
  msg += "list-style: none;";
  msg += "padding-top: 10px;";
  msg += "width: 30ch;";
  msg += "}";

  msg += ".formcontent input {";
  msg += "color: black;";
  msg += "display: grid;";
  msg += "grid-template-areas: 'select';";
  msg += "align-items: center;";
  msg += "position: relative;";
  msg += "min-width: 15ch;";
  msg += "max-width: 24ch;";
  msg += "border: 1px solid var(--select-border);";
  msg += "border-radius: 0.25em;";
  msg += "padding: 0.25em 0.5em;";
  msg += "font-size: 1.25rem;";
  msg += "cursor: pointer;";
  msg += "line-height: 1.1;";
  msg += "background-color: #fff;";
  msg += "text-align: center;";
  msg += "background-image: linear-gradient(to top, #f9f9f9, #fff 33%);";
  msg += "}";
  msg += "button#ScanNetwork,button#showpassword_btn {min-width: 16px !important;}";
  msg += ".formcontent button {";
  msg += "color: black;";
  msg += "display: grid;";
  msg += "grid-template-areas: 'select';";
  msg += "align-items: center;";
  msg += "position: relative;";
  msg += "min-width: 15ch;";
  msg += "max-width: 24ch;";
  msg += "border: 1px solid var(--select-border);";
  msg += "border-radius: 0.25em;";
  msg += "padding: 0.25em 0.5em;";
  msg += "font-size: 1.25rem;";
  msg += "cursor: pointer;";
  msg += "line-height: 1.1;";
  msg += "background-color: #fff;";
  msg += "text-align: center;";
  msg += "background-image: linear-gradient(to top, #f9f9f9, #fff 33%);";
  msg += "}";
  msg += ".scannetworks,.showpassword {position: relative;bottom: 34px;left: 31ch;}";
  msg += "html {";
  msg += "background: linear-gradient(145deg,";
  msg += "rgba(43, 9, 82, 1) 9%,";
  msg += "rgba(2, 2, 255, 1) 18%,";
  msg += "rgba(2, 255, 2, 1) 27%,";
  msg += "rgba(196, 181, 0, 1) 36%,";
  msg += "rgba(255, 165, 0, 1) 45%,";
  msg += "rgba(162, 19, 19, 1) 63%,";
  msg += "rgba(255, 165, 0, 1) 72%,";
  msg += "rgba(196, 181, 0, 1) 81%,";
  msg += "rgba(2, 255, 2, 1) 90%,";
  msg += "rgba(2, 2, 255, 1) 99%,";
  msg += "rgba(43, 9, 82, 1) 100%);";

  msg += "background-size: 400% 400%;";
  msg += "animation: gradient 5s ease infinite;";

  msg += "height: 100vh;";
  msg += "width: 100vw;";
  msg += "align-items: center;";
  msg += "justify-content: center;";
  msg += "}";

  msg += "@keyframes gradient {";
  msg += "0% {";
  msg += "background-position: 0% 50%;";
  msg += "}";
  msg += "50% {";
  msg += "background-position: 100% 50%;";
  msg += "}";
  msg += "100% {";
  msg += "background-position: 0% 50%;";
  msg += "}";
  msg += "}";

  msg += ".loader {";
  msg += "border: 16px solid #f3f3f3; /* Light grey */";
  msg += "border-style: solid;";
  msg += "border-width: 0px;";

  msg += "}";

  msg += "@keyframes spin {";
  msg += "0% {";
  msg += "transform: rotate(0deg);";
  msg += "}";
  msg += "100% {";
  msg += "transform: rotate(360deg);";
  msg += "}";
  msg += "}";

  msg += ".slidecontainer {width: 100%;}";

  msg += ".slider {-webkit-appearance: none;width: 100%;height: 25px;background: #d3d3d3;outline: none;opacity: 0.7;-webkit-transition: .2s;transition: opacity .2s;}";

  msg += ".slider:hover {opacity: 1;}";

  msg += ".slider::-webkit-slider-thumb {-webkit-appearance: none;appearance: none;width: 25px;height: 25px;background: #04AA6D;cursor: pointer;}";

  msg += ".slider::-moz-range-thumb {width: 25px;height: 25px;background: #04AA6D;cursor: pointer;}";
  msg += ".slider:disabled::-webkit-slider-thumb{background: grey;cursor: not-allowed !important;}";
  msg += ".slider:disabled::-moz-range-thumb{background: grey;cursor: not-allowed !important;}";
  msg += ".slider:disabled{cursor: not-allowed !important;}";

  msg += "</style>";

  msg += "<header class='container-col'>";

  msg += "</header>";

  msg += "<div class='formcontent'>";
  msg += "<ul>";
  msg += "<Li>";
  msg += "<H3>Enter your SSID and Password below. </h3>";
  msg += "</Li>";
  msg += "<Li>";
  msg += "<label for='ssid'>SSID</label>";
  msg += "<input list='scannedSSIDs' id='ssid' required type='text' name='ssid'";
  msg += "value='";
  msg += (String)doc["ssid"];
  msg += "' placeholder='ssid'>";

  msg += "<datalist id='scannedSSIDs'>";
  msg += "</datalist>";

  msg += "<span class='scannetworks'><button id='ScanNetwork' onclick='scannetwork()'><svg aria-hidden='true' focusable='false' data-prefix='fak' data-icon='solid-wifi-magnifying-glass' role='img' viewBox='0 0 640 512' class='uploaded-icon-preview svg-inline--fa fa-solid-wifi-magnifying-glass' style='line-height: 1; width: 1.25em;'><path fill='currentColor' d='M8.9 202c-12.2-12.8-11.8-33 .9-45.3C90.3 79.5 199.6 32 320 32s229.7 47.5 310.1 124.8c12.7 12.2 13.1 32.5 .9 45.2s-32.4 13.1-45.2 .9C516.8 136.7 423.2 96 320 96S123.2 136.7 54.2 202.9l-.1 0c-12.7 12.3-33 11.9-45.2-.9zM126.7 309.2c-11.7-13.3-10.5-33.5 2.8-45.2C180.2 219.2 247 192 320 192c10.1 0 20.1 .5 29.9 1.5c-21.2 16.5-38.2 38.2-49 63.3c-49.3 4.2-94 24.3-129.1 55.2c-13.3 11.7-33.5 10.5-45.2-2.8zM256 416c0-22.9 12.2-44 32-55.4c1.5-.9 3-1.7 4.6-2.4C303 401 330.7 437 367.8 458.5c-4.5 5.1-9.8 9.4-15.8 12.9c-19.8 11.4-44.2 11.4-64 0s-32-32.6-32-55.4zm64-96c0-70.7 57.3-128 128-128s128 57.3 128 128c0 26.7-8.2 51.4-22.1 71.9L633 471c9.4 9.4 9.4 24.6 0 33.9s-24.6 9.4-33.9 0L520 425.8l-.1 .1C499.4 439.8 474.7 448 448 448c-70.7 0-128-57.3-128-128zm48 0c0 28.6 15.2 55 40 69.3s55.2 14.3 80 0s40-40.7 40-69.3s-15.2-55-40-69.3s-55.2-14.3-80 0s-40 40.7-40 69.3z' class=''></path></svg></button></span>";
  msg += "</Li>";
  msg += "<li>";
  msg += "<label for='password'>Password</label>";
  msg += "<input id='password'";
  msg += "       type='password'";
  msg += "       name='password'";
  msg += "      value='";
  msg += (String)doc["password"];
  msg += "' placeholder='password' required>";
  msg += "<span class='showpassword'>";
  msg += "<button id='showpassword_btn' onclick='showpassword()'><svg xmlns='http://www.w3.org/2000/svg' aria-hidden='true' focusable='false' data-prefix='fak' data-icon='solid-wifi-magnifying-glass' role='img' viewBox='0 0 576 512' class='uploaded-icon-preview svg-inline--fa fa-solid-wifi-magnifying-glass' style='line-height: 1; width: 1.25em;'><path d='M288 32c-80.8 0-145.5 36.8-192.6 80.6C48.6 156 17.3 208 2.5 243.7c-3.3 7.9-3.3 16.7 0 24.6C17.3 304 48.6 356 95.4 399.4C142.5 443.2 207.2 480 288 480s145.5-36.8 192.6-80.6c46.8-43.5 78.1-95.4 93-131.1c3.3-7.9 3.3-16.7 0-24.6c-14.9-35.7-46.2-87.7-93-131.1C433.5 68.8 368.8 32 288 32zM144 256a144 144 0 1 1 288 0 144 144 0 1 1 -288 0zm144-64c0 35.3-28.7 64-64 64c-7.1 0-13.9-1.2-20.3-3.3c-5.5-1.8-11.9 1.6-11.7 7.4c.3 6.9 1.3 13.8 3.2 20.7c13.7 51.2 66.4 81.6 117.6 67.9s81.6-66.4 67.9-117.6c-11.1-41.5-47.8-69.4-88.6-71.1c-5.8-.2-9.2 6.1-7.4 11.7c2.1 6.4 3.3 13.2 3.3 20.3z'></path></svg></button>";


  //<input type='checkbox' onclick='showpassword()'>Show Password";
  msg += "</span></li>";
  msg += "<li>";
  msg += "<label for='ShowTemp'>Show Temperature every 1 minute</label>";
  msg += "<div class='select'>";
  msg += "<select name='ShowTemp' id='ShowTemp'>";
  msg += "<option value='off'>off</option>";
  msg += "<option value='on' ";
  if ((String)doc["ShowTemp"] == "on") {
    msg += "selected=selected ";
  }
  // msg += "ShowTempSetting";
  msg += ">on</option>";
  msg += "</select>";
  msg += "<span class='focus'></span>";
  msg += "</div>";

  msg += "</li>";

  msg += "<li>";
  msg += "<label for='TempUnits'>Temperature Units</label>";
  msg += "<div class='select'>";
  msg += "<select name='TempUnits' id='TempUnits'>";
  msg += "<option value='C'>C</option>";
  msg += "<option value='F' ";
  if ((String)doc["TempUnits"] == "F") {
    msg += "selected=selected ";
  }
  msg += "ShowTempUnitsSetting";
  msg += ">F</option>";

  msg += "</select>";
  msg += "<span class='focus'></span>";
  msg += "</div>";

  msg += "</li>";
  msg += "<li>";
  msg += "<label for='12hr'>Show 12hr Time</label>";
  msg += "<div class='select'>";
  msg += "<select name='12hr' id='12hr'>";
  msg += "<option value='off'>off</option>";
  msg += "<option value='on' ";
  if ((String)doc["twelvehour"] == "on") {
    msg += "selected=selected ";
  }
  msg += ">on</option>";
  msg += "</select>";
  msg += "<span class='focus'></span>";
  msg += "</div>";

  msg += "</li>";
  msg += "<li>";
  msg += "<label for='timezone'>Timezone</label>";
  msg += "<div class='select'>";
  msg += "<select name='timezone' id='timezone'>";
  msg += "<option value='EST' ";
  if ((String)doc["timezone"] == "EST") {
    msg += "selected=selected ";
  }
  // msg += " selected='selected' ";
  // }
  msg += ">Eastern</option>";
  msg += "<option value='CST' ";
  // if ((String)owner.timezone == "CST") {
  if ((String)doc["timezone"] == "CST") {
    msg += "selected=selected ";
  }
  // }
  msg += ">Central</option>";
  msg += "<option value='MST' ";
  // if ((String)owner.timezone == "MST") {
  if ((String)doc["timezone"] == "MST") {
    msg += "selected=selected ";
  }
  // }
  msg += ">Mountain</option>";
  msg += "<option value='PST' ";
  // if ((String)owner.timezone == "PST") {
  if ((String)doc["timezone"] == "PST") {
    msg += "selected=selected ";
  }
  // }
  msg += ">Pacific</option>";
  msg += "<option value='KST' ";
  // if ((String)owner.timezone == "KST") {
  if ((String)doc["timezone"] == "KST") {
    msg += "selected=selected ";
  }
  // }
  msg += ">Alaska</option>";
  msg += "<option value='HST' ";
  // if ((String)owner.timezone == "HST") {
  if ((String)doc["timezone"] == "HST") {
    msg += "selected=selected ";
  }
  // }
  msg += ">Hawaii</option>";
  msg += "<option value='ZST' ";
  // if ((String)owner.timezone == "ZST") {
  if ((String)doc["timezone"] == "ZST") {
    msg += "selected=selected ";
  }
  // }
  msg += ">Arizona</option>";
  msg += "<option value='GMT' ";
  // if ((String)owner.timezone == "GMT") {
  if ((String)doc["timezone"] == "GMT") {
    msg += "selected=selected ";
  }
  // }
  msg += ">GMT</option>";
  msg += "<option value='CET' ";
  // if ((String)owner.timezone == "CET") {
  if ((String)doc["timezone"] == "CET") {
    msg += "selected=selected ";
  }
  // }
  msg += ">Central European Time</option>";
  msg += "<option value='AST' ";
  // if ((String)owner.timezone == "AST") {
  if ((String)doc["timezone"] == "AST") {
    msg += "selected=selected ";
  }
  // }
  msg += ">Argentina Standard Time</option>";
  msg += "<option value='IST' ";
  // if ((String)owner.timezone == "IST") {
  if ((String)doc["timezone"] == "IST") {
    msg += "selected=selected ";
  }
  // }
  msg += ">Indian Standard Time</option>";

  msg += "</select>";

  msg += "<li>";
  msg += "<label for='dimmer'>Enable Auto Dimming</label>";
  msg += "<div class='select'>";
  msg += "<select name='dimmer' id='dimmer'>";
  msg += "<option value='on'>on</option>";
  msg += "<option value='off' ";
  if ((String)doc["dimmer"] == "off") {
    msg += "selected=selected ";
  }
  msg += ">off</option>";
  msg += "</select>";
  msg += "<span class='focus'></span>";
  msg += "</div>";

  msg += "</li>";
  msg += "<li>";

  msg += "<label for='myrange'>Display Brightness</label>";

  msg += "<div class='slidecontainer'>";
  msg += "<input type='range' name='range' min='1' max='255' ";

  msg += "value='";
  msg += (String)doc["rangevalue"];
  // String Brightnesslevel = (String)owner.brightness;
  // Brightnesslevel.trim();
  // msg += "Brightnesslevel";

  msg += "' class='slider' id='myRange'>";
  msg += "<input type='hidden' name='rangevalue' ";
  msg += "value='";
  msg += (String)doc["rangevalue"];
  // msg += "Brightnesslevel";
  msg += "' id='rangevalue'>";
  // msg+=" <p>Value: <span id='demo'></span></p>";
  msg += " </div>";

  msg += "</li>";

  msg += "<li>";
  msg += "<button id='Submit' name='Submit' value='Submit' onclick='submit();' >Save</button>";
  msg += "</li>";
  msg += "</ul>";
  msg += "</div>\n";
  //  msg+="</form>";
  msg += "<script>\n";
  msg += "function showpassword() {\n";
  msg += "var x = document.getElementById('password');\n";
  msg += "if (x.type === 'password') {\n";
  msg += "    x.type = 'text';\n";
  msg += "} else {\n";
  msg += "  x.type = 'password';\n";
  msg += "}\n";
  msg += "}\n";
  msg += "function scannetwork() {\n";
  msg += "Loader.open();\n";
  msg += "var FormValid = true;\n";
  msg += "var mypost\n";

  msg += "var request = new XMLHttpRequest();\n";
  msg += "request.open('POST', '/scan', true);\n";
  msg += "request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');\n";
  msg += "request.onreadystatechange = function () {\n";
  msg += "if (this.readyState == XMLHttpRequest.DONE && this.status == 200) {\n";
  msg += "console.log(this);\n";

  msg += "const json = this.response;\n";
  msg += "const obj = JSON.parse(json);\n";
  msg += "console.log(obj);\n";

  msg += "const datalist = document.getElementById('scannedSSIDs');\n";
  msg += "       var options = '';\n";

  msg += "for(var i = 0; i < datalist.length; i++){\n";
  msg += "options += '<option value=\"'+ datalist[i] +' \" >';\n";
  msg += "}\n";
  msg += "document.getElementById('scannedSSIDs').innerHTML = options;\n";
  msg += "for (const item of obj) {\n";
  msg += "   datalist.appendChild(new Option('', item));\n";
  msg += "}\n";
  msg += "Loader.close()\n";
  msg += "} else {\n";
  msg += "console.log('server error');\n";
  msg += "Loader.close()\n";
  msg += "}\n";
  msg += "};\n";
  msg += "request.onerror = function () {\n";
  msg += "console.log('something went wrong');\n";
  msg += "Loader.close()\n";
  msg += "};\n";
  msg += "request.send(mypost);\n";
  msg += "}\n";
  msg += "if (document.getElementById('dimmer').value === 'off') {";
  msg += "document.getElementById('myRange').removeAttribute('disabled');";
  msg += "} else {";
  msg += "document.getElementById('myRange').setAttribute('disabled', 'disabled');";
  msg += "}";
  msg += "document.getElementById('dimmer').onchange = function () {";
  msg += "document.getElementById('myRange').setAttribute('disabled', 'disabled');";
  msg += "if (this.value == 'off') {";
  msg += "document.getElementById('myRange').removeAttribute('disabled');";
  msg += "}";
  msg += "};";
  msg += "var slider = document.getElementById('myRange');";
  msg += "var output = document.getElementById('rangevalue');";
  msg += "output.value = slider.value;";
  msg += "slider.oninput = function() {";
  msg += "  output.value = this.value;";
  msg += "  slider.value = this.value;";
  msg += "}\n";
  msg += "var Loader = {";
  msg += "loader: null,";
  msg += "body: null,";
  msg += "html: '<span><svg width=\\'40\\' height=\\'40\\' version=\\'1.1\\' xmlns=\\'http://www.w3.org/2000/svg\\'><circle cx=\\'20\\' cy=\\'20\\' r=\\'15\\'></svg></span>',";
  msg += "cssClass: 'loader',";
  msg += "check: function () {";
  msg += "if (this.body == null) {";
  msg += "this.body = document.getElementsByTagName('body')[0];";
  msg += "}";
  msg += "},";
  msg += "open: function () {";
  msg += "this.check();";
  msg += "if (!this.isOpen()) {";
  msg += "this.loader = document.createElement('div');";
  msg += "this.loader.setAttribute('id', 'loader');";
  msg += "this.loader.classList.add('loader_website');";
  msg += "this.loader.innerHTML = this.html;";
  msg += "this.body.appendChild(this.loader);";
  msg += "setTimeout(function () {";
  msg += "Loader.body.classList.add(Loader.cssClass);";
  msg += "}, 1);";
  msg += "}";
  msg += "return this;";
  msg += "},";
  msg += "close: function () {";
  msg += "this.check();";
  msg += "if (this.isOpen()) {";
  msg += "this.body.classList.remove(this.cssClass);";
  msg += "setTimeout(function () {";
  msg += "Loader.loader.remove();";
  msg += "}, 100);";
  msg += "}";
  msg += "return this;";
  msg += "},";
  msg += "isOpen: function () {";
  msg += "this.check();";
  msg += "return this.body.classList.contains(this.cssClass);";
  msg += "},";
  msg += "ifOpened: function (callback, close) {";
  msg += "this.check();";
  msg += "if (this.isOpen()) {";
  msg += "if (!!close)";
  msg += "this.close();";
  msg += "if (typeof callback === 'function') {";
  msg += "callback();";
  msg += "}";
  msg += "}";
  msg += "return this;";
  msg += "},";
  msg += "ifClosed: function (callback, open) {";
  msg += "this.check();";
  msg += "if (!this.isOpen()) {";
  msg += "if (!!open)";
  msg += "this.open();";
  msg += "if (typeof callback === 'function') {";
  msg += "callback();";
  msg += "}";
  msg += "}";
  msg += "return this;";
  msg += "}";
  msg += "};";
  msg += "function submit() {";
  msg += "Loader.open();\n";
  msg += "var FormValid = true;\n";
  msg += "var data = [];\n";

  msg += " const container = document.querySelector('div.formcontent');";
  msg += " container.querySelectorAll('input').forEach(function (e) {";
  msg += "  if (e.validity.valueMissing) {";
  msg += "    FormValid = false;";
  msg += ";  }";

  msg += "  data[e.id] = e.value;";

  msg += "});";
  msg += "container.querySelectorAll('select').forEach(function (e) {";
  msg += "data[e.id] = e.value;";

  msg += "});";

  msg += "encodeDataToURL = (data) => {";
  msg += "return Object";
  msg += ".keys(data)";
  msg += ".map(value => `${value}=${encodeURIComponent(data[value])}`)";
  msg += ".join('&');";
  msg += "}\n";

  // console.log(encodeDataToURL(data));
  msg += "var mypost = encodeDataToURL(data)\n";
  msg += "mypost = mypost + '&Submit=Submit'\n";
  // console.log(mypost);
  msg += "if (FormValid) {";
  msg += "var request = new XMLHttpRequest();";
  msg += "request.open('POST', '/update', true);";
  msg += "request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');";
  msg += "request.setRequestHeader('mydata','/?'+encodeDataToURL(data));";
  msg += "request.onreadystatechange = function () {";
  msg += "if (this.readyState == XMLHttpRequest.DONE && this.status == 200) {";
  msg += "console.log('succeed');";
  msg += "Loader.close()";
  // msg+="el.classList.add('hidden');";
  //   myresponse.value = request.responseText;
  msg += "} else {";
  msg += "console.log('server error');";
  msg += "Loader.close()";

  msg += "}";
  msg += "};";

  msg += "request.onerror = function () {";
  msg += "console.log('something went wrong');";
  msg += "Loader.close()";
  msg += "};";

  msg += "request.send(mypost);";
  msg += "}";
  msg += "}";
  //          msg+="           </script>";

  msg += "</script>";
  msg += "</BODY>";
  msg += "</HTML>";
  // Serial.printf("%s", msg.c_str());
  return msg;
}
void debugoutput(int top, int mid, int bottom, int dots, int R, int G, int B, int clearall) {
  if (clearall == 1) {
    strip.show();  // Turn OFF all pixels ASAP
    strip.clear();
  }
  if (top == 1) {
    strip.setPixelColor(6, strip.Color(R, G, B));
    strip.setPixelColor(7, strip.Color(R, G, B));
    strip.setPixelColor(20, strip.Color(R, G, B));
    strip.setPixelColor(21, strip.Color(R, G, B));
    strip.setPixelColor(36, strip.Color(R, G, B));
    strip.setPixelColor(37, strip.Color(R, G, B));
    strip.setPixelColor(50, strip.Color(R, G, B));
    strip.setPixelColor(51, strip.Color(R, G, B));
  }
  if (mid == 1) {
    strip.setPixelColor(10, strip.Color(R, G, B));
    strip.setPixelColor(11, strip.Color(R, G, B));
    strip.setPixelColor(24, strip.Color(R, G, B));
    strip.setPixelColor(25, strip.Color(R, G, B));
    strip.setPixelColor(40, strip.Color(R, G, B));
    strip.setPixelColor(41, strip.Color(R, G, B));
    strip.setPixelColor(54, strip.Color(R, G, B));
    strip.setPixelColor(55, strip.Color(R, G, B));
  }
  if (bottom == 1) {
    strip.setPixelColor(0, strip.Color(R, G, B));
    strip.setPixelColor(1, strip.Color(R, G, B));
    strip.setPixelColor(14, strip.Color(R, G, B));
    strip.setPixelColor(15, strip.Color(R, G, B));
    strip.setPixelColor(30, strip.Color(R, G, B));
    strip.setPixelColor(31, strip.Color(R, G, B));
    strip.setPixelColor(44, strip.Color(R, G, B));
    strip.setPixelColor(45, strip.Color(R, G, B));
  }
  if (dots == 1) {
    strip.setPixelColor(28, strip.Color(R, G, B));
    strip.setPixelColor(29, strip.Color(R, G, B));
  }
  strip.show();
}
int numdigits(int i) {
  char str[20];

  sprintf(str, "%d", i);
  return (strlen(str));
}
void software_reset() {
  watchdog_enable(1, 1);
  while (1)
    ;
}
void printWiFiStatus() {
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
String validateSSID(JsonDocument doc) {
  Serial.println((String)doc["ssid"]);
  Serial.println((String)doc["password"]);
  status = WiFi.begin(doc["ssid"], doc["password"]);
  delay(10000);
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("brightness: ");
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  String LocalIP = String() + WiFi.localIP()[0] + "." + WiFi.localIP()[1] + "." + WiFi.localIP()[2] + "." + WiFi.localIP()[3];
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.print("connection status: ");
  return (LocalIP);
}
void getNTPTime() {
  timeClient.begin();
  timeClient.update();
  rtc_init();  //initialize the RTC
  time_t epoch_t = timeClient.getEpochTime();
  rtc_set_datetime(DateTime(epoch_t));  //store the time in UTC into the RTC
  setAlarmRepeat2(0);                   //sets the alarm
  checktime = 0;
  SetTime(doc);  //figures out the current time in local timezone.
}
void setAlarmRepeat(uint8_t alarmSeconds) {
  rtc_get_datetime(&alarmT);
  alarmT.min = alarmT.hour = alarmT.day = alarmT.dotw = alarmT.month = alarmT.year = -1;
  alarmT.sec = alarmSeconds + 1;
  set_RTC_Alarm(&alarmT);

  Serial.print("Set Repeatitive Alarm @ alarmSeconds = ");
  Serial.println(alarmSeconds);
}
void setAlarmRepeat2(uint8_t alarmSeconds) {
  rtc_get_datetime(&alarmT);
  alarmT.min = alarmT.hour = alarmT.day = alarmT.dotw = alarmT.month = alarmT.year = -1;
  alarmT.sec = alarmSeconds + 1;
  set_RTC_Alarm2(&alarmT);
  Serial.print("Set Repeatitive Alarm @ alarmSeconds = ");
  Serial.println(alarmSeconds);
}
void set_RTC_Alarm(datetime_t* alarmTime) {
  rtc_set_alarm(alarmTime, rtcCallback);
}
void set_RTC_Alarm2(datetime_t* alarmTime) {
  rtc_set_alarm(alarmTime, rtcCallback2);
}
void rtcCallback() {
  alarmTriggered = true;
  Serial.println("Alarm triggered.  this should be at 55");
  Serial.print("Temperature (ºC): ");
  Serial.println((String)temperature);
  setAlarmRepeat2(0);
  if (doc["ShowTemp"] == "on") {
    String TempUnits = "C";
    if (doc["TempUnits"] == "F") {
      TempUnits = "F";
    } else {
      TempUnits = "C";
    }
    outputDigitsTemp(temperature, TempUnits);
  }
}
void rtcCallback2() {
  Serial.println("Alarm2 triggered.  this should be at 0");
  SetTime(doc);
  setAlarmRepeat(55);
  alarmTriggered = true;
}
void SetTime(JsonDocument doc) {  //this formats the date/time... which I suppose doesn't act
  Serial.print("UTC Time : ");
  Serial.println(timeClient.getFormattedTime());
  Serial.print("UTC Timestamp : ");
  Serial.println(timeClient.getEpochTime());
  char bufPST[40];
  char mEST[4];
  char mPST[4];
  char mMST[4];
  char mCST[4];
  char mHST[4];
  char mCET[4];
  char mGMT[4];
  char mAKDT[4];
  char mAZDT[4];
  char mAST[4];
  char mIST[4];
  char buf[40];
  char m[4];
  char PSTHour[3];
  char PSTMin[3];
  char PSTSec[3];       // temporary storage for month string (DateStrings.cpp uses shared buffer)
  TimeChangeRule* tcr;  // pointer to the time change rule, use to get the TZ abbrev
  if (doc["timezone"] == "PST") {
    Serial.println("Doing PST");
    time_t tEST = usPT.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mEST, monthShortStr(month(tEST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tEST), minute(tEST), second(tEST), dayShortStr(weekday(tEST)), day(tEST), mEST, year(tEST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tEST));
    sprintf(PSTMin, "%.2d", minute(tEST));
    sprintf(PSTSec, "%.2d", second(tEST));
    Serial.println((String)PSTHour);
    Serial.println((String)PSTMin);
  }
  if (doc["timezone"] == "EST") {
    Serial.println("EST Time Zone...");
    time_t tEST = usET.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mEST, monthShortStr(month(tEST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tEST), minute(tEST), second(tEST), dayShortStr(weekday(tEST)), day(tEST), mEST, year(tEST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tEST));
    sprintf(PSTMin, "%.2d", minute(tEST));
    sprintf(PSTSec, "%.2d", second(tEST));
  }
  if (doc["timezone"] == "CST") {
    Serial.println("CST Time Zone...");
    time_t tCST = usCT.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mCST, monthShortStr(month(tCST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tCST), minute(tCST), second(tCST), dayShortStr(weekday(tCST)), day(tCST), mCST, year(tCST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tCST));
    sprintf(PSTMin, "%.2d", minute(tCST));
    sprintf(PSTSec, "%.2d", second(tCST));
  }
  if (doc["timezone"] == "MST") {
    Serial.println("MST Time Zone...");
    time_t tMST = usMT.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mMST, monthShortStr(month(tMST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tMST), minute(tMST), second(tMST), dayShortStr(weekday(tMST)), day(tMST), mMST, year(tMST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tMST));
    sprintf(PSTMin, "%.2d", minute(tMST));
    sprintf(PSTSec, "%.2d", second(tMST));
  }
  if (doc["timezone"] == "GMT") {
    Serial.println("GMT Time Zone...");
    time_t tGMT = UK.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mGMT, monthShortStr(month(tGMT)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tGMT), minute(tGMT), second(tGMT), dayShortStr(weekday(tGMT)), day(tGMT), mGMT, year(tGMT), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tGMT));
    sprintf(PSTMin, "%.2d", minute(tGMT));
    sprintf(PSTSec, "%.2d", second(tGMT));
  }
  if (doc["timezone"] == "CET") {
    Serial.println("CET Time Zone...");
    time_t tCET = CE.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mCET, monthShortStr(month(tCET)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tCET), minute(tCET), second(tCET), dayShortStr(weekday(tCET)), day(tCET), mCET, year(tCET), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tCET));
    sprintf(PSTMin, "%.2d", minute(tCET));
    sprintf(PSTSec, "%.2d", second(tCET));
  }
  if (doc["timezone"] == "AST") {
    Serial.println("AST Time Zone...");
    time_t tAST = AAST.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mAST, monthShortStr(month(tAST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tAST), minute(tAST), second(tAST), dayShortStr(weekday(tAST)), day(tAST), mAST, year(tAST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tAST));
    sprintf(PSTMin, "%.2d", minute(tAST));
    sprintf(PSTSec, "%.2d", second(tAST));
  }
  if (doc["timezone"] == "IST") {
    Serial.println("IST Time Zone...");
    time_t tIST = IIST.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mIST, monthShortStr(month(tIST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tIST), minute(tIST), second(tIST), dayShortStr(weekday(tIST)), day(tIST), mIST, year(tIST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tIST));
    sprintf(PSTMin, "%.2d", minute(tIST));
    sprintf(PSTSec, "%.2d", second(tIST));
  }
  if (doc["timezone"] == "ZST") {
    Serial.println("AZST Time Zone...");
    time_t tAZST = usAZ.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mAZDT, monthShortStr(month(tAZST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tAZST), minute(tAZST), second(tAZST), dayShortStr(weekday(tAZST)), day(tAZST), mAZDT, year(tAZST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tAZST));
    sprintf(PSTMin, "%.2d", minute(tAZST));
    sprintf(PSTSec, "%.2d", second(tAZST));
  }
  if (doc["timezone"] == "KST") {
    Serial.println("AKST Time Zone...");
    time_t tAKDT = usAK.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mAKDT, monthShortStr(month(tAKDT)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tAKDT), minute(tAKDT), second(tAKDT), dayShortStr(weekday(tAKDT)), day(tAKDT), mAKDT, year(tAKDT), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tAKDT));
    sprintf(PSTMin, "%.2d", minute(tAKDT));
    sprintf(PSTSec, "%.2d", second(tAKDT));
  }
  if (doc["timezone"] == "HST") {
    Serial.println("HST Time Zone...");
    time_t tHST = usHI.toLocal(timeClient.getEpochTime(), &tcr);
    strcpy(mHST, monthShortStr(month(tHST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s", hour(tHST), minute(tHST), second(tHST), dayShortStr(weekday(tHST)), day(tHST), mHST, year(tHST), tcr->abbrev);
    Serial.println(bufPST);
    sprintf(PSTHour, "%.2d", hour(tHST));
    sprintf(PSTMin, "%.2d", minute(tHST));
    sprintf(PSTSec, "%.2d", second(tHST));
  }

  String PSTHours = String(PSTHour);
  if (doc["twelvehr"] == "On") {
    //  Serial.println("12 hour Time!!!");
    // Serial.println((int)PSTHour);
    String fooA = (String)PSTHour;
    int barA = fooA.toInt();
    if ((int)barA > 12) {
      //  Serial.println ("I shouldnt be here for 12 hr time");
      String foo = (String)PSTHour;
      int bar = foo.toInt();
      int www = bar - 12;
      if (www < 10) {
        String foo2 = (String)www;
        String prefix = "0";
        String bar2 = prefix + foo2;
        bar2.toCharArray(PSTHour, 3);
      } else {
        String bar2 = (String)www;
        bar2.toCharArray(PSTHour, 3);
      }
    } else {
      //  Serial.println ("I should be here for 12 hr time");
      String foo = (String)PSTHour;
      int bar = foo.toInt();
      int www = bar;
      if (www < 10) {
        String foo2 = (String)www;
        String prefix = "0";
        String bar2 = prefix + foo2;
        bar2.toCharArray(PSTHour, 3);
      } else {
        String bar2 = (String)www;
        bar2.toCharArray(PSTHour, 3);
      }
    }
  }
  Serial.println((String)PSTHour);
  String PSTMins = String(PSTMin);
  String PSTSeconds = String(PSTSec);
  String PSTMinute1 = String(PSTMin).substring(0, 1);
  String PSTMinute2 = String(PSTMin).substring(1);
  String PSTHour1 = String(PSTHour).substring(0, 1);
  String PSTHour2 = String(PSTHour).substring(1);
  Serial.println((String)PSTHour1);
  Serial.println((String)PSTHour2);

  // now that the time's been split up, we can output it all to the digits of the clock.
  // I'm doing each digit separately.  again as with before, there's probably way better ways of doing this.

  if ((PSTHour1 == "1") && (PSTHour2 == "2") && (PSTMinute1 == "0") && (PSTMinute2 == "0")) {
    // it's midnight, so re-run the time sync
    getNTPTime();
    checktime = 1;
  }

  outputDigits(PSTMinute2.toInt(), PSTMinute1.toInt(), PSTHour2.toInt(), PSTHour1.toInt());
}
void outputDigitsTemp(int Temperature, String units) {
  String temp1;
  String temp2;
  String temp3;
  int digit2;
  int digit3;
  int digit1;
  int Temp3;
  String TempUnits;
  bool ShowThirdDigit = false;
  if (units == "F") {
    Serial.println("CaveMan Temperature");
    Serial.println(numdigits(Temperature));
    Serial.println(Temperature);
    int Temperature2 = (Temperature * 1.8) + 32;
    temp1 = String(Temperature2).substring(0, 1);
    temp2 = String(Temperature2).substring(1, 2);
    if (numdigits(Temperature2) > 2) {
      ShowThirdDigit = true;
      temp3 = String(Temperature2).substring(2, 3);
      digit3 = temp3.toInt();
    } else {
    }
    digit1 = temp1.toInt();
    digit2 = temp2.toInt();
    TempUnits = "F";
  } else {
    temp1 = String(Temperature).substring(0, 1);
    temp2 = String(Temperature).substring(1, 2);
    if (numdigits(Temperature) > 2) {
      ShowThirdDigit = true;
      temp3 = String(Temperature).substring(2, 3);
      digit3 = temp3.toInt();
    } else {
    }
    digit1 = temp1.toInt();
    digit2 = temp2.toInt();
    TempUnits = "C";
  }
  int nums[][15] = {

    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, -1, -1 },  // 0 //seems ok
    { 0, 1, -1, -1, -1, -1, 6, 7, 8, 9, 10, 11, 12, 13 },        // 1 //seems ok
    { -1, -1, 2, 3, -1, -1, -1, -1, 8, 9, -1, -1, -1, -1 },      // 2 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, 8, 9, -1, -1, 12, 13 },    // 3
    { 0, 1, -1, -1, -1, -1, 6, 7, -1, -1, -1, -1, 12, 13 },      // 4
    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, 12, 13 },    // 5////numbers represent the pixels that are NOT LIT
    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1 },    // 6
    { 0, 1, -1, -1, -1, -1, -1, -1, 8, 9, 10, 11, 12, 13 },      // 7 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // 8 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13 },  // 9 //seems ok
    { -1, -1, 2, 3, 4, 5, -1, -1, -1, -1, 10, 11, -1, -1 },      // C (for Temp)
    { 0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1 },        // F (for Temp)
    { 0, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },    // A (for Temp)
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, 12, 13 },            // // - (for Temp)
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },            // all off
  };

  if ((String)temp1 == "-") {
    for (int j = 44; j < 58; j++) {  // fourth digit... a 1
      if (nums[12][j - 44] == -1) {
        pixels[j] = -1;
      } else {
        pixels[j] = nums[12][j - 44] + 44;
      }
    }
  } else {
    for (int j = 44; j < 58; j++) {  // fourth digit... a 1
      if (nums[digit1][j - 44] == -1) {
        pixels[j] = -1;
      } else {
        pixels[j] = nums[digit1][j - 44] + 44;
      }
    }
  }

  for (int j = 30; j < 44; j++) {  // third digit.. a 7
    if (nums[digit2][j - 30] == -1) {
      pixels[j] = -1;
    } else {
      pixels[j] = nums[digit2][j - 30] + 30;
    }
  }
  pixels[28] = 28;  // dots
  pixels[29] = 29;  // dots


  if (ShowThirdDigit) {
    for (int j = 14; j < 28; j++) {  // second digit... a 3
      if (nums[digit3][j - 14] == -1) {
        pixels[j] = -1;
      } else {
        pixels[j] = nums[digit3][j - 14] + 14;
      }
    }
  } else {
    for (int j = 14; j < 28; j++) {  // second digit... a 3
      if (nums[14][j - 14] == -1) {
        pixels[j] = -1;
      } else {
        pixels[j] = nums[14][j - 14] + 14;
      }
    }
  }
  if (units == "F") {
    for (int j = 0; j < 14; j++) {  // first digit.  a 3
      pixels[j] = nums[11][j];
    }
  } else {
    if (units == "A") {
      for (int j = 0; j < 14; j++) {  // first digit.  a 3
        pixels[j] = nums[12][j];
      }
    } else {
      for (int j = 0; j < 14; j++) {  // first digit.  a 3
        pixels[j] = nums[10][j];
      }
    }
  }
}
void outputDigits(int digit1, int digit2, int digit3, int digit4) {
  int nums[][14] = {
    // 0   1   2   3   4   5   6   7   8   9  10  11  12  13
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, -1, -1 },  // 0 //seems ok
    { 0, 1, -1, -1, -1, -1, 6, 7, 8, 9, 10, 11, 12, 13 },        // 1 //seems ok
    { -1, -1, 2, 3, -1, -1, -1, -1, 8, 9, -1, -1, -1, -1 },      // 2 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, 8, 9, -1, -1, 12, 13 },    // 3
    { 0, 1, -1, -1, -1, -1, 6, 7, -1, -1, -1, -1, 12, 13 },      // 4
    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, 12, 13 },    // 5////numbers represent the pixels that are NOT LIT
    { -1, -1, -1, -1, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1 },    // 6
    { 0, 1, -1, -1, -1, -1, -1, -1, 8, 9, 10, 11, 12, 13 },      // 7 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 },  // 8 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13 },  // 9 //seems ok
    { -1, -1, 2, 3, 4, 5, -1, -1, -1, -1, 10, 11, -1, -1 },      // C (for Temp)
    { 0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1 },        // F (for Temp)
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 },            // all off
  };

  for (int j = 0; j < 14; j++) {  // first digit.  a 3
    pixels[j] = nums[digit1][j];
  }

  for (int j = 14; j < 28; j++) {  // second digit... a 3
    if (nums[digit2][j - 14] == -1) {
      pixels[j] = -1;
    } else {
      pixels[j] = nums[digit2][j - 14] + 14;
    }
  }
  pixels[28] = -1;  // dots
  pixels[29] = -1;  // dots

  for (int j = 30; j < 44; j++) {  // third digit.. a 7
    if (nums[digit3][j - 30] == -1) {
      pixels[j] = -1;
    } else {
      pixels[j] = nums[digit3][j - 30] + 30;
    }
  }
  for (int j = 44; j < 58; j++) {  // fourth digit... a 1
    if (nums[digit4][j - 44] == -1) {
      pixels[j] = -1;
    } else {
      pixels[j] = nums[digit4][j - 44] + 44;
    }
  }
}