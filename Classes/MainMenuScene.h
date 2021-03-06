#ifndef __MAINMENUSCENE_H__
#define __MAINMENUSCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <random>

class MainMenuScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    bool init() override;

    void onEnterTransitionDidFinish() override;
    void onExitTransitionDidStart() override;
    void menuNewGameCallback(cocos2d::Ref* pSender);
    void menuCloseCallback(cocos2d::Ref* pSender);
    void onMainMenuSettingsOpen(Ref* sender);
    void onMainMenuSettingsClose(Ref* sender);
    
    cocos2d::ui::Layout* getMenuLayout();

    // implement the "static create()" method manually
    CREATE_FUNC(MainMenuScene);

private:
    cocos2d::Sprite* m_puck = nullptr;
    cocos2d::MotionStreak* m_streak = nullptr;
    cocos2d::ParticleSystemQuad* m_fxEmitter = nullptr;
    std::mt19937 m_gen;
    std::uniform_int_distribution<> m_distr;

};

#endif // __MAINMENUSCENE_H__
