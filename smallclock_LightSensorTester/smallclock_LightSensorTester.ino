int lightvalue;
const int pResistor = A0;


void setup() {
  Serial.begin(9600);

  
  pinMode(pResistor, INPUT); 

}



void loop() {
 lightvalue = analogRead(pResistor);
           Serial.println(lightvalue);

     
      if (lightvalue > 800) {

      }
      if ((lightvalue > 250) && (lightvalue < 700)) {

      }
       if ((lightvalue > 50) && (lightvalue < 250)) {

      }
      if (lightvalue < 50) {

      }


}
