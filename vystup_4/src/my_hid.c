#include "my_hid.h"

#include "bsp/board.h"
#include "tusb.h"
#include "class/hid/hid.h"

#include <time.h>
#include <stdio.h>

#include "write_history.h"


void hid_init() {
    board_init();
    tusb_init();
}

void hid_task() {
    tud_task();
}

typedef struct {
    uint8_t keycode;   // HID keycode (0 if none)
    uint8_t modifier;  // modifier bits (e.g., KEYBOARD_MODIFIER_LEFTSHIFT)
} hid_key_t;

static void wait_and_send(hid_key_t key) {
    uint8_t keycodes[6] = { key.keycode, 0, 0, 0, 0, 0 };
    while (!tud_hid_ready()) tud_task();
    tud_hid_keyboard_report(0, key.modifier, keycodes);
}

static void wait_and_release() {
    while (!tud_hid_ready()) tud_task();
    tud_hid_keyboard_report(0, 0, NULL);
}

// Map a single ASCII character to HID keycode + modifier
hid_key_t char_to_hid(uint8_t c) {
    hid_key_t result = {0, 0};

    // Letters
    if (c >= 'a' && c <= 'z') {
        result.keycode = HID_KEY_A + (c - 'a');
    } else if (c >= 'A' && c <= 'Z') {
        result.keycode = HID_KEY_A + (c - 'A');
        result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
    }
    // Numbers
    else if (c >= '1' && c <= '9') {
        result.keycode = HID_KEY_1 + (c - '1');
    } else if (c == '0') {
        result.keycode = HID_KEY_0;
    }
    // Space
    else if (c == ' ') {
        result.keycode = HID_KEY_SPACE;
    }
    // Enter / Return
    else if (c == '\n' || c == '\r') {
        result.keycode = HID_KEY_ENTER;
    }
    // Common punctuation
    else {
        switch(c) {
            case '!': result.keycode = HID_KEY_1; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '@': result.keycode = HID_KEY_2; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '#': result.keycode = HID_KEY_3; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '$': result.keycode = HID_KEY_4; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '%': result.keycode = HID_KEY_5; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '^': result.keycode = HID_KEY_6; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '&': result.keycode = HID_KEY_7; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '*': result.keycode = HID_KEY_8; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '(': result.keycode = HID_KEY_9; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case ')': result.keycode = HID_KEY_0; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '-': result.keycode = HID_KEY_MINUS; break;
            case '_': result.keycode = HID_KEY_MINUS; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '=': result.keycode = HID_KEY_EQUAL; break;
            case '+': result.keycode = HID_KEY_EQUAL; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            // case '[': result.keycode = HID_KEY_LEFT_BRACE; break;
            // case '{': result.keycode = HID_KEY_LEFT_BRACE; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            // case ']': result.keycode = HID_KEY_RIGHT_BRACE; break;
            // case '}': result.keycode = HID_KEY_RIGHT_BRACE; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '\\': result.keycode = HID_KEY_BACKSLASH; break;
            case '|': result.keycode = HID_KEY_BACKSLASH; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case ';': result.keycode = HID_KEY_SEMICOLON; break;
            case ':': result.keycode = HID_KEY_SEMICOLON; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            // case '\'': result.keycode = HID_KEY_QUOTE; break;
            // case '"': result.keycode = HID_KEY_QUOTE; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '`': result.keycode = HID_KEY_GRAVE; break;
            case '~': result.keycode = HID_KEY_GRAVE; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case ',': result.keycode = HID_KEY_COMMA; break;
            case '<': result.keycode = HID_KEY_COMMA; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '.': result.keycode = HID_KEY_PERIOD; break;
            case '>': result.keycode = HID_KEY_PERIOD; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            case '/': result.keycode = HID_KEY_SLASH; break;
            case '?': result.keycode = HID_KEY_SLASH; result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT; break;
            default:
                result.keycode = 0; // unsupported character
                break;
        }
    }

    return result;
}

// void send_word(const uint8_t* word) {
//     clock_t t; 
//     t = clock(); 

//     for (int i = 0; word[i] != '\0'; i++) {
//         uint8_t c = word[i];
//         hid_key_t key = char_to_hid(c);
        
//         wait_and_send(key);
//     }

//     wait_and_release();

//     t = clock() - t; 
//     double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds 

//     printf("took %fs to write\n", time_taken);
// }

int send_word(const uint8_t* word, int word_len) {
    if (!word || !*word) return 0;

    hid_key_t prev_key = char_to_hid(word[0]);
    wait_and_send(prev_key);
    int chars_sent = 1;

    for (size_t i = 1; i < word_len && word[i]; i++) {
        hid_key_t key = char_to_hid(word[i]);

        // Release if modifier changed or a keycode is repeated
        if (key.modifier != prev_key.modifier || key.keycode == prev_key.keycode) {
            wait_and_release();
        }

        wait_and_send(key);
        prev_key = key;
        chars_sent++;
    }

    // send a space
    // this is a placeholder and should be handled properly
    wait_and_release();
    wait_and_send((hid_key_t){ HID_KEY_SPACE, 0 });
    chars_sent++;

    // Release all keys at the end
    wait_and_release();

    return chars_sent;
}

void undo_chars(int num_chars) {
    hid_key_t backspace = { HID_KEY_BACKSPACE, 0 };
    for (int i = 0; i < num_chars; i++) {
        wait_and_send(backspace);
        wait_and_release();
    }
}

// handle history undo, send the word over HID and add to history
void process_write(const uint8_t* word_ptr, int word_len, int seq_len) {
    int chars_to_undo = 0;
    for (int i = 0; i < seq_len - 1; i++) {
        int chars = get_history(i);
        if (chars < 0) {
            printf("HISTORY ERROR: not enough history to undo\n");
            return;
        }

        chars_to_undo += chars;

        rewrite_history(i, 0);
    }

    undo_chars(chars_to_undo);

    int written = send_word(word_ptr, word_len);

    push_history(written);
}