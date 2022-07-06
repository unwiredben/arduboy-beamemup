#pragma once

constexpr uint8_t of_text_width = 7;
constexpr uint8_t of_text_height = 5;

const unsigned char PROGMEM of_text_img[] =
{
// width, height,
7, 5,
// FRAME 00
0x1f, 0x11, 0x1f, 0x00, 0x1f, 0x05, 0x05,
};

constexpr uint8_t cows_text_width = 17;
constexpr uint8_t cows_text_height = 5;

const unsigned char PROGMEM cows_text_img[] =
{
// width, height,
17, 5,
// FRAME 00
0x0e, 0x11, 0x11, 0x00, 0x1f, 0x11, 0x1f, 0x00, 0x0f, 0x10, 0x08, 0x10, 0x0f, 0x00, 0x13, 0x15, 0x19,
};

