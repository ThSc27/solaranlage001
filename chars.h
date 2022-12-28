#define CHAR_GRADC  0
#define PrintLiegendeAcht()         (lcd.write(byte(0xF3)))
#define PrintPfeilnachrechts()      (lcd.write(byte(0x7E)))

byte gradcelsius[8] = {
  B11000,
  B11011,
  B00100,
  B01100,
  B01100,
  B01100,
  B00111
};
#define CHAR_HEATER  1
byte heater[8] = {
  B10010,
  B01001,
  B10010,
  B10010,
  B01001,
  B11111,
  B11111
};

#define CHAR_OFF  2
byte onoff_off[8] = {
  B00000,
  B01110,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000
};

#define CHAR_ON  3
byte onoff_on[8] = {
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000
};

#define CHAR_PROGR1  4
byte progress1[8] = {
  B00000,
  B00000,
  B11000,
  B11000,
  B11000,
  B01110,
  B00000
};
#define CHAR_PROGR2  5
byte progress2[8] = {
  B00000,
  B01110,
  B11110,
  B10000,
  B10000,
  B00000,
  B00000
};
#define CHAR_PROGR3  6
byte progress3[8] = {
  B00000,
  B01110,
  B00011,
  B00011,
  B00011,
  B00000,
  B00000
};
#define CHAR_PROGR4  7
byte progress4[8] = {
  B00000,
  B00000,
  B00001,
  B00001,
  B01111,
  B01110,
  B00000
};
