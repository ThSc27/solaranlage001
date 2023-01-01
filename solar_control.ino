// include local headers 
#include "pinout.h"
#include "tempsense.h"
#include "chars.h"
#include "wr_state.h"
#include "inputs.h"
#include "para.h"

// include libraries
#include <LiquidCrystal.h>
#include <OneWire.h>

const int ledPin =  LED_BUILTIN;

#undef Debug_Serial
  
typedef enum {
    dcdc_off     = 0,
    fix_value    = 1,
    auto_regel   = 2,
    test         = 3  
}dcdccontrolT;

/* create object instances */
LiquidCrystal       lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);     // LiquidCrystal(rs, enable, d4, d5, d6, d7) 
OneWire             ds1(DS_SENS1);
OneWire             ds2(DS_SENS2);
TempSenseDatT       temp1, temp2;
unsigned char       uptime_sec, uptime_min, uptime_h, uptime_ms;
unsigned long       prevMillis = 0;
bool                heater_state = false;
dcdccontrolT        dcdc_controlmode = dcdc_off;

void setup() {
    pinMode(ledPin, OUTPUT);

    // initialize and start lcd display
    delay(100);
    lcd.createChar(CHAR_GRADC,  gradcelsius);
    lcd.createChar(CHAR_HEATER, heater);
    lcd.createChar(CHAR_OFF,    onoff_off);
    lcd.createChar(CHAR_ON,     onoff_on);
    lcd.createChar(CHAR_PROGR1, progress1);
    lcd.createChar(CHAR_PROGR2, progress2);
    lcd.createChar(CHAR_PROGR3, progress3);
    lcd.createChar(CHAR_PROGR4, progress4);
    delay(100);
    lcd.begin(20,4);

    // initialize and start temp sensors
    ds1.search(temp1.addr);
    ds1.reset();
    ds2.search(temp2.addr);
    ds2.reset();
    temp2.u8_Counter = 5;

    // initialize and start RS485 communication with DCDC-converter
    pinMode(RS485_DE, OUTPUT);
    Serial2.begin(9600);

    // initialize LS drivers
    pinMode(LS_WR1SRC, OUTPUT);
    pinMode(LS_ELKODIREKT, OUTPUT);
    pinMode(LS_WR2SRC, OUTPUT);
    pinMode(LS_DCDCPOWER, OUTPUT);
    pinMode(LS_HEIZUNG, OUTPUT);
    pinMode(LS_6, OUTPUT);

#ifdef Debug_Serial
    Serial.begin(9600);
#endif
}

/* **** Mainloop timing: 100ms scheduling **********************************************/
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - prevMillis >= 100) {
    prevMillis = currentMillis;
  
  count_uptime();
  ModeSetting();
  ReadTempSensor1();
  ReadTempSensor2();
  ReadAndDebounceLasteingang();
  ReadAndDebounceT1();
  ReadAndDebounceT2();
  HeatControl();
  WrControl();        /* Relais-sequenzen für Wechselrichterschaltung */ 
  if (wr2_status == run)  {
    dcdc_controlmode = fix_value;
  }
  else  {
    dcdc_controlmode = dcdc_off;
  }
  ControlDCDC();      /* WR2 DCDC-Wandler einstellen */
  RefreshDisplay();
  }
}

void count_uptime(void)
{
  uptime_ms++;
  if (uptime_ms==10){
  uptime_ms=0;
  uptime_sec++;
  if(uptime_sec==60){
    uptime_sec = 0;
    uptime_min++;
    if(uptime_min==60){
      uptime_min = 0;
      uptime_h++;    }}}
}

