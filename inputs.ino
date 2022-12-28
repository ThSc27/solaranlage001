#include "inputs.h"
#include "para.h"
#include "pinout.h"

unsigned char        u8_LasteingangDebTimer, u8_Ta1DebTimer, u8_Ta2DebTimer;

/**************************************************************************/
void ModeSetting(void)
{
  if (e_Taste2Event == ACTIVE)
  {
    heater_mode = next_heater_mode;
    wr1_mode = next_wr1_mode;
    wr2_mode = next_wr2_mode;
    wr2_target_power = next_wr2_target_power;
    selector ++;
    if (selector == max_sel) {selector = sel_none;}
  }
  if (e_Taste1Event == ACTIVE)
  {
    switch (selector) {
      case sel_heater: 
             if (next_heater_mode == perm_off)  {next_heater_mode = perm_on;}
        else if (next_heater_mode == perm_on)   {next_heater_mode = automatic;}
        else if (next_heater_mode == automatic) {next_heater_mode = perm_off;}
        break;
      case sel_wr1: 
             if (next_wr1_mode == perm_off)  {next_wr1_mode = perm_on;}
        else if (next_wr1_mode == perm_on)   {next_wr1_mode = perm_off;}
//      else if (next_wr1_mode == perm_on)   {next_wr1_mode = automatic;}
        break;
      case sel_wr2: 
             if (next_wr2_mode == perm_off)  {next_wr2_mode = perm_on;}
        else if (next_wr2_mode == perm_on)   {next_wr2_mode = perm_off;}
//      else if (next_wr2_mode == perm_on)   {next_wr2_mode = automatic;}
        break;
     case sel_wr2_power:
             if (next_wr2_target_power == 10)   {next_wr2_target_power = 15;}
        else if (next_wr2_target_power == 15)   {next_wr2_target_power = 20;}
        else if (next_wr2_target_power == 20)   {next_wr2_target_power = 25;}
        else if (next_wr2_target_power == 25)   {next_wr2_target_power = 30;}
        else if (next_wr2_target_power == 30)   {next_wr2_target_power = 10;}
        break;
    }
  }
  e_Taste1Event = INACTIVE;
  e_Taste2Event = INACTIVE;
}



/**************************************************************************/
void ReadAndDebounceLasteingang(void) {

  u16_LasteingangRawVal = analogRead(IN_LASTFREIGABE);
  
  if      (u16_LasteingangRawVal < INACT_THRESH)
  {
    Deb0Inact();
  }
  else if (u16_LasteingangRawVal > ACT_THRESH)
  {
    Deb0Act();
  }
  else  {  }
}

void Deb0Act(void)
{
    if (u8_LasteingangDebTimer > 0)   { u8_LasteingangDebTimer--;  }
    else    {      e_LasteingangState = ACTIVE;    }
}
void Deb0Inact(void)
{
    if (u8_LasteingangDebTimer < DEBTIME)    {  u8_LasteingangDebTimer++;    }
    else  {      e_LasteingangState = INACTIVE;    }
}

void ReadAndDebounceT1(void) {
  uint16_t RawVal = analogRead(IN_TA1);
  if      (RawVal < INACT_THRESH)
  {
    DebT1Act();
  }
  else if (RawVal > ACT_THRESH)
  {
    DebT1Inact();
  }
  else  {  }
}

void DebT1Act(void)
{
    if (u8_Ta1DebTimer > 0)   { u8_Ta1DebTimer--;  }
    else    {      
      if(e_Taste1 != ACTIVE)
      {  e_Taste1Event = ACTIVE; }
      e_Taste1 = ACTIVE;
      }
}
void DebT1Inact(void)
{
    if (u8_Ta1DebTimer < TA_DEBTIME)    {  u8_Ta1DebTimer++;    }
    else  {      e_Taste1 = INACTIVE;    }
}

void ReadAndDebounceT2(void) {
  uint16_t RawVal = analogRead(IN_TA2);
  if      (RawVal < INACT_THRESH)
  {
    DebT2Act();
  }
  else if (RawVal > ACT_THRESH)
  {
    DebT2Inact();
  }
  else  {  }
}

void DebT2Act(void)
{
    if (u8_Ta2DebTimer > 0)   { u8_Ta2DebTimer--;  }
    else    {
      if(e_Taste2 != ACTIVE)
      {  e_Taste2Event = ACTIVE; }
      e_Taste2 = ACTIVE;    
      }
}
void DebT2Inact(void)
{
    if (u8_Ta2DebTimer < TA_DEBTIME)    {  u8_Ta2DebTimer++;    }
    else  {      e_Taste2 = INACTIVE;    }
}
