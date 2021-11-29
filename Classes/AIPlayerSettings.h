#ifndef __AIRHOCKEY_AIPLAYERSETTINGS_H__
#define __AIRHOCKEY_AIPLAYERSETTINGS_H__

#include "cocos2d.h"


namespace airhockey
{
	using namespace cocos2d;

	struct Pyramid
	{
		explicit Pyramid(const Vec2& p_top, const Vec2& p_left, const Vec2& p_right) :
			pyramidTop(p_top),
			pyramidLeft(p_left),
			pyramidRight(p_right)
		{}

		Vec2 pyramidTop, pyramidLeft, pyramidRight;
	};

	struct AIPlayerSettings
	{
		explicit AIPlayerSettings(const Pyramid& pyramid, float attack_radius) :
			pyramid(pyramid),
			attackRadius(attack_radius)
		{}

		Pyramid pyramid;
		float attackRadius;
	};

}

#endif // __AIRHOCKEY_AIPLAYERSETTINGS_H__