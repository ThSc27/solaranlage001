#ifndef _TEMPSENSE_
#define _TEMPSENSE_


typedef struct {
    int                 TempGanz;
    int                 TempNachkomma;
}TemperaturT;

typedef struct {
    byte                addr[8];
    byte                u8_Counter;
    TemperaturT         Temperatur;
    unsigned char       gu8_TempSensFound;
}TempSenseDatT;

#endif
