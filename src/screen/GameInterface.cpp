#include "GameInterface.hpp"

namespace RYSIC::Interface
{
	CharacterScreen::CharacterScreen(Rect _rect, World::World *world)
		: Container(_rect), m_world(world)
	{
		m_lbl_header = Ref<Label>(new Label({1, 1}, "", C_WHITE, std::nullopt));
		add(m_lbl_header);
		m_bar_level = Ref<ProgressBar>(new ProgressBar({1, 1}, 10, "Level ??", C_WHITE, C_BLUE, C_DARK_BLUE));
		add(m_bar_level);
	}

}