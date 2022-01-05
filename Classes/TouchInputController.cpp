#include "TouchInputController.h"

namespace airhockey
{

    USING_NS_CC;

    void TouchInputController::bindInputListeners()
    {
        m_listener = EventListenerTouchOneByOne::create();
        m_listener->setSwallowTouches(true);
        m_listener->onTouchBegan = [this](Touch* touch, Event* event) { onTouchBegan(touch, event); return true; };
        m_listener->onTouchMoved = [this](Touch* touch, Event* event) { onTouchMoved(touch, event); };
        m_listener->onTouchEnded = [this](Touch* touch, Event* event) { onTouchEnded(touch, event); };

        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_listener, 1);
    }

    TouchInputController::~TouchInputController()
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(m_listener);
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
        Vec2 touch_pos = Director::getInstance()->convertToGL(touch->getLocationInView());
        Vec2 paddle_pos = m_myPaddle->getPosition();
        return (touch_pos.getDistance(paddle_pos) < m_myPaddle->getRadius() + m_touchMargin);
    }

    void TouchInputController::onTouchBegan(cocos2d::Touch* touch, Event* event)
    {
        m_isMyPaddleGrabbed = true;
        Vec2 touch_pos = Director::getInstance()->convertToGL(touch->getLocationInView());
        touch_pos += m_touchOffset;
        m_myPaddle->setPosition(touch_pos);
    }

    void TouchInputController::onTouchEnded(cocos2d::Touch* touch, Event* event)
    {
        m_isMyPaddleGrabbed = false;
        sendStopPaddleEvent();
    }

    void TouchInputController::onTouchMoved(cocos2d::Touch* touch, Event* event)
    {
        Vec2 touch_pos = Director::getInstance()->convertToGL(touch->getLocationInView());
        touch_pos += m_touchOffset;
        m_myPaddle->setPositionImmideately(touch_pos);
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

}