
#ifndef _PINOUT_
#define _PINOUT_
/***********************************************************************************************************************************
 * Belegung AT Mega Board 
 ***********************************************************************************************************************************/

/* --- LCD Display --- */
#define LCD_RS          2
#define LCD_EN          3
#define LCD_D4          4
#define LCD_D5          5
#define LCD_D6          6
#define LCD_D7          7

/* --- temp sensoren --- DS18B20 Sicht auf glatte Seite unten links Pin1=GND, Pin2=Data Pin3=VCC; Data mit 4K7 Pullup */
#define DS_SENS1        25
#define DS_SENS2        27

/* --- RS485 transceiver --- */
#define RS485_RO        
#define RS485_RE        
#define RS485_DE        22
#define RS485_DI        

/* --- LowSides / Relaistreiber --- */
#define LS_WR1SRC       39    /* Rel 1: Quellenumschaltung WR1. Off = PV, On = Batterie direkt */
#define LS_ELKODIREKT   37    /* Rel 2: aktiv: Elkovorwiderstand überbrückt */
#define LS_WR2SRC       35    /* Rel 3: Quellenumschaltung WR2. Off = PV, On = DcDc-Wandler */
#define LS_DCDCPOWER    33    /* Rel 4: DcDc-Wandler Ein-Schalter */
#define LS_HEIZUNG      31    /* Rel 5: Relaistreiber für Heizmatte und Umluft-Lüfter */
#define LS_6            29

/* --- Inputs --- */
#define IN_LASTFREIGABE  8    /* Lastfreigabe vom Victron Laderegler Pin A8*/
#define IN_TA1          14
#define IN_TA2          15


/* --- Belegung Peripherie ---- */
// Belegung LCD Modul:  Pin1 - GND
//                      Pin2 - +5
//                      Pin3 - /
//                      Pin4 - RS
//                      Pin 5 - R/W
//                      Pin6 - E
//                      Pin7 - D0 ... 


#endif
