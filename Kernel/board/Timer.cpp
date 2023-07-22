#include "Timer.h"

u32 Timer::GetTicks() {
    return TIMER_BASE->clo;
}

void Timer::Alert(u32 usecs) {
    TIMER_BASE->c1 = TIMER_BASE->clo + usecs;
    TIMER_BASE->cs = 1 << 1;
}

void Timer::Delay(u32 usecs) {
    u32 start = TIMER_BASE->clo;
    while (TIMER_BASE->clo - start < usecs);
}

void Timer::DelayMs(u32 msecs) {
    Delay(msecs * 1000);
}


