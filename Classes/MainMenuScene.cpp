#include "MainMenuScene.h"
#include "GameScene.h"
#include "GameLevel.h"
#include "audio/include/AudioEngine.h"
#include "UISettings.h"
#include "UIButtonMenu.h"
#include "MainMenuSettingsLayer.h"
#include "audio/include/AudioEngine.h"
#include <cstdlib>



USING_NS_CC;

namespace
{
    enum MainMenuLayersTags
    {
        TAG_SETTINGS_LAYER = 1
    };

    enum MainMenuTags
    {
        LAYOUT_TAG = 11
    };


    Label* createMirror(Label* label, const std::string& font_name)
    {
        auto mirror = Label::createWithTTF(label->getString(), font_name, label->getRenderingFontSize());
        mirror->setScaleY(-1);
        
        Color4B color = label->getTextColor();
        Color4B color_delta = Color4B(80, 0, 80, 0);
        color.r = (color.r - color_delta.r > 0) ? color.r - color_delta.r : 0;
        color.g = (color.g - color_delta.g > 0) ? color.g - color_delta.g : 0;
        color.b = (color.b - color_delta.b > 0) ? color.b - color_delta.b : 0;
        color.a = (color.a - color_delta.a > 0) ? color.a - color_delta.a : 0;

        mirror->setTextColor(color);
        mirror->setOpacity(150);
        mirror->setClipMarginEnabled(true);
        float height_coeff = 0.2f;
        mirror->setPosition(label->getPosition() - Vec2(0, label->getBoundingBox().size.height * (1.0f - height_coeff)));
        return mirror;
    }

    int genRandomInRange(int min, int max)
    {
        int output = min + (rand() * (int)(max - min) / RAND_MAX);
        return output;
    }
}

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

    // dots imitating air hockey table surface
    DrawNode* dots = DrawNode::create();
    const int dot_step = 60;
    for (int y = dot_step / 2; y < s.height; y += dot_step)
    {
        float color_step = (float)y / s.height;
        for (int x = dot_step / 2; x < s.width; x += dot_step)
        {
            dots->drawDot(Vec2((float)x, (float)y), 3.0f, Color4F(0.1f, 0.1f, 1.0f - color_step, 1.0f));
        }
    }
    back_layer->addChild(dots, 1);

    // flying puck with motion streak
    m_puck = Sprite::create("HD/puck.png");
    m_puck->setPosition(Vec2(-1000, -1000));
    const float spawn_interval = 2.0f;
    const float streak_interval = 1.0f;
    m_streak = MotionStreak::create(streak_interval, 5, m_puck->getBoundingBox().size.width, Color3B::WHITE, "HD/streak.png");
    const std::string puck_name = "puck";
    back_layer->addChild(m_puck, 2, puck_name);
    const std::string streak_name = "puck_motion_streak";
    back_layer->addChild(m_streak, 1, streak_name);

    const float MARGIN = 100;
    const float WIDTH = 500;
    const Size horiz(s.width, WIDTH);
    const Size vert(WIDTH, s.height);
    std::vector<Rect> rects = {
        Rect(Vec2(-MARGIN - WIDTH, 0), vert),
        Rect(Vec2(0, s.height + MARGIN), horiz),
        Rect(Vec2(s.width + MARGIN, 0), vert),
        Rect(Vec2(0, -MARGIN - WIDTH), horiz)
    };
    schedule([this, s, streak_interval, rects = std::move(rects)](float) {
        int start_rect_index = rand() % rects.size();
        int end_rect_index = 0;
        while (start_rect_index == end_rect_index)
        {
            end_rect_index = rand() % rects.size();
        }
        Rect start_rect = rects[start_rect_index];
        Rect end_rect = rects[end_rect_index];

        float start_x = genRandomInRange(0, (int)start_rect.size.width) + start_rect.origin.x;
        float start_y = genRandomInRange(0, (int)start_rect.size.height) + start_rect.origin.y;
        float end_x = genRandomInRange(0, (int)end_rect.size.width) + end_rect.origin.x;
        float end_y = genRandomInRange(0, (int)end_rect.size.height) + end_rect.origin.y;

        auto seq = Sequence::create(
            DelayTime::create(streak_interval),
            Hide::create(),
            MoveTo::create(0, Vec2(start_x, start_y)),
            ScaleTo::create(0.0f, 1.0f / float(genRandomInRange(1, 8))),
            DelayTime::create(streak_interval),
            Show::create(),
            Spawn::createWithTwoActions(MoveTo::create(1.0f, Vec2(end_x, end_y)),  // TODO: parameterize motion interval
                                        ScaleTo::create(1.0f, 1)),
            nullptr);
        m_puck->runAction(seq);
        }, 
        spawn_interval + streak_interval,
        "puck_streak_schedule");

    schedule([this](float) {
        m_streak->setVisible(m_puck->isVisible());
        m_streak->setPosition(m_puck->getPosition());
        m_streak->setStroke(m_puck->getBoundingBox().size.width);
        }
    , "streak_move");

    auto interm_layer = LayerGradient::create(Color4B(0, 0, 150, 100), Color4B(0, 0, 0, 100));
    this->addChild(interm_layer, 1);

    // caption
    const std::string caption_font = "fonts/RetronoidItalic-ln9V.ttf";
    const float caption_font_size = 150;
    const float caption_y = center.y + 700;

    auto caption_left = Label::createWithTTF("AIR", caption_font, caption_font_size);
    caption_left->setClipMarginEnabled(true);
    caption_left->setTextColor(Color4B(255, 127, 0, 255));
    this->addChild(caption_left, 1, 666);

    auto caption_right = Label::createWithTTF("HOCKEY", caption_font, caption_font_size);
    caption_right->setAnchorPoint(Vec2(0.0, 0.5));
    caption_right->setClipMarginEnabled(true);
    caption_right->setTextColor(Color4B::YELLOW);
    this->addChild(caption_right, 1, 667);

    caption_left->setPosition(Vec2(-1000, caption_y));
    caption_right->setPosition(Vec2(2000, caption_y));

    auto caption_mirror_left = createMirror(caption_left, caption_font);
    auto caption_mirror_right = createMirror(caption_right, caption_font);
    float mirror_y = caption_mirror_left->getPosition().y;
    caption_mirror_left->setPosition(Vec2(-1000, mirror_y));
    caption_mirror_right->setPosition(Vec2(2000, mirror_y));
    caption_mirror_right->setAnchorPoint(Vec2(0.0, 0.5));
    this->addChild(caption_mirror_left, 1, 668);
    this->addChild(caption_mirror_right, 1, 669);

    // buttons
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
    layout->setPosition(center - Vec2(0, 100));

    this->addChild(layout, 2, MainMenuTags::LAYOUT_TAG);

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

