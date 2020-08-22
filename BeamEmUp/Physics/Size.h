/*
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

#include "Common.h"

class Size2
{
public:
	// Fields
	NumberU width;
	NumberU height;
	
public:
	// Constructors
	constexpr Size2(void) = default;
	constexpr Size2(NumberU width, NumberU height) : width(width), height(height) {}
};

inline constexpr bool operator ==(Size2 left, Size2 right)
{
	return ((left.width == right.width) && (left.height == right.height));
}

inline constexpr bool operator !=(Size2 left, Size2 right)
{
	return ((left.width != right.width) || (left.height != right.height));
}