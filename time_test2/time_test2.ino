
#include <Adafruit_NeoPixel.h>

#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <Timezone.h>  


//#include <RTCZero.h>
//
///* Create an rtc object */
//
//RTCZero rtcUK;
//RTCZero rtcCA;
int status = WL_IDLE_STATUS;
//const byte CAseconds = 0;
//
//const byte CAminutes = 0;
//
//const byte CAhours = 16;
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = "RainaIsolatedSSID";        // your network SSID (name)
char pass[] = "nevermindthebuzzcocks22";    // your network password (use for WPA, or use as key for WEP)
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#define LED_PIN    6
#define LED_COUNT 21
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel pixels(21, PIN, NEO_GRB + NEO_KHZ800);
IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
TimeChangeRule BST = {"BST", Last, Sun, Mar, 1, 60};        // British Summer Time
TimeChangeRule GMT = {"GMT", Last, Sun, Oct, 2, 0};         // Standard Time
Timezone UK(BST, GMT);


// US Pacific Time Zone (Las Vegas, Los Angeles)
TimeChangeRule usPDT = {"PDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"PST", First, Sun, Nov, 2, -480};
Timezone usPT(usPDT, usPST);
const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
unsigned int localPort = 2390;  
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
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  // END of Trinket-specific code.

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
}


// loop() function -- runs repeatedly as long as board is on ---------------
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
    printDateTime(UK, epoch, " London");
    printDateTime(usPT, epoch, " Los Angeles");
  }
}
void loop() {
  
//  print2digits(rtcCA.getHours());
//
//  Serial.print(":");
//
//  print2digits(rtcCA.getMinutes());
//
//  Serial.print(":");
//
//  print2digits(rtcCA.getSeconds());
//Serial.println(' ');
//  delay(1000);
  // Fill along the length of the strip in various colors...
//  colorWipe(strip.Color(255,   0,   0), 50); // Red
//  colorWipe(strip.Color(  0, 255,   0), 50); // Green
//  colorWipe(strip.Color(  0,   0, 255), 50); // Blue
//
//  // Do a theater marquee effect in various colors...
//  theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
//  theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
//  theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness
//
  //rainbow(10);             // Flowing rainbow cycle along the whole strip
//  theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
}

void printDateTime(Timezone tz, time_t utc, const char *descr)
{
    char buf[40];
    char m[4];    // temporary storage for month string (DateStrings.cpp uses shared buffer)
    TimeChangeRule *tcr;        // pointer to the time change rule, use to get the TZ abbrev

    time_t t = tz.toLocal(utc, &tcr);
//
//      rtcCA.begin(); // initialize RTC

 Serial.println('beginning time parsing');
char hr[3];
char min[3];
char sec[3];
int digit;
String Minute1;
String Minute2;
sprintf(hr, "%.2d",hour(t));
//Serial.println(hr);
sprintf(min, "%.2d",minute(t));
sprintf(sec, "%.2d",second(t));
Serial.println(hr);
Serial.println(String(min));
Serial.println(String(sec));
//String(min).getBytes(CAminutes,2);
//  String(hr).getBytes(CAhours,2) ;   
//  String(sec).getBytes(CAseconds,2)  
//  rtcCA.setHours(CAhours);
//  rtcCA.setMinutes(CAminutes);
//  rtcCA.setSeconds(CAseconds);

Minute2 = String(min).substring(0,1);
Minute2 = String(min).substring(1);
Serial.println(String(min).substring(0,1));
//
Serial.println(String(min).substring(1));
showlastminutedigit(Minute2.toInt());
}