void MainMenuScene::onEnterTransitionDidFinish()
{
    auto caption_left = this->getChildByTag(666);
    if (!caption_left)
        return;
    auto caption_right = this->getChildByTag(667);
    if (!caption_right)
        return;
    auto caption_mirror_left = this->getChildByTag(668);
    if (!caption_mirror_left)
        return;
    auto caption_mirror_right = this->getChildByTag(669);
    if (!caption_mirror_right)
        return;

    auto s = Director::getInstance()->getWinSize();
    Vec2 center = Vec2(s.width / 2, s.height / 2);

    const float caption_y = center.y + 700;
    float caption_x_start = 200;
    const float offset_x = caption_left->getBoundingBox().size.width - 70;
    const float ease_rate = 10;
    float mirror_y = caption_mirror_left->getPosition().y;

    auto seq_appear = [ease_rate](const Vec2& from, const Vec2& to) {
        auto seq = Sequence::create(MoveTo::create(0.0f, from),
            EaseOut::create(MoveTo::create(1.0f, to), ease_rate),
            nullptr);
        return seq;
    };

    caption_left->runAction(seq_appear(Vec2(-1000, caption_y), Vec2(caption_x_start, caption_y)));
    caption_right->runAction(seq_appear(Vec2(2000, caption_y), Vec2(caption_x_start + offset_x, caption_y)));

    caption_mirror_left->runAction(seq_appear(Vec2(-1000, mirror_y), Vec2(caption_x_start, mirror_y)));
    caption_mirror_right->runAction(seq_appear(Vec2(2000, mirror_y), Vec2(caption_x_start + offset_x, mirror_y)));

    // particle effect for caption
    m_fxEmitter = ParticleSystemQuad::create("vfx/Flower.plist");
    float fx_start_pos_y = caption_left->getPosition().y + 10;
    m_fxEmitter->setPosition(Vec2(-200, fx_start_pos_y));
    m_fxEmitter->runAction(Sequence::create(
        DelayTime::create(1.1f),
        MoveTo::create(2.0f, Vec2(s.width + 200, fx_start_pos_y)),
        DelayTime::create(5),
        RemoveSelf::create(),
        nullptr)
    );
    this->addChild(m_fxEmitter, 2, "fx_emitter");

}

void MainMenuScene::onExitTransitionDidStart()
{
    if (m_fxEmitter && getChildByName("fx_emitter"))
    {
        m_fxEmitter->runAction(RemoveSelf::create());
    }
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
    float level_width = design_resolution.width * 0.9f;
    float level_height = level_width * 16.0f / 9.0f;
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
        static_cast<MainMenuSettingsLayer*>(settings_layer)->updateToggleVFXButton();
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
