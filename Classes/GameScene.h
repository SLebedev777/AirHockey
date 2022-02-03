#ifndef __GAMESCENE_H__
#define __GAMESCENE_H__

#include "cocos2d.h"
#include "GameLevel.h"
#include "Paddle.h"
#include "PlayerInputController.h"
#include "GameField.h"
#include "FSMContext.h"
#include "IFSMState.h"
#include "DebugLogger.h"

USING_NS_CC;


class GameScene : public Scene
{
public:
    GameScene(airhockey::GameLevel& level);
    static Scene* createScene(airhockey::GameLevel& level);

    virtual bool init();

    static GameScene* create(airhockey::GameLevel& level);

    bool onContactBegin(PhysicsContact& contact);
    bool onContactSeparateVFX(PhysicsContact& contact);

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

    void rethrowPuck(cocos2d::Vec2& puck_throw_pos);
    void onNewGameStart();

    void updateTimer(float dt);
    void drawHUDString(int str_tag, const std::string& str);
    void onScoreChanged();

    void update(float dt) override;

    static void resetTotalScore() { m_totalGames = 0; m_totalScore1 = 0; m_totalScore2 = 0; }

private:
    void startDelay(float duration, const std::string& wait_node_name = std::string("WaitNode"), int action_tag = 12345);
    bool isDelayOver(const std::string& wait_node_name = std::string("WaitNode"), int action_tag = 12345);

protected:
    enum class GoalHitBy
    {
        NONE = 0,
        PLAYER1,
        PLAYER2
    };

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
    cocos2d::Vec2 m_labelScore1StartPos, m_labelScore2StartPos;
    bool m_isPuckPlayable = true;  // false during period from a goal occured till puck is set at the edge of central circle and ready to continue game
    GoalHitBy m_goalHitBy = GoalHitBy::NONE;
    const int MAX_SCORE = 7;

    static uint32_t m_totalScore1, m_totalScore2, m_totalGames;  // overall score of games played, until player exits to main menu

    airhockey::FSMContextPtr m_AI = nullptr;
    airhockey::IFSMStatePtr m_AIIdleState = nullptr;

    DebugLoggerPtr m_logger = nullptr;
};


#endif // __GAMESCENE_H__
