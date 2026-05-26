#ifndef _STIMER_H
#define _STIMER_H

#include "stdint.h"
#include "stdbool.h"
#include "stimer_config.h"


typedef uint32_t timer_t;

uint32_t stimer_get_ms(void);
timer_t stimer_set(const uint32_t AddTimeMs);
bool stimer_is_expired(const timer_t Timer);
uint32_t stimer_remaining_ms(const timer_t Timer);

void stimer_init(void);



#endif //_STIMER_H