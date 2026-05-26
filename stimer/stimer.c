
#include <util/atomic.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "stimer.h"

static volatile uint32_t TimeMs = 0;

// Обработчик прерывания (вызывается каждые 10 мс)
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

        // 1. Сбрасываем все настройки Timer1
        TCCR1A = 0;
        TCCR1B = 0;
        TCNT1  = 0;           // счётчик в 0

        // 2. Устанавливаем режим CTC (Clear Timer on Compare Match)
        // WGM13:10 = 0100 → CTC с OCR1A
        TCCR1B |= (1 << WGM12);

        // 3. Предделитель
        TCCR1B |= (1 << CS11);                            //8       Предделитель = 8 → частота таймера = 3686000 / 8 = 460750 Гц
        //TCCR1B |= (1 << CS11) | (1 << CS10);            //64      Предделитель = 64 → частота таймера = 3686000 / 64 = 57600 Гц
        //TCCR1B |= (1 << CS12);                          //256     Предделитель = 256 → частота таймера = 3686000 / 256 = 14398 Гц

        // 5. Вычисляем значение для OCR1A
        // 8 Период одного тика = 1 / 460750 ≈ 2.170 мкс
        // 64 Период одного тика = 1 / 57600 ≈ 17.361 мкс
        // 256 Период одного тика = 1 / 14398 ≈ 69.454 мкс

        // Нужно 1 мс = 1000 мкс
        // Количество тиков = 1000 / 2.170 ≈ 461
        // → OCR1A = 460 (так как счёт начинается с 0 → 0..460 = 461 тик)

        OCR1A = 460;

        // 4. Разрешаем прерывание по совпадению с OCR1A
        TIMSK |= (1 << OCIE1A);
    }
}


void stimer_init(void)
{
    TimeMs = 0;	// Обнуляем переменную времени. 
    stimer_hw_init();  // Запускаем аппаратный таймер
}