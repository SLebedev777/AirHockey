#include "MainMenuScene.h"
#include "GameScene.h"
#include "GameLevel.h"
#include "audio/include/AudioEngine.h"
#include "ui/CocosGUI.h"
#include "UISettings.h"
#include "UIButtonMenu.h"

USING_NS_CC;

Scene* MainMenuScene::createScene()
{
    return MainMenuScene::create();
}


// on "init" you need to initialize your instance
bool MainMenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto s = Director::getInstance()->getWinSize();
    Vec2 center = Vec2(s.width / 2, s.height / 2);

    auto button_start = ui::Button::create("HD/ui/right.png", "HD/ui/right_pressed.png");
    button_start->setScale(2.0f);
    button_start->addClickEventListener([=](Ref* sender) { menuNewGameCallback(sender); });
    button_start->runAction(UIButtonMenu::defaultFocusedButtonActionCallback());

    auto button_quit = ui::Button::create("HD/ui/home.png", "HD/ui/home_pressed.png");
    button_quit->setScale(1.5f);
    button_quit->addClickEventListener([=](Ref* sender) { menuCloseCallback(sender); });

    ui::Layout* layout = ui::Layout::create();
    layout->setLayoutType(ui::Layout::Type::VERTICAL);
    layout->setContentSize(Size(400, 600));
    layout->setBackGroundColor(Color3B(50, 0, 50), Color3B(20, 0, 20));
    layout->setBackGroundColorOpacity(220);
    layout->setBackGroundColorType(ui::Layout::BackGroundColorType::GRADIENT);

    ui::LinearLayoutParameter* liner = ui::LinearLayoutParameter::create();
    liner->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL); //Center horizontally

    std::string font_filename("fonts/Abduction.ttf");
    const float font_size = 64;
    auto text_title = ui::Text::create("MAIN MENU", font_filename, font_size);
    ui::LinearLayoutParameter* liner_top = ui::LinearLayoutParameter::create();
    liner_top->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL); //Center horizontally
    liner_top->setMargin(ui::Margin(0, 25, 0, 20));
    text_title->setLayoutParameter(liner_top);

    button_start->setLayoutParameter(liner);
    button_quit->setLayoutParameter(liner);

    Color3B ui_color_secondary = Color3B(230, 220, 250);
    Color3B ui_color_primary = Color3B(190, 255, 255);
    button_start->setColor(ui_color_primary);
    button_quit->setColor(ui_color_secondary);

    layout->addChild(text_title);
    layout->addChild(button_start);
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


void MainMenuScene::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();
    AudioEngine::end();
}


void MainMenuScene::menuNewGameCallback(cocos2d::Ref* pSender)
{
    Size design_resolution = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    float level_width = design_resolution.width * 0.9;
    float level_height = level_width * 16 / 9;
    airhockey::GameLevel level = airhockey::GameLevel(0, level_width, level_height);

    Director::getInstance()->pushScene(GameScene::create(level));
}
