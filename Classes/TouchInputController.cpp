#include "TouchInputController.h"

USING_NS_CC;

void TouchInputController::bindInputListeners()
{
    auto touch_listener = EventListenerTouchOneByOne::create();
    touch_listener->onTouchBegan = [this](Touch* touch, Event* event) { onTouchBegan(touch, event); return true; };
    touch_listener->onTouchMoved = [this](Touch* touch, Event* event) { onTouchMoved(touch, event); };
    touch_listener->onTouchEnded = [this](Touch* touch, Event* event) { onTouchEnded(touch, event); };
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touch_listener, 1);
}

void TouchInputController::sendStopPaddleEvent()
{
    m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_LEFT));
    m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_RIGHT));
    m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_UP));
    m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_DOWN));
    m_myPaddle->setNormalizedVelocity(Vec2(0, 0));
}

bool TouchInputController::isTouchWithinPaddle(Touch* touch)
{
    // check that touch position is within paddle's circle
    Vec2 touch_pos = Director::getInstance()->convertToUI(touch->getLocation());
    return (touch_pos.getDistance(m_myPaddle->getPosition()) < m_myPaddle->getRadius());
}

void TouchInputController::onTouchBegan(cocos2d::Touch* touch, Event* event)
{
    if (isTouchWithinPaddle(touch))
    {
        m_isMyPaddleGrabbed = true;
        return;
    }
    m_isMyPaddleGrabbed = false;
    sendStopPaddleEvent();
}

void TouchInputController::onTouchEnded(cocos2d::Touch* touch, Event* event)
{
    m_isMyPaddleGrabbed = false;
    sendStopPaddleEvent();
}

void TouchInputController::onTouchMoved(cocos2d::Touch* touch, Event* event)
{
    if (!m_isMyPaddleGrabbed)
    {
        return;
    }

    Vec2 touch_pos = Director::getInstance()->convertToUI(touch->getLocation());
    m_myPaddle->setPosition(touch_pos);
}


void TouchInputController::scheduleDebugOutput(cocos2d::Node* layer)
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