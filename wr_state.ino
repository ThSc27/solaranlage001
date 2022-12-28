#include "wr_state.h"

uint8_t wr1_timer, wr2_timer;

/* statemachine für WR1 und WR2 steuerung - Aufruf in 100ms rate */
void WrControl(void) {
  
  /* --- state machine für WR1 (300W-fest-Kanal) --- */
  if (wr1_mode == perm_off){wr1_status = off;}
  switch (wr1_status) {
    case off:
                digitalWrite(LS_WR1SRC, 0);       /* Source default = PV */
                digitalWrite(LS_ELKODIREKT, 0);   /* Elko wird über Rv immer geladen */

                if ((isLasteingang()==ACTIVE) && (wr1_mode==perm_on))
                {
                  wr1_timer  = WR1_CHARGETIME;
                  digitalWrite(LS_ELKODIREKT, 1); /* Elkovorwiderstand überbrücken */
                  wr1_status = charge;            /* und state wechseln */
                }
      break;
    case charge:
                digitalWrite(LS_WR1SRC, 0);       /* Source noch default = PV */
                digitalWrite(LS_ELKODIREKT, 1);   /* Elkovorwiderstand überbrückt */
                if (wr1_timer > 0)
                {
                  wr1_timer--;
                }
                else
                {
                  digitalWrite(LS_WR1SRC, 1);     /* jetzt Source = Batterie direkt */
                  wr1_status = run;               /* und state wechseln */
                }
      break;
    case run:
                digitalWrite(LS_ELKODIREKT, 1);   /* Elkovorwiderstand überbrückt */
                digitalWrite(LS_WR1SRC, 1);       /* Source = Batterie */
                if (isLasteingang()==INACTIVE)
                {
                  digitalWrite(LS_WR1SRC, 0);     /* Batterie direkt zuerst abschalten */
                  wr1_timer  = WR1_DISCHARGETIME;
                  wr1_status = discharge;
                }
      break;
    case discharge:
                digitalWrite(LS_WR1SRC, 0);       /* Source nicht mehr Batterie */
                digitalWrite(LS_ELKODIREKT, 1);   /* Elko noch aktiv Widerstand */
                if (wr1_timer > 0)
                {
                  wr1_timer--;
                }
                else
                {
                  digitalWrite(LS_ELKODIREKT, 0); /* jetztElko auch wieder abschalten */
                  wr1_status = off;               /* und state wechseln */
                }
      break;
  }


  /* --- state machine für WR2 (Kanal mit DCDC-Wandler im gesteuerten oder geregeltem Betrieb) --- */
  if (wr2_mode == perm_off){wr2_status = off;}
  switch (wr2_status) {
    case off:
                digitalWrite(LS_WR2SRC, 0);       /* Source default = PV */
                digitalWrite(LS_DCDCPOWER, 0);    /* DcDc-Wandler ist AUS */

                if ((isLasteingang()==ACTIVE) && (wr2_mode == perm_on))
                {
                  wr2_timer  = WR2_CHARGETIME;
                  digitalWrite(LS_DCDCPOWER, 1);  /* DcDc-Wandler Einschalten */
                  wr2_status = charge;            /* und state wechseln */
                }
      break;
    case charge:
                digitalWrite(LS_WR2SRC, 0);       /* Source noch default = PV */
                digitalWrite(LS_DCDCPOWER, 1);    /* DcDc-Wandler ist EIN */
                if (wr2_timer > 0)
                {
                  wr2_timer--;
                }
                else
                {
                  digitalWrite(LS_WR2SRC, 1);     /* jetzt Source = DCDC */
                  wr2_status = run;               /* und state wechseln */
                }
      break;
    case run:
                digitalWrite(LS_DCDCPOWER, 1);    /* DcDc-Wandler ist EIN */
                digitalWrite(LS_WR2SRC, 1);       /* Source = DCDC */
                if (isLasteingang()==INACTIVE)
                {
                  digitalWrite(LS_DCDCPOWER, 0);  /* DcDc-Wandler zuerst abschalten */
                  wr2_timer  = WR2_DISCHARGETIME;
                  wr2_status = discharge;
                }
      break;
    case discharge:
                digitalWrite(LS_WR2SRC, 1);       /* Source noch DCDC */
                digitalWrite(LS_DCDCPOWER, 0);    /* DcDc-Wandler ist schon AUS */
                if (wr2_timer > 0)
                {
                  wr2_timer--;
                }
                else
                {
                  digitalWrite(LS_WR2SRC, 0);     /* jetzt Source wieder auf default = PV */
                  wr2_status = off;               /* und state wechseln */
                }
      break;

  }
  
}
