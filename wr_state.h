
#ifndef _WRSTATE_
#define _WRSTATE_

typedef enum{
  off       = 0,    /* aus, DCDC abgeschaltet */
  charge    = 1,    /* WR1: Elko laden,     WR2: DCDC aufstarten */
  run       = 2,    /* WR1: volle Leistung, WR2: DCDC gesteuert oder geregelt */
  discharge = 3     /* WR1: Elko entladen,  WR2: DCDC ausschalten, dann WR Eingang trennen */
}WrStatusT;


WrStatusT           wr1_status, wr2_status;
bool                wr1_enabled, wr2_enabled;

#endif
