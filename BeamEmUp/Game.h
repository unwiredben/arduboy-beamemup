/*
   Copyright (C) 2020 Ben Combee (@unwiredben)

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#include <Arduboy2.h>
#include <ArduboyTones.h>

#include <FixedPoints.h>
using Number = SFixed<7, 8>;
using BigNumber = SFixed<15, 16>;

#include "Util.h"

#include "BeamEmUp_bmp.h"
#include "Font4x6.h"
#include "cow_bmp.h"
#include "got_em_all_bmp.h"
#include "meteor_bmp.h"
#include "moo_bmp.h"
#include "objective_bmp.h"
#include "press_a_bmp.h"
#include "small_text_bmp.h"
#include "squid_bmp.h"
#include "unwired_logo_bmp.h"

/* global definitions for APIs */
Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Sprites sprites;

namespace BeamEmUpGame {

// Simulates friction
// Not actually how a real coefficient of friction works
constexpr Number CoefficientOfFriction = 0.95;

// Simulates gravity
// Earth's gravitational pull is 9.8 m/s squared
// But that's far too powerful for the tiny screen
// So I picked something small
constexpr Number CoefficientOfGravity = 0.5;

// Simulates bounciness
// Again, not quite like the real deal
constexpr Number CoefficientOfRestitution = 0.7;

// Prevents never-ending bounciness
constexpr Number RestitutionThreshold = Number::Epsilon * 16;

// Amount of force the player exerts
constexpr Number InputForce = 0.25;

// maximum x coordinate for left edge of the screen
constexpr BigNumber window_x_min = 0;
constexpr BigNumber window_x_max = 4 * WIDTH;

// how much left/right screen margin to keep
constexpr BigNumber squid_margin = 16;

struct GameObject {
  BigNumber x = 0, x_min = window_x_min, x_max = window_x_max, x_vel = 0;
  Number y = 0, y_min = 0, y_max = HEIGHT, y_vel = 0;

  void move(BigNumber newX, Number newY) {
    x = newX;
    x = constrain(x, x_min, x_max);
    y = newY;
    y = constrain(y, y_min, y_max);
  }
  void adjust(BigNumber dx, Number dy) { move(x + dx, y + dy); }
  void applyXVelocity() {
    if (x_vel != 0) {
      x = x + x_vel;
      if (x < x_min) {
        x = x_min;
        x_vel = -x_vel;
      } else if (x > x_max) {
        x = x_max;
        x_vel = -x_vel;
      }
      x_vel = x_vel * CoefficientOfFriction;
    }
  }

  void applyYVelocity() {
    if (y_vel != 0) {
      y = y + y_vel;
      if (y < y_min) {
        y = y_min;
        y_vel = -y_vel;
      } else if (y > y_max) {
        y = y_max;
        y_vel = -y_vel * CoefficientOfRestitution;
      }
    }
    y_vel = y_vel + CoefficientOfGravity;
  }

  void applyVelocity() {
    applyXVelocity();
    applyYVelocity();
  }
};

struct Window : public GameObject {
  void reset() { x = 0; }

  void keep_obj_in_window(BigNumber pos, BigNumber width, BigNumber margin) {
    if (pos < x + margin) {
      move(pos - margin, y);
    } else if (pos + width > x + WIDTH - margin) {
      move(pos + width + margin - WIDTH, y);
    }
  }
} window;

struct Landscape {
  static constexpr uint8_t topY = 62;
  static constexpr uint8_t botY = 63;

  void draw(BigNumber win_x) {
    arduboy.drawFastHLine(0, botY, WIDTH);

    // convert x to integer in [0..31] range
    // draw the panels to simulate movement
    auto ix = win_x.getInteger() % 18;
    for (int16_t i = -4 - ix; i < WIDTH; i += 18) {
      arduboy.drawFastHLine(i, topY, 4);
    }
  }
} landscape;

uint8_t num_cows = 0;
constexpr uint8_t max_num_cows = 15;

struct Cow : public GameObject {
  static constexpr uint8_t default_y = HEIGHT - cow_height;

  uint8_t frame;
  uint8_t moo_frame;

