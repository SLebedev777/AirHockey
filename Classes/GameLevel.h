#ifndef __GAMELEVEL_H__
#define __GAMELEVEL_H__

#include <memory>

namespace airhockey
{
	struct GameLevel
	{
		explicit GameLevel(
			int number,
			float width,
			float height
		) :
			m_number(number),
			m_width(width),
			m_height(height)
		{}

		~GameLevel() {}

		int m_number;
		float m_width;
		float m_height;
	};

	typedef std::shared_ptr<GameLevel> GameLevelPtr;

}
#endif // __GAMELEVEL_H__