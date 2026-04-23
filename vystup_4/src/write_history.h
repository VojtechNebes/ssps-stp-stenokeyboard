#pragma once

void push_history(int x);
unsigned int history_size();
void rewrite_history(unsigned int how_far_back, int new_value);
int undo_history();
int get_history(unsigned int how_far_back);