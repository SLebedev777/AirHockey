#ifndef __TOUCHINPUTCONTROLLER_H__
#define __TOUCHINPUTCONTROLLER_H__

#include "PlayerInputController.h"

namespace airhockey
{

	class TouchInputController : public IPlayerInputController
	{
	public:
		TouchInputController(const std::string& name, PaddlePtr my_paddle, float touch_margin=0.0f,
					   const cocos2d::Vec2& touch_offset=cocos2d::Vec2::ZERO) :
			IPlayerInputController(name, my_paddle),
			m_touchMargin((touch_margin >= 0.0f) ? touch_margin : 0.0f),
			m_touchOffset(touch_offset)
		{
			bindInputListeners();
		}
		TouchInputController(TouchInputController& other) :
			IPlayerInputController(other.m_name, other.m_myPaddle),
			m_touchMargin(other.getTouchMargin()),
			m_touchOffset(other.getTouchOffset())
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

		void setTouchMargin(float touch_margin) { m_touchMargin = (touch_margin >= 0.0f) ? touch_margin : 0.0f; }
		float getTouchMargin() const { return m_touchMargin; }
		cocos2d::Vec2 getTouchOffset() const { return m_touchOffset; }

	private:
		bool m_isMyPaddleGrabbed = false;
		cocos2d::Node* m_HUDLayer = nullptr;
		float m_touchMargin = 0.0f;  // addition to paddle radius when calculating is touch within paddle. Earned to ease grabbing paddle.
		cocos2d::Vec2 m_touchOffset = cocos2d::Vec2::ZERO;
	};

}
#endif  // __TOUCHINPUTCONTROLLER_H__