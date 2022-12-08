const int pResistor = A0;
int lightvalue;

void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
lightvalue = analogRead(pResistor);
  Serial.println(lightvalue);
}