void RefreshDisplay(void)
{
  static char toggle, rotate;
  bool rot_now = false;
      rotate++; if (rotate == 8){rotate = 0;}
      /* --- uptime ----------------------------------------------------------------------------------------- */
      toggle++; if (toggle == 10){toggle = 0;}
      if ((toggle == 0)||(toggle == 4)) {
        lcd.setCursor(0, 0);
        if (uptime_h<10){lcd.print("0");}
            lcd.print(uptime_h);
        if (toggle == 0){lcd.print(":");} else if (toggle == 4){lcd.print(" ");}
        if (uptime_min<10){lcd.print("0");}
            lcd.print(uptime_min);
        if (toggle == 0){lcd.print(":");} else if (toggle == 4){lcd.print(" ");}
        if (uptime_sec<10){lcd.print("0");}
            lcd.print(uptime_sec);
      }
      /* --- WR 1 status -------------------------------------------------------------------------------------- */
      lcd.setCursor(10, 0);
      lcd.print("   output");
      lcd.setCursor(8, 1);
      if (selector == sel_wr1){PrintPfeilnachrechts();}else{lcd.print(" ");}
      if (next_wr1_mode == perm_off){lcd.write(byte(CHAR_OFF));}else if(next_wr1_mode == perm_on){lcd.write(byte(CHAR_ON));}else{PrintLiegendeAcht();}
      lcd.print("WR1 ");
      switch (wr1_status) {
        case off:       lcd.print("off  "); break;
        case charge:    lcd.print("chg  "); rot_now = true; break;
        case run:       lcd.print("300W "); rot_now = true; break;
        case discharge: lcd.print("dis  "); rot_now = true; break;
      }
      if (rot_now == true) {
        switch (rotate){
          case 0: lcd.write(byte(CHAR_PROGR1)); break;
          case 2: lcd.write(byte(CHAR_PROGR2)); break;
          case 4: lcd.write(byte(CHAR_PROGR3)); break;
          case 6: lcd.write(byte(CHAR_PROGR4)); break;
        } }
        else { lcd.print(" ");}
      rot_now = false;
      /* --- WR 2 status -------------------------------------------------------------------------------------- */
      lcd.setCursor(8, 2);
      if (selector == sel_wr2){PrintPfeilnachrechts();}else{lcd.print(" ");}
      if (next_wr2_mode == perm_off){lcd.write(byte(CHAR_OFF));}else if(next_wr2_mode == perm_on){lcd.write(byte(CHAR_ON));}else{PrintLiegendeAcht();}
      lcd.print("WR2 ");
      if (selector == sel_wr2_power){
        PrintPfeilnachrechts();
        lcd.print(next_wr2_target_power);lcd.print("0W ");
        }
      else {
        switch (wr2_status) {
          case off:       lcd.print("off  "); break;
          case charge:    lcd.print("start"); rot_now = true; break;
          case run:       lcd.print(wr2_target_power);lcd.print("0W "); rot_now = true; break;
          case discharge: lcd.print("dis  "); rot_now = true; break;
        }
      }
      if (rot_now == true) {
        switch (rotate){
          case 0: lcd.write(byte(CHAR_PROGR1)); break;
          case 2: lcd.write(byte(CHAR_PROGR2)); break;
          case 4: lcd.write(byte(CHAR_PROGR3)); break;
          case 6: lcd.write(byte(CHAR_PROGR4)); break;
        } }
      else { lcd.print(" ");}
      rot_now = false;
      /* --- Lastfreigabe  ---------------------------------------------------------------------------------- */
      lcd.setCursor(11, 3);
      lcd.print("LoadEn ");
      if (isLasteingang()==ACTIVE){ lcd.write(byte(CHAR_ON));} else { lcd.write(byte(CHAR_OFF));}
      
      /* --- TempSensoren ----------------------------------------------------------------------------------- */
      lcd.setCursor(1,1);
      if (temp1.gu8_TempSensFound > 0) {
        lcd.print(temp1.Temperatur.TempGanz); lcd.print("."); lcd.print(temp1.Temperatur.TempNachkomma);
        lcd.write(byte(CHAR_GRADC)); lcd.print(" ");  }
     else { lcd.print("/     "); }
      lcd.setCursor(1, 2);
      if (temp2.gu8_TempSensFound > 0) {
        lcd.print(temp2.Temperatur.TempGanz); lcd.print("."); lcd.print(temp2.Temperatur.TempNachkomma);
        lcd.write(byte(CHAR_GRADC)); lcd.print(" ");  }
      else { lcd.print("/     "); }
  
      /* --- Heaterstate ----------------------------------------------------------------------------------- */
      lcd.setCursor(0, 3);
      if (selector == sel_heater){PrintPfeilnachrechts();}else{lcd.print(" ");}
      if (next_heater_mode == perm_off){lcd.write(byte(CHAR_OFF));}else if(next_heater_mode == perm_on){lcd.write(byte(CHAR_ON));}//else{PrintLiegendeAcht();}
      lcd.write(byte(CHAR_HEATER));
      if (heater_state == true){lcd.print(" on ");}else{lcd.print(" off");}



/* misc / test */
//      if (e_Taste1==ACTIVE){ lcd.write(byte(CHAR_ON));} else { lcd.write(byte(CHAR_OFF));}
//      if (e_Taste2==ACTIVE){ lcd.write(byte(CHAR_ON));} else { lcd.write(byte(CHAR_OFF));}

//      PrintLiegendeAcht();
//      lcd.write(byte(0x7E));
      
//      lcd.print(u16_LasteingangRawVal);
//        switch (rotate){
//          case 0: lcd.write(byte(CHAR_PROGR1)); break;
//          case 2: lcd.write(byte(CHAR_PROGR2)); break;
//          case 4: lcd.write(byte(CHAR_PROGR3)); break;
//          case 6: lcd.write(byte(CHAR_PROGR4)); break;
//        } 


}


