
#include <dcf77.h>

void padded_write2lcd(BCD::bcd_t n );

#if defined(__AVR__)
const uint8_t dcf77_analog_sample_pin = 15;
const uint8_t dcf77_sample_pin = A15;
const uint8_t dcf77_inverted_samples = 0;
const uint8_t dcf77_analog_samples = 1;
const uint8_t dcf77_pin_mode = INPUT;  // disable internal pull up
//const uint8_t dcf77_pin_mode = INPUT_PULLUP;  // enable internal pull up

const uint8_t dcf77_monitor_led = 39;  // A4 == d18

uint8_t ledpin(const uint8_t led) {
    return led;
}
#elif defined(__STM32F1__)
const uint8_t dcf77_sample_pin = PB6;
const uint8_t dcf77_inverted_samples = 1; //output from HKW EM6 DCF 3V
const WiringPinMode dcf77_pin_mode = INPUT_PULLUP;  // enable internal pull up
const uint8_t dcf77_monitor_led = PC13;
uint8_t ledpin(const int8_t led) {
    return led;
}
#else
const uint8_t dcf77_sample_pin = 53;
const uint8_t dcf77_inverted_samples = 0;

// const uint8_t dcf77_pin_mode = INPUT;  // disable internal pull up
const uint8_t dcf77_pin_mode = INPUT_PULLUP;  // enable internal pull up

const uint8_t dcf77_monitor_led = 19;

uint8_t ledpin(const uint8_t led) {
    return led<14? led: led+(54-14);
}
#endif


uint8_t sample_input_pin() {
    const uint8_t sampled_data =
        #if defined(__AVR__)
        dcf77_inverted_samples ^ (dcf77_analog_samples? (analogRead(dcf77_analog_sample_pin) > 200)
                                                      : digitalRead(dcf77_sample_pin));
        #else
        dcf77_inverted_samples ^ digitalRead(dcf77_sample_pin);
        #endif

//    if (sampled_data == 0){digitalWrite(39, 0);}else{digitalWrite(39, 1);}
        
    return sampled_data;
}

void dcf_setup() {

    using namespace Clock;
    pinMode(dcf77_sample_pin, dcf77_pin_mode);

    DCF77_Clock::setup();
    DCF77_Clock::set_input_provider(sample_input_pin);
}

void padded_write2lcd(BCD::bcd_t n )
{
    lcd.print(n.digit.hi);
    lcd.print(n.digit.lo);
}

void dcf_loop() {

    Clock::time_t now;
    static char toggle;
    
    DCF77_Clock::get_current_time(now);

    if (now.month.val > 0) {
        lcd.setCursor(0, 0);
        if (DCF77_Clock::get_clock_state() == Clock::useless)
        {
            lcd.print("...wait for DCF sync");
        }
        else
        {
           padded_write2lcd(now.day);
            lcd.print('.'); 
           padded_write2lcd(now.month); 
            lcd.print('.'); 
           padded_write2lcd(now.year); 
            lcd.print(' '); 
           padded_write2lcd(now.hour);
            if (toggle == 0) {lcd.print(':');} else {lcd.print(' ');}
           padded_write2lcd(now.minute); 
            if (toggle == 0) {lcd.print(':'); toggle = 1;} else {lcd.print(' '); toggle = 0;}
           padded_write2lcd(now.second); 
            switch (DCF77_Clock::get_clock_state()) {
            case Clock::useless: ;break;
            case Clock::dirty:   lcd.print(" lo"); break;
            case Clock::locked:  lcd.print(" ok"); break;
            case Clock::synced:  lcd.print(" hi"); break;
            }
        }
    }
}
