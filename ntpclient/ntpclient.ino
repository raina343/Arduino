
/*

 Udp NTP Client

 Get the time from a Network Time Protocol (NTP) time server

 Demonstrates use of UDP sendPacket and ReceivePacket

 For more on NTP time servers and the messages needed to communicate with them,

 see http://en.wikipedia.org/wiki/Network_Time_Protocol

 created 4 Sep 2010

 by Michael Margolis

 modified 9 Apr 2012

 by Tom Igoe

 This code is in the public domain.

 */

 #include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>

int status = WL_IDLE_STATUS;
#include "arduino_secrets.h"
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "RainaIsolatedSSID";        // your network SSID (name)
char pass[] = "nevermindthebuzzcocks22";    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;            // your network key Index number (needed only for WEP)
#define PIN        6 // On Trinket or Gemma, suggest changing this to 1


#define NUMPIXELS 21 // Popular NeoPixel ring size

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
unsigned int localPort = 2390;      // local port to listen for UDP packets
#define DELAYVAL 500 
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP

WiFiUDP Udp;

void setup() {

  // Open serial communications and wait for port to open:

  Serial.begin(9600);

  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }

  // check for the WiFi module:

  if (WiFi.status() == WL_NO_MODULE) {

    Serial.println("Communication with WiFi module failed!");

    // don't continue

    while (true);

  }
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  pixels.begin();
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
}

void loop() {

  sendNTPpacket(timeServer); // send an NTP packet to a time server
pixels.clear(); // Set all pixel colors to 'off'

  // The first NeoPixel in a strand is #0, second is 1, all the way up
  // to the count of pixels minus one.
  for(int i=0; i<NUMPIXELS; i++) { // For each pixel...

    // pixels.Color() takes RGB values, from 0,0,0 up to 255,255,255
    // Here we're using a moderately bright green color:
    pixels.setPixelColor(i, pixels.Color(0, 150, 0));

    pixels.show();   // Send the updated pixel colors to the hardware.

    delay(DELAYVAL); // Pause before next pass through loop
  }
  // wait to see if a reply is available

  delay(1000);

  if (Udp.parsePacket()) {

    Serial.println("packet received");

    // We've received a packet, read the data from it

    Udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,

    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);

    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);

    // combine the four bytes (two words) into a long integer

    // this is NTP time (seconds since Jan 1 1900):

    unsigned long secsSince1900 = highWord << 16 | lowWord;

    Serial.print("Seconds since Jan 1 1900 = ");

    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:

    Serial.print("Unix time = ");

    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:

    const unsigned long seventyYears = 2208988800UL;

    // subtract seventy years:

    unsigned long epoch = secsSince1900 - seventyYears;

    // print Unix time:

    Serial.println(epoch);

    // print the hour, minute and second:

    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)

    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)

    Serial.print(':');

    if (((epoch % 3600) / 60) < 10) {

      // In the first 10 minutes of each hour, we'll want a leading '0'

      Serial.print('0');

    }

    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)

    Serial.print(':');

    if ((epoch % 60) < 10) {

      // In the first 10 seconds of each minute, we'll want a leading '0'

      Serial.print('0');

    }

    Serial.println(epoch % 60); // print the second

  }

  // wait ten seconds before asking for the time again

  delay(10000);
}

// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address) {

  //Serial.println("1");

  // set all bytes in the buffer to 0

  memset(packetBuffer, 0, NTP_PACKET_SIZE);

  // Initialize values needed to form NTP request

  // (see URL above for details on the packets)

  //Serial.println("2");

  packetBuffer[0] = 0b11100011;   // LI, Version, Mode

  packetBuffer[1] = 0;     // Stratum, or type of clock

  packetBuffer[2] = 6;     // Polling Interval

  packetBuffer[3] = 0xEC;  // Peer Clock Precision

  // 8 bytes of zero for Root Delay & Root Dispersion

  packetBuffer[12]  = 49;

  packetBuffer[13]  = 0x4E;

  packetBuffer[14]  = 49;

  packetBuffer[15]  = 52;

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
