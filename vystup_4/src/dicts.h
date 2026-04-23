#pragma once

#include <stdint.h>

#define MAX_SEQ_LEN 5
#define MAX_WORD_LEN 18

typedef struct {
    const uint8_t* start;
    const uint8_t* end;
    // const uint8_t size[];

    int word_len;
    // int seq_len;
} Dict;

const Dict get_dict(int seq_len);