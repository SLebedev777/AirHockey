#include "CCHelpers.h"

namespace CCHelpers
{

	void CallCCNodeMethodRecursively(cocos2d::Node* node, std::function<void(cocos2d::Node*)> func)
	{
		if (!node)
			return;

		func(node);

		for (auto obj : node->getChildren())
		{
			CallCCNodeMethodRecursively(obj, func);
		}
	}

	std::string Vec2Str(const cocos2d::Vec2& v)
	{
		std::string str = "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ")";
		return str;
	}
}