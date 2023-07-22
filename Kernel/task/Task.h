#pragma once
#include <Ati/Types.h>
#include <Ati/IntrusiveList.h>

struct TaskState {
    u32 r0;
    u32 r1;
    u32 r2;
    u32 r3;
    u32 r4;
    u32 r5;
    u32 r6;
    u32 r7;
    u32 r8;
    u32 r9;
    u32 r10;
    u32 r11;
    u32 cpsr;
    u32 sp;
    u32 lr;
};

enum TaskKind {
    TaskKind_User,
    TaskKind_Kernel,
};

struct Task {
    TaskState *state;
    void *stack;

    u32 pid;
    TaskKind kind;

    char name[32];

    Task *prev;
    Task *next;
};

using TaskEntry = void (*)();

namespace Tasks {
    void Initialize();

    void Create(TaskEntry entry, TaskKind kind, char const *name);
    void Cleanup();

    void Preempt(void *);
    void UpdateTimers(void *);
}// namespace Tasks
