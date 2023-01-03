#pragma once

#include <Ati/Types.h>

template<typename T>
class IntrusiveList {
public:
    IntrusiveList() : head(nullptr), tail(nullptr) {}

    void Add(T *item) {
        if (head == nullptr) {
            head = item;
            tail = item;
            item->next = nullptr;
            item->prev = nullptr;
        } else {
            tail->next = item;
            item->prev = tail;
            item->next = nullptr;
            tail = item;
        }
    }

    void Remove(T *item) {
        if (item->prev == nullptr) {
            head = item->next;
        } else {
            item->prev->next = item->next;
        }

        if (item->next == nullptr) {
            tail = item->prev;
        } else {
            item->next->prev = item->prev;
        }
    }

    T *GetHead() {
        return head;
    }

    T *GetTail() {
        return tail;
    }

    T* PopHead() {
        T *item = head;
        Remove(item);
        return item;
    }

    usize GetCount() {
        usize count = 0;
        for (T *item = head; item != nullptr; item = item->next) {
            count++;
        }
        return count;
    }

private:
    T *head;
    T *tail;
};