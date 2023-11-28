#include "Colors.hpp"

namespace RYSIC
{
	TCOD_ColorRGB GetColorFraction(const TCOD_ColorRGB col, int tone_idx, int tone_cnt)
	{
		return
		{
			(uint8_t) (((double) col.r / tone_cnt) * tone_idx),
			(uint8_t) (((double) col.g / tone_cnt) * tone_idx),
			(uint8_t) (((double) col.b / tone_cnt) * tone_idx)
		};
	}

	TCOD_ColorRGB DarkenColor(const TCOD_ColorRGB col)
	{
		TCOD_ColorRGB ret_col = col;
		TCOD_color_set_value(&ret_col, TCOD_color_get_value(ret_col) * 0.5f);
		return ret_col;
	}
	TCOD_ColorRGB LightenColor(const TCOD_ColorRGB col)
	{
		TCOD_ColorRGB ret_col = col;
		TCOD_color_set_value(&ret_col, MAX(TCOD_color_get_value(ret_col) * 2.0f, 1.0f));
		return ret_col;
	}
}