  Cow() {
    x_min = squid_margin - 2 + window_x_min;
    x_max = window_x_max - cow_width - squid_margin + 2; 
    y_max = default_y;
  }

  void reset() {
    x = randomSFixed(x_min, x_max);
    y = default_y;
    x_vel = 0;
    y_vel = 0;
    frame = x.getInteger() % 3;
    moo_frame = 0;
  }

  void startMoo() { moo_frame = 10; }

  void draw(BigNumber win_x) {
    auto drawn_x = x - win_x;
    if (in_open_range(drawn_x, BigNumber(0), BigNumber(WIDTH))) {
      int ix = drawn_x.getInteger();
      int iy = y.getInteger();
      sprites.drawPlusMask(ix, iy, animcow_plus_mask,
                           frame + (x_vel > 0 ? 3 : 0));
      if ((arduboy.frameCount & 3) == 0 && ++frame == 3) {
        frame = 0;
      }
      if (moo_frame) {
        if (y == default_y) {
          sprites.drawPlusMask(ix, iy - 7, moo_plus_mask, 0);
        }
        --moo_frame;
      }
    }
  }
} cows[max_num_cows];

struct SquidShip : public GameObject {
  uint8_t frame;
  int8_t beam_height;
  uint8_t penalty_frames;
  static constexpr int8_t beam_height_max = 14;
  static constexpr uint8_t num_penalty_frames = 15;

  SquidShip() {
    x_min = squid_margin + window_x_min;
    x_max = window_x_max - squid_width - squid_margin;
    y_min = 8;
    y_max = HEIGHT - squid_height - 10;
  }

  void reset() {
    x = center_x(squid_width);
    y = center_y(squid_height);
    x_vel = 0;
    frame = 0;
    beam_height = 0;
    penalty_frames = 0;
  }

  void nextFrame() { frame = (frame + 1) % squid_frames; }

  void meteorHit() {
    beam_height = 0;
    penalty_frames = num_penalty_frames;
  }

  void adjust_beam(int8_t delta) {
    if (penalty_frames) {
      beam_height = 0;
      return;
    }
    beam_height += delta;
    if (beam_height < 0) {
      beam_height = 0;
    } else if (beam_height > beam_height_max) {
      beam_height = beam_height_max;
    }
  }

  void draw(BigNumber win_x) {
    auto drawn_x = x - win_x;
    if (in_open_range(drawn_x, BigNumber(0), BigNumber(WIDTH))) {
      int8_t ix = drawn_x.getInteger(), iy = y.getInteger();
      sprites.drawSelfMasked(ix, iy, squid_img, frame >= 2);
      if (beam_height > 0) {
        arduboy.drawLine(ix + 2, iy + squid_height, ix,
                         min(iy + squid_height + beam_height, 60));
        arduboy.drawLine(ix + squid_width - 3, iy + squid_height,
                         ix + squid_width - 1,
                         min(iy + squid_height + beam_height, 60));
      }
    }
    if (penalty_frames) {
      --penalty_frames;
    }
  }
} squid;

struct Meteor : public GameObject {
  Meteor() {
    x_min = window_x_min;
    x_max = window_x_max - meteor_width;
    y_min = 8;
    y_max = HEIGHT - meteor_height - 10;
  }

  void reset() {
    // start at opposite end from the squid
    if (squid.x < (window_x_max / 2)) {
      x = x_max;
      x_vel = -2.5;
    } else {
      x = x_min;
      x_vel = 2.5;
    }
    move(x, squid.y + randomSFixed(Number(-8), Number(8)));
  }

  void applyXVelocity() {
    // no friction
    if (x_vel != 0) {
      x = x + x_vel;
      if (x < x_min) {
        x = x_min;
        x_vel = -x_vel;
        y = randomSFixed(y_min, y_max);
      } else if (x > x_max) {
        x = x_max;
        x_vel = -x_vel;
        y = randomSFixed(y_min, y_max);
      }
    }
  }

