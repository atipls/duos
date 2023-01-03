#include "Timer.h"
#include "Interrupt.h"
#include "Uart.h"
#include "task/Task.h"

static void TimerIrqHandler() {
    Tasks::Preeempt();

    Timer::Alert(1000);
}

static void TimerIrqClearer() {
    TIMER->cs = 1 << 1;
}

void Timer::Initialize() {
    Interrupt::Register(1, TimerIrqHandler, TimerIrqClearer);
}

u32 Timer::GetTicks() {
    return TIMER->clo;
}

void Timer::Alert(u32 usecs) {
    TIMER->c1 = TIMER->clo + usecs;
    TIMER->cs = 1 << 1;
}

void Timer::Delay(u32 usecs) {
    RW32 current = TIMER->clo;
    RW32 target = TIMER->clo - current;
    while (target < usecs)
        target = TIMER->clo - current;
}

extern "C" void timer_set(u32 usecs) {
    Timer::Alert(usecs);
}