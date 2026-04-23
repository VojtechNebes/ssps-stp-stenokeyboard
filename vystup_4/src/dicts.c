#include "dicts.h"


// extern const uint8_t _binary_output_dict_{i}_bin_start[];
// extern const uint8_t _binary_output_dict_{i}_bin_end[];
// extern const uint8_t _binary_output_dict_{i}_bin_size[];

extern const uint8_t _binary_output_dict_5_bin_start[];
extern const uint8_t _binary_output_dict_5_bin_end[];
extern const uint8_t _binary_output_dict_5_bin_size[];

extern const uint8_t _binary_output_dict_4_bin_start[];
extern const uint8_t _binary_output_dict_4_bin_end[];
extern const uint8_t _binary_output_dict_4_bin_size[];

extern const uint8_t _binary_output_dict_3_bin_start[];
extern const uint8_t _binary_output_dict_3_bin_end[];
extern const uint8_t _binary_output_dict_3_bin_size[];

extern const uint8_t _binary_output_dict_2_bin_start[];
extern const uint8_t _binary_output_dict_2_bin_end[];
extern const uint8_t _binary_output_dict_2_bin_size[];

extern const uint8_t _binary_output_dict_1_bin_start[];
extern const uint8_t _binary_output_dict_1_bin_end[];
extern const uint8_t _binary_output_dict_1_bin_size[];


// Note: the length of this array must match MAX_SEQ_LEN
static const Dict dicts[] = {
    { _binary_output_dict_1_bin_start, _binary_output_dict_1_bin_end, MAX_WORD_LEN },
    { _binary_output_dict_2_bin_start, _binary_output_dict_2_bin_end, MAX_WORD_LEN },
    { _binary_output_dict_3_bin_start, _binary_output_dict_3_bin_end, MAX_WORD_LEN },
    { _binary_output_dict_4_bin_start, _binary_output_dict_4_bin_end, MAX_WORD_LEN },
    { _binary_output_dict_5_bin_start, _binary_output_dict_5_bin_end, MAX_WORD_LEN },
};

const Dict get_dict(int seq_len) {
    return dicts[seq_len - 1];
}