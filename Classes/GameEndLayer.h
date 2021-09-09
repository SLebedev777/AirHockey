#ifndef __GAMEENDMENULAYER_H__
#define __GAMEENDMENULAYER_H__

#include "cocos2d.h"
// use fwd decl to skip include GameScene.h in this header (include only in .cpp)
// we can do that because we use only GameScene*
class GameScene;

class GameEndMenuLayer : public cocos2d::LayerColor
{
public:
    static GameEndMenuLayer* create(GameScene* from);

    GameEndMenuLayer(GameScene* from);
    ~GameEndMenuLayer() {}

    bool init() override;

    // play one more game
    void menuContinueCallback(cocos2d::Ref* pSender);
    // back to main menu
    void menuBackToMainMenuCallback(cocos2d::Ref* pSender);

private:
    GameScene* game_scene = nullptr;
};

#endif // __GAMEENDMENULAYER_H__
