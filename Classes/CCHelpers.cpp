#include "CCHelpers.h"
#include "cocos2d.h"

namespace CCHelpers
{
	USING_NS_CC;

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

	void startDelay(cocos2d::Node* parent, float duration, const std::string& wait_node_name, int action_tag)
	{
		if (!parent)
			return;
		auto wait_action = cocos2d::DelayTime::create(duration);
		wait_action->setTag(action_tag);
		auto wait_node = cocos2d::Node::create();
		wait_node->runAction(wait_action);
		parent->addChild(wait_node, 2, wait_node_name);
	}

	bool isDelayOver(cocos2d::Node* parent, const std::string& wait_node_name, int action_tag)
	{
		if (!parent)
			return true;
		return !parent->getChildByName(wait_node_name)->getActionByTag(action_tag);
	}

	bool saveTransformedSpriteImageToFile(const std::string& src_filename,
		std::function<cocos2d::Sprite* (const std::string&)> transform_func, const std::string& dst_filename)
	{
		Sprite* dst_sprite = transform_func(src_filename);
		dst_sprite->setAnchorPoint(Vec2(0.5f, 0.5f));
		Size size = dst_sprite->getContentSize();
		dst_sprite->setPosition(size.width / 2, size.height / 2);
		RenderTexture* renderTexture = RenderTexture::create(static_cast<int>(size.width), static_cast<int>(size.height), backend::PixelFormat::RGBA8888);
		renderTexture->setKeepMatrix(false);
		renderTexture->begin();
		dst_sprite->visit();
		renderTexture->end();
		// will be saved by Cocos to platform dependent writable directory
		// for ex, for Win32 it will be Users/<CurrentUser>/AppData/Local/airhockey/
		return renderTexture->saveToFile(dst_filename, Image::Format::PNG);
	}
}