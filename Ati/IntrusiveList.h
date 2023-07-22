#pragma once

#include <Ati/Types.h>

template<typename IntrusiveListNode>
class IntrusiveList {
public:
    IntrusiveList() : head(nullptr), tail(nullptr) {}

    void Add(IntrusiveListNode *item) {
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

    void Remove(IntrusiveListNode *item) {
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

    IntrusiveListNode *GetHead() { return head; }
    IntrusiveListNode *GetTail() { return tail; }

    IntrusiveListNode *PopHead() {
        IntrusiveListNode *item = head;
        Remove(item);
        return item;
    }

    usize GetCount() {
        usize count = 0;
        for (IntrusiveListNode *item = head; item != nullptr; item = item->next) {
            count++;
        }
        return count;
    }

private:
    IntrusiveListNode *head;
    IntrusiveListNode *tail;
};