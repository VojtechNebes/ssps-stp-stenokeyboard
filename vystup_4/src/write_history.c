#include "write_history.h"

#define HISTORY_SIZE 100

int history[HISTORY_SIZE];
unsigned int head = 0;
unsigned int tail = 0;

void push_history(int x) {
    history[head] = x;
    head = (head + 1) % HISTORY_SIZE;

    // Buffer is full, overwrite the oldest entry
    if (head == tail) {
        tail = (tail + 1) % HISTORY_SIZE;
    }
}

unsigned int history_size() {
    if (head >= tail) {
        return head - tail;
    } else {
        return HISTORY_SIZE - (tail - head);
    }
}

void rewrite_history(unsigned int how_far_back, int new_value) {
    if (how_far_back >= history_size()) {
        // Not enough history to rewrite
        return;
    }

    unsigned int index = (head + HISTORY_SIZE - 1 - how_far_back) % HISTORY_SIZE;
    history[index] = new_value;
}

// LIFO pop
int undo_history() {
    if (history_size() == 0) {
        // History is empty
        return -1; // or some error code
    }

    head = (head + HISTORY_SIZE - 1) % HISTORY_SIZE;
    return history[head];
}

int get_history(unsigned int how_far_back) {
    if (how_far_back >= history_size()) {
        // Not enough history to get
        return -1; // or some error code
    }

    unsigned int index = (head + HISTORY_SIZE - 1 - how_far_back) % HISTORY_SIZE;
    return history[index];
}