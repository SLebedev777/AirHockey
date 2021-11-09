#pragma once

#include <functional>
#include <string>
#include "cocos/2d/CCNode.h"
#include "cocos/math/Vec2.h"

namespace CCHelpers
{
	void CallCCNodeMethodRecursively(cocos2d::Node* node, std::function<void(cocos2d::Node*)> func);

	std::string Vec2Str(const cocos2d::Vec2& v);
}