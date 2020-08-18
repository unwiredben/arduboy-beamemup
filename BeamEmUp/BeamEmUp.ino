#include <Arduboy2.h>

#include "unwired_logo_bmp.h"
#include "BeamEmUp_bmp.h"
#include "press_a_bmp.h"

Arduboy2 arduboy;
Sprites sprites;
BeepPin1 beep;

constexpr uint8_t center_x(uint8_t w) {
  return (arduboy.width() - w) / 2;
}

constexpr uint8_t center_y(uint8_t w) {
  return (arduboy.height() - w) / 2;
}

// const uint8_t saucer[] PROGMEM = { 16, 16,
// /* frame 0 */
// 0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 0x00,0xC2,0x37,0x04,0x07,0x0C,0x0F,0x04,0x07,0x37,0xC2,0x00,0x00,0x00,0x00,0x00,
// /* frame 1 */
// 0x00,0x00,0x00,0x80,0x80,0xC0,0xC0,0x80,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
// 0x00,0xC2,0x37,0x07,0x04,0x0F,0x0C,0x07,0x04,0x37,0xC2,0x00,0x00,0x00,0x00,0x00,
//  };

constexpr uint8_t squid_width = 14;
constexpr uint8_t squid_height = 14;

const uint8_t squid[] PROGMEM = { 14, 16,
/* frame 0 */
0xC0,0xE0,0xF0,0xD0,0xB8,0xDC,0xBC,0xDC,0xBC,0xD8,0xB0,0xF0,0xE0,0xC0,
0x01,0x1D,0x03,0x3A,0x06,0x05,0x05,0x05,0x05,0x06,0x3A,0x03,0x1D,0x01,
/* frame 1 */
0xC0,0xE0,0xF0,0xB0,0xD8,0xBC,0xDC,0xBC,0xDC,0xB8,0xD0,0xF0,0xE0,0xC0,
0x19,0x05,0x03,0x0A,0x36,0x05,0x05,0x05,0x05,0x36,0x0A,0x03,0x05,0x19,
 };

const uint16_t squidFreqs[8] = {
  beep.freq(200),
  beep.freq(240),
  beep.freq(280),
  beep.freq(320),
  beep.freq(360),
  beep.freq(320),
  beep.freq(280),
  beep.freq(240),
};

struct BeamEmUpGame {

  constexpr static uint8_t x_max = arduboy.width() - squid_width;
  constexpr static uint8_t y_max = arduboy.height() - squid_height;

  uint8_t squid_x = 0;
  uint8_t squid_y = 0;
  uint8_t squid_frame = 0;

  enum GameState {
    INITIAL_LOGO,
    TITLE_SCREEN,
    GAME_ACTIVE,
    GAME_OVER
  } state = INITIAL_LOGO;

  void enter_state(GameState newState) {
    arduboy.frameCount = 0;
    state = newState;

    if (newState == GAME_ACTIVE) {
      squid_x = center_x(squid_width);
      squid_y = center_y(squid_height);
      squid_frame = 0;
    }
  }

  void initial_logo() {
    if (arduboy.frameCount == 1) {
      arduboy.clear();
      arduboy.drawCompressed(0, 0, unwiredgames_logo_cmpimg);
    }
    if (arduboy.frameCount > 60) {
      enter_state(TITLE_SCREEN);
    }
  }

  void title_screen() {
    arduboy.pollButtons();
    if (arduboy.frameCount == 1) {
      arduboy.clear();
      arduboy.drawCompressed(center_x(beam_em_up_width), 10, beam_em_up_cmpimg);
      arduboy.drawCompressed(
        center_x(press_a_to_start_width),
        arduboy.height() - press_a_to_start_height - 10,
        press_a_to_start_cmpimg);
    }
    if (arduboy.justPressed(A_BUTTON)) {
      enter_state(GAME_ACTIVE);
    }
  }

  void game_active() {
    arduboy.pollButtons();

    uint8_t update_frame = 0;

    if (arduboy.pressed(RIGHT_BUTTON) && (squid_x < x_max)) {
      squid_x++; update_frame = true;
    }
    if (arduboy.pressed(LEFT_BUTTON) && (squid_x > 0)) {
      squid_x--; update_frame = true;
    }
    if (arduboy.pressed(UP_BUTTON) && (squid_y > 0)) {
      squid_y--; update_frame = true;
    }
    if (arduboy.pressed(DOWN_BUTTON) && (squid_y < y_max)) {
      squid_y++; update_frame = true;
    }

    if (update_frame) {
      squid_frame = (squid_frame + 1) & 7;
    }

    bool draw_beam = arduboy.pressed(A_BUTTON);
    if (arduboy.justPressed(A_BUTTON)) {
      beep.tone(squidFreqs[arduboy.frameCount & 7]);
    }
    else if (arduboy.justReleased(A_BUTTON)) {
      beep.noTone();
    }

    arduboy.clear();
    sprites.drawSelfMasked(squid_x, squid_y, squid, squid_frame >= 4);
    if (draw_beam) {
      arduboy.drawLine(
        squid_x + 2, squid_y + squid_height,
        squid_x, min(squid_y + squid_height + 30, arduboy.height()));
      arduboy.drawLine(
        squid_x + squid_width - 3, squid_y + squid_height,
        squid_x + squid_width - 1, min(squid_y + squid_height + 30, arduboy.height()));
      beep.tone(squidFreqs[(arduboy.frameCount * 4) & 7]);
    }
    else {
      beep.noTone();
    }

    // temporary change to allow resetting
    if (arduboy.justPressed(B_BUTTON)) {
      enter_state(BeamEmUpGame::INITIAL_LOGO);
    }
  }

  void game_over() {
  }

  bool process_frame() {

    switch (state) {
      case INITIAL_LOGO: initial_logo(); break;
      case TITLE_SCREEN: title_screen(); break;
      case GAME_ACTIVE:  game_active(); break;
      case GAME_OVER:    game_over(); break;
    }

    return true;
  }

  void begin() {
    enter_state(INITIAL_LOGO);
  }

} game;

void setup() {
  arduboy.begin();
  beep.begin();
  arduboy.setFrameRate(30);
  game.begin();
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  if (game.process_frame()) {
    arduboy.display();
  }
}