void ReadTempSensor1(void) {
  byte i, data[12];
  bool nichtnull = false;
  temp1.u8_Counter++;
  if (temp1.u8_Counter == 1) {
    if ( !ds1.search(temp1.addr)) {
        ds1.reset_search();
        if (temp1.gu8_TempSensFound > 0){temp1.gu8_TempSensFound--;}
        return;
    }
    ds1.reset();
    ds1.select(temp1.addr);
    ds1.write(0x44,1);             // start conversion
  }
  else if (temp1.u8_Counter == 10){
    temp1.gu8_TempSensFound = 10;
    ds1.reset();
    ds1.select(temp1.addr);    
    ds1.write(0xBE);               // read result
    for ( i = 0; i < 9; i++) {    // 9 bytes
      data[i] = ds1.read();
      if (data[i] != 0){nichtnull = true;}
    }
    uint8_t crc = OneWire::crc8(data, 8);
    if ((crc == data[8]) && (nichtnull == true))
    {
      temp1.Temperatur = ConvertToRealTemp(data[1], data[0]);
      temp1.u8_Counter = 0;
    }
    else
    {
      temp1.u8_Counter = 0;
      temp1.gu8_TempSensFound = 0;
    }
  }
  else { }
}

void ReadTempSensor2(void) {
  byte i, data[12];
  bool nichtnull = false;
  temp2.u8_Counter++;
  if (temp2.u8_Counter == 1) {
    if ( !ds2.search(temp2.addr)) {
        ds2.reset_search();
        if (temp2.gu8_TempSensFound > 0){temp2.gu8_TempSensFound--;}
        return;
    }
    ds2.reset();
    ds2.select(temp2.addr);
    ds2.write(0x44,1);             // start conversion
  }
  else if (temp2.u8_Counter == 10){
    temp2.gu8_TempSensFound = 10;
    ds2.reset();
    ds2.select(temp2.addr);    
    ds2.write(0xBE);               // read result
    for ( i = 0; i < 9; i++) {    // 9 bytes
      data[i] = ds2.read();
      if (data[i] != 0){nichtnull = true;}
    }
    uint8_t crc = OneWire::crc8(data, 8);

#ifdef Debug_Serial
    for ( i = 0; i < 9; i++) {    // 9 bytes
      Serial.print(data[i]);
      Serial.print(" ");}
      Serial.print(" crc:");
      Serial.print(crc);
      Serial.print("\n");
#endif

  if ((crc == data[8]) && (nichtnull == true))
  {
    temp2.Temperatur = ConvertToRealTemp(data[1], data[0]);
    temp2.u8_Counter = 0;
  }
  else
  {
    temp2.u8_Counter = 0;
    temp2.gu8_TempSensFound = 0;
  }
  }
  else { }
}


