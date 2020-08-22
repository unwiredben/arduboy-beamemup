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

class Vector2
{
public:
	// Fields
	Number x;
	Number y;
	
public:
	// Constructors
	constexpr Vector2(void) = default;
	constexpr Vector2(int8_t x, int8_t y) : x(x), y(y) {}
	constexpr Vector2(int16_t x, int16_t y) : x(x), y(y) {}
	constexpr Vector2(Number x, Number y) : x(x), y(y) {}
	
	// Can't do a regular get magnitude without using float because
	// I don't have a fixed point sqrt
	constexpr NumberU getMagnitudeSquared(void) const
	{
		return fromSigned((x * x) + (y * y));
	}
	
	Vector2 & operator +=(Vector2 other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}
	
	Vector2 & operator -=(Vector2 other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}
	
	Vector2 & operator *=(Number factor)
	{
		this->x *= factor;
		this->y *= factor;
		return *this;
	}
	
	/*Vector2 & operator *=(NumberU factor)
	{
		this->x *= fromUnsigned(factor);
		this->y *= fromUnsigned(factor);
		return *this;
	}*/
	
	Vector2 & operator /=(Number factor)
	{
		const auto inverseFactor = (1 / factor);
		this->x *= inverseFactor;
		this->y *= inverseFactor;
		return *this;
	}
	
	/*Vector2 & operator /=(NumberU factor)
	{
		const auto inverseFactor = fromUnsigned(1 / factor);
		this->x *= inverseFactor;
		this->y *= inverseFactor;
		return *this;
	}*/
	
	Vector2 & operator -(void)
	{
		this->x = -this->x;
		this->y = -this->y;
		return *this;
	}
};

inline constexpr bool operator ==(Vector2 left, Vector2 right)
{
	return ((left.x == right.x) && (left.y == right.y));
}

inline constexpr bool operator !=(Vector2 left, Vector2 right)
{
	return ((left.x != right.x) || (left.y != right.y));
}

// Adding a vector to a vector creates a new vector
inline constexpr Vector2 operator +(Vector2 left, Vector2 right)
{
	return Vector2(left.x + right.x, left.y + right.y);
}

// Subtracting a vector from a vector creates a new vector
inline constexpr Vector2 operator -(Vector2 left, Vector2 right)
{
	return Vector2(left.x - right.x, left.y - right.y);
}

// Multiplying a vector by a factor scales the vector
inline constexpr Vector2 operator *(Vector2 vector, Number factor)
{
	return Vector2(vector.x * factor, vector.y * factor);
}

// Multiplying a vector by a factor scales the vector
/*inline constexpr Vector2 operator *(Vector2 vector, NumberU factor)
{
	return Vector2(vector.x * fromUnsigned(factor), vector.y * fromUnsigned(factor));
}*/

// Dividing a vector by a factor scales the vector
inline constexpr Vector2 operator /(Vector2 vector, Number factor)
{
	// Multiplying by the inverse might be cheaper
	return vector * (1 / factor);
}

// Dividing a vector by a factor scales the vector
/*inline constexpr Vector2 operator /(Vector2 vector, NumberU factor)
{
	// Multiplying by the inverse might be cheaper
	return vector * fromUnsigned(1 / factor);
}*/
