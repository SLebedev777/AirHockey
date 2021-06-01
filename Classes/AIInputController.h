#pragma once
#include "PlayerInputController.h"
#include "Paddle.h"

class AIInputController : public IPlayerInputController
{
public:
	AIInputController(const std::string& name, PaddlePtr my_paddle) :
		IPlayerInputController(name),
		m_myPaddle(my_paddle)
	{
		bindInputListeners();
	}
	AIInputController(AIInputController& other) :
		IPlayerInputController(other.m_name),
		m_myPaddle(other.m_myPaddle)
	{
		bindInputListeners();
	}

	~AIInputController() {}

	void bindInputListeners() override;
	void onMyPaddleMove();

private:
	PaddlePtr m_myPaddle = nullptr;
};