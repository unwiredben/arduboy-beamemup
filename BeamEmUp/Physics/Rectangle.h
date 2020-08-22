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
#include "Point.h"
#include "Size.h"

class Rectangle
{
public:
	// Fields
	Point2 position;
	Size2 size;

public:
	// Constructors
	constexpr Rectangle(void) = default;
	constexpr Rectangle(Point2 position) : position(position), size(1, 1) {}
	constexpr Rectangle(Point2 position, Size2 size) : position(position), size(size) {}
	constexpr Rectangle(Point2 position, uint8_t width, uint8_t height) : position(position), size(width, height) {}
	constexpr Rectangle(Number x, Number y) : position(x, y), size(1, 1) {}
	constexpr Rectangle(Number x, Number y, Size2 size) : position(x, y), size(size) {}
	constexpr Rectangle(Number x, Number y, uint8_t width, uint8_t height) : position(x, y), size(width, height) {}
	
	constexpr Number getX(void) const
	{
		return this->position.x;
	}
	
	constexpr Number getY(void) const
	{
		return this->position.y;
	}
	
	constexpr Size2 getSize(void) const
	{
		return this->size;
	}
	
	constexpr NumberU getWidth(void) const
	{
		return this->size.width;
	}
	
	constexpr NumberU getHeight(void) const
	{
		return this->size.height;
	}
	
	constexpr Number getLeft(void) const
	{
		return this->getX();
	}
	
	constexpr Number getRight(void) const
	{
		return (this->getX() + fromUnsigned(this->getWidth()));
	}
	
	constexpr Number getTop(void) const
	{
		return this->getY();
	}
	
	constexpr Number getBottom(void) const
	{
		return (this->getY() + fromUnsigned(this->getHeight()));
	}
	
	// Returns true if the point intersects the rectangle
	constexpr bool intersects(Point2 point) const
	{
		return
			(point.x >= this->getLeft()) &&
			(point.x <= this->getRight()) &&
			(point.y >= this->getTop()) &&
			(point.y <= this->getBottom());
	}
};
	
// Returns true if the rectangles intersect each other
constexpr inline bool intersects(Rectangle first, Rectangle second)
{
	return
	!(
		(first.getRight() < second.getLeft()) ||
		(first.getLeft() > second.getRight()) ||
		(first.getBottom() < second.getTop()) ||
		(first.getTop() > second.getBottom())
	);
}