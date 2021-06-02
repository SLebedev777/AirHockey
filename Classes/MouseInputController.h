#pragma once
#include "PlayerInputController.h"
#include "Paddle.h"

class MouseInputController : public IPlayerInputController
{
public:
	MouseInputController(const std::string& name, PaddlePtr my_paddle) :
		IPlayerInputController(name),
		m_myPaddle(my_paddle)
	{
		bindInputListeners();
	}
	MouseInputController(MouseInputController& other) :
		IPlayerInputController(other.m_name),
		m_myPaddle(other.m_myPaddle)
	{
		bindInputListeners();
	}

	~MouseInputController() {}

	void bindInputListeners() override;

	bool isMouseWithinPaddle(EventMouse* em);
	void sendStopPaddleEvent();
	void onMouseDown(cocos2d::Event* event);
	void onMouseUp(cocos2d::Event* event);
	void onMouseMove(cocos2d::Event* event);
	void onMouseScroll(cocos2d::Event* event);

private:
	PaddlePtr m_myPaddle = nullptr;
	bool m_isMyPaddleGrabbed = false;
};