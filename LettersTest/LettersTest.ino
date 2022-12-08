#include <Adafruit_NeoPixel.h>

#include "RTClib.h"
//RTCZero rtc;
RTC_DS3231 rtc;
#define DayLED_PIN    4
#define Day_Count  78
#define DateLED_PIN    3
#define Date_Count  106
int pixelsDOW[78];
Adafruit_NeoPixel stripDay(Day_Count, DayLED_PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripDate(Date_Count, DateLED_PIN, NEO_GRB + NEO_KHZ800);
//each digit has 26 pixels
int pixelsDate[106];
int numdigits(int i) {
  char str[20];

  sprintf(str, "%d", i);
  return (strlen(str));
}


void outputDigitsDOW(char letter1, char letter2, char letter3) {
  int nums[][26] = {
    // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25
    {  0,  1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, -1, -1, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // m //0
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // o //1
    {  0,  1, -1, -1, -1, -1,  6,  7, -1, -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, 20, 21, 22, 23, -1, -1  }, // n //2
    {  0,  1,  2,  3,  4,  5, -1, -1,  8,  9, 10, 11, 12, 13, -1, -1, 16, 17, 18, 19, 20, 21, -1, -1, 24, 25  }, // t //3
    { -1, -1, -1, -1, -1, -1,  6,  7, -1, -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // u //4
    { -1, -1,  2,  3,  4,  5, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // e //5
    { -1, -1, -1, -1, -1, -1,  6,  7, -1, -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, -1, -1, 24, 25  }, // w //6
    {  0, -1, -1, -1, -1, -1, -1,  7,  8,  9, 10, 11, 12, 13, -1, -1, 16, 17, 18, 19, 20, 22, -1, -1, 24, 25  }, // d //7
    {  0,  1, -1, -1, -1, -1,  6,  6, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // h //8
    {  0,  1,  2,  3, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, -1, -1  }, // r //9
    {  0,  1,  2,  3,  4,  5, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // f //10
    {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, -1, -1, 16, 17, 18, 19, 20, 21, -1, -1, 24, 25  }, // i //11
    { -1, -1, -1, -1,  4,  5, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  }, // s //12
    {  0,  1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // a //13
  };
  switch (letter1) {
      break;
    case 'm':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[0][j - 0] ;
      }
      break;
    case 'o':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[1][j - 0] ;
      }
      break;
    case 'n':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[2][j - 0] ;
      }
      break;
    case 't':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[3][j - 0] ;
      }
      break;
    case 'u':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[4][j - 0] ;
      }
      break;
    case 'e':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[5][j - 0];
      }
      break;
    case 'w':
      for (int j = 0; j < 26; j++) {  //
        pixelsDOW[j] = nums[6][j - 0];
      }
      break;
    case 'd':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[7][j - 0] ;
      }
      break;
    case 'h':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[8][j - 0] ;
      }
      break;
    case 'r':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[9][j - 0] ;
      }
      break;
    case 'f':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[10][j - 0] ;
      }
      break;
    case 'i':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[11][j - 0] ;
      }
      break;
    case 's':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[12][j - 0] ;
      }
      break;
    case 'a':
      for (int j = 0; j < 26; j++) {
        pixelsDOW[j] = nums[13][j - 0] ;
      }
      break;
    default:
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = j;
      }
      break;
  }
  switch (letter2) {
      break;
    case 'm':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[0][j - 26] ;
      }
      break;
    case 'o':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[1][j - 26] ;
      }
      break;
    case 'n':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[2][j - 26] ;
      }
      break;
    case 't':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[3][j - 26] ;
      }
      break;
    case 'u':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[4][j - 26] ;
      }
      break;
    case 'e':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[5][j - 26];
      }
      break;
    case 'w':
      for (int j = 26; j < 52; j++) {  //
        pixelsDOW[j] = nums[6][j - 26];
      }
      break;
    case 'd':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[7][j - 26] ;
      }
      break;
    case 'h':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[8][j - 26] ;
      }
      break;
    case 'r':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[9][j - 26] ;
      }
      break;
    case 'f':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[10][j - 26] ;
      }
      break;
    case 'i':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[11][j - 26] ;
      }
      break;
    case 's':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[12][j - 26] ;
      }
      break;
    case 'a':
      for (int j = 26; j < 52; j++) {
        pixelsDOW[j] = nums[13][j - 26] ;
      }
      break;
    default:
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = j - 26;
      }

      break;
  }
  switch (letter3) {
      break;
    case 'm':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[0][j - 52] ;
      }
      break;
    case 'o':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[1][j - 52] ;
      }
      break;
    case 'n':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[2][j - 52] ;
      }
      break;
    case 't':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[3][j - 52] ;
      }
      break;
    case 'u':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[4][j - 52] ;
      }
      break;
    case 'e':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[5][j - 52];
      }
      break;
    case 'w':
      for (int j = 52; j < 78; j++) {  //
        pixelsDOW[j] = nums[6][j - 52];
      }
      break;
    case 'd':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[7][j - 52] ;
      }
      break;
    case 'h':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[8][j - 52] ;
      }
      break;
    case 'r':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[9][j - 52] ;
      }
      break;
    case 'f':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[10][j - 52] ;
      }
      break;
    case 'i':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[11][j - 52] ;
      }
      break;
    case 's':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[12][j - 52] ;
      }
      break;
    case 'a':
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = nums[13][j - 52] ;
      }
      break;
    default:
      for (int j = 52; j < 78; j++) {
        pixelsDOW[j] = j - 52;
      }
      break;
  }
}

