#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "GameLevel.h"

USING_NS_CC;

class GameScene : public Scene
{
public:
    GameScene(GameLevel& level);
    static Scene* createScene(GameLevel& level);

    virtual bool init();

    static GameScene* create(GameLevel& level);


    void onMouseDown(Event* event);
    void onMouseUp(Event* event);
    void onMouseMove(Event* event);
    void onMouseScroll(Event* event);

    void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
    void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

    void onGameMenuClose(Event* event);
    void onGameMenuOpen(Ref* sender);

    void onGameWin(Ref* sender);
    void onGameLoose(Ref* sender);

    void updateTimer(float dt);

    // calc up and right based on pressed and released keyboard keys
    void updateInputDirectionState();

    void update(float dt) override;

protected:
    GameLevel m_currLevel;

    bool m_upPressed = false;
    bool m_downPressed = false;
    bool m_rightPressed = false;
    bool m_leftPressed = false;
    int m_up = 0;    // 1 = up, -1 = down
    int m_right = 0; // 1 = right, -1 = left
    
    float m_timeElapsed = 0.0f;
};

#endif // __GAMESCENE_H__
