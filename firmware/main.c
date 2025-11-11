#include "pico/stdlib.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "my_hid.h"


int main() {
    stdio_init_all();
    hid_init();

    send_word("Hello world!\0");
}