#include "pico/stdlib.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "hw_keyboard.h"
#include "dictionary.h"
#include "my_hid.h"
#include "special_chords.h"


// /*
int main() {
    stdio_init_all();
    hw_keyboard_init();
    hid_init();

    while (true) {
        hid_task();

        hw_keyboard_scan();

        if (is_keyboard_released()) {
            if (!processed_chord) {
                printf("=== RELEASED KEYBOARD ===\n");

                hid_task();

                print_bitfield(chord_state, NUM_KEYS);
                bool was_special_chord = handle_special_chord();
                if (!was_special_chord) {
                    map_raw_chord();
                    print_bitfield(mapped_chord_state, MAPPED_KEYS);
                    printf("------ process sequence\n");
                    process_sequence();
                    printf("------ end process sequence\n");
                } else {
                    printf("Special chord was handled, skipping sequence processing.\n");
                }

                reset_chord_state();
                processed_chord = true;
            }
        } else {
            processed_chord = false;
        }

        hid_task();
        sleep_us(5000);
        // sleep_ms(100);
    }
}
// */

/*
int main() {
    stdio_init_all();

    // wait 5 seconds
    for (int i = 0; i < 5; i++) {
        printf("Starting in %d...\n", 5 - i);
        sleep_ms(1000);
    }

    // read 4 records and print one on each line using get_bit to print bits
    for (int i = 0; i < 4; i++) {
        uint8_t* record = read_record(i);
        for (int bit = 0; bit < RECORD_LEN*8; bit++) {
            printf("%c%s", (get_bit(record, bit) ? '1' : '0'), (bit % 8 == 7) ? " " : "");
        }
        printf("end\n");
        free(record);
    }

    while (true) {}
}//*/

// NOTE: the key coordinates in the grid are (row, column) format.
// First key is at (0, 0), second key is at (0, 1), etc.