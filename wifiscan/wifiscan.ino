#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "Tuppence";  // the name of your network

void setup() {
  Serial.begin(9600);

  int status = WiFi.begin(ssid);

  if (status != WL_CONNECTED) {
    Serial.println("Couldn't get a WiFi connection");
    while (true);
  }
  else {
    // if you are connected, scan for available WiFi networks and print the number discovered:
    Serial.println("** Scan Networks **");
    byte numSsid = WiFi.scanNetworks();

    Serial.print("Number of available WiFi networks discovered:");
    Serial.println(numSsid);
  }
}

void loop() {}
