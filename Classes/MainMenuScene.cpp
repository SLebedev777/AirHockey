#include "MainMenuScene.h"
#include "GameScene.h"
#include "GameLevel.h"
#include "audio/include/AudioEngine.h"
#include "UISettings.h"
#include "UIButtonMenu.h"
#include "MainMenuSettingsLayer.h"
#include "audio/include/AudioEngine.h"


USING_NS_CC;

static enum MainMenuLayersTags
{
    TAG_SETTINGS_LAYER = 1
};

static enum MainMenuTags
{
    LAYOUT_TAG = 11
};

Scene* MainMenuScene::createScene()
{
    return MainMenuScene::create();
}


bool MainMenuScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    auto s = Director::getInstance()->getWinSize();
    Vec2 center = Vec2(s.width / 2, s.height / 2);

    auto back_layer = LayerGradient::create(Color4B::BLUE, Color4B::BLACK);
    this->addChild(back_layer);

    auto button_start = airhockey::createUIButton("PLAY");
    button_start->addClickEventListener([=](Ref* sender) { menuNewGameCallback(sender); });
    button_start->runAction(UIButtonMenu::defaultFocusedButtonActionCallback());

    auto button_settings = airhockey::createUIButton("SETTINGS");
    button_settings->addClickEventListener([=](Ref* sender) { onMainMenuSettingsOpen(sender); });

    auto button_quit = airhockey::createUIButton("QUIT");
    button_quit->addClickEventListener([=](Ref* sender) { menuCloseCallback(sender); });

    ui::Layout* layout = ui::Layout::create();
    layout->setLayoutType(ui::Layout::Type::VERTICAL);
    layout->setContentSize(Size(600, 600));
    layout->setBackGroundColor(Color3B(50, 0, 50), Color3B(20, 0, 20));
    layout->setBackGroundColorOpacity(170);
    layout->setBackGroundColorType(ui::Layout::BackGroundColorType::GRADIENT);

    ui::LinearLayoutParameter* liner = ui::LinearLayoutParameter::create();
    liner->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL); //Center horizontally
    liner->setMargin(ui::Margin(0, 25, 0, 20));

    const float font_size = 64;
    auto text_title = ui::Text::create("MAIN MENU", airhockey::FONT_FILENAME_MENU, font_size);
    ui::LinearLayoutParameter* liner_top = ui::LinearLayoutParameter::create();
    liner_top->setGravity(ui::LinearLayoutParameter::LinearGravity::CENTER_HORIZONTAL); //Center horizontally
    liner_top->setMargin(ui::Margin(0, 25, 0, 20));
    text_title->setLayoutParameter(liner_top);

    button_start->setLayoutParameter(liner);
    button_settings->setLayoutParameter(liner);
    button_quit->setLayoutParameter(liner);

    Color3B ui_color_secondary = Color3B(230, 220, 250);
    Color3B ui_color_primary = Color3B(190, 255, 255);
    Color3B ui_color_yellow = Color3B(200, 200, 50);
    Color3B ui_color_red = Color3B(200, 50, 50);
    Color3B ui_color_purple = Color3B(127, 50, 127);

    button_start->getRendererNormal()->setColor(ui_color_red);
    button_start->setTitleColor(Color3B(255, 170, 170));

    button_settings->getRendererNormal()->setColor(ui_color_yellow);
    button_settings->setTitleColor(Color3B(255, 200, 170));

    button_quit->getRendererNormal()->setColor(ui_color_purple);
    button_quit->setTitleColor(Color3B(255, 170, 255));

    layout->addChild(text_title);
    layout->addChild(button_start);
    layout->addChild(button_settings);
    layout->addChild(button_quit);

    layout->setAnchorPoint(Vec2(0.5, 0.5));
    layout->setPosition(center);

    this->addChild(layout, 1, MainMenuTags::LAYOUT_TAG);

    auto settings_layer = MainMenuSettingsLayer::create();
    this->addChild(settings_layer, 255, MainMenuLayersTags::TAG_SETTINGS_LAYER);
    settings_layer->setVisible(false);
    //onMainMenuSettingsClose(nullptr);

    auto _main_menu_settings_layer_close_listener = EventListenerCustom::create("event_main_menu_settings_layer_close", [=](EventCustom* event) {
        onMainMenuSettingsClose(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_main_menu_settings_layer_close_listener, this);

    return true;
}

ui::Layout* MainMenuScene::getMenuLayout()
{
    return static_cast<ui::Layout*>(this->getChildByTag(MainMenuTags::LAYOUT_TAG));
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

void MainMenuScene::onMainMenuSettingsOpen(Ref* sender)
{
    auto settings_layer = this->getChildByTag(MainMenuLayersTags::TAG_SETTINGS_LAYER);
    if (settings_layer)
    {
        getMenuLayout()->setVisible(false);

        settings_layer->setVisible(true);
        settings_layer->resume();
        static_cast<MainMenuSettingsLayer*>(settings_layer)->updateToggleAudioButton();
    }
}

void MainMenuScene::onMainMenuSettingsClose(Ref* sender)
{
    auto settings_layer = this->getChildByTag(MainMenuLayersTags::TAG_SETTINGS_LAYER);
    if (settings_layer)
    {
        settings_layer->setVisible(false);
        settings_layer->pause();

        getMenuLayout()->setVisible(true);
    }
}
