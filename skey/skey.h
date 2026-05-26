#ifndef _S_KEY_H
#define _S_KEY_H

#include <avr/io.h>
#include "stdint.h"
#include "stdbool.h"
#include "skey_config.h"



#define PINH(PORTX, PINX)     .h={&DDR##PORTX, &PORT##PORTX, &PIN##PORTX, PINX}


typedef enum
{
    KEY_ACTIVE_LOW = 0,
    KEY_ACTIVE_HIGH = 1
} TKeyActiveLvl;


enum KeyPullResistor
{
    PULL_DISABLE = 0,
    PULL_ENABLE = 1
};



typedef enum
{
    KEY_IDLE,                                              
    KEY_CLICK,                                      // return 1 time after key clicked
    KEY_CLICK_NO_EVENT,                             // returned after user code get "KEY_CLICK" event and until "KEY_HOLD" event will occur
    KEY_HOLD,
    KEY_HOLD_NO_EVENT
} KeyState;


typedef struct {
    volatile uint8_t* DDRx;     // DDRx
    volatile uint8_t* PORTx;    // PORTx
    volatile uint8_t* PINx;     // PINx
    uint8_t           pin;      // pin number (0..7)
} TPin;


typedef struct{
    const uint8_t id;
    const TKeyActiveLvl active_level;
    const TPin h;
    const uint8_t pull_resistor_en;

    uint8_t pin_new_state;
    uint8_t pin_prv_state;
    uint8_t pin_state;

    uint8_t debounce_cnt;

    KeyState state;
    uint16_t state_cnt;

    uint8_t event_active;
} TKey;


void key_process(void);
void key_init(void);

KeyState get_key(uint8_t idx);
bool check_key(uint8_t idx, KeyState check_state);
void clear_keys();


#endif /* _S_KEY_H */