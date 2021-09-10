#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "GameLevel.h"
#include "Paddle.h"
#include "PlayerInputController.h"
#include "GameField.h"

USING_NS_CC;

class GameScene : public Scene
{
public:
    GameScene(airhockey::GameLevel& level);
    static Scene* createScene(airhockey::GameLevel& level);

    virtual bool init();

    static GameScene* create(airhockey::GameLevel& level);


    void onMouseDown(Event* event);
    void onMouseUp(Event* event);
    void onMouseMove(Event* event);
    void onMouseScroll(Event* event);

    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

    void onGameMenuClose(Event* event);
    void onGameMenuOpen(Ref* sender);

    void onGameEndMenuOpen(Ref* sender);
    void onGameEndMenuClose(Event* event);

    void updateTimer(float dt);
    void drawHUDString(int str_tag, const std::string& str);

    void update(float dt) override;

protected:
    airhockey::GameLevel m_currLevel;

    airhockey::GameFieldPtr m_field;

    airhockey::PaddlePtr m_paddle1, m_paddle2;
    cocos2d::Sprite* m_puck;
 
    airhockey::IPlayerInputControllerPtr m_keyboardController = nullptr;
    airhockey::IPlayerInputControllerPtr m_mouseController = nullptr;
    airhockey::IPlayerInputControllerPtr m_mouseController2 = nullptr;
    airhockey::IPlayerInputControllerPtr m_AIController = nullptr;
    airhockey::IPlayerInputControllerPtr m_touchController = nullptr;

    bool m_upPressed = false;
    bool m_downPressed = false;
    bool m_rightPressed = false;
    bool m_leftPressed = false;
    int m_up = 0;    // 1 = up, -1 = down
    int m_right = 0; // 1 = right, -1 = left
    
    float m_timeElapsed = 0.0f;

    uint32_t m_score1 = 0, m_score2 = 0;
};

#endif // __GAMESCENE_H__
