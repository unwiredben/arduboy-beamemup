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
#include <stdint.h>

using Number = SFixed<7, 8>;
using NumberU = UFixed<8, 8>;

#include "BeamEmUp_bmp.h"
#include "Physics.h"
#include "press_a_bmp.h"
#include "squid_bmp.h"
#include "unwired_logo_bmp.h"

/* global definitions for APIs */
Arduboy2 arduboy;
Sprites sprites;
BeepPin1 beep;

constexpr uint8_t center_x(uint8_t w) { return (arduboy.width() - w) / 2; }
constexpr uint8_t center_y(uint8_t w) { return (arduboy.height() - w) / 2; }

const uint16_t squidFreqs[8] = {
    beep.freq(200), beep.freq(240), beep.freq(280), beep.freq(320),
    beep.freq(360), beep.freq(320), beep.freq(280), beep.freq(240),
};

struct SquidShip {
  Number x = 0;
  Number y = 0;
  uint8_t frame = 0;
  int8_t beam_height = 0;
  static constexpr uint8_t x_max = arduboy.width() - squid_width;
  static constexpr uint8_t y_max = arduboy.height() - squid_height;
  static constexpr int8_t beam_height_max = 20;

  void adjust_beam(int8_t delta) {
    beam_height += delta;
    if (beam_height < 0) {
      beam_height = 0;
    }
    else if (beam_height > beam_height_max) {
      beam_height = beam_height_max;
    }
  }

  void move(Number dx, Number dy) {
    x += dx;
    if (x < 0) {
      x = 0;
    } else if (x > x_max) {
      x = x_max;
    }
    y += dy;
    if (y < 0) {
      y = 0;
    } else if (y > y_max) {
      y = y_max;
    }
    frame = (frame + 1) % 4;
  }

  void draw() {
    int8_t ix = x.getInteger(), iy = y.getInteger();
    sprites.drawSelfMasked(ix, iy, squid_img, frame >= 2);
    if (beam_height > 0) {
      arduboy.drawLine(ix + 2, iy + squid_height, ix,
                       min(iy + squid_height + beam_height, arduboy.height()));
      arduboy.drawLine(ix + squid_width - 3, iy + squid_height,
                       ix + squid_width - 1,
                       min(iy + squid_height + beam_height, arduboy.height()));
    }
  }
};

struct BeamEmUpGame {
  // Simulates friction
  // Not actually how a real coefficient of friction works
  static constexpr Number CoefficientOfFriction = 0.95;

  // Simulates gravity
  // Earth's gravitational pull is 9.8 m/s squared
  // But that's far too powerful for the tiny screen
  // So I picked something small
  static constexpr Number CoefficientOfGravity = 0.5;

  // Simulates bounciness
  // Again, not quite like the real deal
  static constexpr Number CoefficientOfRestitution = 0.3;

  // Prevents never-ending bounciness
  static constexpr Number RestitutionThreshold = Number::Epsilon * 16;

  // Amount of force the player exerts
  static constexpr Number InputForce = 0.25;

  enum GameState {
    INITIAL_LOGO,
    TITLE_SCREEN,
    GAME_ACTIVE,
    GAME_OVER
  } state = INITIAL_LOGO;

  SquidShip squid;

  void enter_state(GameState newState) {
    arduboy.frameCount = 0;
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
    if (arduboy.frameCount > 60) {
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
    if (arduboy.pressed(RIGHT_BUTTON)) {
      squid.move(1, 0);
    }
    if (arduboy.pressed(LEFT_BUTTON)) {
      squid.move(-1, 0);
    }
    if (arduboy.pressed(UP_BUTTON)) {
      squid.move(0, 1);
    }
    if (arduboy.pressed(DOWN_BUTTON)) {
      squid.move(0, -1);
    }

    squid.adjust_beam(arduboy.pressed(A_BUTTON) ? 1 : -2);
    if (squid.beam_height) {
      beep.tone(squidFreqs[(arduboy.frameCount >> 2) & 7]);
    } else {
      beep.noTone();
    }

    draw_game_objects();

    // temporary change to allow resetting
    if (arduboy.justPressed(B_BUTTON)) {
      enter_state(BeamEmUpGame::INITIAL_LOGO);
    }
  }

  void draw_game_objects() {
    arduboy.clear();
    squid.draw();
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
};
