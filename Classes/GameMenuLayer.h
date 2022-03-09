#ifndef __GAMEMENULAYER_H__
#define __GAMEMENULAYER_H__

#include "cocos2d.h"
// use fwd decl to skip include GameScene.h in this header (include only in .cpp)
// we can do that because we use only GameScene*
class GameScene;

class GameMenuLayer : public cocos2d::LayerColor
{
public:
    static GameMenuLayer* create(GameScene* from);

    GameMenuLayer(GameScene* from);
    ~GameMenuLayer() {}

    bool init() override;

    void menuResumeCallback(cocos2d::Ref* pSender);
    void menuBackToMainMenuCallback(cocos2d::Ref* pSender);
    void toggleAudioCallback(cocos2d::Ref* pSender);
    void toggleVFXCallback(cocos2d::Ref* pSender);
    
    void updateToggleAudioButton();

private:
    GameScene* game_scene = nullptr;
};

#endif // __GAMEMENULAYER_H__
