#ifndef __MAINMENUSCENE_H__
#define __MAINMENUSCENE_H__

#include "cocos2d.h"

class MainMenuScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    void menuNewGameCallback(cocos2d::Ref* pSender);
    void menuCloseCallback(cocos2d::Ref* pSender);
    void onMainMenuSettingsOpen(Ref* sender);
    void onMainMenuSettingsClose(Ref* sender);

    // implement the "static create()" method manually
    CREATE_FUNC(MainMenuScene);


};

#endif // __MAINMENUSCENE_H__
