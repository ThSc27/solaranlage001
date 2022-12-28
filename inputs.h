#ifndef INPUTS
#define INPUTS

#include "para.h"

typedef enum
{
  INACTIVE            = 0,
  ACTIVE              = 1
}InputT;

typedef enum{
  perm_off  = 0,    /* permanent off */
  perm_on   = 1,    /* permanent on */
  automatic = 2     /* automatic mode */
}ModeT;

#define sel_none      0
#define sel_heater    1
#define sel_wr1       2
#define sel_wr2       3
#define sel_wr2_power 4
#define max_sel       5

unsigned int         u16_LasteingangRawVal;
InputT               e_LasteingangState;
InputT               e_Taste1, e_Taste2; 
InputT               e_Taste1Event, e_Taste2Event; 
unsigned char        selector;
unsigned char        wr2_target_power = 10, next_wr2_target_power = 10;     /* WR2 Sollleistung in 10W (10 = 100W) */

ModeT               heater_mode       = HEATER_DEFAULT_MODE;
ModeT               next_heater_mode  = HEATER_DEFAULT_MODE;

ModeT               wr1_mode      = WR1_DEFAULT_MODE;
ModeT               next_wr1_mode = WR1_DEFAULT_MODE;
ModeT               wr2_mode      = WR2_DEFAULT_MODE;
ModeT               next_wr2_mode = WR2_DEFAULT_MODE;


inline InputT isLasteingang() {return e_LasteingangState;}

#endif