void setup() {
  Serial.begin(9600);
  stripDay.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  stripDay.show();   // Turn OFF all pixels ASAP
  stripDay.clear();
  stripDay.setBrightness(150);

  stripDate.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  stripDate.show();   // Turn OFF all pixels ASAP
  stripDate.clear();
  stripDate.setBrightness(150);
  rtc.begin();
  //outputDigits (9,2,3,5);
  //  char *c = &PSTDOW1[0];
  //     char *d = &PSTDOW2[0];
  //     char *e = &PSTDOW3[0];
  //  outputDigitsDOW ('s', 'a', 't');
  //stripDay.setPixelColor(i, stripDay.Color(150, 0,  150));

}
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return stripDay.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return stripDay.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return stripDay.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
void loop() {
  uint16_t i, j;

  for (i = 0; i < stripDay.numPixels(); i++) {
    if (pixelsDOW[i] == -1) {
      //      stripDay.setPixelColor(i, stripDay.Color(150, 0,  150));
      stripDay.setPixelColor(i, Wheel((i * 21 + j) & 255));

    } else {
      stripDay.setPixelColor(i, stripDay.Color(0, 0,  0));

      //  stripSecondRow.setPixelColor(i, Wheel((i * 21 + j) & 255));
    }
  }

  for (i = 0; i < stripDate.numPixels(); i++) {
    if (pixelsDate[i] == -1) {
      //      stripDay.setPixelColor(i, stripDay.Color(150, 0,  150));
      stripDate.setPixelColor(i, Wheel((i * 21 + j) & 255));

    } else {
      stripDate.setPixelColor(i, stripDate.Color(0, 0,  0));

      //  stripSecondRow.setPixelColor(i, Wheel((i * 21 + j) & 255));
    }
  }




  delay(3000);
  stripDay.show();
  stripDate.show();
  //Serial.println(rtc.getTemperature());ppp
  // outputDigitsDOW ('m', 'o', 'n');
  //outputDigitsDOW ('t', 'u', 'e');
  //outputDigitsDOW ('w', 'e', 'd');
  //outputDigitsDOW ('t', 'h', 'u');
  //outputDigitsDOW ('f', 'r', 'i');
  //outputDigitsDOW ('s', 'a', 't');
  //outputDigitsDOW ('s', 'u', 'n');

  Serial.print("Temperature: ");
  Serial.print(rtc.getTemperature());
  Serial.println(" C");




  Serial.println("xyzzy");
  outputDigitsTemp(40, "F");
  //  outputDigitsTemp(rtc.getTemperature(), "F");
}



