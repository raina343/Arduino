#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <Timezone.h>    // https://github.com/JChristensen/Timezone
#include <RTCZero.h>

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
//unixTime =  makeTime(te);

Serial.println(rtc.getEpoch());
Serial.println(rtc.getYear());
  //ok so I need to convert the time into the correct timezones... 
  //which means I need to convert this time into unixtime
    print2digits(hours);

  Serial.print(":");

  print2digits(minutes);

  Serial.print(":");

  print2digits(seconds);

  Serial.println();
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
    SetDateTime(usPT, epoch, " Los Angeles");
  }
}
// given a Timezone object, UTC and a string description, convert and print local time with time zone
void SetDateTime(Timezone tz, time_t utc, const char *descr){
    char buf[40];
    char pdthour[3];
    char pdtMin[3];
    char pdtSec[3];
char pdtDay[3];
char pdtMonth[3];
char pdtyear[3];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    TimeChangeRule *tcr;        // pointer to the time change rule, use to get the TZ abbrev
    time_t tUTC = UTC.toLocal(utc, &tcr);
    Serial.println(tUTC);
    strcpy(m, monthShortStr(month(tUTC)));
    sprintf(pdthour,"%.2d",hour(tUTC));
    sprintf(pdtMin,"%.2d",minute(tUTC));
    sprintf(pdtSec,"%.2d",second(tUTC));
        sprintf(pdtDay,"%.2d",day(tUTC));
    sprintf(pdtMonth,"%.2d",month(tUTC));
    sprintf(pdtyear,"%.2d",year(tUTC));
    rtc.begin(); // initialize RTC
    String PDTHours = String(pdthour); 
    String PDTMins = String(pdtMin); 
    String PDTSeconds = String(pdtSec); 
        String PDTDay = String(pdtDay); 
    String PDTMonth = String(pdtMonth); 
    String PDTYear = String(pdtyear); 
//    Serial.println(PDTDay);
//    Serial.println(PDTMonth);
    Serial.println(PDTYear);
    rtc.setEpoch(utc)
//    rtc.setHours(PDTHours.toInt());
//    rtc.setMinutes(PDTMins.toInt());
//    rtc.setSeconds(PDTSeconds.toInt());
//    rtc.setDay(PDTDay.toInt());
//    rtc.setMonth(PDTMonth.toInt());
//    rtc.setYear(21);
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
