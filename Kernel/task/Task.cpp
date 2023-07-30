#include "Task.h"
#include "board/Uart.h"
#include "support/Logging.h"
#include <board/Interrupt.h>
#include <board/Timer.h>
#include <mem/Memory.h>
#include <support/Runtime.h>

#define TASK_QUANTUM_USECS 10000

#define DEFAULT_STACK_SIZE 32 * PAGE_SIZE

static IntrusiveList<Task> s_tasks;
static IntrusiveList<Task> s_runQueue;

static Task *s_currentTask = nullptr;
static u32 s_taskIdCounter = 0;

extern u32 __kernel_stack_top;

extern "C" void SwitchToTask(TaskState *oldTask, TaskState *newTask);

void Tasks::Initialize() {
    Interrupt::EnterCriticalSection();

    Task *idle = allocate<Task>();

    idle->kind = TaskKind_Kernel;
    idle->pid = s_taskIdCounter++;
    idle->stack = &__kernel_stack_top;

    Runtime::Copy(idle->name, "Kernel Idle", 12);

    s_tasks.Add(idle);
    s_currentTask = idle;

    Interrupt::Register(1, Tasks::Preempt);

    Interrupt::LeaveCriticalSection();

    Timer::Alert(TASK_QUANTUM_USECS);
}

void Tasks::Create(TaskEntry entry, TaskKind kind, char const *name) {
    Interrupt::EnterCriticalSection();

    Task *task = allocate<Task>();

    task->kind = kind;
    task->pid = s_taskIdCounter++;
    task->stack = Memory::Allocate(DEFAULT_STACK_SIZE);
    task->state = allocate<TaskState>();

    Runtime::Copy(task->name, name, Runtime::StringLength(name));

    task->state->lr = (usize) entry;
    task->state->sp = (usize) task->stack + DEFAULT_STACK_SIZE;
    task->state->fpexc = (1 << 30);
    task->state->fpscr = (1 << 25);

    s_tasks.Add(task);
    s_runQueue.Add(task);

    Interrupt::LeaveCriticalSection();
}

void Tasks::Cleanup() {
    // We always have the kernel idle task to keep us company.
    // :)

    auto oldTask = s_currentTask;
    auto newTask = s_runQueue.PopHead();

    s_currentTask = newTask;

    Memory::DeallocatePage(s_currentTask->stack);
    deallocate(oldTask);

    Timer::Alert(TASK_QUANTUM_USECS);
    SwitchToTask(oldTask->state, newTask->state);
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

    Logging::Info("kernel", "Preempting task %s (interrupts are %s)", oldTask->name, Interrupt::Status() ? "enabled" : "disabled");

    Timer::Alert(TASK_QUANTUM_USECS);
    SwitchToTask(oldTask->state, newTask->state);
}
