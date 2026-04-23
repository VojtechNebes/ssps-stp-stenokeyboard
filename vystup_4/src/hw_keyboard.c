#include "hw_keyboard.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <time.h>


static const uint8_t col_pins[NUM_COLS] = { 2, 3, 4, 5 };
static const uint8_t row_pins[NUM_ROWS] = { 6, 7, 8, 9, 10, 11, 12 };
// static uint8_t row_state[ROW_BYTES];
uint8_t key_state[KEY_BYTES];

uint8_t chord_state[KEY_BYTES];
bool processed_chord = true;
uint8_t mapped_chord_state[MAPPED_KEY_BYTES];

void hw_keyboard_init() {
    // Initialize cols as outputs and set low
    for (int c = 0; c < NUM_COLS; c++) {
        gpio_init(col_pins[c]);
        gpio_set_dir(col_pins[c], GPIO_OUT);
        gpio_put(col_pins[c], false);
    }

    // Initialize rows as inputs
    for (int r = 0; r < NUM_ROWS; r++) {
        gpio_init(row_pins[r]);
        gpio_set_dir(row_pins[r], GPIO_IN);
        gpio_pull_down(row_pins[r]);
    }
}

// set a bit in an array of bytes, with option to reverse bit order within each byte
// because the bits in RP2040-Zero's flash memory are stored in reverse order for some reason
static void set_bit(uint8_t *array, int index, bool val, bool reverse_bit_order) {
    int byte = index / 8;
    int bit  = index % 8;
    if (reverse_bit_order) {
        bit = 7 - bit;
    }
    if (val) array[byte] |=  (1 << bit);
    else     array[byte] &= ~(1 << bit);
}

bool get_bit(uint8_t *array, int index) {
    int byte = index / 8;
    int bit  = index % 8;
    return (array[byte] >> bit) & 1;
}

// static void prescan_rows() {
//     for (int r = 0; r < NUM_ROWS; r++) {
//         bool high = gpio_get(row_pins[r]); // read row
//         set_bit(row_state, r, high);
//         // printf("row %d = %s\n", r, high ? "1" : "0");
//     }
// }

// Scan columns and build current key_state
static void scan_matrix() {
    // Clear current key state
    for (int i = 0; i < KEY_BYTES; i++) key_state[i] = 0;

    // int single_key = -1;

    for (int c = 0; c < NUM_COLS; c++) {
        // printf("col %d: ", c);

        gpio_put(col_pins[c], true); // turn col on

        sleep_us(100);

        for (int r = 0; r < NUM_ROWS; r++) {
            // if (get_bit(row_state, r)) {
                if (gpio_get(row_pins[r])) {
                    // printf("Y");
                    int key_index = r * NUM_COLS + c;
                    // single_key = key_index;
                    set_bit(key_state, key_index, true, false);
                }// else printf("n");
            // } else printf("_");

            sleep_us(10);
        }

        gpio_put(col_pins[c], false); // restore col low

        sleep_us(100);

        // printf("\n");
    }

    // printf("%d\n", single_key);
}

void print_bitfield(uint8_t *bitfield, int bits) {
    for (int i = 0; i < bits; i++) {
        printf("%c", get_bit(bitfield, i) ? '1' : '0');
    }
    printf("\n");
}

void hw_keyboard_scan() {
    // prescan_rows();

    // printf("ks after prescan: ");
    // print_key_state();

    // for (int r = 0; r < NUM_ROWS; r++) printf("%s", get_bit(row_state, r) ? "1" : "0");
    // printf("\n");

    scan_matrix();

    // print_key_state();

    // printf("chord update\n");
    // print_chord_state();

    for (int i = 0; i < KEY_BYTES; i++) {
        chord_state[i] |= key_state[i];
    }

    // print_chord_state();
    // printf("update done\n");
}

static bool is_bitfield_empty(uint8_t *bitfield, int bytes) {
    for (int i = 0; i < bytes; i++) {
        if (key_state[i] != 0) return false;
    }
    return true;
}

bool is_keyboard_released() {
    return is_bitfield_empty(key_state, KEY_BYTES);
}

void reset_chord_state() {
    for (int i = 0; i < KEY_BYTES; i++) chord_state[i] = 0;
}

void map_raw_chord() {
    // Clear mapped chord state
    for (int i = 0; i < MAPPED_KEY_BYTES; i++) {
        mapped_chord_state[i] = 0;
    }

    // LHS: 0-6
    // Middle: 7-10
    // RHS: 11-20
    // Asterisks: 21
    // Number keys: 22
    const int8_t mapping[NUM_KEYS] = {
        0,
        1,
        2,
        3,
        21,
        11,
        21,
        16,
        12,
        13,
        14,
        15,
        0,
        4,
        5,
        6,
        21,
        21,
        22,
        7,
        17,
        18,
        19,
        20,
        8,
        9,
        10,
        22
    };

    // Map each bit from raw chord to mapped chord using OR logic
    for (int raw_idx = 0; raw_idx < NUM_KEYS; raw_idx++) {
        if (get_bit(chord_state, raw_idx)) {
            int mapped_idx = mapping[raw_idx];
            // set the mapped bit in mapped_chord_state with also reversing the bit order
            // for comparisons with flash memory, which is stored in reverse bit order for some reason
            set_bit(mapped_chord_state, mapped_idx, true, true);
        }
    }
}