#ifndef __GAMEMENULAYER_H__
#define __GAMEMENULAYER_H__

#include "cocos2d.h"

class MainMenuSettingsLayer : public cocos2d::LayerColor
{
public:
    static MainMenuSettingsLayer* create();

    MainMenuSettingsLayer() {}
    ~MainMenuSettingsLayer() {}

    bool init() override;

    // back to main menu scene callback
    void menuBackToMainMenuCallback(cocos2d::Ref* pSender);
    void toggleAudioCallback(cocos2d::Ref* sender);
    void updateToggleAudioButton();
    void toggleVFXCallback(cocos2d::Ref* sender);
    void updateToggleVFXButton();

};

#endif // __GAMEMENULAYER_H__
