#include "Interface.hpp"

namespace RYSIC
{
	namespace World
	{
		class World;
	}

	namespace Interface
	{
		class CharacterScreen : public Container
		{
		private:
			World::World* m_world;

			Ref<Label> m_lbl_header;
			Ref<ProgressBar> m_bar_level;
			Ref<ProgressBar> m_bar_health;

		public:
			CharacterScreen() = default;
			CharacterScreen(Rect _rect, World::World *world);
		};
	}
}