#include "MouseInputController.h"

USING_NS_CC;

void MouseInputController::bindInputListeners()
{
    auto mouse_listener = EventListenerMouse::create();
    mouse_listener->onMouseMove = CC_CALLBACK_1(MouseInputController::onMouseMove, this);
    mouse_listener->onMouseUp = CC_CALLBACK_1(MouseInputController::onMouseUp, this);
    mouse_listener->onMouseDown = CC_CALLBACK_1(MouseInputController::onMouseDown, this);
    mouse_listener->onMouseScroll = CC_CALLBACK_1(MouseInputController::onMouseScroll, this);

    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(mouse_listener, 1);
}

void MouseInputController::sendStopPaddleEvent()
{
    m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_LEFT));
    m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_RIGHT));
    m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_UP));
    m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_DOWN));
}

void MouseInputController::onMouseDown(Event* event)
{
    EventMouse* em = static_cast<EventMouse*>(event);
    if (em->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        if (em->getDelta().getLength() > 5.0f)
        {
            m_isMouseButtonPressed = true;
        }
        else
        {
            sendStopPaddleEvent();
        }
    }
}

void MouseInputController::onMouseUp(Event* event)
{
    EventMouse* em = static_cast<EventMouse*>(event);
    if (em->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        m_isMouseButtonPressed = false;
        sendStopPaddleEvent();
    }
}

void MouseInputController::onMouseMove(Event* event)
{
    if (!m_isMouseButtonPressed)
        return;

    EventMouse* em = static_cast<EventMouse*>(event);

    Vec2 mouse_pos = Director::getInstance()->convertToUI(em->getLocation());
    // check that mouse cursor is within paddle's circle
    if (mouse_pos.getDistance(m_myPaddle->getPosition()) > m_myPaddle->getRadius())
        return;

    Vec2 delta = m_myPaddle->getPosition() - mouse_pos;
    if (delta.x < 0)
    {
        m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_MOVE_RIGHT));
    }
    if (delta.x > 0)
    {
        m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_MOVE_LEFT));
    }
    if (delta.y < 0)
    {
        m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_MOVE_UP));
    }
    if (delta.y > 0)
    {
        m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_MOVE_DOWN));
    }
}

void MouseInputController::onMouseScroll(Event* event)
{}