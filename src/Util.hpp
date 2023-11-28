#pragma once

namespace RYSIC::Util
{

	template<typename T, size_t LL, size_t RL>
	constexpr std::array<T, LL+RL> join(std::array<T, LL> rhs, std::array<T, RL> lhs)
	{
		std::array<T, LL+RL> arr;
		auto current = std::copy(rhs.begin(), rhs.end(), arr.begin());
		std::copy(lhs.begin(), lhs.end(), current);
		return arr;
	}

	constexpr bool isWithin(int x, int y, int ax, int ay, int aw, int ah)
		{ return x >= ax && x < ax + aw && y >= ay && y < ay + ah; }
	constexpr bool isAt(int x, int y, int px, int py)
		{ return x == px && y == py; }
	
}