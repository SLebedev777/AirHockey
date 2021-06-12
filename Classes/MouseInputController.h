#pragma once
#include "PlayerInputController.h"

class MouseInputController : public IPlayerInputController
{
public:
	MouseInputController(const std::string& name, PaddlePtr my_paddle) :
		IPlayerInputController(name, my_paddle)
	{
		bindInputListeners();
	}
	MouseInputController(MouseInputController& other) :
		IPlayerInputController(other.m_name, other.m_myPaddle)
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

	void scheduleDebugOutput(cocos2d::Node* layer);

private:
	bool m_isMyPaddleGrabbed = false;
	cocos2d::Node* m_HUDLayer = nullptr;
};