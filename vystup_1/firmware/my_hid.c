#include "my_hid.h"

#include "bsp/board.h"
#include "tusb.h"
#include "class/hid/hid.h"

#include <time.h>
#include <stdio.h>


void hid_init() {
    board_init();
    tusb_init();
}

void hid_task() {
    tud_task();
}

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

void send_word(const uint8_t* word) {
    if (!word || !*word) return;

    hid_key_t prev_key = char_to_hid(word[0]);
    wait_and_send(prev_key);

    for (size_t i = 1; word[i]; i++) {
        hid_key_t key = char_to_hid(word[i]);

        // Release if modifier changed
        if (key.modifier != prev_key.modifier || key.keycode == prev_key.keycode) {
            wait_and_release();
        }

        wait_and_send(key);
        prev_key = key;
    }

    // Release all keys at the end
    wait_and_release();
}