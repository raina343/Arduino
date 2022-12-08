
#include <WiFiNINA.h>
#include <FlashStorage.h>
char ssid[] = "RainbowClockSetup"; // your network SSID (name)
char pass[] = "";                  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                  // your network key Index number (needed only for WEP)
typedef struct{
  boolean valid;
  char wifissid[100];
  char Password[100];
  char temp[3];
  char twelvehr[3];
} Credentials;

int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;
WiFiServer server(80);
String readString;
int WifiSetup = 1;
FlashStorage(my_flash_store, Credentials);
 Credentials owner;
void setup(){
  Serial.begin(9600);

  while (!Serial){}
  owner = my_flash_store.read();
//
//      Serial.println(owner.wifissid);
//            Serial.println(owner.twelvehr);
//            Serial.println(owner.Password);
//            Serial.println(owner.temp);
//             Serial.println(owner.valid);
   if (owner.valid == false) {
    status = WiFi.beginAP(ssid);
    if (status != WL_AP_LISTENING){
      Serial.println("Creating access point failed");
      // don't continue
      while (true)
        ;
    }
    delay(10000);
    server.begin();
    printWiFiStatus();
  Serial.println("Access Point Web Server");
  if (WiFi.status() == WL_NO_MODULE){
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION){
    Serial.println("Please upgrade the firmware");
  }
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  } else {
    Serial.print("Stored SSID:");
    Serial.println((String)owner.wifissid);
    WifiSetup = 0;
    validateSSID(owner);
    delay(10000);
    server.begin();
  }
}

void printWiFiStatus(){
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

void loop(){
  if (WifiSetup == 0){
  }

  WiFiClient client = server.available();
  if (client)
  {
    while (client.connected())
    {
      if (client.available())
      {
        char c = client.read();

        //read char by char HTTP request
        if (readString.length() < 100)
        {

          //store characters to string
          readString += c;
          //Serial.print(c); //uncomment to see in serial monitor
        }

        //if HTTP request has ended
        if (c == '\n')
        {
           if (readString.indexOf("Submit=Submit") > 0)
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
                  name = strtok(NULL, "="); // Get the next name
                }
              }
            }
            String returnedip = validateSSID(owner);
            if (returnedip == "0.0.0.0"){
              Serial.println("Wifi Failed");
              status = WiFi.beginAP(ssid);
              printWiFiStatus();
            }else{
              owner.valid = true;
//              Serial.println("-----------------------");
//              Serial.println(owner.wifissid);
              my_flash_store.write(owner);

            }
//            Serial.println(owner.wifissid);
//            Serial.println(owner.twelvehr);
//            Serial.println(owner.Password);
//            Serial.println(owner.temp);
//             Serial.println(owner.valid);
            
          }
          String twelvehour = "";
          if ((String)owner.twelvehr=="on"){
            twelvehour=" selected='selected' ";
          }
          String ShowTempSetting = "";
          if ((String)owner.temp=="on"){
            ShowTempSetting=" selected ";
          }
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          client.println("<HTML>");
          client.println("<HEAD>");
          client.println("<TITLE>Rainbow Clock Setup</TITLE>");
          client.println("</HEAD>");
          client.println("<BODY>");
          client.println("<H1>Enter your SSID, Password below.  if I haven't fucked up, after you hit Save, it should reconnect to your main Wifi and this Wireless Access Point will vanish (if the info entered is correct)<br>You can revisit this setup page by checking your router or other tools for the clocks new ip address</H1>");
          client.print("<form method='get' action=''>");
          client.print("<input id='ssid' required type='text' name='ssid' ");
          client.print("value='");
          client.print(owner.wifissid);
          client.print("' placeholder='ssid'><label for='ssid'>SSID</label><br>");
          client.print("<input id='password' type='password' name='password'");
          client.print("value='");
          client.print(owner.Password);
          client.print("' placeholder='password' required><label for='password'>Password</label><br>");
          client.print("<select name='ShowTemp' id='ShowTemp'>");
          client.print("<option value='off'>off</option>");
          client.print("<option value='on' ");
          client.print (ShowTempSetting);
          client.print(">on</option>");
          client.print("</select>");
          client.print("<label for='ShowTemp'>Show Temperature every 1 minute</label><br>");
          client.print("<select name='12hr' id='12hr'>");
          client.print("<option value='off'>off</option>");
          client.print("<option value='on' ");
          client.print (twelvehour);
          client.print (">on</option>");
          client.print("</select>");
          client.print("<label for='12hr'>Show 12hr Time</label><br>");
          client.print("<button id='Submit' name='Submit' value='Submit'type='submit'>Save</button>");
          client.print("</form>");
          client.println("");
          client.println("</BODY>");
          client.println("</HTML>");
          delay(1);
          client.stop();
          readString = "";
        }
      }
    }
  }
}
String validateSSID(Credentials owner){
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
