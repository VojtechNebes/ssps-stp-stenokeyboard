#pragma once
#include <stdint.h>


void hid_init();
void hid_task();

int send_word(const uint8_t* word, int word_len);

void undo_chars(int num_chars);
void process_write(const uint8_t* word_ptr, int word_len, int seq_len);