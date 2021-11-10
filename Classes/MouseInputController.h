#ifndef __MOUSEINPUTCONTROLLER_H__
#define __MOUSEINPUTCONTROLLER_H__

#include "PlayerInputController.h"

namespace airhockey
{
	class MouseInputController : public IPlayerInputController
	{
	public:
		MouseInputController(const std::string& name, PaddlePtr my_paddle, float touch_margin = 0.0f) :
			IPlayerInputController(name, my_paddle),
			m_touchMargin((touch_margin >= 0.0f) ? touch_margin : 0.0f)
		{
			bindInputListeners();
		}
		MouseInputController(MouseInputController& other) :
			IPlayerInputController(other.m_name, other.m_myPaddle),
			m_touchMargin(other.getTouchMargin())
		{
			bindInputListeners();
		}

		~MouseInputController() {}

		void bindInputListeners() override;

		bool isMouseWithinPaddle(cocos2d::EventMouse* em);
		void sendStopPaddleEvent();
		void onMouseDown(cocos2d::Event* event);
		void onMouseUp(cocos2d::Event* event);
		void onMouseMove(cocos2d::Event* event);
		void onMouseScroll(cocos2d::Event* event);

		void scheduleDebugOutput(cocos2d::Node* layer);

		void setTouchMargin(float touch_margin) { m_touchMargin = (touch_margin >= 0.0f) ? touch_margin : 0.0f; }
		float getTouchMargin() const { return m_touchMargin; }

	private:
		bool m_isMyPaddleGrabbed = false;
		cocos2d::Node* m_HUDLayer = nullptr;
		float m_touchMargin = 0.0f;  // addition to paddle radius when calculating is touch within paddle. Earned to ease grabbing paddle.

	};

}
#endif // __MOUSEINPUTCONTROLLER_H__