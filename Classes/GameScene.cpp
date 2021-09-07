#include "GameScene.h"
#include "KeyboardInputController.h"
#include "AIInputController.h"
#include "MouseInputController.h"
#include "TouchInputController.h"


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
    if (!Scene::initWithPhysics())
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
    /*
    auto field_draw_border = DrawNode::create(5);
    field_draw_border->drawRect(GAMEFIELDRECT.origin, GAMEFIELDRECT.origin + GAMEFIELDRECT.size, Color4F::RED);
    game_layer->addChild(field_draw_border, 1, TAG_GAME_LAYER_FIELD_BORDER_RECT);
    // add table boundaries to physics world to make ball bounce from it
    Node* field_node = Node::create();
    field_node->setPosition(frameCenter);
    PhysicsBody* field_box = PhysicsBody::createEdgeBox(GAMEFIELDRECT.size, PhysicsMaterial(0.1f, 1.0f, 0.0f));
    field_box->setDynamic(false);
    field_node->addComponent(field_box);
    game_layer->addChild(field_node, 1);
    */
    
    GameFieldBuilder builder = GameFieldBuilder();
    // building steps
    builder.addPlayRect(GAMEFIELDRECT);
    
    const int SIDE_WIDTH = 40;
    const int SIDE_HEIGHT = 40;

    float gap = 10;
    GameFieldSidePtr left_long_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::UP);
    Size long_part_size = Size(SIDE_WIDTH, GAMEFIELDRECT.size.height / 2 - 2*gap); 
    left_long_side->addSidePart(std::make_unique<GameFieldSidePart>(long_part_size, Color4F::GREEN), gap);
    left_long_side->addSidePart(std::make_unique<GameFieldSidePart>(long_part_size, Color4F::RED), 2*gap);
    builder.addSide(std::move(left_long_side));

    GameFieldSidePtr upper_short_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::RIGHT);
    Size short_part_size = Size(GAMEFIELDRECT.size.width / 3 - 2*gap, SIDE_HEIGHT);
    upper_short_side->addSidePart(std::make_unique<GameFieldSidePart>(short_part_size, Color4F::YELLOW), gap);
    upper_short_side->addSidePart(std::make_unique<GameFieldSidePart>(short_part_size, Color4F::WHITE), short_part_size.width + 4*gap);
    builder.addSide(std::move(upper_short_side));

    GameFieldSidePtr right_long_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::DOWN);
    right_long_side->addSidePart(std::make_unique<GameFieldSidePart>(long_part_size, Color4F::GREEN), gap);
    right_long_side->addSidePart(std::make_unique<GameFieldSidePart>(long_part_size, Color4F::RED), 2*gap);
    builder.addSide(std::move(right_long_side));

    GameFieldSidePtr lower_short_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::LEFT);
    lower_short_side->addSidePart(std::make_unique<GameFieldSidePart>(short_part_size, Color4F::YELLOW), gap);
    lower_short_side->addSidePart(std::make_unique<GameFieldSidePart>(short_part_size, Color4F::WHITE), short_part_size.width + 4*gap);
    builder.addSide(std::move(lower_short_side));
    
    Size corner_size = Size(SIDE_WIDTH*2, SIDE_WIDTH*2);
    builder.addCorner(std::make_unique<GameFieldSidePart>(corner_size, Color4F::BLACK), GameField::GameFieldPlayRectCornerType::BOTTOM_LEFT);
    builder.addCorner(std::make_unique<GameFieldSidePart>(corner_size, Color4F::WHITE), GameField::GameFieldPlayRectCornerType::TOP_LEFT);
    builder.addCorner(std::make_unique<GameFieldSidePart>(corner_size, Color4F::BLACK), GameField::GameFieldPlayRectCornerType::TOP_RIGHT);
    builder.addCorner(std::make_unique<GameFieldSidePart>(corner_size, Color4F::WHITE), GameField::GameFieldPlayRectCornerType::BOTTOM_RIGHT);
    
    builder.addGoalGate(std::make_unique<GoalGate>(short_part_size, GoalGateLocationType::LOWER));
    builder.addGoalGate(std::make_unique<GoalGate>(short_part_size, GoalGateLocationType::UPPER));

    builder.addCentralCircleMarking(CentralCircleMarkingSettings(100, 3, Color4F::MAGENTA, Color4F::GRAY));
    //
    m_field = builder.getResult();
    m_field->setParent(game_layer);
    

    // human (lower)
    Rect PLAYER1_FIELDRECT = Rect(frameCenter.x - m_currLevel.m_width / 2,
        frameCenter.y - m_currLevel.m_height / 2,
        m_currLevel.m_width, m_currLevel.m_height / 2);
    auto field_draw_border1 = DrawNode::create(5);
    field_draw_border1->drawRect(PLAYER1_FIELDRECT.origin, PLAYER1_FIELDRECT.origin + PLAYER1_FIELDRECT.size, Color4F::GREEN);
    game_layer->addChild(field_draw_border1, 1);

    // AI (upper)
    Rect PLAYER2_FIELDRECT = Rect(frameCenter.x - m_currLevel.m_width / 2,
        frameCenter.y,
        m_currLevel.m_width, m_currLevel.m_height / 2);
    auto field_draw_border2 = DrawNode::create(5);
    field_draw_border2->drawRect(PLAYER2_FIELDRECT.origin, PLAYER2_FIELDRECT.origin + PLAYER2_FIELDRECT.size, Color4F::BLACK);
    game_layer->addChild(field_draw_border2, 1);

    _physicsWorld->setGravity(Vec2::ZERO);

    // PUCK
    m_puck = Sprite::create("puck.png");
    PhysicsMaterial puck_material = PhysicsMaterial(0.1f, 1.0f, 0.2f);
    m_puck->addComponent(PhysicsBody::createCircle(m_puck->getBoundingBox().size.width / 2, puck_material));
    m_puck->setPosition(frameCenter.x, frameCenter.y - GAMEFIELDRECT.size.height / 4);
    game_layer->addChild(m_puck, 1);

    m_paddle1 = std::make_shared<Paddle>("paddle.png", frameCenter.x, GAMEFIELDRECT.getMinY() + 150, 1000, 1000, 50, PLAYER1_FIELDRECT, game_layer,
        this->getPhysicsWorld());

    m_keyboardController = std::make_shared<KeyboardInputController>("KB", m_paddle1);

    m_touchController = std::make_shared<TouchInputController>("TOUCH", m_paddle1);

    m_paddle2 = std::make_shared<Paddle>("paddle.png", frameCenter.x, GAMEFIELDRECT.getMaxY() - 150, 100, 100, 50, PLAYER2_FIELDRECT, game_layer,
        this->getPhysicsWorld());

    m_AIController = std::make_shared<AIInputController>("AI", m_paddle2);

    //////////////////////////////////////////////////
    // HUD LAYER
    // non-interactive elements over main game process
    
    auto hud_layer = LayerColor::create(Color4B(0, 0, 0, 0));
    m_touchController->scheduleDebugOutput(hud_layer);

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
    m_paddle1->move(dt);
    m_paddle2->move(dt);

}