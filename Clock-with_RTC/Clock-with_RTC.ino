#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

#include <Timezone.h>    // https://github.com/JChristensen/Timezone

#include <RTCZero.h>



RTCZero rtc;
char ssid[] = "RainaIsolatedSSID";        // your network SSID (name)
char pass[] = "nevermindthebuzzcocks22";    // your network password (use for WPA, or use as key for WEP)

const byte rtcseconds = 0;

const byte rtcminutes = 11;

const byte rtchours = 16;

/* Change these values to set the current initial date */

const byte rtcday = 12;

const byte rtcmonth = 7;

const byte rtcyear = 21;
int status = WL_IDLE_STATUS;
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
unsigned int localPort = 2390;  
WiFiUDP Udp;




// United Kingdom (London, Belfast)
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         // Standard Time
Timezone UK(BST, GMT);


// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usPT(usPDT, usPST);

void setup()
{
    Serial.begin(115200);


    
  if (WiFi.status() == WL_NO_MODULE) {

    Serial.println("Communication with WiFi module failed!");

    // don't continue

    while (true);

  }
  String fv = WiFi.firmwareVersion();

  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {

    Serial.println("Please upgrade the firmware");

  }

  // attempt to connect to Wifi network:

  while (status != WL_CONNECTED) {

    Serial.print("Attempting to connect to SSID: ");

    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(10000);

  }
  
  Serial.println("Connected to wifi");

  printWifiStatus();

  Serial.println("\nStarting connection to server...");

  Udp.begin(localPort);
  sendNTPpacket(timeServer);
    
}

void loop(){
    time_t utc = now();
  //  printDateTime(UK, utc, " London");
    printDateTime(usPT, utc, " Los Angeles");
    
//  print2digits(rtc.getDay());
//
//  Serial.print("/");
//
//  print2digits(rtc.getMonth());
//
//  Serial.print("/");
//
//  print2digits(rtc.getYear());
//
//  Serial.print(" ");
//
//  // ...and time

  print2digits(rtc.getHours());

  Serial.print(":");

  print2digits(rtc.getMinutes());

  Serial.print(":");

  print2digits(rtc.getSeconds());

  Serial.println();
    delay(10000);
}

//// Function to return the compile date and time as a time_t value
//time_t compileTime()
//{
//    const time_t FUDGE(10);     // fudge factor to allow for compile time (seconds, YMMV)
//    const char *compDate = __DATE__, *compTime = __TIME__, *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
//    char chMon[4], *m;
//    tmElements_t tm;
//
//    strncpy(chMon, compDate, 3);
//    chMon[3] = '\0';
//    m = strstr(months, chMon);
//    tm.Month = ((m - months) / 3 + 1);
//
//    tm.Day = atoi(compDate + 4);
//    tm.Year = atoi(compDate + 7) - 1970;
//    tm.Hour = atoi(compTime);
//    tm.Minute = atoi(compTime + 3);
//    tm.Second = atoi(compTime + 6);
//    time_t t = makeTime(tm);
//    return t + FUDGE;           // add fudge factor to allow for compile time
//}

// given a Timezone object, UTC and a string description, convert and print local time with time zone
void printDateTime(Timezone tz, time_t utc, const char *descr)
{
    char buf[40];
    char pdthour[3];
    char pdtMin[3];
    char pdtSec[3];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    TimeChangeRule *tcr;        // pointer to the time change rule, use to get the TZ abbrev

    time_t t = tz.toLocal(utc, &tcr);

    strcpy(m, monthShortStr(month(t)));
    sprintf(pdthour,"%.2d",hour(t));
    sprintf(pdtMin,"%.2d",minute(t));
    sprintf(pdtSec,"%.2d",second(t));
    sprintf(buf, "%.2d:%.2d:%.2d %s %.2d %s %d %s",hour(t), minute(t), second(t), dayShortStr(weekday(t)), day(t), m, year(t), tcr -> abbrev);
    rtc.begin(); // initialize RTC
String PDTHours = String(pdthour); 
String PDTMins = String(pdtMin); 
String PDTSeconds = String(pdtSec); 

  rtc.setHours(PDTHours.toInt());
  rtc.setMinutes(PDTMins.toInt());
  rtc.setSeconds(PDTSeconds.toInt());

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
//    printDateTime(usPT, epoch, " Los Angeles");
  }
}
