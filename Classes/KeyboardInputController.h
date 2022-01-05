#ifndef __KEYBOARDINPUTCONTROLLER_H__
#define __KEYBOARDINPUTCONTROLLER_H__

#include "PlayerInputController.h"

namespace airhockey
{

	class KeyboardInputController : public IPlayerInputController
	{
	public:
		KeyboardInputController(const std::string& name, PaddlePtr my_paddle) :
			IPlayerInputController(name, my_paddle)
		{
			bindInputListeners();
		}
		KeyboardInputController(KeyboardInputController& other) :
			IPlayerInputController(other.m_name, other.m_myPaddle)
		{
			bindInputListeners();
		}

		~KeyboardInputController();

		void bindInputListeners() override;

		void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
		void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);

	private:
		cocos2d::EventListenerKeyboard* m_listener = nullptr;

	};

}
#endif  // __KEYBOARDINPUTCONTROLLER_H__