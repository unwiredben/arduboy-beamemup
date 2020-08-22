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
#include "Vector.h"

class RigidBody
{
public:
	// Fields
	Point2 position = Point2(0, 0);
	Vector2 velocity = Vector2(0, 0);
	Number mass = 1.0;

public:
	// Constructors
	constexpr RigidBody(void) = default;
	constexpr RigidBody(Point2 position) : position(position), velocity(), mass(1.0) {}
	constexpr RigidBody(Point2 position, Number mass) : position(position), velocity(), mass(mass) {}
	
	constexpr Number getX(void) const
	{
		return this->position.x;
	}
	
	constexpr Number getY(void) const
	{
		return this->position.y;
	}
	
	void applyForce(Vector2 force)
	{
		this->velocity += (force / mass);
	}
};