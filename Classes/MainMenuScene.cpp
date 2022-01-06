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

    std::string font_filename(airhockey::FONT_FILENAME_MENU);

    auto create_button = [=](const std::string& title_text, bool fit_to_title=false) -> ui::Button* {
        auto button = ui::Button::create("HD/ui/button_frame.png", "HD/ui/button_frame.png");
        button->setScale9Enabled(true);
        //button->setUnifySizeEnabled(true);
        //button->ignoreContentAdaptWithSize(true);
        Vec2 cap_inset_origin(25, 25);
        Size button_size = button->getContentSize();
        Rect cap_insets(cap_inset_origin, Size(button_size.width - 2 * cap_inset_origin.x, button_size.height - 2 * cap_inset_origin.y));
        button->setCapInsets(cap_insets);
        button->setTitleFontName(font_filename);
        button->setTitleFontSize(50);
        button->setTitleText(title_text);
        auto fit_button_to_title = [](ui::Button* button, float coeff = 1.1f) {
            auto lbl_size = button->getTitleRenderer()->getContentSize();
            button->setContentSize(Size(lbl_size.width * coeff, lbl_size.height * coeff));
        };
        if (fit_to_title) 
        { 
            fit_button_to_title(button); 
        }
        else 
        { 
            button->setContentSize(Size(550, 100)); 
        }
        return button;
    };

    auto button_start = create_button("1PLAYER");
    button_start->addClickEventListener([=](Ref* sender) { menuNewGameCallback(sender); });
    button_start->runAction(UIButtonMenu::defaultFocusedButtonActionCallback());

    auto button_settings = create_button("SETTINGS");
    //button_settings->addClickEventListener([=](Ref* sender) { menuNewGameCallback(sender); });

    auto button_quit = create_button("QUIT");
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
    auto text_title = ui::Text::create("MAIN MENU", font_filename, font_size);
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