  void draw(BigNumber win_x) {
    auto drawn_x = x - win_x;
    if (in_open_range(drawn_x, BigNumber(0), BigNumber(WIDTH))) {
      int8_t ix = drawn_x.getInteger(), iy = y.getInteger();
      sprites.drawPlusMask(ix, iy, meteor_plus_mask,
                           ((arduboy.frameCount >> 2) % meteor_frames) +
                               ((x_vel < 0) ? 0 : meteor_frames));
    }
  }
} meteor;

// coordinating game state
enum GameState {
  INITIAL_LOGO,
  TITLE_SCREEN,
  OBJECTIVE_SCREEN,
  GAME_ACTIVE,
  LEVEL_COMPLETE
} state = INITIAL_LOGO;

void enter_state(GameState newState) {
  arduboy.frameCount = 0;
  sound.noTone();
  state = newState;

  if (newState == TITLE_SCREEN) {
    num_cows = 3;
  } else if (newState == GAME_ACTIVE) {
    randomSeed(arduboy.generateRandomSeed());
    window.reset();
    squid.reset();
    meteor.reset();
    for (auto i = 0; i < num_cows; ++i)
      cows[i].reset();
  }
}

void initial_logo() {
  if (arduboy.frameCount == 1) {
    arduboy.clear();
    arduboy.drawCompressed(0, 0, unwiredgames_logo_cmpimg);
  }
  if (arduboy.frameCount > 45) {
    enter_state(TITLE_SCREEN);
  }
}

void title_screen() {
  if (arduboy.frameCount == 1) {
    arduboy.clear();
    arduboy.drawCompressed(center_x(beam_em_up_width), 10, beam_em_up_cmpimg);
    arduboy.drawCompressed(center_x(press_a_to_start_width),
                           HEIGHT - press_a_to_start_height - 10,
                           press_a_to_start_cmpimg);
  }
  if (arduboy.justPressed(A_BUTTON)) {
    enter_state(OBJECTIVE_SCREEN);
  }
}

void objective_screen() {
  if (arduboy.frameCount == 1) {
    arduboy.clear();
    arduboy.drawCompressed(0, 0, objective_cmpimg);
  }
  if (arduboy.frameCount > 60) {
    enter_state(GAME_ACTIVE);
  }
}

bool meteor_collision_with_squid() {
  return in_open_range(meteor.y, squid.y - meteor_height,
                       squid.y + squid_height) &&
         in_open_range(meteor.x, squid.x - meteor_width, squid.x + squid_width);
}

constexpr static uint16_t squidFreqs[8] = {
    200, 240, 280, 320, 360, 320, 280, 240,
};

void game_active() {
  // process input
  if (arduboy.pressed(RIGHT_BUTTON)) {
    squid.x_vel += 0.3;
    squid.nextFrame();
  }
  if (arduboy.pressed(LEFT_BUTTON)) {
    squid.x_vel -= 0.3;
    squid.nextFrame();
  }
  if (arduboy.pressed(UP_BUTTON)) {
    squid.adjust(0, -1);
    squid.nextFrame();
  }
  if (arduboy.pressed(DOWN_BUTTON)) {
    squid.adjust(0, 1);
    squid.nextFrame();
  }
  squid.applyXVelocity();

  window.keep_obj_in_window(squid.x, squid_width, 16);

  // if meteor collides, apply x_vel change and drop beam
  if (meteor_collision_with_squid()) {
    squid.meteorHit();
    squid.x_vel = meteor.x_vel * 1.5;
    meteor.reset();
    sound.tone(100, 256, 80, 128, 100, 256);
  } else {
    meteor.applyXVelocity();
  }

  squid.adjust_beam(arduboy.pressed(A_BUTTON) ? 2 : -4);
  if (squid.beam_height) {
    // 137/1024 ~ 4 frames of animation
    sound.tone(squidFreqs[(arduboy.frameCount >> 2) & 7], 137);
  }

  auto squid_center = squid.x + squid_width / 2;
  uint8_t cows_in_beam = 0;
  for (auto i = 0; i < num_cows; ++i) {
    auto &cow = cows[i];
    auto cow_center = cow.x + cow_width / 2;
    if (squid.beam_height &&
        in_open_range(cow_center, squid.x - 1, squid.x + squid_width + 1)) {
      if (in_open_range(cow.y, squid.y,
                        squid.y + squid_height + squid.beam_height)) {
        ++cows_in_beam;
      }
      if (in_open_range(cow.y, squid.y + squid_height,
                        squid.y + squid_height + squid.beam_height)) {
        cow.y_vel = -0.6 - CoefficientOfGravity;
        if (cow_center < squid_center) {
          cow.x_vel = squid.x_vel + 0.2;
        } else if (cow_center > squid_center) {
          cow.x_vel = squid.x_vel - 0.2;
        }
      }
    }
    // cows on the ground occasionally start to wander
    if (cow.y == Cow::default_y && random(32) == 0) {
      cow.x_vel = randomSFixed(BigNumber(-2), BigNumber(2));
    }

    if (random(128) == 0) {
      cow.startMoo();
    }
    cow.applyVelocity();
  }

  arduboy.clear();
  landscape.draw(window.x);
  squid.draw(window.x);
  for (auto i = 0; i < num_cows; ++i) {
    cows[i].draw(window.x);
  }
  meteor.draw(window.x);

  // draw how many cows are in the beam
  uint8_t score_width = cows_text_width + of_text_width + 3 + 3 + 5 + 5 +
                        (cows_in_beam >= 10) * 5 + (num_cows >= 10) * 5;
  uint8_t score_x = WIDTH - score_width - 1;

  if (cows_in_beam >= 10) {
    sprites.drawOverwrite(score_x, 0, font4x6_digits, cows_in_beam / 10);
    score_x += 4 + 1;
  }
  sprites.drawOverwrite(score_x, 0, font4x6_digits, cows_in_beam % 10);
  score_x += 4 + 3;
  sprites.drawOverwrite(score_x, 1, of_text_img, 0);
  score_x += of_text_width + 3;
  if (num_cows >= 10) {
    sprites.drawOverwrite(score_x, 0, font4x6_digits, num_cows / 10);
    score_x += 4 + 1;
  }
  sprites.drawOverwrite(score_x, 0, font4x6_digits, num_cows % 10);
  score_x += 4 + 3;
  sprites.drawOverwrite(score_x, 1, cows_text_img, 0);

  if (cows_in_beam == num_cows) {
    enter_state(LEVEL_COMPLETE);
  }

  // temporary change to allow resetting
  if (arduboy.pressed(A_BUTTON) && arduboy.justPressed(B_BUTTON)) {
    enter_state(INITIAL_LOGO);
  }
} // namespace BeamEmUpGame

constexpr static uint16_t level_theme[] PROGMEM = {
    0x01EE, 0x00AF, 0x01B8, 0x00AF, 0x0188, 0x00AF, 0x01B8,
    0x00AF, 0x024B, 0x00AF, 0x01EE, 0x015E, 0x024B, 0x00AF,
    0x01EE, 0x015E, 0x015D, 0x00AF, 0x0188, 0x00AF, TONES_END};

void level_complete() {
  if (arduboy.frameCount == 1) {
    num_cows = min(num_cows + 1, max_num_cows);
  }
  if (arduboy.frameCount == 15) {
    // show "level complete"
    constexpr uint8_t x = (WIDTH - got_em_all_width) / 2;
    constexpr uint8_t y = (HEIGHT - got_em_all_height) / 2;
    arduboy.fillRect(x - 1, y - 1, got_em_all_width + 2, got_em_all_height + 2,
                     BLACK);
    arduboy.drawCompressed(x, y, got_em_all_cmpimg);
    sound.tones(level_theme);
  }
  if (arduboy.frameCount > 75 && !sound.playing()) {
    enter_state(GAME_ACTIVE);
  }
}

void setup(void) {
  arduboy.begin();
  arduboy.setFrameRate(30);
  enter_state(INITIAL_LOGO);
}

void loop(void) {
  if (!(arduboy.nextFrame()))
    return;

  arduboy.pollButtons();

  switch (state) {
  case INITIAL_LOGO:
    initial_logo();
    break;
  case TITLE_SCREEN:
    title_screen();
    break;
  case OBJECTIVE_SCREEN:
    objective_screen();
    break;
  case GAME_ACTIVE:
    game_active();
    break;
  case LEVEL_COMPLETE:
    level_complete();
    break;
  }

  arduboy.display();
}

} // namespace BeamEmUpGame
