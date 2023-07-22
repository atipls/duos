#include "Task.h"
#include "board/Uart.h"
#include "support/Logging.h"
#include <board/Interrupt.h>
#include <board/Timer.h>
#include <mem/Memory.h>
#include <support/Runtime.h>

#define TASK_QUANTUM_USECS 10000

extern "C" usize __binary_end;

static IntrusiveList<Task> s_tasks;
static IntrusiveList<Task> s_runQueue;

static Task *s_currentTask = nullptr;
static u32 s_taskIdCounter = 0;

extern "C" void SwitchToTask(Task *oldTask, Task *newTask);

void Tasks::Initialize() {
    Task *idle = allocate<Task>();
    Runtime::Zero(idle, sizeof(Task));

    idle->kind = TaskKind_Kernel;
    idle->pid = s_taskIdCounter++;
    idle->stack = (void *) ((usize) &__binary_end + KERNEL_STACK_SIZE);
    idle->state = (TaskState *) ((usize) idle->stack + PAGE_SIZE - sizeof(TaskState));

    Runtime::Copy(idle->name, "Kernel Idle", 12);

    s_tasks.Add(idle);
    s_currentTask = idle;

    Interrupt::Register(1, Tasks::Preempt);
    Interrupt::Register(3, Tasks::UpdateTimers);

    TIMER_BASE->c3 = TIMER_BASE->clo + 1000;
    TIMER_BASE->cs |= 1 << 3;

    Timer::Alert(TASK_QUANTUM_USECS);
}

void Tasks::Create(TaskEntry entry, TaskKind kind, char const *name) {
    Task *task = allocate<Task>();

    task->kind = kind;
    task->pid = s_taskIdCounter++;
    task->stack = Memory::AllocatePage();

    Runtime::Copy(task->name, name, Runtime::StringLength(name));

    auto state = (TaskState *) ((usize) task->stack + PAGE_SIZE - sizeof(TaskState));
    Runtime::Zero(state, sizeof(TaskState));

    state->lr = (usize) entry;
    state->sp = (usize) Tasks::Cleanup;
    state->cpsr = 0x13 | (8 << 1);

    task->state = state;

    s_tasks.Add(task);
    s_runQueue.Add(task);
}

void Tasks::Cleanup() {
    Interrupt::Disable();

    // We always have the kernel idle task to keep us company.
    // :)

    auto oldTask = s_currentTask;
    auto newTask = s_runQueue.PopHead();

    s_currentTask = newTask;

    Memory::DeallocatePage(s_currentTask->stack);
    deallocate(oldTask);

    Timer::Alert(TASK_QUANTUM_USECS);
    SwitchToTask(oldTask, newTask);
}

void Tasks::Preempt(void *) {
    if (s_runQueue.GetCount() == 0) {
        Timer::Alert(TASK_QUANTUM_USECS);
        return;
    }

    Task *oldTask = s_currentTask;
    Task *newTask = s_runQueue.PopHead();

    s_currentTask = newTask;
    s_runQueue.Add(oldTask);

    Timer::Alert(TASK_QUANTUM_USECS);
    SwitchToTask(oldTask, newTask);
}


typedef unsigned TKernelTimerHandle;
typedef void TKernelTimerHandler(TKernelTimerHandle hTimer, void *pParam, void *pContext);

struct KernelTimer {
    TKernelTimerHandler *handler;
    void *param;
    void *context;
    unsigned hertzDelay;
    unsigned hertzCounter;
} static s_kernelTimers[16];

static TKernelTimerHandle s_kernelTimerHandleCounter = 0;

unsigned StartKernelTimer(unsigned hertzDelay, TKernelTimerHandler *handler, void *param, void *context) {
    s_kernelTimers[s_kernelTimerHandleCounter].handler = handler;
    s_kernelTimers[s_kernelTimerHandleCounter].param = param;
    s_kernelTimers[s_kernelTimerHandleCounter].context = context;
    s_kernelTimers[s_kernelTimerHandleCounter].hertzDelay = hertzDelay;
    s_kernelTimers[s_kernelTimerHandleCounter].hertzCounter = 0;
    return s_kernelTimerHandleCounter++;
}

void CancelKernelTimer(unsigned timerHandle) {
    s_kernelTimers[timerHandle].handler = nullptr;
    s_kernelTimers[timerHandle].param = nullptr;
    s_kernelTimers[timerHandle].context = nullptr;
}

void Tasks::UpdateTimers(void *) {
    for (u32 i = 0; i < s_kernelTimerHandleCounter; i++) {
        auto &timer = s_kernelTimers[i];
        if (timer.handler == nullptr)
            continue;
        timer.hertzCounter++;
        if (timer.hertzCounter >= timer.hertzDelay) {
            timer.hertzCounter = 0;
            timer.handler(i, timer.param, timer.context);
        }
    }

    TIMER_BASE->c3 = TIMER_BASE->clo + 1000;
    TIMER_BASE->cs = 1 << 3;
}
