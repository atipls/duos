#include "TaskTimer.h"
#include <board/Interrupt.h>

#define CLOCKHZ 1000000
#define HZ 100

typedef unsigned TKernelTimerHandle;
typedef void TKernelTimerHandler(TKernelTimerHandle hTimer, void *pParam, void *pContext);

struct KernelTimer {
    TKernelTimerHandler *handler;
    void *param;
    void *context;
    u32 expiration;
} static s_kernelTimers[16];

struct TaskTimerInformation {
    u32 msDelay;
    u32 usDelay;
    u32 tickCount;
    u32 hertzCounter;
};

static TKernelTimerHandle s_kernelTimerHandleCounter = 0;
static TaskTimerInformation s_taskTimerInformation;

void TaskTimerTuneDelayFactors() {
    auto ticks = s_taskTimerInformation.tickCount;
    TaskTimer::DelayMs(1000);
    ticks = s_taskTimerInformation.tickCount - ticks;

    auto speedFactor = 100 * HZ / ticks;
    s_taskTimerInformation.msDelay = s_taskTimerInformation.msDelay * speedFactor / 100;
    s_taskTimerInformation.usDelay = (s_taskTimerInformation.msDelay + 500) / 1000;
}

void TaskTimer::Initialize() {
    s_taskTimerInformation.msDelay = 12500;
    s_taskTimerInformation.usDelay = 12500 / 1000;

    Interrupt::Register(3, TaskTimer::HandleTimerInterrupt);

    DataMemBarrier();

    TIMER_BASE->clo = -(30 * CLOCKHZ);
    TIMER_BASE->c3 = TIMER_BASE->clo + CLOCKHZ / HZ;
    TaskTimerTuneDelayFactors();

    DataMemBarrier();
}

void TaskTimer::DelayUs(u32 us) {
    DelayClocks(us * s_taskTimerInformation.usDelay);
}

void TaskTimer::DelayMs(u32 ms) {
    DelayClocks(ms * s_taskTimerInformation.msDelay);
}

unsigned StartKernelTimer(unsigned hertzDelay, TKernelTimerHandler *handler, void *param, void *context) {
    Interrupt::EnterCriticalSection();

    TKernelTimerHandle kernelTimerHandle = 0;
    for (; kernelTimerHandle < 16; kernelTimerHandle++) {
        if (s_kernelTimers[kernelTimerHandle].handler == nullptr)
            break;
    }

    s_kernelTimers[kernelTimerHandle].handler = handler;
    s_kernelTimers[kernelTimerHandle].param = param;
    s_kernelTimers[kernelTimerHandle].context = context;
    s_kernelTimers[kernelTimerHandle].expiration = s_taskTimerInformation.tickCount + hertzDelay;
    Interrupt::LeaveCriticalSection();

    return kernelTimerHandle;
}

void CancelKernelTimer(unsigned timerHandle) {
    s_kernelTimers[timerHandle].handler = nullptr;
    s_kernelTimers[timerHandle].param = nullptr;
    s_kernelTimers[timerHandle].context = nullptr;
}

void TaskTimer::HandleTimerInterrupt(void *) {
    auto timerCompare = TIMER_BASE->c3 + CLOCKHZ / HZ;
    TIMER_BASE->c3 = timerCompare;
    if (timerCompare < TIMER_BASE->clo) {
        timerCompare = TIMER_BASE->clo + CLOCKHZ / HZ;
        TIMER_BASE->c3 = timerCompare;
    }
    TIMER_BASE->cs = 1 << 3;

    DataMemBarrier();

    if (++s_taskTimerInformation.tickCount % HZ == 0)
        s_taskTimerInformation.hertzCounter++;

    Interrupt::EnterCriticalSection();
    for (u32 i = 0; i < 16; i++) {
        volatile auto timer = &s_kernelTimers[i];
        if (timer->handler == nullptr)
            continue;

        auto handler = timer->handler;
        auto timeDelta = (i32)(timer->expiration - s_taskTimerInformation.tickCount);
        if (timeDelta <= 0) {
            timer->handler = nullptr;
            handler(i, timer->param, timer->context);
        }
    }
    Interrupt::LeaveCriticalSection();

}
