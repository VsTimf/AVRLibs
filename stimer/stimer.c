
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "stimer.h"

static volatile uint32_t TimeMs = 0;

STIMER_ISR
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
		TimeMs += STIMER_CALL_PERIOD_MS;
	}
}



uint32_t stimer_get_ms(void)
{
    return TimeMs;
}


uint32_t stimer_set(const uint32_t AddTimeMs)
{
    uint32_t set_time = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        set_time = TimeMs + AddTimeMs;
    }
    return set_time;
}


bool stimer_is_expired(const timer_t Timer)
{
    bool expired = false;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        expired = ((TimeMs - Timer) < (1UL << 31));
    }
    return expired;
}


uint32_t stimer_remaining_ms(const timer_t Timer)
{
    uint32_t time_remaining = 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        time_remaining = ((TimeMs - Timer) > (1UL << 31)) ? (Timer - TimeMs) : 0;
    }
    return time_remaining;
}



static void stimer_hw_init(void)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1  = 0;          

        // CTC (Clear Timer on Compare Match)
        TCCR1B |= (1 << WGM12);

        // Divider (TimerFS = CoreFs / Divider)
        TCCR1B |= (1 << CS11);                            // 8
        //TCCR1B |= (1 << CS11) | (1 << CS10);            // 64
        //TCCR1B |= (1 << CS12);                          // 256

        // Calculate OCR1A
        // TickTime = 1/TimerFS
        // OCR1A = (TimerPeriodMS*1000 / TickTime) - 1
        OCR1A = 460;

        // 4. Enable OCR1A compare interrupt
        TIMSK |= (1 << OCIE1A);
    }
}


void stimer_init(void)
{
    TimeMs = 0;	
    stimer_hw_init();
}