void showlastminutedigit(int digit){
  Serial.println("digit printing");
  strip.clear(); 
  switch (digit){
    case 1:
    Serial.println("1");
    for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (3 * 65536L / strip.numPixels());
     strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
      strip.setPixelColor(5, strip.gamma32(strip.ColorHSV(pixelHue)));
       strip.setPixelColor(6, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.setPixelColor(7, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(8, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(4, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
     strip.show(); // Update strip with new contents
    break;
    case 2:
    Serial.println("2");
        for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (0 * 65536L / strip.numPixels());
     strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(pixelHue)));
      strip.setPixelColor(2, strip.gamma32(strip.ColorHSV(pixelHue)));
       strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.setPixelColor(7, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(8, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(9, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(10, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(11, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(12, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(16, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(17, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(18, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(19, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(20, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(1, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
     strip.show(); // Update strip with new contents
    break;
    case 3:
    Serial.println("3");
        for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (0 * 65536L / strip.numPixels());
     strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(pixelHue)));
      strip.setPixelColor(1, strip.gamma32(strip.ColorHSV(pixelHue)));
       strip.setPixelColor(2, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(4, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(5, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(6, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(7, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(8, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(9, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(10, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(11, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(16, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(17, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(18, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
     strip.show(); // Update strip with new contents
    break;
    case 4:
    Serial.println("4");
        for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (3 * 65536L / strip.numPixels());
     strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
      strip.setPixelColor(4, strip.gamma32(strip.ColorHSV(pixelHue)));
       strip.setPixelColor(5, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.setPixelColor(6, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(7, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(8, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(12, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(13, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(14, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(15, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(16, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(17, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
     strip.show(); // Update strip with new contents
    break;
    case 5:
    Serial.println("5");
        for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (1 * 65536L / strip.numPixels());
     strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(pixelHue)));
      strip.setPixelColor(1, strip.gamma32(strip.ColorHSV(pixelHue)));
       strip.setPixelColor(2, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(4, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(5, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(9, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(10, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(11, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(12, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(13, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(14, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(15, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(16, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(17, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
     strip.show(); // Update strip with new contents
    break;
    case 6:
    Serial.println("6");
        for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (0 * 65536L / strip.numPixels());
     strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(pixelHue)));
      strip.setPixelColor(1, strip.gamma32(strip.ColorHSV(pixelHue)));
       strip.setPixelColor(2, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(4, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(5, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(12, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(13, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(14, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(15, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(16, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(17, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(18, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(10, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(20, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
     strip.show(); // Update strip with new contents
    break;
    case 7:
    Serial.println("7");
        for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (4 * 65536L / strip.numPixels());
     strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
      strip.setPixelColor(4, strip.gamma32(strip.ColorHSV(pixelHue)));
       strip.setPixelColor(5, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.setPixelColor(6, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(7, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(8, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(9, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(10, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(11, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
     strip.show(); // Update strip with new contents
    break;
    case 8:
    Serial.println("8");
        for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (0 * 65536L / strip.numPixels());
     strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(pixelHue)));
      strip.setPixelColor(2, strip.gamma32(strip.ColorHSV(pixelHue)));
       strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.setPixelColor(4, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(5, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(6, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(7, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(8, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(9, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(10, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(11, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(12, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(13, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(14, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(15, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(16, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(17, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(18, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(19, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(20, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(1, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
     strip.show(); // Update strip with new contents
    break;
    case 9:
    Serial.println("9");
        for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (3 * 65536L / strip.numPixels());
     
        strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(4, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(5, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(6, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(7, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(8, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(9, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(10, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(11, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(12, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(13, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(14, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(15, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(16, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(17, strip.gamma32(strip.ColorHSV(pixelHue)));
          
     }
      strip.show(); // Update strip with new contents
    break;
    case 0:
    Serial.println("0");
        for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
      int pixelHue = firstPixelHue + (1 * 65536L / strip.numPixels());
     strip.setPixelColor(0, strip.gamma32(strip.ColorHSV(pixelHue)));
      strip.setPixelColor(1, strip.gamma32(strip.ColorHSV(pixelHue)));
       strip.setPixelColor(2, strip.gamma32(strip.ColorHSV(pixelHue)));
        strip.setPixelColor(3, strip.gamma32(strip.ColorHSV(pixelHue)));
         strip.setPixelColor(4, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(5, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(6, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(7, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(8, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(9, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(10, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(11, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(12, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(13, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(14, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(18, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(19, strip.gamma32(strip.ColorHSV(pixelHue)));
          strip.setPixelColor(20, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
     strip.show(); // Update strip with new contents
    break;
  }
//   strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
}


// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
void print2digits(int number) {

  if (number < 10) {

    Serial.print("0"); // print a 0 before if the number is < than 10

  }

  Serial.print(number);
}
