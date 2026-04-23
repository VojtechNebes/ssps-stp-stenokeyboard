#include "dictionary.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "hw_keyboard.h"
#include "my_hid.h"
#include "dicts.h"
#include "write_history.h"


// this is the old big dict with all records on one pile
// extern const uint8_t _binary_big_bin_start[];
// extern const uint8_t _binary_big_bin_end[];
// extern const uint8_t _binary_big_bin_size[];

uint8_t sequence_buffer[MAX_SEQ_LEN][MAPPED_KEY_BYTES];
int current_seq_len = 0;

void reset_sequence() {
    current_seq_len = 0;
    for (int i = 0; i < MAX_SEQ_LEN; i++) {
        for (int j = 0; j < MAPPED_KEY_BYTES; j++) {
            sequence_buffer[i][j] = 0;
        }
    }
}

static void push_sequence(uint8_t chord[MAPPED_KEY_BYTES]) {
    memmove(
        sequence_buffer[0],
        sequence_buffer[1],
        (MAX_SEQ_LEN - 1) * MAPPED_KEY_BYTES
    );

    memcpy(
        sequence_buffer[MAX_SEQ_LEN - 1],
        chord,
        MAPPED_KEY_BYTES
    );

    if (current_seq_len < MAX_SEQ_LEN) {
        current_seq_len++;
    }
}

const uint8_t* word_ptr;


static inline uint8_t bitrev8(uint8_t x) {
    x = (x >> 4) | (x << 4);
    x = ((x & 0x33) << 2) | ((x & 0xCC) >> 2);
    x = ((x & 0x55) << 1) | ((x & 0xAA) >> 1);
    return x;
}

static void reverse_bits_array(uint8_t *buf, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        buf[i] = bitrev8(buf[i]);
    }
}

// static uint8_t* read_record(int index) {
//     uint8_t* buf = (uint8_t*) malloc((RECORD_LEN + 1) * sizeof(uint8_t));
//     buf[RECORD_LEN] = '\0';

//     int offset = index * RECORD_LEN;
//     memcpy(buf, _binary_big_bin_start + offset, RECORD_LEN);

//     return buf;
// }

// bool binary_search_by_chord(uint8_t* chord, uint8_t* out_word_buf) {
//     reverse_bits_array(chord, MAPPED_KEY_BYTES);

//     int left = 0;
//     int right = (_binary_big_bin_end - _binary_big_bin_start) / RECORD_LEN - 1;

//     while (left <= right) {
//         int mid = (left + right) / 2;
//         // printf("left: %d, mid: %d, right: %d\n", left, mid, right);
//         uint8_t* mid_record = read_record(mid);
        
//         int cmp = memcmp(chord, mid_record, MAPPED_KEY_BYTES);

//         int chord_val = 0;
//         for (int i = 0; i < MAPPED_KEY_BYTES; i++) {
//             chord_val = (chord_val << 8) | chord[i];
//         }
//         // printf("chord: %d\n", chord_val);

//         int mid_val = 0;
//         for (int i = 0; i < MAPPED_KEY_BYTES; i++) {
//             mid_val = (mid_val << 8) | mid_record[i];
//         }
//         // printf("mid_record: %d\n", mid_val);
        
//         // for (int i = 0; i < MAPPED_KEY_BYTES*8; i++) printf("%c", (get_bit(chord, i) ? '1' : '0'));
//         // printf("\n");
//         // for (int i = 0; i < MAPPED_KEY_BYTES*8; i++) printf("%c", (get_bit(mid_record, i) ? '1' : '0'));
//         // printf("\ncmp: %d\n", cmp);

//         if (cmp == 0) {
//             memcpy(out_word_buf, mid_record + MAPPED_KEY_BYTES, RECORD_LEN - MAPPED_KEY_BYTES);
//             free(mid_record);
//             return true;
//         } else if (cmp > 0) {
//             // printf("move right\n");
//             right = mid - 1;
//         } else {
//             // printf("move left\n");
//             left = mid + 1;
//         }
//     }

//     return false;
// }

// returns whether found and points `word_ptr` to the result
bool binary_search_by_seq_len(int seq_len) {
    // reverse_bits_array(chord, MAPPED_KEY_BYTES);

    const Dict dict = get_dict(seq_len);
    const int record_len = seq_len * MAPPED_KEY_BYTES + dict.word_len;

    int left = 0;
    int right = (dict.end - dict.start) / record_len - 1;

    while (left <= right) {
        int mid = (left + right) / 2;
        int cmp = 0;
        const uint8_t* mid_record = dict.start + mid * record_len;

        printf("  Comparing with record: ");
        print_bitfield(mid_record, 8*record_len);
        print_bitfield(mid_record, 8*seq_len*MAPPED_KEY_BYTES);
        
        // compare each chord in the sequence with the corresponding part of mid_record
        for (int i = 0; i < seq_len; i++) {
            printf("    Comparing chord:\n");
            print_bitfield(sequence_buffer[MAX_SEQ_LEN - seq_len + i], 8*MAPPED_KEY_BYTES);
            print_bitfield(mid_record + i * MAPPED_KEY_BYTES, 8*MAPPED_KEY_BYTES);

            cmp = memcmp(
                sequence_buffer[MAX_SEQ_LEN - seq_len + i],
                mid_record + i * MAPPED_KEY_BYTES,
                MAPPED_KEY_BYTES
            );

            if (cmp != 0) {
                break;
            }
        }

        if (cmp == 0) {
            printf("sequence was equal\n");
            // use mid_record to save the address of the found record to a word_buffer pointer and return true
            word_ptr = mid_record + seq_len * MAPPED_KEY_BYTES;
            return true;
        } else if (cmp > 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    return false;
}

// void process_chord() {
//     // print_chord_state();
//     print_bitfield(mapped_chord_state, MAPPED_KEYS);

//     uint8_t* word = (uint8_t*) malloc((RECORD_LEN - MAPPED_KEY_BYTES + 1) * sizeof(uint8_t));
//     word[RECORD_LEN - MAPPED_KEY_BYTES] = '\0';

//     if (binary_search_by_chord(mapped_chord_state, word)) {
//         // reverse_bits_array(word, RECORD_LEN - MAPPED_KEY_BYTES);
//         printf("Chord translation found: (%s)\n", (char*) word);
//         // for (int i = 0; i < (RECORD_LEN - MAPPED_KEY_BYTES) * 8; i++) printf("%c%s", (get_bit(word, i) ? '1' : '0'), (i % 8 == 7) ? " " : "");
//         // printf("\n");

//         //send word over USB HID
//         send_word(word);
//     } else {
//         printf("Chord not found in dictionary.\n");
//     }

//     free(word);
// }

void process_sequence() {
    push_sequence(mapped_chord_state);

    printf("Processing chord sequence:\n");
    for (int i = 0; i < MAX_SEQ_LEN; i++) {
        printf(" chord %d: ", i);
        print_bitfield(sequence_buffer[i], 8*MAPPED_KEY_BYTES);
    }
    printf(" (current_seq_len: %d)\n", current_seq_len);

    int seq_len;
    bool found = false;

    for (seq_len = current_seq_len; seq_len > 0; seq_len--) {
        printf("Looking into dict %d\n", seq_len);

        found = binary_search_by_seq_len(seq_len);

        if (found) {
            break;
        }
    }

    if (!found) {
        printf("Chord sequence not found in any dict.\n");
        push_history(0);
        return;
    }

    printf("Chord sequence found in dict %d: (%.*s)\n", seq_len, MAX_WORD_LEN, (const char*) word_ptr);

    process_write(word_ptr, get_dict(seq_len).word_len, seq_len);
}