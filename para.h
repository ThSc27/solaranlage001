#include "wr_state.h"

#define DEBTIME               (5)
#define INACT_THRESH          (100)
#define ACT_THRESH            (200)

#define TA_DEBTIME            (1)


#define HZG_THRESH_ON         (6)     /* both sensors below 6°C --> switch Heater on */
#define HZG_THRESH_OFF        (8)     /* one sensor above 8°C --> switch Heater off */

#define WR1_CHARGETIME        (5)     /* Verzögerung bevor WR source umgeschaltet wird */
#define WR1_DISCHARGETIME     (30)    /* Verzögerung beim Ausschalten */

#define WR2_CHARGETIME        (50)    /* 5 sec Wartezeit nach DCDC-Wandler-Einschalten */
#define WR2_DISCHARGETIME     (30)    /* 3 sec Wartezeit nach DCDC-Wandler-abschalten */

#define HEATER_DEFAULT_MODE   (automatic)
#define WR1_DEFAULT_MODE      (perm_off)
#define WR2_DEFAULT_MODE      (perm_off)
