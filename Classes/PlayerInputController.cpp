#include "PlayerInputController.h"

USING_NS_CC;

IPlayerInputController::IPlayerInputController(const std::string& name, PaddlePtr paddle) :
	m_name(name),
	m_eventDispatcher(Director::getInstance()->getEventDispatcher()),
	m_myPaddle(paddle)
{
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_LEFT, [=](EventCustom* event) { m_myPaddle->onMovingLeft();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_RIGHT, [=](EventCustom* event) { m_myPaddle->onMovingRight();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_UP, [=](EventCustom* event) { m_myPaddle->onMovingUp();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_DOWN, [=](EventCustom* event) { m_myPaddle->onMovingDown();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_LEFT, [=](EventCustom* event) { m_myPaddle->onStopMovingLeft();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_RIGHT, [=](EventCustom* event) { m_myPaddle->onStopMovingRight();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_UP, [=](EventCustom* event) { m_myPaddle->onStopMovingUp();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_DOWN, [=](EventCustom* event) { m_myPaddle->onStopMovingDown();  });
}

IPlayerInputController::IPlayerInputController(IPlayerInputController& other) :
	m_name(other.m_name),
	m_eventDispatcher(Director::getInstance()->getEventDispatcher()),
	m_myPaddle(other.m_myPaddle)
{
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_LEFT, [=](EventCustom* event) { m_myPaddle->onMovingLeft();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_RIGHT, [=](EventCustom* event) { m_myPaddle->onMovingRight();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_UP, [=](EventCustom* event) { m_myPaddle->onMovingUp();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_DOWN, [=](EventCustom* event) { m_myPaddle->onMovingDown();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_LEFT, [=](EventCustom* event) { m_myPaddle->onStopMovingLeft();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_RIGHT, [=](EventCustom* event) { m_myPaddle->onStopMovingRight();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_UP, [=](EventCustom* event) { m_myPaddle->onStopMovingUp();  });
	bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_DOWN, [=](EventCustom* event) { m_myPaddle->onStopMovingDown();  });
}


std::string IPlayerInputController::makeCustomEventName(const std::string& input_action_event_name)
{
	return m_name + "__" + input_action_event_name;
}

void IPlayerInputController::bindActionEventListener(const std::string& input_action_event_name, const std::function<void(EventCustom*)>& callback)
{
	m_eventDispatcher->addCustomEventListener(makeCustomEventName(input_action_event_name), callback);
}