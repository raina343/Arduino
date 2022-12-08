int TimeDigits[172];
//Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
int nums[][21] = {
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 15, 16, 17, -1, -1, -1},//0
  { 0,  1,  2, -1, -1, -1, -1, -1, -1,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20},//1
  {-1, -1, -1,  3,  4,  5, -1, -1, -1, -1, -1, -1, 12, 13, 14, -1, -1, -1, -1, -1, -1},//2
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, -1, -1, -1, 18, 19, 20},//3
  { 0,  1,  2, -1, -1, -1, -1, -1, -1,  9, 10, 11, -1, -1, -1, -1, -1, -1, 18, 19, 20},//4
  {-1, -1, -1, -1, -1, -1,  6,  7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, 19, 20}, //5
  {-1, -1, -1, -1, -1, -1,  6, -7,  8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},//6
  {-1, -1, -1,  3,  4,  5,  6,  7,  8,  9, 10, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1},//7
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},//8
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 18, 19, 20},//9
  
};




void setup() {
  Serial.begin(9600);

  //so lets presume I need a time of 17:33
  int pixels[172];
  //this means I need a 1, 7, 3 and 3
  for ( int j = 0; j < 21; j++ ) { //first digit.  a 3
    pixels[j] = nums[3][j];
  }

  for ( int j = 21; j < 42; j++ ) { //second digit... a 3
    if (nums[3][j - 21] == -1) {
      pixels[j] = -1;
    } else {
      pixels[j] = nums[3][j - 21] + 21;
    }
  }
  pixels[42] = 42; //dots
  pixels[43] = 43;// dots

  for ( int j = 44; j < 65; j++ ) { //third digit.. a 7
    if (nums[7][j - 44] == -1) {
      pixels[j] = -1;
    } else {

      pixels[j] = nums[7][j - 44] + 44;
    }
  }
  for ( int j = 65; j < 86; j++ ) { //fourth digit... a 1
    if (nums[1][j - 65] == -1) {
      pixels[j] = -1;
    } else {
      pixels[j] = nums[1][j - 65] + 65;
    }
  }

  for ( int j = 86; j < 107; j++ ) { //fifth digit... a 3
    if (nums[3][j - 86] == -1) {
      pixels[j] = -1;
    } else {

      pixels[j] = nums[3][j - 86] + 86;
    }
  }
  for ( int j = 107; j < 128; j++ ) { //sixth digit.. a 3
    if (nums[3][j - 107] == -1) {
      pixels[j] = -1;
    } else {

      pixels[j] = nums[3][j - 107] + 107;
    }
  }
  pixels[128] = 128; //dots
  pixels[129] = 129; //dots

  for ( int j = 130; j < 151; j++ ) { //sevenths digit... a 4
    if (nums[4][j - 130] == -1) {
      pixels[j] = -1;
    } else {

      pixels[j] = nums[4][j - 130] + 130;
    }
  }
  for ( int j = 151; j < 172; j++ ) { //eighth digit.. a 0
    if (nums[0][j - 151] == -1) {
      pixels[j] = -1;
    } else {
      pixels[j] = nums[0][j - 151] + 151;
    }
  }

  for (int jww = 0; jww < 172; jww++) {
    //
    //  if (pixels[jww] == -1) {} else {
    Serial.println(pixels[jww]);

    //}

  }

}

void loop() {

}



//for(int i = 0; i < 10; i++){
//  Serial.println(number[0][i]);
//}
//}
