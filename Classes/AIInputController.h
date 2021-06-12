#pragma once
#include "PlayerInputController.h"

class AIInputController : public IPlayerInputController
{
public:
	AIInputController(const std::string& name, PaddlePtr my_paddle) :
		IPlayerInputController(name, my_paddle)
	{
		bindInputListeners();
	}
	AIInputController(AIInputController& other) :
		IPlayerInputController(other.m_name, other.m_myPaddle)
	{
		bindInputListeners();
	}

	~AIInputController() {}

	void bindInputListeners() override;

	void onMyPaddleMove();

};