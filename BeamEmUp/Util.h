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

constexpr uint8_t center_x(uint8_t w) { return (Arduboy2::width() - w) / 2; }
constexpr uint8_t center_y(uint8_t h) { return (Arduboy2::height() - h) / 2; }

template <class T>
constexpr bool in_open_range(const T &val, const T &bottom, const T &top) {
  return val >= bottom && val < top;
}

template <class T>
constexpr bool in_closed_range(const T &val, const T &bottom, const T &top) {
  return val >= bottom && val <= top;
}