TemperaturT ConvertToRealTemp(byte au8_Hibyte, byte au8_Lowbyte) 
{
  int  u16_TReadRaw, SignBit, Tc_100;
  TemperaturT retval;
  u16_TReadRaw = (au8_Hibyte << 8) + au8_Lowbyte;
  SignBit = u16_TReadRaw & 0x8000;                  // test most sig bit
  if (SignBit) {                                    // negative
    u16_TReadRaw = (u16_TReadRaw ^ 0xffff) + 1;     // 2's comp
  }
  Tc_100 = (6 * u16_TReadRaw) + u16_TReadRaw / 4;   // mult by (100 * 0.0625) = 6.25
  retval.TempGanz       = Tc_100 / 100;             // Ganzzahlen und Brüche trennen
  retval.TempNachkomma  = (Tc_100 % 100) / 10;      // auf 0,1deg Genauigkeit
  return retval;
}

/* simple HeatControl 1.0.0: 
 *  - wenn beide Sensoren unter T_on --> Heizung ein 
 *  - wenn ein Sensor über T_off     --> Heizung aus 
 */
void HeatControl(void) {
switch (heater_mode){
  case perm_off: digitalWrite(LS_HEIZUNG, 0);  /* Heizung AUS */
    heater_state = false;
    break;
  case perm_on:  digitalWrite(LS_HEIZUNG, 1);  /* Heizung EIN */
    heater_state = true;
    break;
  case automatic:
    if ((temp1.gu8_TempSensFound > 0) && (temp2.gu8_TempSensFound > 0))
    {
      if((temp1.Temperatur.TempGanz < HZG_THRESH_ON) && (temp2.Temperatur.TempGanz < HZG_THRESH_ON))
      {
        digitalWrite(LS_HEIZUNG, 1);  /* Heizung EIN */
        heater_state = true;
      }
      else if((temp1.Temperatur.TempGanz > HZG_THRESH_OFF) || (temp2.Temperatur.TempGanz > HZG_THRESH_OFF))
      {
        digitalWrite(LS_HEIZUNG, 0);  /* Heizung AUS */
        heater_state = false;
      }
      else
      {/* keep previous state */}
    }
    else
    {
      digitalWrite(LS_HEIZUNG, 0);  /* Heizung AUS */
      heater_state = false;
    }
    break;
  }
}

void DCDC_out(bool onoff)
{
  digitalWrite(RS485_DE, 1);
  if (onoff == true){ Serial2.print(":01w12=1,\r\n"); }
  else  {             Serial2.print(":01w12=0,\r\n"); }
  Serial2.flush();                                  // wait until complete pattern has been sent
  digitalWrite(RS485_DE, 0);
}

/* set output voltage in 10mV (1000 = 10,0V) */
bool DCDC_setVoltage(int voltage)
{
  bool retval = false;
  String RxBuf;
  digitalWrite(RS485_DE, 1);                        // select tx mode
  Serial2.print(":01w10=");                         // send "set voltage" command
  if      (voltage < 10)  {Serial2.print("000");}   // fill voltage number with leading 0's
  else if (voltage < 100) {Serial2.print("00");}
  else if (voltage < 1000){Serial2.print("0");}
  Serial2.print(voltage);                           // final value
  Serial2.print(",\r\n");                           // complete command
  Serial2.flush();                                  // wait until complete pattern has been sent
  digitalWrite(RS485_DE, 0);                        // select rx mode
//  Serial2.setTimeout(20);                           // adjust 20ms rx timeout
//  RxBuf = Serial2.readString();                     // read answer
//  if (RxBuf.startsWith(":01ok"))                    // good ok answer?
//  {
//    retval = true;
//  }
//  return retval;
}

/* set output current in mA */
void DCDC_setCurrent(int current)
{
  digitalWrite(RS485_DE, 1);
  Serial2.print(":01w11=");
  if      (current < 10)  {Serial2.print("000");}
  else if (current < 100) {Serial2.print("00");}
  else if (current < 1000){Serial2.print("0");}
  Serial2.print(current);
  Serial2.print(",\r\n");
  Serial2.flush();                                  // wait until complete pattern has been sent
  digitalWrite(RS485_DE, 0);                        // select rx mode
}
/* read actual output current in mA */
int DCDC_readCurrent(void)
{
  String RxBuf, CurrVal;
  int result;
  digitalWrite(RS485_DE, 1);      // select tx mode
  Serial2.print(":01r31=0,\r\n"); // send "read actual current"-command
  Serial2.flush();                // wait until complete command has been sent
  digitalWrite(RS485_DE, 0);      // switch back to receive mode
  Serial2.setTimeout(60);         // adjust 40ms rx timeout
  RxBuf = Serial2.readString();   // read answer
  if (RxBuf.startsWith(":01r31="))                    // good ok answer?
  {
    CurrVal = RxBuf[7];
    if (RxBuf[8] != ".") {
        CurrVal += RxBuf[8];
        if (RxBuf[9] != ".") {
            CurrVal += RxBuf[9];
            if (RxBuf[10] != ".") {
                CurrVal += RxBuf[10];}}}
    result = CurrVal.toInt();
  }
  return result;
}



