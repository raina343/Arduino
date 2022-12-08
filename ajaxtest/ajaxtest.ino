
/*

  WiFi Web Server

  A simple web server that shows the value of the analog input pins.

  using a WiFi shield.

  This example is written for a network using WPA encryption. For

  WEP or WPA, change the Wifi.begin() call accordingly.

  Circuit:

   WiFi shield attached

   Analog inputs attached to pins A0 through A5 (optional)

  created 13 July 2010

  by dlf (Metodo2 srl)

  modified 31 May 2012

  by Tom Igoe

*/

#include <SPI.h>
#include <WiFiNINA.h>

char ssid[] = "Tuppence";      // your network SSID (name)
char pass[] = "nevermindthebuzzcocks22";   // your network password
int keyIndex = 0;                 // your network key Index number (needed only for WEP)
String readString;
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {

  //Initialize serial and wait for port to open:

  Serial.begin(9600);

  while (!Serial) {

    ; // wait for serial port to connect. Needed for native USB port only

  }

  // check for the presence of the shield:

  if (WiFi.status() == WL_NO_SHIELD) {

    Serial.println("WiFi shield not present");

    // don't continue:

    while (true);

  }

  String fv = WiFi.firmwareVersion();

  if (fv != "1.1.0") {

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

  server.begin();

  // you're connected now, so print out the status:

  printWifiStatus();
}

void loop() {

  // listen for incoming clients

  WiFiClient client = server.available();

  if (client) {

    Serial.println("new client");

    // an http request ends with a blank line

    bool currentLineIsBlank = true;

    while (client.connected()) {

      if (client.available()) {

        char c = client.read();

    
        readString += c;


        if (c == '\n' && currentLineIsBlank) {

     

          client.println("HTTP/1.1 200 OK");

          client.println("Content-Type: text/html");

          client.println("Connection: close");  // the connection will be closed after completion of the response



          client.println();

          client.println("<!DOCTYPE HTML>");

          client.println("<html>");

          // output the value of each analog input pin
 client.println("HTTP/1.1 200 OK");

          client.println("<TITLE>Rainbow Clock Setup</TITLE>");
          client.println("</HEAD>");

          client.println("<BODY>");
   
          client.println("<style type='text/css'>");
          client.println("*,");
          client.println("*::before,");
          client.println("*::after {");
          client.println("box-sizing: border-box;");
          client.println("}");



          client.println(":root {");
          client.println("--select-border: #777;");
          client.println("--select-focus: blue;");
          client.println("--select-arrow: var(--select-border);");
          client.println("}");

          client.println("select {");
          client.println("color: black;");
          client.println("-webkit-appearance: none;");
          client.println("-moz-appearance: none;");
          client.println("appearance: none;");
          client.println("background-color: transparent;");
          client.println("border: none;");
          client.println("padding: 0 1em 0 0;");
          client.println("margin: 0;");
          client.println("width: 100%;");
          client.println("font-family: inherit;");
          client.println("font-size: inherit;");
          client.println("cursor: inherit;");
          client.println("line-height: inherit;");
          client.println("z-index: 1;");
          client.println("outline: none;");
          client.println("}");

          client.println("select::-ms-expand {");
          client.println("display: none;");
          client.println("}");

          client.println(".select {");
          client.println("color: black;");
          client.println("display: grid;");
          client.println("grid-template-areas: 'select';");
          client.println("align-items: center;");
          client.println("position: relative;");
          client.println("min-width: 15ch;");
          client.println("max-width: 30ch;");
          client.println("border: 1px solid var(--select-border);");
          client.println("border-radius: 0.25em;");
          client.println("padding: 0.25em 0.5em;");
          client.println("font-size: 1.25rem;");
          client.println("cursor: pointer;");
          client.println("line-height: 1.1;");
          client.println("background-color: #fff;");
          client.println("background-image: linear-gradient(to top, #f9f9f9, #fff 33%);");
          client.println("}");

          client.println(".select select, .select::after {");
          client.println("grid-area: select;");
          client.println("}");

          client.println(".select:not(.select--multiple)::after {");
          client.println("content: '';");
          client.println("justify-self: end;");
          client.println("width: 0.8em;");
          client.println("height: 0.5em;");
          client.println("background-color: var(--select-arrow);");
          client.println("-webkit-clip-path: polygon(100% 0%, 0 0%, 50% 100%);");
          client.println("clip-path: polygon(100% 0%, 0 0%, 50% 100%);");
          client.println("}");

          client.println("select:focus + .focus {");
          client.println("position: absolute;");
          client.println("top: -1px;");
          client.println("left: -1px;");
          client.println("right: -1px;");
          client.println("bottom: -1px;");
          client.println("border: 2px solid var(--select-focus);");
          client.println("border-radius: inherit;");
          client.println("}");

          client.println("select[multiple] {");

          client.println("height: 6rem;");
          client.println("}");

          client.println("select[multiple] option {");
          client.println("white-space: normal;");
          client.println("outline-color: var(--select-focus);");
          client.println("}");

          client.println(".select--disabled {");
          client.println("cursor: not-allowed;");
          client.println("background-color: #eee;");
          client.println("background-image: linear-gradient(to top, #ddd, #eee 33%);");
          client.println("}");

          client.println("label {");
          client.println("font-size: 1.125rem;");
          client.println("font-weight: 500;");
          client.println("}");

          client.println(".select + label {");
          client.println("margin-top: 2rem;");
          client.println("}");

          client.println("body {");
          client.println("min-height: 100vh;");
          client.println("display: grid;");
          client.println("place-content: center;");
          client.println("grid-gap: 0.5rem;");
          client.println("font-family: 'Baloo 2', sans-serif;");
          client.println("/*background-color: #e9f2fd;*/");
          client.println("padding: 1rem;");
          client.println("}");

          client.println(".formcontent label {");
          client.println("/*color: red;*/");
          client.println("display: block;");
          client.println("}");

          client.println(".formcontent {");
          client.println("text-align: center;");
          client.println("width: 30ch;");
          client.println("margin: 0 auto;");
          client.println("}");

          client.println(".formcontent li {");
          client.println("list-style: none;");
          client.println("padding-top: 10px;");
          client.println("width: 30ch;");
          client.println("}");



          client.println(".formcontent input {");
          client.println("color: black;");
          client.println("display: grid;");
          client.println("grid-template-areas: 'select';");
          client.println("align-items: center;");
          client.println("position: relative;");
          client.println("min-width: 15ch;");
          client.println("max-width: 24ch;");
          client.println("border: 1px solid var(--select-border);");
          client.println("border-radius: 0.25em;");
          client.println("padding: 0.25em 0.5em;");
          client.println("font-size: 1.25rem;");
          client.println("cursor: pointer;");
          client.println("line-height: 1.1;");
          client.println("background-color: #fff;");
          client.println("text-align: center;");
          client.println("background-image: linear-gradient(to top, #f9f9f9, #fff 33%);");
          client.println("}");

          client.println(".formcontent button {");
          client.println("color: black;");
          client.println("display: grid;");
          client.println("grid-template-areas: 'select';");
          client.println("align-items: center;");
          client.println("position: relative;");
          client.println("min-width: 15ch;");
          client.println("max-width: 24ch;");
          client.println("border: 1px solid var(--select-border);");
          client.println("border-radius: 0.25em;");
          client.println("padding: 0.25em 0.5em;");
          client.println("font-size: 1.25rem;");
          client.println("cursor: pointer;");
          client.println("line-height: 1.1;");
          client.println("background-color: #fff;");
          client.println("text-align: center;");
          client.println("background-image: linear-gradient(to top, #f9f9f9, #fff 33%);");
          client.println("}");

          client.println("html {");

          client.println("background: linear-gradient(145deg,");
          client.println("rgba(43, 9, 82, 1) 9%,");
          client.println("rgba(2, 2, 255, 1) 18%,");
          client.println("rgba(2, 255, 2, 1) 27%,");
          client.println("rgba(196, 181, 0, 1) 36%,");
          client.println("rgba(255, 165, 0, 1) 45%,");
          client.println("rgba(162, 19, 19, 1) 63%,");
          client.println("rgba(255, 165, 0, 1) 72%,");
          client.println("rgba(196, 181, 0, 1) 81%,");
          client.println("rgba(2, 255, 2, 1) 90%,");
          client.println("rgba(2, 2, 255, 1) 99%,");
          client.println("rgba(43, 9, 82, 1) 100%);");

          client.println("background-size: 400% 400%;");
          client.println("animation: gradient 5s ease infinite;");

          client.println("height: 100vh;");
          client.println("width: 100vw;");
          client.println("align-items: center;");
          client.println("justify-content: center;");
          client.println("}");

          client.println("@keyframes gradient {");
          client.println("0% {");
          client.println("background-position: 0% 50%;");
          client.println("}");
          client.println("50% {");
          client.println("background-position: 100% 50%;");
          client.println("}");
          client.println("100% {");
          client.println("background-position: 0% 50%;");
          client.println("}");
          client.println("}");

          client.println(".loader {");
          client.println("border: 16px solid #f3f3f3; /* Light grey */");
          client.println("border-style: solid;");
          client.println("border-width: 0px;");
          client.println("/*border-top: 16px solid #3498db; !* Blue *!*/");
          client.println("/*border-radius: 50%;*/");
          client.println("width: 120px;");
          client.println("height: 120px;");
          client.println("/*animation: spin 2s linear infinite;*/");
          client.println("}");

          client.println("@keyframes spin {");
          client.println("0% {");
          client.println("transform: rotate(0deg);");
          client.println("}");
          client.println("100% {");
          client.println("transform: rotate(360deg);");
          client.println("}");
          client.println("}");


          client.println(".slidecontainer {width: 100%;}");

          client.println(".slider {-webkit-appearance: none;width: 100%;height: 25px;background: #d3d3d3;outline: none;opacity: 0.7;-webkit-transition: .2s;transition: opacity .2s;}");

          client.println(".slider:hover {opacity: 1;}");

          client.println(".slider::-webkit-slider-thumb {-webkit-appearance: none;appearance: none;width: 25px;height: 25px;background: #04AA6D;cursor: pointer;}");

          client.println(".slider::-moz-range-thumb {width: 25px;height: 25px;background: #04AA6D;cursor: pointer;}");
 client.println(".slider:disabled::-webkit-slider-thumb{background: grey;cursor: not-allowed !important;}");
 client.println(".slider:disabled::-moz-range-thumb{background: grey;cursor: not-allowed !important;}");
 client.println(".slider:disabled{cursor: not-allowed !important;}");

          client.println("</style>");

          client.println("<header class='container-col'>");
          client.println("<H1>Enter your SSID, Password below. if I haven't fucked up, after you hit Save, it should reconnect to your main");
          client.println("Wifi");
          client.println("and this Wireless Access Point will vanish (if the info entered is correct)<br>You can revisit this setup page");
          client.println("by");
          client.println("checking your router or other tools for the clocks new ip address</H1>");
          client.println("</header>");
         // client.print("<form method='get' action=''>");
          client.println("<div class='formcontent'>");
          client.println("<ul>");
          client.println("<Li>");
          client.println("<label for='ssid'>SSID</label>");
          client.println("<input id='ssid' required type='text' name='ssid'");
          client.print("value='");
         // client.print(owner.wifissid);
          client.print("' placeholder='ssid'>");
          client.println("</Li>");
          client.println("<li>");
          client.println("<label for='password'>Password</label>");
          client.println("<input id='password'");
          client.println("       type='password'");
          client.println("       name='password'");
          client.println("      value='");
         // client.print(owner.Password);
          client.print("' placeholder='password' required><label for='password'>Password</label><br>");


          client.println("</li>");
          client.println("<li>");
          client.println("<label for='ShowTemp'>Show Temperature every 1 minute</label>");
          client.println("<div class='select'>");
          client.print("<select name='ShowTemp' id='ShowTemp'>");
          client.print("<option value='off'>off</option>");
          client.print("<option value='on' ");
         // client.print (ShowTempSetting);
          client.print(">on</option>");
          client.print("</select>");
          client.println("<span class='focus'></span>");
          client.println("</div>");

          client.println("</li>");

          client.println("<li>");
          client.println("<label for='TempUnits'>Temperature Units</label>");
          client.println("<div class='select'>");
          client.print("<select name='TempUnits' id='TempUnits'>");
          client.print("<option value='C'>C</option>");
          client.print("<option value='F' ");
         // client.print (ShowTempUnitsSetting);
          client.print(">F</option>");
          client.print("</select>");
          client.println("<span class='focus'></span>");
          client.println("</div>");

          client.println("</li>");
          client.println("<li>");
          client.println("<label for='12hr'>Show 12hr Time</label>");
          client.println("<div class='select'>");
          client.println("<select name='12hr' id='12hr'>");
          client.print("<option value='off'>off</option>");
          client.print("<option value='on' ");
          //client.print (twelvehour);
          client.print (">on</option>");
          client.print("</select>");
          client.println("<span class='focus'></span>");
          client.println("</div>");

          client.println("</li>");
          client.println("<li>");
          client.println("<label for='timezone'>Timezone</label>");
          client.println("<div class='select'>");
          client.println("<select name='timezone' id='timezone'>");
          client.print("<option value='EST' ");
  
          client.print (">Eastern</option>");
          client.print("<option value='CST' ");

          client.print (">Central</option>");
          client.print("<option value='MST' ");
 
          client.print (">Mountain</option>");
          client.print("<option value='PST' ");
   
          client.print (">Pacific</option>");
          client.print("<option value='AKST' ");
    
          client.print (">Alaska</option>");
          client.print("<option value='HST' ");
        
          client.print (">Hawaii</option>");

    //      client.print (twelvehour);

          client.print("</select>");

          client.println("<li>");
          client.println("<label for='dimmer'>Enable Auto Dimming</label>");
          client.println("<div class='select'>");
          client.println("<select name='dimmer' id='dimmer'>");
          client.print("<option value='on'>on</option>");
          client.print("<option value='off' ");
        ////  client.print (DimmerSetting);
          client.print (">off</option>");
          client.print("</select>");
          client.println("<span class='focus'></span>");
          client.println("</div>");

          client.println("</li>");
          client.println("<li>");



          client.println("<label for='myrange'>Display Brightness</label>");

          client.println("<div class='slidecontainer'>");
          client.println("<input type='range' name='range' min='1' max='255' ");


                    client.print("value='");
          
         //// String Brightnesslevel = (String)owner.brightness;
         //// Brightnesslevel.trim();
    //      client.print(Brightnesslevel);



          client.print("' class='slider' id='myRange'>");
//          client.println("<input type='hidden' name='rangevalue' ");
//            client.print("value='");
      //// client.print(Brightnesslevel);
//          client.print("' id='rangevalue'>");
         // client.println(" <p>Value: <span id='demo'></span></p>");
          client.println(" </div>");

          client.println("</li>");



          client.println("<li>");
          client.println("<button id='Submit' name='Submit' value='Submit' onclick='submit();' type='submit'>Save</button>");
          client.println("</li>");
          client.println("</ul>");
          client.println("</div>");
  client.println("<script>");
          client.println("function submit() {");
    client.println("var FormValid = true;");
    client.println("var data = [];");

   client.println(" const container = document.querySelector('div.formcontent');");
   client.println(" container.querySelectorAll('input').forEach(function (e) {");
    client.println("  if (e.validity.valueMissing) {");
    client.println("    FormValid = false;");
    client.println(";  }");
    
    client.println("  data[e.id] = e.value;");

    client.println("});");
    client.println("container.querySelectorAll('select').forEach(function (e) {");
      client.println("data[e.id] = e.value;");

    client.println("});");


    client.println("encodeDataToURL = (data) => {");
      client.println("return Object");
        client.println(".keys(data)");
        client.println(".map(value => `${value}=${encodeURIComponent(data[value])}`)");
        client.println(".join('&');");
    client.println("}");

    //console.log(encodeDataToURL(data));
   client.println("var mypost = encodeDataToURL(data)");
   client.println("mypost = mypost + '&Submit=Submit'");
    //console.log(mypost);
    client.println("if (FormValid) {");
      client.println("var request = new XMLHttpRequest();");
      client.println("request.open('GET', '/', true);");
      client.println("request.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');");
client.println("request.setRequestHeader('mydata','/?'+encodeDataToURL(data));");
      client.println("request.onreadystatechange = function () {");
        client.println("if (this.readyState == XMLHttpRequest.DONE && this.status == 200) {");
          client.println("console.log('succeed');");
          //client.println("el.classList.add('hidden');");
          //  myresponse.value = request.responseText;
        client.println("} else {");
          client.println("console.log('server error');");
        client.println("}");
      client.println("};");

      client.println("request.onerror = function () {");
        client.println("console.log('something went wrong');");
      client.println("};");

      client.println("request.send(mypost);");
    client.println("}");
  client.println("}");
          client.println("           </script>");





          client.println("</html>");

          break;

        }

        if (c == '\n') {
          Serial.println(readString);
          if (readString.indexOf("ssid") > 0) {
            char Buf[250];
            Serial.println(readString);
            readString.toCharArray(Buf, 250);
            char *token = strtok(Buf, "/?"); // Get everything up to the /if(token) // If we got something
            {
              char *name = strtok(NULL, "="); // Get the first name. Use NULL as first argument to keep parsing same string
              while (name){
                char *valu = strtok(NULL, "&");
                if (valu)
                {
                   Serial.println(String(name));
                  Serial.println(String(valu));
                   name = strtok(NULL, "="); // Get the next name
                }
              }
            }
          }
            client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          readString = "";
          // you're starting a new line

          currentLineIsBlank = true;

        } else if (c != '\r') {

          // you've gotten a character on the current line
          //Serial.println("b");
          currentLineIsBlank = false;

        }

      }

    }

    // give the web browser time to receive the data

    delay(1);

    // close the connection:

    client.stop();

    Serial.println("client disonnected");

  }
}

void printWifiStatus() {

  // print the SSID of the network you're attached to:

  Serial.print("SSID: ");

  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:

  IPAddress ip = WiFi.localIP();

  Serial.print("IP Address: ");

  Serial.println(ip);

  // print the received signal strength:

  long rssi = WiFi.RSSI();

  Serial.print("signal strength (RSSI):");

  Serial.print(rssi);

  Serial.println(" dBm");
}
