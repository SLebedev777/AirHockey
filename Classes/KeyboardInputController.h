#pragma once
#include "PlayerInputController.h"

class KeyboardInputController : public IPlayerInputController
{
public:
	KeyboardInputController(const std::string& name) :
		IPlayerInputController(name)
	{
		bindInputListeners();
	}
	KeyboardInputController(KeyboardInputController& other) :
		IPlayerInputController(other.m_name)
	{
		bindInputListeners();
	}

	~KeyboardInputController() {}

	void bindInputListeners() override;

	void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
	void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

};