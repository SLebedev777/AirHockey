#include "GameMenuLayer.h"
#include "GameScene.h"
#include "ui/CocosGUI.h"
#include "UISettings.h"
#include "UIButtonMenu.h"

USING_NS_CC;


GameMenuLayer::GameMenuLayer(GameScene* from) : 
    game_scene(from)
{
}

GameMenuLayer* GameMenuLayer::create(GameScene* from)
{
    GameMenuLayer* pRet = new (std::nothrow) GameMenuLayer(from);
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        delete pRet;
        pRet = nullptr;
    }
    return pRet;
}

bool GameMenuLayer::init()
{
    this->initWithColor(Color4B(127, 0, 127, 100));

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto s = Director::getInstance()->getWinSize();
    Vec2 center = Vec2(s.width / 2, s.height / 2);

    auto button_resume = ui::Button::create("HD/ui/right.png", "HD/ui/right_pressed.png");
    button_resume->setScale(2.0f);
    button_resume->addClickEventListener([=](Ref* sender) { menuResumeCallback(sender); });
    button_resume->runAction(UIButtonMenu::defaultFocusedButtonActionCallback());

    auto button_toggle_audio = ui::CheckBox::create("HD/ui/audioOn.png", "HD/ui/cross.png");
    button_toggle_audio->setScale(1.5f);

    auto button_quit = ui::Button::create("HD/ui/home.png", "HD/ui/home_pressed.png");
    button_quit->setScale(1.5f);
    button_quit->addClickEventListener([=](Ref* sender) { menuBackToMainMenuCallback(sender); });

    ui::Layout* layout = ui::Layout::create();
    layout->setLayoutType(ui::Layout::Type::VERTICAL);
    layout->setContentSize(Size(400, 500));
    layout->setBackGroundColor(Color3B::MAGENTA);
    layout->setBackGroundColorOpacity(100);
    layout->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);

    ui::LinearLayoutParameter* linerP1 = ui::LinearLayoutParameter::create();
    linerP1->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL); //Center horizontally

    button_resume->setLayoutParameter(linerP1);
    button_toggle_audio->setLayoutParameter(linerP1);
    button_quit->setLayoutParameter(linerP1);

    layout->addChild(button_resume);
    layout->addChild(button_toggle_audio);
    layout->addChild(button_quit);

    layout->setAnchorPoint(Vec2(0.5, 0.5));
    layout->setPosition(center);

    this->addChild(layout, 1);

    return true;
}

void GameMenuLayer::menuResumeCallback(cocos2d::Ref* pSender)
{
    if (!game_scene)
    {
        return;
    }
    EventCustom custom_event("event_game_menu_close");
    _eventDispatcher->dispatchEvent(&custom_event);
}

void GameMenuLayer::menuBackToMainMenuCallback(cocos2d::Ref* pSender)
{
    Director::getInstance()->popScene();
}