void ControlDCDC(void)
{
    /* used commands for DPM8624 DC-DC-Converter:
     * ":01w10=0123,\r\n" - output voltage in 10mV (0123 = 1,23V)
     * ":01w11=1234,\r\n" - output current in mA (1234 = 1,234A)
     * ":01w12=1,\r\n"    - switch output ON      
     * ":01w12=0,\r\n"    - switch output OFF
     *
     * ":01r31=0,\r\n" - read actual output current
     */
    static int dcdc_delay=0, dcdc_outputon=0, spannung, curr_meas;
    /* */
//  static uint8_t prev_power;

  dcdc_delay++; /* Update alle 3 sekunden */
  if (dcdc_delay >= 30){ 
//    dcdc_delay = 0;
//    if (prev_power != wr2_target_power){
    switch (dcdc_controlmode) {
      case dcdc_off:
            if (dcdc_delay == 30) {DCDC_setVoltage(0);}
            if (dcdc_delay == 31) {DCDC_setCurrent(0);}
            if (dcdc_delay == 32) {DCDC_out(false); dcdc_delay = 0; }
            break;
      case fix_value:
        /* festen Leistungswert an DCDC-Wandler schicken */
        switch (wr2_target_power) {
          case 10:  /* 100W: Spannung = 33V, Strom = 3,3A */
            if (dcdc_delay == 30) {DCDC_setVoltage(3300);}
            if (dcdc_delay == 31) {DCDC_setCurrent(3300);}
            if (dcdc_delay == 32) {DCDC_out(true); dcdc_delay = 0; }
            break;
          case 15:  /* 150W: Spannung = 33V, Strom = 5A */
            if (dcdc_delay == 30) {DCDC_setVoltage(3300);}
            if (dcdc_delay == 31) {DCDC_setCurrent(4800);}
            if (dcdc_delay == 32) {DCDC_out(true); dcdc_delay = 0; }
            break;
          case 20:  /* 200W: Spannung = 31V, Strom = 6,5A */
            if (dcdc_delay == 30) {DCDC_setVoltage(3300);}
            if (dcdc_delay == 31) {DCDC_setCurrent(6500);}
            if (dcdc_delay == 32) {DCDC_out(true); dcdc_delay = 0; }
            break;
          case 25:  /* 250W: Spannung = 35V, Strom = 7,2A */
            if (dcdc_delay == 30) {DCDC_setVoltage(3300);}
            if (dcdc_delay == 31) {DCDC_setCurrent(8000);}
            if (dcdc_delay == 32) {DCDC_out(true); dcdc_delay = 0; }
            break;
          case 30:  /* 300W: Spannung = 40V, Strom = 10A (wird durch WR begrenzt) */
            if (dcdc_delay == 30) {DCDC_setVoltage(3300);}
            if (dcdc_delay == 31) {DCDC_setCurrent(10000);}
            if (dcdc_delay == 32) {DCDC_out(true); dcdc_delay = 0; }
            break;
          default:
            break;
      }
    break;
  
  case auto_regel:
  break;
  case test:
    dcdc_delay++;
    spannung++;
    if (dcdc_delay == 5)
    {
      if (spannung < 300 ){ DCDC_setVoltage(spannung);}
    }
    if (dcdc_delay == 10)
    {
      dcdc_delay = 0;
      if (dcdc_outputon == 0)
      { 
        dcdc_outputon = 1;
        DCDC_out(true);
      }
      else
      {
        curr_meas = DCDC_readCurrent();
        lcd.setCursor(10, 0);
        lcd.print(curr_meas);
        lcd.print("mA");
      }
    }
  break;  
  }
//    }
//  prev_power = wr2_target_power;
    
  }
}
