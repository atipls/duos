#include "Timer.h"

u32 Timer::GetTicks() {
    return TIMER_BASE->clo;
}

void Timer::Alert(u32 usecs) {
    TIMER_BASE->c1 = TIMER_BASE->clo + usecs;
    TIMER_BASE->cs = 1 << 1;
}

