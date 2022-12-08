#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <Timezone.h>    // https://github.com/JChristensen/Timezone
#include <RTCZero.h>
#include <Adafruit_NeoPixel.h>
RTCZero rtc;
//RTCZero UKrtc;
const byte rtcseconds = 0;
const byte rtcminutes = 11;
const byte rtchours = 16;
const byte rtcday = 12;
const byte rtcmonth = 7;
const byte rtcyear = 21;
int status = WL_IDLE_STATUS;
char ssid[] = "RainaIsolatedSSID";        // your network SSID (name)
char pass[] = "nevermindthebuzzcocks22";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
unsigned int localPort = 2390;      // local port to listen for UDP packets
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets
WiFiUDP Udp;
 int secondcounter = 1;
int number1[] = {3,4,5,6,7,8};
int number2[] = {0,1,2,3,7,8,9,10,11,12,16,17,18,19,20};
int number3[] = {0,1,2,3,4,5,6,7,8,9,11,16,17,18};
int number4[] = {3,4,5,6,7,8,12,13,14,15,16,17};
int number5[] = {0,1,2,3,4,5,9,10,11,12,13,14,15,16,17};
int number6[] = {0,1,2,3,4,5,12,13,14,15,16,17,18,10,20};
int number7[] = {3,4,5,6,7,8,9,10,11};
int number8[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
int number9[] = {3,4,5,6,7,8,9,10,11,12,13,14,15,16,17};
int number0[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,18,19,20};


// United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         // Standard Time
Timezone UK(BST, GMT);


// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usPT(usPDT, usPST);
TimeChangeRule utcRule = {"UTC", Last, Sun, Mar, 1, 0};     // UTC
Timezone UTC(utcRule);
void setup()
{
    Serial.begin(115200);
 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  getNTPTime();
}

void loop(){

//so we now have an RTC going with UTC.  now every tick, we need to get two separate times, one for PST and GMT and then pass those times off onto the output function
if (secondcounter==86400){
  //this should make sure the time is upto date every day
  getNTPTime();
}
gettimeasstringdigits(rtc.getHours(),rtc.getMinutes(),rtc.getSeconds());

secondcounter++;
    delay(1000);
}
void gettimeasstringdigits(byte hours,byte minutes,byte seconds){
    char bufPST[40];
    char mPST[4];  
        char bufUK[40];
    char mUK[4];  
TimeChangeRule *tcr; 
    time_t tUK = UK.toLocal(rtc.getEpoch(), &tcr);
    time_t tPST = usPT.toLocal(rtc.getEpoch(), &tcr);
    char PSTHour[3];
    char PSTMin[3];
    char PSTSec[3];
    char UKHour[3];
    char UKMin[3];
    char UKSec[3];

    strcpy(mUK, monthShortStr(month(tUK)));
    sprintf(bufUK, "%.2d:%.2d:%.2d %s %.2d %s %d %s",hour(tUK), minute(tUK), second(tUK), dayShortStr(weekday(tUK)), day(tUK), mUK, year(tUK), tcr -> abbrev);
        strcpy(mPST, monthShortStr(month(tPST)));
    sprintf(bufPST, "%.2d:%.2d:%.2d %s %.2d %s %d %s",hour(tPST), minute(tPST), second(tPST), dayShortStr(weekday(tPST)), day(tPST), mPST, year(tPST), tcr -> abbrev);
//    Serial.println(bufPST);
//    Serial.println(bufUK);
    sprintf(PSTHour,"%.2d",hour(tPST));
    sprintf(PSTMin,"%.2d",minute(tPST));
    sprintf(PSTSec,"%.2d",second(tPST));
    sprintf(UKHour,"%.2d",hour(tUK));
    sprintf(UKMin,"%.2d",minute(tUK));
    sprintf(UKSec,"%.2d",second(tUK));

    String PSTHours = String(PSTHour); 
    String PSTMins = String(PSTMin); 
    String PSTSeconds = String(PSTSec); 
    String UKHours = String(UKHour); 
    String UKMins = String(UKMin); 
    String UKSeconds = String(UKSec); 

String PSTMinute1 = String(PSTMin).substring(0,1);
String PSTMinute2 = String(PSTMin).substring(1);
String UKMinute1 = String(UKMin).substring(0,1);
String UKMinute2 = String(UKMin).substring(1);

String PSTHour1 = String(PSTHour).substring(0,1);
String PSTHour2 = String(PSTHour).substring(1);
String UKHour1 = String(UKHour).substring(0,1);
String UKHour2 = String(UKHour).substring(1);
    Serial.print(PSTHour1);
    Serial.print(PSTHour2);
    Serial.print(":");
    Serial.print(PSTMinute1);
    Serial.print(PSTMinute2);
Serial.println(' ');
    Serial.print(UKHour1);
    Serial.print(UKHour2);
    Serial.print(":");
    Serial.print(UKMinute1);
    Serial.print(UKMinute2);

//so now we need to split each number (just hrs and minutes for both time zones into two)


}
void getNTPTime(){
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();
  Serial.println("\nStarting connection to server...");
  Udp.begin(localPort);
  sendNTPpacket(timeServer); // send an NTP packet to a time server
  delay(1000);
  secondcounter=0; //reset the counter, so we always get the correct time every 24 hrs
  if (Udp.parsePacket()) {
    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
//    Serial.println('---------------------------------');
//    Serial.println(epoch);
//    Serial.println('---------------------------------');
    SetDateTime(usPT, epoch, " Los Angeles");
  }
}
// given a Timezone object, UTC and a string description, convert and print local time with time zone
void SetDateTime(Timezone tz, time_t utc, const char *descr){
    rtc.begin(); // initialize RTC
    rtc.setEpoch(utc);
}

void print2digits(int number) {

  if (number < 10) {

    Serial.print("0"); // print a 0 before if the number is < than 10

  }

  Serial.print(number);
}
unsigned long sendNTPpacket(IPAddress& address) {

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
  if (Udp.parsePacket()) {

    Serial.println("packet received");


    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);

    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    const unsigned long seventyYears = 2208988800UL;
    unsigned long epoch = secsSince1900 - seventyYears;
//    printDateTime(UK, epoch, " London");
//    SetDateTime(usPT, epoch, " Los Angeles");
  }
}
