#pragma once
#include <stdint.h>
#include <stdbool.h>


#define NUM_COLS 4
#define NUM_ROWS 7

#define NUM_KEYS (NUM_ROWS * NUM_COLS)
#define KEY_BYTES ((NUM_KEYS + 7) / 8)
#define ROW_BYTES ((NUM_ROWS + 7) / 8)
#define MAPPED_KEYS 23
// The length of one chord in bytes.
#define MAPPED_KEY_BYTES ((MAPPED_KEYS + 7) / 8)

extern uint8_t chord_state[KEY_BYTES];
extern bool processed_chord;
extern uint8_t key_state[KEY_BYTES];
extern uint8_t mapped_chord_state[MAPPED_KEY_BYTES];

void hw_keyboard_init();
void hw_keyboard_scan();
bool is_keyboard_released();
void reset_chord_state();
void map_raw_chord();
void print_bitfield(uint8_t *bitfield, int bits);

// bool get_bit(uint8_t *array, int index);