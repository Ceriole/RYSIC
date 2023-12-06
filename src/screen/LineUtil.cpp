#include "LineUtil.hpp"

#include <exception>

#define SIGN(N) ((N) < 0 ? -1 : ((N) > 0 ? 1 : 0))

namespace RYSIC::Util::Line
{

	const std::vector<Pos> Walk(const Pos &a, const Pos &b)
	{
		std::vector<Pos> points;

		int dx = b.x - a.x, dy = b.y - a.y;
		int sx = SIGN(dx), sy = SIGN(dy);
		dx = abs(dx), dy = abs(dy);
		int d = MAX(dx, dy);
		double r = d / 2;
		Pos xy = a;

		if(dx > dy)
		{
			for(int i = 0; i < d; i++)
			{
				points.push_back(xy);
				xy.x += sx; r += dy;
				if(r >= dx)
					xy.y += sy, r -= dx;
			}
		}
		else
		{
			for(int i = 0; i < d; i++)
			{
				points.push_back(xy);
				xy.y += sy; r += dx;
				if(r >= dy)
					xy.y += sx, r -= dy;
			}
		}

		return points;
	}

}