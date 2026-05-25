#include "skey.h"
#include <avr/io.h>


TKey key[KEY_QNT] = {KEY_LIST};





// Return 1 if pin state is active (ACTIVE_LOW or ACTIVE_HIGH)
uint8_t get_pin_state(TKey* k){
    return (k->active_level == KEY_ACTIVE_LOW) ? (*(k->h.PINx) & _BV(k->h.pin)) == 0 : (*(k->h.PINx) & _BV(k->h.pin)) != 0;
}



static void debounce(TKey* k)
{
    k->pin_new_state = get_pin_state(k);

    k->debounce_cnt = k->pin_prv_state == k->pin_new_state ? k->debounce_cnt+KEY_CALL_PERIOD_MS : 0;
    k->pin_prv_state = k->pin_new_state;
    
    if(k->debounce_cnt >= KEY_DEBOUNCE_MS)
        k->pin_state = k->pin_new_state;
}




void key_process(void)
{
    TKey* k;
    for(uint8_t idx = 0; idx<KEY_QNT; idx++)
    {
        k = &key[idx];
        debounce(k);
        
        switch (k->state)
        {
        case KEY_IDLE:
            if (k->pin_state)
            {
                k->state = KEY_CLICK;
                k->state_cnt = 0;

                k->event_active = 1;
            }
            break;

        case KEY_CLICK:
            if (k->pin_state)
            {
                k->state_cnt += KEY_CALL_PERIOD_MS;
                if (k->state_cnt > KEY_HOLD_MS)
                {
                    k->state = KEY_HOLD;
                    k->state_cnt = 0;
                    k->event_active = 1;
                }
            }
            else
                k->state = KEY_IDLE;
            break;


        case KEY_HOLD:
            if (k->pin_state)
            {
                k->state_cnt += KEY_CALL_PERIOD_MS;
                if (k->state_cnt > KEY_HOLD_REPEAT_EVENT_MS)
                {
                    k->event_active = 1;
                    k->state_cnt = 0;
                }
            }
            else
                k->state = KEY_IDLE; 
            break;
        
        default:
            break;
        }
    }
}




KeyState get_key(uint8_t idx)
{
    if(idx >= KEY_QNT)
        while(1);             // wrong key index
        

    if (key[idx].state == KEY_CLICK)
    {
        if (key[idx].event_active)
        {
            key[idx].event_active = 0;
            return KEY_CLICK;
        }
        return KEY_CLICK_NO_EVENT;
    }

    if (key[idx].state == KEY_HOLD)
    {
        if (key[idx].event_active)
        {
            key[idx].event_active = 0;
            return KEY_HOLD;
        }
        return KEY_HOLD_NO_EVENT;
    }
    
    return key[idx].state;
}





bool check_key(uint8_t idx, KeyState check_state)
{
  if(idx >= KEY_QNT)
     while(1);             // wrong key index
  
  if (key[idx].state == check_state)
  {
      if(check_state == KEY_CLICK || check_state == KEY_HOLD)
      {
        if(key[idx].event_active)
        {
          key[idx].event_active = 0;
          return true;
        }
      
        return false;
      }
      return true;
  }
  return false;  
}



void clear_keys(void)
{
    for(uint8_t idx = 0; idx<KEY_QNT; idx++)
        key[idx].event_active = 0;
}



void key_init(void)
{
    TKey* k;
    for(uint8_t idx = 0; idx<KEY_QNT; idx++)
    {
        k = &key[idx];

        *(k->h).DDRx &= ~(_BV(k->h.pin));
        if(k->pull_resistor_en)
            *(k->h).PORTx |= _BV(k->h.pin);
    }
}

