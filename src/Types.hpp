#pragma once

namespace RYSIC
{

	struct Pos
	{
		int x = 0, y = 0;

		inline Pos operator+(const int rhs) const
		{ return {x + rhs, y + rhs}; };
		inline Pos operator-(const int rhs) const
		{ return {x - rhs, y - rhs}; };
		inline Pos operator*(const int rhs) const
		{ return {x * rhs, y * rhs}; };
		inline Pos operator/(const int rhs) const
		{ return {x / rhs, y / rhs}; };

		inline Pos operator+(const Pos &rhs) const
		{ return {x + rhs.x, y + rhs.y}; };
		inline Pos operator-(const Pos &rhs) const
		{ return {x - rhs.x, y - rhs.y}; };
		inline Pos operator*(const Pos &rhs) const
		{ return {x * rhs.x, y * rhs.y}; };
		inline Pos operator/(const Pos &rhs) const
		{ return {x / rhs.x, y / rhs.y}; };

		inline Pos& operator+=(const int rhs)
		{ x += rhs; y += rhs; return *this; };
		inline Pos& operator-=(const int rhs)
		{ x -= rhs; y -= rhs; return *this; };
		inline Pos& operator*=(const int rhs)
		{ x *= rhs; y *= rhs; return *this; };
		inline Pos& operator/=(const int rhs)
		{ x /= rhs; y /= rhs; return *this; };

		inline Pos& operator+=(const Pos &rhs)
		{ x += rhs.x; y += rhs.y; return *this; };
		inline Pos& operator-=(const Pos &rhs)
		{ x -= rhs.x; y -= rhs.y; return *this; };
		inline Pos& operator*=(const Pos &rhs)
		{ x *= rhs.x; y *= rhs.y; return *this; };
		inline Pos& operator/=(const Pos &rhs)
		{ x /= rhs.x; y /= rhs.y; return *this; };

		inline bool operator==(const Pos &rhs) const
		{ return x == rhs.x && y == rhs.y; };
		inline bool operator!=(const Pos &rhs) const
		{ return x != rhs.x && y != rhs.y; };
	};

	struct Rect
	{
		int x, y, w, h;
	};
	
}
