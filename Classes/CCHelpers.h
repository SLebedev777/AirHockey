#pragma once

#include <functional>
#include <string>
#include "cocos/2d/CCNode.h"
#include "cocos/math/Vec2.h"

namespace CCHelpers
{
	void CallCCNodeMethodRecursively(cocos2d::Node* node, std::function<void(cocos2d::Node*)> func);

	std::string Vec2Str(const cocos2d::Vec2& v);

	void startDelay(cocos2d::Node* parent, float duration, const std::string& wait_node_name, int action_tag);
	bool isDelayOver(cocos2d::Node* parent, const std::string& wait_node_name, int action_tag);
}