#ifndef __AIRHOCKEY_AIPLAYERSETTINGS_H__
#define __AIRHOCKEY_AIPLAYERSETTINGS_H__

#include "cocos2d.h"
#include <functional>


namespace airhockey
{
	using namespace cocos2d;

	/* Key points for AI defense */
	struct Pyramid
	{
		explicit Pyramid(const Vec2& p_top, const Vec2& p_left, const Vec2& p_right) :
			pyramidTop(p_top),
			pyramidLeft(p_left),
			pyramidRight(p_right),
			pyramidBase(p_top.x, (p_left.y + p_right.y) / 2)
		{}

		Vec2 pyramidTop, pyramidLeft, pyramidRight, pyramidBase;
	};

	struct AIPlayerSettings
	{
		using AttackRadiusFunction = std::function<float (const cocos2d::Vec2&)>;

		explicit AIPlayerSettings(const Pyramid& pyramid, float attack_radius, 
			AttackRadiusFunction attack_radius_func) :
			pyramid(pyramid),
			attackRadius(attack_radius),
			attackRadiusFunc(attack_radius_func)
		{}

		Pyramid pyramid;
		float attackRadius;
		AttackRadiusFunction attackRadiusFunc;
	};

}

#endif // __AIRHOCKEY_AIPLAYERSETTINGS_H__