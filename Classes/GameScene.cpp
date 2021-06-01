#include "GameScene.h"
#include "KeyboardInputController.h"
#include "AIInputController.h"
#include "MouseInputController.h"


USING_NS_CC;

enum LayersTags
{
    TAG_GAME_LAYER = 1,
    TAG_HUD_LAYER,
    TAG_HUD_CONTROL_LAYER,
    TAG_GAME_MENU_LAYER
};

enum GameLayerTags
{
    TAG_GAME_LAYER_FIELD_BORDER_RECT = 1
};

enum HUDLayerTags
{
};

GameScene::GameScene(GameLevel& level) :
    m_currLevel(level)
{}

GameScene* GameScene::create(GameLevel& level)
{
    GameScene* pRet = new(std::nothrow) GameScene(level);
    if (pRet && pRet->init()) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

Scene* GameScene::createScene(GameLevel& level)
{
    return GameScene::create(level);
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in GameScene.cpp\n");
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Vec2 frameCenter = Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);

    //////////////////////////////////////////////////
    // GAME LAYER
    //
    auto game_layer = Layer::create();
 
    auto background = cocos2d::LayerGradient::create(cocos2d::Color4B::BLUE, cocos2d::Color4B::ORANGE);
    game_layer->addChild(background, 1);

    Rect GAMEFIELDRECT = Rect(frameCenter.x - m_currLevel.m_width / 2, 
                              frameCenter.y - m_currLevel.m_height / 2, 
                              m_currLevel.m_width, m_currLevel.m_height);
    auto field_draw_border = DrawNode::create(5);
    field_draw_border->drawRect(GAMEFIELDRECT.origin, GAMEFIELDRECT.origin + GAMEFIELDRECT.size, Color4F::RED);
    game_layer->addChild(field_draw_border, 1, TAG_GAME_LAYER_FIELD_BORDER_RECT);

    m_paddle1 = std::make_shared<Paddle>("paddle.png", frameCenter.x, GAMEFIELDRECT.getMinY() + 150, 10, 10, 100);
    game_layer->addChild(m_paddle1->getSprite(), 1);

    //m_keyboardController = std::make_unique<KeyboardInputController>("KB");
    //m_paddle1->attachInputController(std::move(m_keyboardController));

    m_mouseController = std::make_unique<MouseInputController>("MOUSE", m_paddle1);
    m_paddle1->attachInputController(std::move(m_mouseController));

    m_paddle2 = std::make_shared<Paddle>("paddle.png", frameCenter.x, GAMEFIELDRECT.getMaxY() - 150, 2, 2, 100);
    game_layer->addChild(m_paddle2->getSprite(), 1);

    m_AIController = std::make_unique<AIInputController>("AI", m_paddle2);
    m_paddle2->attachInputController(std::move(m_AIController));


    //////////////////////////////////////////////////
    // HUD LAYER
    // non-interactive elements over main game process
    
    auto hud_layer = LayerColor::create(Color4B(0, 0, 0, 0));


    //////////////////////////////////////////////////
    // HUD CONTROL LAYER
    // interactive elements over main game process

    auto hud_control_layer = LayerColor::create(Color4B(0, 0, 0, 0));
    
    //////////////////////////////////////////////////

    // construct game scene from layers
    this->addChild(game_layer, 1, TAG_GAME_LAYER);
    this->addChild(hud_layer, 2, TAG_HUD_LAYER);
    this->addChild(hud_control_layer, 3, TAG_HUD_CONTROL_LAYER);

    //////////////////////////////////////////////////

    // add input event listeners
    auto _mouseListener = EventListenerMouse::create();
    _mouseListener->onMouseMove = CC_CALLBACK_1(GameScene::onMouseMove, this);
    _mouseListener->onMouseUp = CC_CALLBACK_1(GameScene::onMouseUp, this);
    _mouseListener->onMouseDown = CC_CALLBACK_1(GameScene::onMouseDown, this);
    _mouseListener->onMouseScroll = CC_CALLBACK_1(GameScene::onMouseScroll, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, game_layer);

    auto _keyboard_listener = EventListenerKeyboard::create();
    _keyboard_listener->onKeyPressed = CC_CALLBACK_2(GameScene::onKeyPressed, this);
    _keyboard_listener->onKeyReleased = CC_CALLBACK_2(GameScene::onKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboard_listener, this);

    scheduleUpdate();

    return true;
}

void GameScene::onMouseDown(Event* event)
{
}

void GameScene::onMouseUp(Event* event)
{
}

void GameScene::onMouseMove(Event* event)
{
}

void GameScene::onMouseScroll(Event* event)
{
}

void GameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        if (this->getChildByTag(TAG_GAME_MENU_LAYER))
        {
            onGameMenuClose(event);
        }
        else
        {
            onGameMenuOpen(nullptr);
        }
        return;
    }
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
}

void GameScene::onGameMenuOpen(Ref* sender)
{
}

void GameScene::onGameMenuClose(Event* event)
{
}


void GameScene::onGameWin(Ref* sender)
{
}

void GameScene::onGameLoose(Ref* sender)
{
}

void GameScene::updateTimer(float dt)
{
    m_timeElapsed += dt;
}


void GameScene::update(float dt)
{
    // GAME LOGIC HERE
    m_paddle1->move();
    m_paddle2->move();
}