cmake_minimum_required(VERSION 3.13)

# replace these lines with the location of Pico SDK on your machine
# SDK can be downloaded from https://github.com/raspberrypi/pico-sdk
set(PICO_SDK_PATH "/home/nebes/pico-sdk")
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(stenokeyboard C CXX ASM)
pico_sdk_init()

file(GLOB DICTIONARIES "dicts/*.o")

add_executable(${PROJECT_NAME}
    main.c
    hw_keyboard.c
    dictionary.c
    dicts.c
    write_history.c
    my_hid.c
    special_chords.c
    usb_descriptors.c
    ${DICTIONARIES}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

# target_link_libraries(${PROJECT_NAME} pico_stdlib)
target_link_libraries(${PROJECT_NAME} pico_stdlib tinyusb_device tinyusb_board)

pico_enable_stdio_usb(${PROJECT_NAME} 1) # If 0, /dev/ttyACM0 shows no output
pico_enable_stdio_uart(${PROJECT_NAME} 0) # If 0, no GPIOs are used for UART

pico_add_extra_outputs(${PROJECT_NAME})
