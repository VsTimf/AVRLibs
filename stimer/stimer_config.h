/* This is simple software timmer library for AVR */

/* HOW TO USE */

// 1. Define timing in milliseconds
#define STIMER_CALL_PERIOD_MS 1

// 2. Define hardware timer ISR function
#define STIMER_ISR ISR(TIMER1_COMPA_vect)

// 3.  Implement "stimer_hw_init" function. It should initialize hardware timer.
//     Make sure that STIMER_ISR function calling every STIMER_CALL_PERIOD_MS period


// 4. Example
//
//  main() 
// {
//      stimer_init();
//
//      static timer_t key_timer; 
//      key_timer = stimer_set(CALL_PERIOD_MS);      
//
//
//      while(1) 
//      {
//  		if(stimer_is_expired(key_timer))
//  		{
//              do_smth_usufull();
//              key_timer = stimer_set(CALL_PERIOD_MS);      
//          }
//      }
// }

