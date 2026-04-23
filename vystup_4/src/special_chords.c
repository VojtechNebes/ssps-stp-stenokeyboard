#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "hw_keyboard.h"
#include "my_hid.h"
#include "write_history.h"
#include "dictionary.h"


static void manual_undo() {
    int chars_to_undo = 0;
    while (chars_to_undo == 0) {
        chars_to_undo = undo_history();
    }
    undo_chars(chars_to_undo);
    reset_sequence();
}


uint8_t chord_manual_undo[KEY_BYTES] = {0x00, 0x00, 0x04, 0x08};

// Compare raw chord state with special chords and handle them.
// Return whether a special chord was handled.
bool handle_special_chord() {
    printf("Checking for special chords...\n");
    printf("Current chord state: ");
    print_bitfield(chord_state, KEY_BYTES*8);
    printf("Manual undo chord  : ");
    print_bitfield(chord_manual_undo, KEY_BYTES*8);

    if (memcmp(chord_state, chord_manual_undo, KEY_BYTES) == 0) {
        printf("Manual undo chord detected!\n");
        
        manual_undo();
        return true;
    }

    return false;
}