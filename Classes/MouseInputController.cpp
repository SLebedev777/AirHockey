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
    m_myPaddle->setNormalizedVelocity(Vec2(0, 0));
}

bool MouseInputController::isMouseWithinPaddle(EventMouse* em)
{
    // check that mouse cursor is within paddle's circle
    Vec2 mouse_pos = Director::getInstance()->convertToUI(em->getLocation());
    return (mouse_pos.getDistance(m_myPaddle->getPosition()) < m_myPaddle->getRadius());
}

void MouseInputController::onMouseDown(Event* event)
{
    EventMouse* em = static_cast<EventMouse*>(event);
    if (em->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT && isMouseWithinPaddle(em))
    {
        m_isMyPaddleGrabbed = true;
        return;
    }
    m_isMyPaddleGrabbed = false;
    sendStopPaddleEvent();
}

void MouseInputController::onMouseUp(Event* event)
{
    EventMouse* em = static_cast<EventMouse*>(event);
    if (em->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT)
    {
        m_isMyPaddleGrabbed = false;
        sendStopPaddleEvent();
    }
}

void MouseInputController::onMouseMove(Event* event)
{
    if (!m_isMyPaddleGrabbed)
    {
        return;
    }

    EventMouse* em = static_cast<EventMouse*>(event);
    Vec2 mouse_pos = Director::getInstance()->convertToUI(em->getLocation());
    m_myPaddle->setPosition(mouse_pos);
}

void MouseInputController::onMouseScroll(Event* event)
{}

void MouseInputController::scheduleDebugOutput(cocos2d::Node* layer)
{
    const std::string font_path = "fonts/arial.ttf";
    Label* label_is_my_paddle_grabbed = Label::createWithTTF("is_my_paddle_grabbed: ", font_path, 24);
    layer->addChild(label_is_my_paddle_grabbed, 1, "label_is_my_paddle_grabbed");
    label_is_my_paddle_grabbed->setAnchorPoint(Vec2(0, 0));
    label_is_my_paddle_grabbed->setPosition(Vec2(50, 500));
    layer->schedule([=](float dt) {
        label_is_my_paddle_grabbed->setString("is_my_paddle_grabbed: " + std::to_string(int(m_isMyPaddleGrabbed)));
        }, "callback_is_my_paddle_grabbed");
}