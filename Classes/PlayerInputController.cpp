#include "PlayerInputController.h"

USING_NS_CC;

IPlayerInputController::IPlayerInputController(const std::string& name) :
	m_name(name),
	m_eventDispatcher(Director::getInstance()->getEventDispatcher())
{
}

IPlayerInputController::IPlayerInputController(IPlayerInputController& other) :
	m_name(other.m_name),
	m_eventDispatcher(Director::getInstance()->getEventDispatcher())
{
}


std::string IPlayerInputController::makeCustomEventName(const std::string& input_action_event_name)
{
	return m_name + "__" + input_action_event_name;
}

void IPlayerInputController::bindActionEventListener(const std::string& input_action_event_name, const std::function<void(EventCustom*)>& callback)
{
	m_eventDispatcher->addCustomEventListener(makeCustomEventName(input_action_event_name), callback);
}