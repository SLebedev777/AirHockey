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
    layout->setContentSize(Size(400, 600));
    layout->setBackGroundColor(Color3B(50, 0, 50), Color3B(20, 0, 20));
    layout->setBackGroundColorOpacity(220);
    layout->setBackGroundColorType(ui::Layout::BackGroundColorType::GRADIENT);

    ui::LinearLayoutParameter* liner = ui::LinearLayoutParameter::create();
    liner->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL); //Center horizontally

    std::string score_font_filename("fonts/Abduction.ttf");
    const float score_font_size = 64;
    auto text_title = ui::Text::create("PAUSED", score_font_filename, score_font_size);
    ui::LinearLayoutParameter* liner_top = ui::LinearLayoutParameter::create();
    liner_top->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL); //Center horizontally
    liner_top->setMargin(ui::Margin(0, 25, 0, 20));
    text_title->setLayoutParameter(liner_top);

    button_resume->setLayoutParameter(liner);
    button_toggle_audio->setLayoutParameter(liner);
    button_quit->setLayoutParameter(liner);

    Color3B ui_color_secondary = Color3B(230, 220, 250);
    Color3B ui_color_primary = Color3B(190, 255, 255);
    button_resume->setColor(ui_color_primary);
    button_toggle_audio->setColor(ui_color_secondary);
    button_quit->setColor(ui_color_secondary);

    layout->addChild(text_title);
    layout->addChild(button_resume);
    layout->addChild(button_toggle_audio);
    layout->addChild(button_quit);

    Rect layout_rect = layout->getBoundingBox();
    DrawNode* border = DrawNode::create();
    Color4F border_color = Color4F(ui_color_primary); // Color4F(Color3B(140, 90, 240));
    border->setLineWidth(5);
    border->drawRect(layout_rect.origin, layout_rect.origin + layout_rect.size, border_color);
    layout->addChild(border, 1);

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