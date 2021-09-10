#ifndef __TOUCHINPUTCONTROLLER_H__
#define __TOUCHINPUTCONTROLLER_H__

#include "PlayerInputController.h"

namespace airhockey
{

	class TouchInputController : public IPlayerInputController
	{
	public:
		TouchInputController(const std::string& name, PaddlePtr my_paddle) :
			IPlayerInputController(name, my_paddle)
		{
			bindInputListeners();
		}
		TouchInputController(TouchInputController& other) :
			IPlayerInputController(other.m_name, other.m_myPaddle)
		{
			bindInputListeners();
		}

		~TouchInputController() {}

		void bindInputListeners() override;

		bool isTouchWithinPaddle(cocos2d::Touch* touch);
		void sendStopPaddleEvent();
		void onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event);
		void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event);
		void onTouchMoved(cocos2d::Touch* touch, cocos2d::Event* event);

		void scheduleDebugOutput(cocos2d::Node* layer);

	private:
		bool m_isMyPaddleGrabbed = false;
		cocos2d::Node* m_HUDLayer = nullptr;
	};

}
#endif  // __TOUCHINPUTCONTROLLER_H__