void outputDigitsTemp(int Temperature, String units) {
  String temp1;
  String temp2;
  String temp3;
  int digit2;
  int digit3;
  int digit1;
  int Temp3;
  String TempUnits;
  bool ShowThirdDigit = false;
  if (units == "F") {
    Serial.println("CaveMan Temperature");
    Serial.println(numdigits(Temperature));
    Serial.println(Temperature);
    int Temperature2 = (Temperature * 1.8) + 32;
    temp1 = String(Temperature2).substring(0, 1);
    temp2 = String(Temperature2).substring(1, 2);
    if (numdigits(Temperature2) > 2) {
      ShowThirdDigit = true;
      temp3 = String(Temperature2).substring(2, 3);
      digit3 = temp3.toInt();
    } else {
    }
    digit1 = temp1.toInt();
    digit2 = temp2.toInt();
    Serial.println((String)digit1);
    Serial.println((String)digit2);
    TempUnits = "F";
  } else {
    temp1 = String(Temperature).substring(0, 1);
    temp2 = String(Temperature).substring(1, 2);
    if (numdigits(Temperature) > 2) {
      ShowThirdDigit = true;
      temp3 = String(Temperature).substring(2, 3);
      digit3 = temp3.toInt();
    } else {
    }
    digit1 = temp1.toInt();
    digit2 = temp2.toInt();
    TempUnits = "C";
  }
  int nums[][26] = {
    // 0   1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19  20  21  22  23  24  25  26
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // 0 //seems ok
    {  0,  1, -1, -1, -1, -1,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  },        // 1 //seems ok
    { -1, -1,  2,  3, -1, -1, -1, -1,  8,  9, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // 2 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1,  8,  9, -1, -1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  },   // 3
    {  0,  1, -1, -1, -1, -1,  6,  7, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  },      // 4 //ok
    { -1, -1, -1, -1,  4,  5, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  },   // 5////numbers represent the pixels that are NOT LIT
    { -1, -1, -1, -1,  4,  5, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  },   // 6
    {  0,  1, -1, -1, -1, -1, -1, -1,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  },      // 7 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  }, // 8 //seems ok
    { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  }, // 9 //seems ok
    { -1, -1,  2,  3,  4,  5, -1, -1, -1, -1, 10, 11, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  },     // C (for Temp)
    {  0,  1,  2,  3,  4,  5, -1, -1, -1, -1, -1, -1, 12, 13, 14, 15, 16, 17, -1, -1, 20, 21, 22, 23, 24, 25  },        // F (for Temp)
    {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  },            // all off
    {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25  },            // negative
  };

  if ((String)temp1 == "-") {
    for (int j = 0; j < 26; j++) { //first date digit.  month1
      pixelsDate[j] = nums[13][j];
    }
  } else {
    for (int j = 0; j < 26; j++) { //first date digit.  month1
      pixelsDate[j] = nums[digit1][j];
    }
  }
  for (int j = 26; j < 52; j++) {  // third digit.. a 7
    if (nums[digit2][j - 26] == -1) {
      pixelsDate[j] = -1;
    } else {
      pixelsDate[j] = nums[digit2][j - 26] + 26;
    }
  }

  if (ShowThirdDigit) {
    for (int j = 53; j < 79; j++) {  //third date digit.  day1
      if (nums[digit3][j - 53] == -1) {
        pixelsDate[j] = -1;
      } else {
        pixelsDate[j] = nums[digit3][j - 53] + 53;
      }
    }
  } else {
    for (int j = 53; j < 79; j++) {  // second digit... a 3
      if (nums[12][j - 53] == -1) {
        pixelsDate[j] = -1;
      } else {
        pixelsDate[j] = nums[12][j - 53] + 53;
      }
    }
  }



  if (units == "F") {
    for (int j = 79; j < 105; j++) {  // first digit.  a 3
      if (nums[11][j - 79] == -1) {
        pixelsDate[j] = -1;
      } else {
        pixelsDate[j] = nums[11][j - 79] + 79;
      }
    }
  } else {
    for (int j = 0; j < 14; j++) {  // first digit.  a 3
      for (int j = 79; j < 105; j++) {  // first digit.  a 3
        if (nums[10][j - 79] == -1) {
          pixelsDate[j] = -1;
        } else {
          pixelsDate[j] = nums[10][j - 79] + 79;
        }
      }
    }
  }
}
