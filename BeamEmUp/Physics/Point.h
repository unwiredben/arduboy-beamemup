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
#include "Vector.h"

class Point2
{
public:
	// Fields
	Number x;
	Number y;
	
public:
	// Constructors
	constexpr Point2() = default;
	constexpr Point2(int8_t x, int8_t y) : x(x), y(y) {}
	constexpr Point2(int16_t x, int16_t y) : x(x), y(y) {}
	constexpr Point2(Number x, Number y) : x(x), y(y) {}
	
	Point2 & operator +=(Vector2 other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
	
	Point2 & operator -=(Vector2 other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}
};

inline constexpr bool operator ==(Point2 left, Point2 right)
{
	return ((left.x == right.x) && (left.y == right.y));
}

inline constexpr bool operator !=(Point2 left, Point2 right)
{
	return ((left.x != right.x) || (left.y != right.y));
}

// Shorthand to get square distance between two points
inline constexpr NumberU distanceSquared(Point2 firstPoint, Point2 secondPoint)
{
	// Constexpr version:
	return fromSigned(square(firstPoint.x - secondPoint.x) + square(firstPoint.y - secondPoint.y));
}

//
// Vector & Point interaction
//

// Adding a vector to a point offsets the point
inline constexpr Point2 operator +(Point2 point, Vector2 offset)
{
        return Point2(point.x + offset.x, point.y + offset.y);
}

// Subtracting a vector from a point offsets the point
inline constexpr Point2 operator -(Point2 point, Vector2 offset)
{
        return Point2(point.x - offset.x, point.y - offset.y);
}

// Subtracting two points gets the vector between them
inline constexpr Vector2 operator -(Point2 firstPoint, Point2 secondPoint)
{
        return Vector2(firstPoint.x - secondPoint.x, firstPoint.y - secondPoint.y);
}

/*// Shorthand to get square distance between two points
inline constexpr NumberU distanceSquared(Point2 firstPoint, Point2 secondPoint)
{
        // Readable Version:
        // const auto vector = firstPoint - secondPoint;
        // return vector.getMagnitudeSquared();

        // Constexpr version:
        return (firstPoint - secondPoint).getMagnitudeSquared();
}*/
