/*
   Copyright (C) 2020 Ben Combee (@unwiredben)
   Copyright (C) 2018 Pharap (@Pharap)

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

#include <FixedPoints.h>
using Number = SFixed<7, 8>;
using BigNumber = SFixed<15, 8>;

#include "Physics.h"

#include "Util.h"

#include "BeamEmUp_bmp.h"
#include "cow_bmp.h"
#include "press_a_bmp.h"
#include "squid_bmp.h"
#include "unwired_logo_bmp.h"

/* global definitions for APIs */
Arduboy2 arduboy;
Sprites sprites;
BeepPin1 beep;

namespace BeamEmUpGame {

// utility functions

// maximum x coordinate for left edge of the screen
constexpr BigNumber window_x_min = 0;
constexpr BigNumber window_x_max = 3 * 128; // this allows 4x width

struct GameObject {
  BigNumber x = 0, x_min = window_x_min, x_max = window_x_max;
  Number y = 0, y_min = 0, y_max = arduboy.height();
  void move(BigNumber newX, Number newY) {
    x = newX;
    x = constrain(x, x_min, x_max);
    y = newY;
    y = constrain(y, y_min, y_max);
  }
  void adjust(BigNumber dx, Number dy) { move(x + dx, y + dy); }
};

struct Window : public GameObject {
  void keep_obj_in_window(BigNumber pos, BigNumber width, BigNumber margin) {
    if (pos < x + margin) {
      move(pos - margin, y);
    } else if (pos + width > x + arduboy.width() - margin) {
      move(pos + width + margin - arduboy.width(), y);
    }
  }
} window;

struct Landscape {
  static constexpr uint8_t topY = 62;
  static constexpr uint8_t botY = 63;

  void draw(BigNumber win_x) {
    arduboy.drawFastHLine(0, botY, arduboy.width());

    // convert x to integer in [0..31] range
    // draw the panels to simulate movement
    auto ix = win_x.getInteger() % 18;
    for (int16_t i = -4 - ix; i < 128; i += 18) {
      arduboy.drawFastHLine(i, topY, 4);
    }
  }
} landscape;

struct Cow : public GameObject {
  Cow(BigNumber x_pos) {
    x = x_pos;
    y = arduboy.height() - cow_height;
  }

  void draw(BigNumber win_x) {
    // draw rock on landscape
    auto drawn_x = (x - win_x).getInteger();
    if (drawn_x < 128) {
      auto frame = ((arduboy.frameCount >> 3) + x.getInteger()) % 3;
      sprites.drawPlusMask(drawn_x, y.getInteger(), animcow_plus_mask, frame);
    }
  }
} cows[6] = {Cow(20), Cow{70}, Cow{110}, Cow{170}, Cow{225}, Cow{300}};

struct SquidShip : public GameObject {
  uint8_t frame = 0;
  int8_t beam_height = 0;
  static constexpr int8_t beam_height_max = 14;

  SquidShip() {
    x_min = 10 + window_x_min;
    x_max = window_x_max - squid_width - 10;
    y_max = arduboy.height() - squid_height - 10;
  }

  void nextFrame() { frame = (frame + 1) % 4; }

  void adjust_beam(int8_t delta) {
    beam_height += delta;
    if (beam_height < 0) {
      beam_height = 0;
    } else if (beam_height > beam_height_max) {
      beam_height = beam_height_max;
    }
  }

  void draw(BigNumber win_x) {
    auto drawn_x = x - win_x;
    if (drawn_x < 128) {
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
  }
} squid;

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
constexpr Number CoefficientOfRestitution = 0.3;

// Prevents never-ending bounciness
constexpr Number RestitutionThreshold = Number::Epsilon * 16;

// Amount of force the player exerts
constexpr Number InputForce = 0.25;

// coordinating game state
enum GameState {
  INITIAL_LOGO,
  TITLE_SCREEN,
  GAME_ACTIVE,
  GAME_OVER
} state = INITIAL_LOGO;

void enter_state(GameState newState) {
  arduboy.frameCount = 0;
  beep.noTone();
  state = newState;

  if (newState == GAME_ACTIVE) {
    squid.x = center_x(squid_width);
    squid.y = center_y(squid_height);
    squid.frame = 0;
  }
}

void initial_logo() {
  if (arduboy.frameCount == 1) {
    arduboy.clear();
    arduboy.drawCompressed(0, 0, unwiredgames_logo_cmpimg);
  }
  if (arduboy.frameCount > 30) {
    enter_state(TITLE_SCREEN);
  }
}

void title_screen() {
  if (arduboy.frameCount == 1) {
    arduboy.clear();
    arduboy.drawCompressed(center_x(beam_em_up_width), 10, beam_em_up_cmpimg);
    arduboy.drawCompressed(center_x(press_a_to_start_width),
                           arduboy.height() - press_a_to_start_height - 10,
                           press_a_to_start_cmpimg);
  }
  if (arduboy.justPressed(A_BUTTON)) {
    enter_state(GAME_ACTIVE);
  }
}

void game_active() {
  // process input
  if (arduboy.pressed(RIGHT_BUTTON)) {
    squid.adjust(1, 0);
    squid.nextFrame();
  }
  if (arduboy.pressed(LEFT_BUTTON)) {
    squid.adjust(-1, 0);
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

  window.keep_obj_in_window(squid.x, squid_width, 10);

  constexpr uint16_t squidFreqs[8] = {
      beep.freq(200), beep.freq(240), beep.freq(280), beep.freq(320),
      beep.freq(360), beep.freq(320), beep.freq(280), beep.freq(240),
  };

  squid.adjust_beam(arduboy.pressed(A_BUTTON) ? 2 : -4);
  if (squid.beam_height) {
    beep.tone(squidFreqs[(arduboy.frameCount >> 2) & 7]);
  } else {
    beep.noTone();
  }

  // FIXME: handle intersection between beam and cows - if beam is in cow, cow
  // moves toward x and y position of squid

  // FIXME: if cow is not in beam, cow starts moving toward ground, gaining
  // velocity.  If contact with ground is too fast, cow becomes particle system
  // and dies.

  arduboy.clear();
  landscape.draw(window.x);
  squid.draw(window.x);
  for (auto &cow : cows) {
    cow.draw(window.x);
  }

  // arduboy.setCursor(0, 0);
  // arduboy.print(F("winX "));
  // arduboy.print(window.x.getInteger());
  // arduboy.print(F(" sqdX "));
  // arduboy.print(squid.x.getInteger());

  // temporary change to allow resetting
  if (arduboy.pressed(A_BUTTON) && arduboy.justPressed(B_BUTTON)) {
    enter_state(BeamEmUpGame::INITIAL_LOGO);
  }
}

void game_over() {}

void setup(void) {
  arduboy.begin();
  beep.begin();
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
  case GAME_ACTIVE:
    game_active();
    break;
  case GAME_OVER:
    game_over();
    break;
  }

  arduboy.display();
}

} // namespace BeamEmUpGame
