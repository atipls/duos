#include "Task.h"
#include "board/Uart.h"
#include <board/Interrupt.h>
#include <board/Timer.h>
#include <mem/Memory.h>
#include <util/Runtime.h>

#include <Ati/IntrusiveList.h>

extern usize __stack_start;


static IntrusiveList<Task> s_tasks;
static IntrusiveList<Task> s_runQueue;

static Task *s_currentTask = nullptr;
static u32 s_taskIdCounter = 0;

extern "C" void SwitchToTask(Task *oldTask, Task *newTask);

void Tasks::Initialize() {
    Task *idle = allocate<Task>();
    Runtime::Zero(idle, sizeof(Task));

    idle->kind = TaskKind_Kernel;
    idle->pid = 0;
    idle->stack = &__stack_start;

    Runtime::Copy(idle->name, "Kernel Idle", 12);

    s_tasks.Add(idle);

    s_currentTask = idle;

    Timer::Alert(10000);
}

void Tasks::Create(TaskEntry entry, TaskKind kind, char const *name) {
    Task *task = allocate<Task>();

    task->kind = kind;
    task->pid = s_taskIdCounter++;
    task->stack = allocate<u8>(4096);

    Runtime::Copy(task->name, name, Runtime::Length(name));

    auto state = (TaskState *) ((usize) task->stack + 4096 - sizeof(TaskState));
    Runtime::Zero(state, sizeof(TaskState));

    state->lr = (usize) entry;
    state->sp = (usize) Tasks::Cleanup;
    state->cpsr = 0x13 | (8 << 1);

    s_tasks.Add(task);
    s_runQueue.Add(task);
}

void Tasks::Cleanup() {
    Interrupt::Disable();

    Uart::Write("Task exited\n");
}

void Tasks::Preeempt() {
    Interrupt::Disable();

    if (s_runQueue.GetCount() == 0)
        return;

    Task *oldTask = s_currentTask;
    Task *newTask = s_runQueue.PopHead();

    s_currentTask = newTask;

    s_runQueue.Add(oldTask);

    Uart::Write(oldTask->name);
    Uart::Write(" -> ");
    Uart::Write(newTask->name);
    Uart::Write("\n");

    //SwitchToTask(oldTask, newTask);

    Interrupt::Enable();
}
