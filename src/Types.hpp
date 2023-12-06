#pragma once

#include "screen/Colors.hpp"

namespace RYSIC
{

	struct Pos
	{
		int x = 0, y = 0;

		Pos() = default;
		Pos(int _x, int _y)
			: x(_x), y(_y)
		{}
		Pos(const std::array<int, 2> &xy)
			: x(xy[0]), y(xy[1])
		{}

		inline Pos operator+(const int rhs) const
		{ return {x + rhs, y + rhs}; }
		inline Pos operator-(const int rhs) const
		{ return {x - rhs, y - rhs}; }
		inline Pos operator*(const int rhs) const
		{ return {x * rhs, y * rhs}; }
		inline Pos operator/(const int rhs) const
		{ return {x / rhs, y / rhs}; }

		inline Pos operator+(const Pos &rhs) const
		{ return {x + rhs.x, y + rhs.y}; }
		inline Pos operator-(const Pos &rhs) const
		{ return {x - rhs.x, y - rhs.y}; }
		inline Pos operator*(const Pos &rhs) const
		{ return {x * rhs.x, y * rhs.y}; }
		inline Pos operator/(const Pos &rhs) const
		{ return {x / rhs.x, y / rhs.y}; }

		inline Pos& operator+=(const int rhs)
		{ x += rhs; y += rhs; return *this; }
		inline Pos& operator-=(const int rhs)
		{ x -= rhs; y -= rhs; return *this; }
		inline Pos& operator*=(const int rhs)
		{ x *= rhs; y *= rhs; return *this; }
		inline Pos& operator/=(const int rhs)
		{ x /= rhs; y /= rhs; return *this; }

		inline Pos& operator+=(const Pos &rhs)
		{ x += rhs.x; y += rhs.y; return *this; }
		inline Pos& operator-=(const Pos &rhs)
		{ x -= rhs.x; y -= rhs.y; return *this; }
		inline Pos& operator*=(const Pos &rhs)
		{ x *= rhs.x; y *= rhs.y; return *this; }
		inline Pos& operator/=(const Pos &rhs)
		{ x /= rhs.x; y /= rhs.y; return *this; }

		inline bool operator==(const Pos &rhs) const
		{ return x == rhs.x && y == rhs.y; }
		inline bool operator!=(const Pos &rhs) const
		{ return x != rhs.x && y != rhs.y; }

		operator std::array<int, 2>() const { return {x, y}; }

		Pos clamp(int min, int max) const
		{ return { CLAMP(x, min, max), CLAMP(y, min, max) }; }

		bool zero() const
		{ return (x == 0 && y == 0); }
		bool positive() const
		{ return (x > 0 && y > 0); }
		bool negative() const
		{ return (x < 0 && y < 0); }

		double distance(const Pos& b) const
		{ return sqrt(pow(b.x - x, 2) + pow(b.y - y, 2)); }
	};

	struct Rect
	{
		int x, y, w, h;

		Rect() = default;
		Rect(int _x, int _y, int _w, int _h)
			: x(_x), y(_y), w(_w), h(_h)
		{}
		Rect(const std::array<int, 4> &xywh)
			: x(xywh[0]), y(xywh[1]), w(xywh[2]), h(xywh[3])
		{}

		operator std::array<int, 4>() const { return {x, y, w, h}; }

		bool intersects(const Rect& rect) const
		{
			const int
				a_x1 = x,				a_y1 = y,
				a_x2 = x + w,			a_y2 = y + h,
				b_x1 = rect.x,			b_y1 = rect.y,
				b_x2 = rect.x + rect.w,	b_y2 = rect.y + rect.h;
			
			return
				!(a_x2 < b_x1 || a_x1 > b_x2 ||
				a_y2 < b_y1 || a_y1 > b_y2);
		}

		const Pos center() const { return {x + (w / 2), y + (h / 2)}; }
		const Rect inner(int thickness = 1) const { return {x + thickness, y + thickness, w - (thickness * 2), h - (thickness * 2)}; }
	};

	struct Glyph
	{
		int ch = 0;
		Color fg = std::nullopt, bg = std::nullopt;

		bool operator==(const Glyph& rhs) const
		{ return memcmp(this, &rhs, sizeof(Glyph)); }
	};
	
}
