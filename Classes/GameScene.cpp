#include "GameScene.h"
#include "KeyboardInputController.h"
#include "AIInputController.h"
#include "MouseInputController.h"
#include "TouchInputController.h"
#include "GameMenuLayer.h"
#include "GameEndLayer.h"
#include "AIIdleState.h"
#include "AINullState.h"
#include "AIPlayer.h"
#include "CCHelpers.h"
#include "ui/CocosGUI.h"
#include "GlobalSettings.h"
#include "Physics.h"
#include <memory>
#include <chrono>
#include "audio/include/AudioEngine.h"
#include "Sound.h"
#include "GameScene_VFX.h"

USING_NS_CC;

uint32_t GameScene::m_totalScore1 = 0;
uint32_t GameScene::m_totalScore2 = 0;
uint32_t GameScene::m_totalGames = 0;

enum LayersTags
{
    TAG_GAME_LAYER = 1,
    TAG_HUD_LAYER,
    TAG_HUD_CONTROL_LAYER,
    TAG_GAME_MENU_LAYER,
    TAG_GAME_END_MENU_LAYER
};

enum GameLayerTags
{
    TAG_GAME_LAYER_FIELD_BORDER_RECT = 1
};

enum HUDLayerTags
{
    TAG_HUD_LAYER_SCORE1_STRING = 1,
    TAG_HUD_LAYER_SCORE2_STRING = 2,
    TAG_HUD_LAYER_TOTAL_SCORE1_STRING = 3,
    TAG_HUD_LAYER_TOTAL_SCORE2_STRING = 4
};

GameScene::GameScene(airhockey::GameLevel& level) :
    m_currLevel(level)
{}

GameScene* GameScene::create(airhockey::GameLevel& level)
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

Scene* GameScene::createScene(airhockey::GameLevel& level)
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

    using namespace airhockey;

    m_logger = std::make_shared<DebugLogger>("log.txt");
    m_logger->log("start");

    //////////////////////////////////////////////////
    // GAME LAYER
    //
    auto game_layer = Layer::create();
 
    Rect GAMEFIELDRECT = Rect(frameCenter.x - m_currLevel.m_width / 2, 
                              frameCenter.y - m_currLevel.m_height / 2, 
                              m_currLevel.m_width, m_currLevel.m_height);

    auto field_background_layer = cocos2d::LayerGradient::create(cocos2d::Color4B(70, 70, 150, 255), cocos2d::Color4B(50, 50, 170, 255));
    field_background_layer->setContentSize(GAMEFIELDRECT.size);

    GameFieldBuilder builder = GameFieldBuilder();

    // building steps
    builder.addPlayRect(GAMEFIELDRECT, field_background_layer);
    
    const int SIDE_WIDTH = 20;
    const int SIDE_HEIGHT = 20;
    const float GAP = 0;
    const float PUCK_RADIUS = 50;
    const Size GOAL_GATE_SIZE = Size(8 * PUCK_RADIUS, 1000);  // do gate height large enough to catch puck even on high speed intersecting gate
    const Size LONG_PART_SIZE = Size(SIDE_WIDTH, GAMEFIELDRECT.size.height / 2 - 2 * GAP);
    const Size SHORT_PART_SIZE = Size((GAMEFIELDRECT.size.width - GOAL_GATE_SIZE.width)/2 - GAP, SIDE_HEIGHT);

    Color4F SIDE_COLOR = Color4F(0.5, 0.6, 0.15, 1.0);
    GameFieldSidePtr left_long_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::UP);
    left_long_side->addSidePart(std::make_unique<GameFieldSidePart>(LONG_PART_SIZE, SIDE_COLOR), GAP);
    left_long_side->addSidePart(std::make_unique<GameFieldSidePart>(LONG_PART_SIZE, SIDE_COLOR), 2 * GAP);
    builder.addSide(std::move(left_long_side));

    GameFieldSidePtr upper_short_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::RIGHT);
    upper_short_side->addSidePart(std::make_unique<GameFieldSidePart>(SHORT_PART_SIZE, SIDE_COLOR), GAP);
    upper_short_side->addSidePart(std::make_unique<GameFieldSidePart>(SHORT_PART_SIZE, SIDE_COLOR), GOAL_GATE_SIZE.width);
    builder.addSide(std::move(upper_short_side));

    GameFieldSidePtr right_long_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::DOWN);
    right_long_side->addSidePart(std::make_unique<GameFieldSidePart>(LONG_PART_SIZE, SIDE_COLOR), GAP);
    right_long_side->addSidePart(std::make_unique<GameFieldSidePart>(LONG_PART_SIZE, SIDE_COLOR), 2 * GAP);
    builder.addSide(std::move(right_long_side));

    GameFieldSidePtr lower_short_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::LEFT);
    lower_short_side->addSidePart(std::make_unique<GameFieldSidePart>(SHORT_PART_SIZE, SIDE_COLOR), GAP);
    lower_short_side->addSidePart(std::make_unique<GameFieldSidePart>(SHORT_PART_SIZE, SIDE_COLOR), GOAL_GATE_SIZE.width);
    builder.addSide(std::move(lower_short_side));
    
    const Size CORNER_SIZE = Size(SIDE_WIDTH, SIDE_WIDTH);
    Color4F CORNER_COLOR = SIDE_COLOR;
    builder.addCorner(std::make_unique<GameFieldSidePart>(CORNER_SIZE, CORNER_COLOR), GameField::GameFieldPlayRectCornerType::BOTTOM_LEFT);
    builder.addCorner(std::make_unique<GameFieldSidePart>(CORNER_SIZE, CORNER_COLOR), GameField::GameFieldPlayRectCornerType::TOP_LEFT);
    builder.addCorner(std::make_unique<GameFieldSidePart>(CORNER_SIZE, CORNER_COLOR), GameField::GameFieldPlayRectCornerType::TOP_RIGHT);
    builder.addCorner(std::make_unique<GameFieldSidePart>(CORNER_SIZE, CORNER_COLOR), GameField::GameFieldPlayRectCornerType::BOTTOM_RIGHT);
    
    builder.addGoalGate(std::make_unique<GoalGate>(GOAL_GATE_SIZE, GoalGateLocationType::LOWER));
    builder.addGoalGate(std::make_unique<GoalGate>(GOAL_GATE_SIZE, GoalGateLocationType::UPPER));

    builder.addCentralCircleMarking(CentralCircleMarkingSettings(GOAL_GATE_SIZE.width / 2, 3, Color4F(0, 0, 0, 0), Color4F::GRAY));
    builder.addCentralLine(Color4F::GRAY, 3);

    builder.addGoalGateMarking(GoalGateMarkingSettings(GOAL_GATE_SIZE.width, 3, Color4F::MAGENTA, Color4F::GRAY, 
        GoalGateMarkingSettings::GoalGateMarkingShapeType::RECTANGLE), Vec2::ZERO);
    
    //
    m_field = builder.getResult();
    m_field->setParent(game_layer);

    // PUCK
    m_puck = Sprite::create("puck.png");
    PhysicsMaterial puck_material = PhysicsMaterial(0.1f, 1.0f, 0.2f);
    auto puck_body = PhysicsBody::createCircle(m_puck->getBoundingBox().size.width / 2, puck_material);
    puck_body->setLinearDamping(0.15f); // simulate friction when puck glides on table surface, to prevent endless motion
    puck_body->setAngularDamping(0.2f);
    puck_body->setVelocityLimit(3500);
    puck_body->setName("puck_body");
    puck_body->setCategoryBitmask(airhockey::Physics::CollisionCategoryBitMask::CCBM_PUCK);
    puck_body->setContactTestBitmask(airhockey::Physics::contactTestBitmask_Puck);
    m_puck->addComponent(puck_body);
    game_layer->addChild(m_puck, 1);

    const float PADDLE_RADIUS = 2 * PUCK_RADIUS;

    // human (lower)
    Rect PLAYER1_FIELDRECT = Rect(GAMEFIELDRECT.getMinX(), GAMEFIELDRECT.getMinY(), GAMEFIELDRECT.size.width, PADDLE_RADIUS + GAMEFIELDRECT.size.height / 2);

    // AI (upper)
    Rect PLAYER2_FIELDRECT = Rect(GAMEFIELDRECT.getMinX(), GAMEFIELDRECT.getMidY() - PADDLE_RADIUS, GAMEFIELDRECT.size.width, PADDLE_RADIUS + GAMEFIELDRECT.size.height / 2);

    _physicsWorld->setGravity(Vec2::ZERO);
    _physicsWorld->setSpeed(1.0f);
    _physicsWorld->setUpdateRate(0.5f);

    const float PADDLE_START_Y_CENTER_OFFSET = GOAL_GATE_SIZE.width / 2;

    m_paddle1 = std::make_shared<Paddle>("paddle_gimp.png", m_field->getCenter().x, m_field->getPlayRect().getMinY() + PADDLE_START_Y_CENTER_OFFSET, 1000, 1000, PADDLE_RADIUS,
        PLAYER1_FIELDRECT, game_layer, this->getPhysicsWorld());
    m_paddle1->getSprite()->setColor(Color3B::RED);
    m_paddle1->getPhysicsBody()->setCategoryBitmask(airhockey::Physics::CollisionCategoryBitMask::CCBM_PADDLE1);
    m_paddle1->getPhysicsBody()->setContactTestBitmask(airhockey::Physics::contactTestBitmask_Paddle1);

    assert(m_paddle1->getSprite()->getBoundingBox().size.width == 2 * PADDLE_RADIUS);

    //m_keyboardController = std::make_shared<KeyboardInputController>("KB", m_paddle1);
    m_touchController = std::make_shared<TouchInputController>("TOUCH", m_paddle1, /*touch_margin*/PUCK_RADIUS,
                                                               /*touch_offset*/Vec2(0, PADDLE_RADIUS));

    m_paddle2 = std::make_shared<Paddle>("paddle_gimp.png", m_field->getCenter().x, m_field->getPlayRect().getMaxY() - PADDLE_START_Y_CENTER_OFFSET, 1000, 1000, PADDLE_RADIUS,
        PLAYER2_FIELDRECT, game_layer, this->getPhysicsWorld());
    m_paddle2->getSprite()->setColor(Color3B(120, 220, 100));
    m_paddle2->getPhysicsBody()->setCategoryBitmask(airhockey::Physics::CollisionCategoryBitMask::CCBM_PADDLE2);
    m_paddle2->getPhysicsBody()->setContactTestBitmask(airhockey::Physics::contactTestBitmask_Paddle2);

    //m_touchController = std::make_shared<TouchInputController>("TOUCH", m_paddle2);

    //adds contact event listener
    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onContactBegin, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    auto contactListenerVFX = EventListenerPhysicsContact::create();
    contactListenerVFX->onContactSeparate = CC_CALLBACK_1(GameScene::onContactSeparateVFX, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListenerVFX, this);


    const float ATTACK_RADIUS = abs(m_paddle2->getStartPosition().y - m_field->getCenter().y) + PUCK_RADIUS;
    
    Rect ai_gate_rect = m_field->getGoalGate(GoalGateLocationType::UPPER).getRect();
    Vec2 ai_pyramid_left(m_field->getCenter().x - 0.5 * GOAL_GATE_SIZE.width, ai_gate_rect.getMinY() - 1.5*PADDLE_RADIUS);
    Vec2 ai_pyramid_right(ai_pyramid_left + Vec2(GOAL_GATE_SIZE.width, 0));
    Vec2 ai_pyramid_top(m_field->getCenter().x, ai_pyramid_left.y - PADDLE_RADIUS);
    Pyramid pyramid(ai_pyramid_top, ai_pyramid_left, ai_pyramid_right);

    auto constant_attack_radius_func = [=](const Vec2&) { return ATTACK_RADIUS; };

    const float MAX_ATTACK_RADIUS = ATTACK_RADIUS;
    const float MIN_ATTACK_RADIUS = 4 * PADDLE_RADIUS;
    const float PUCK_VEL_FOR_MAX_ATTACK_RADIUS = 70;
    const float PUCK_VEL_FOR_MIN_ATTACK_RADIUS = 1200;
    const float PUCK_VEL_TANGENT_COEFF = 2;

    auto linear_attack_radius_func = [=](const Vec2& puck_vel) {
        float vel_scalar = puck_vel.length();

        if (puck_vel.y <= 0.0f)
        {
            return MAX_ATTACK_RADIUS;
        }

        if (vel_scalar <= PUCK_VEL_FOR_MAX_ATTACK_RADIUS)
        {
            return MAX_ATTACK_RADIUS;
        }
        else if (vel_scalar > PUCK_VEL_FOR_MIN_ATTACK_RADIUS || puck_vel.y < puck_vel.x * PUCK_VEL_TANGENT_COEFF)
        {
            return MIN_ATTACK_RADIUS;
        }
        else
        {
            float PUCK_VEL_RANGE = PUCK_VEL_FOR_MIN_ATTACK_RADIUS - PUCK_VEL_FOR_MAX_ATTACK_RADIUS;
            float ATTACK_RADIUS_RANGE = MAX_ATTACK_RADIUS - MIN_ATTACK_RADIUS;
            return MAX_ATTACK_RADIUS - ATTACK_RADIUS_RANGE * (vel_scalar - PUCK_VEL_FOR_MAX_ATTACK_RADIUS) / PUCK_VEL_RANGE;
        }
        
    };

    AIPlayerSettings ai_settings(pyramid, ATTACK_RADIUS, linear_attack_radius_func);
    m_AI = std::make_shared<AIPlayer>(m_field.get(), m_paddle2, m_paddle1, m_puck, ai_settings);
    m_AIIdleState = static_cast<AIPlayer*>(m_AI.get())->createIdleState();
    m_AI->setLogger(m_logger);
    m_AI->getLogger()->log("test log from AI");
    m_AI->pushState(std::move(m_AIIdleState));
    
    //////////////////////////////////////////////////
    // HUD LAYER
    // non-interactive elements over main game process
    
    auto hud_layer = LayerColor::create(Color4B(0, 0, 0, 0));

    std::string score_font_filename("fonts/Abduction.ttf");
    const float score_font_size = 72;
    auto label_score1 = Label::createWithTTF("0", score_font_filename, score_font_size);
    label_score1->setClipMarginEnabled(true);
    label_score1->setRotation(90.f);
    m_labelScore1StartPos = frameCenter - Vec2(0, 2*PUCK_RADIUS);
    label_score1->setPosition(m_labelScore1StartPos);
    hud_layer->addChild(label_score1, 1, TAG_HUD_LAYER_SCORE1_STRING);

    auto label_score2 = Label::createWithTTF("0", score_font_filename, score_font_size);
    label_score1->setClipMarginEnabled(true);
    label_score2->setRotation(90.f);
    m_labelScore2StartPos = frameCenter + Vec2(0, 2*PUCK_RADIUS);
    label_score2->setPosition(m_labelScore2StartPos);
    hud_layer->addChild(label_score2, 1, TAG_HUD_LAYER_SCORE2_STRING);

    auto label_total_score1 = Label::createWithTTF("0", score_font_filename, score_font_size * 0.7);
    label_total_score1->setTextColor(Color4B::RED);
    label_total_score1->setClipMarginEnabled(true);
    label_total_score1->setRotation(90.f);
    label_total_score1->setPosition(frameCenter + Vec2(m_field->getPlayRect().size.width / 2 - 90, -2 * PUCK_RADIUS));
    hud_layer->addChild(label_total_score1, 1, TAG_HUD_LAYER_TOTAL_SCORE1_STRING);

    auto label_total_score2 = Label::createWithTTF("0", score_font_filename, score_font_size * 0.7);
    label_total_score2->setTextColor(Color4B::GREEN);
    label_total_score2->setClipMarginEnabled(true);
    label_total_score2->setRotation(90.f);
    label_total_score2->setPosition(frameCenter + Vec2(m_field->getPlayRect().size.width / 2 - 90, 2 * PUCK_RADIUS));
    hud_layer->addChild(label_total_score2, 1, TAG_HUD_LAYER_TOTAL_SCORE2_STRING);

    //////////////////////////////////////////////////
    // HUD CONTROL LAYER
    // interactive elements over main game process

    auto hud_control_layer = LayerColor::create(Color4B(0, 0, 0, 0));
 
    auto button_pause = ui::Button::create("pause.png", "pause_pressed.png");
    button_pause->setPosition(m_field->getCenter() + Vec2(m_field->getPlayRect().size.width/2 - button_pause->getBoundingBox().size.width*0.75, 0));
    button_pause->addClickEventListener([=](Ref* sender) {
        onGameMenuOpen(sender);
        });
    hud_control_layer->addChild(button_pause);

    //////////////////////////////////////////////////

    // construct game scene from layers
    this->addChild(game_layer, 1, TAG_GAME_LAYER);
    this->addChild(hud_layer, 2, TAG_HUD_LAYER);
    this->addChild(hud_control_layer, 3, TAG_HUD_CONTROL_LAYER);

    auto game_menu_layer = GameMenuLayer::create(this);
    this->addChild(game_menu_layer, 255, TAG_GAME_MENU_LAYER);
    onGameMenuClose(nullptr);

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

    auto _game_menu_close_listener = EventListenerCustom::create("event_game_menu_close", [=](EventCustom* event) {
        onGameMenuClose(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_game_menu_close_listener, this);

    auto _game_end_menu_close_listener = EventListenerCustom::create("event_game_end_menu_close", [=](EventCustom* event) {
        onGameEndMenuClose(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_game_end_menu_close_listener, this);


    scheduleUpdate();

    onNewGameStart();

    return true;
}

bool GameScene::onContactBegin(PhysicsContact& contact)
{
    using namespace airhockey::Physics;
    using namespace airhockey::Sound;

    static std::chrono::time_point<std::chrono::system_clock> last_time = std::chrono::system_clock::now();

    std::chrono::duration<double> diff = std::chrono::system_clock::now() - last_time;
    double elapsed_time = diff.count();
    if (elapsed_time <= 0.1)
    {
        return true;
    }

    PhysicsBody* a = contact.getShapeA()->getBody();
    PhysicsBody* b = contact.getShapeB()->getBody();
    Vec2 delta_vel = a->getVelocity() - b->getVelocity();
    float d_vel_scalar = delta_vel.lengthSquared();
    const float MIN_DVEL = 40000.0f;
    const float MAX_DVEL = 1000000.0f;
    const float MIN_VOLUME = 0.01f;
    const float MAX_VOLUME = 0.3f;
    float volume = 0.0f;
    if (d_vel_scalar <= MIN_DVEL)
    {
        volume = MIN_VOLUME;
    }
    else if (d_vel_scalar > MAX_DVEL)
    {
        volume = MAX_VOLUME;
    }
    else
    {
        volume = MIN_VOLUME + (d_vel_scalar - MIN_DVEL) * (MAX_VOLUME - MIN_VOLUME) / (MAX_DVEL - MIN_DVEL);
    }

    int a_cat = a->getCategoryBitmask();
    int b_cat = b->getCategoryBitmask();
    switch (a_cat | b_cat)
    {
    case CCBM_GAME_FIELD | CCBM_PUCK:
        AudioEngine::play2d(AUDIOFILE_COLLIDE_PUCK_WALLS, false, volume);
        break;
    case CCBM_PADDLE1 | CCBM_PUCK:
    case CCBM_PADDLE2 | CCBM_PUCK:
        AudioEngine::play2d(AUDIOFILE_COLLIDE_PUCK_PADDLE, false, volume);
        break;
    default: break;
    }

    last_time = std::chrono::system_clock::now();

    return true;
}

bool GameScene::onContactSeparateVFX(PhysicsContact& contact)
{
    if (!airhockey::GlobalSettings::isVFXEnabled)
        return true;

    using namespace airhockey::Physics;
    using namespace airhockey::VFX;

    static std::chrono::time_point<std::chrono::system_clock> last_time = std::chrono::system_clock::now();

    std::chrono::duration<double> diff = std::chrono::system_clock::now() - last_time;
    double elapsed_time = diff.count();
    if (elapsed_time <= 0.1)
    {
        return true;
    }

    PhysicsBody* a = contact.getShapeA()->getBody();
    PhysicsBody* b = contact.getShapeB()->getBody();

    int a_cat = a->getCategoryBitmask();
    int b_cat = b->getCategoryBitmask();

    cocos2d::ParticleSystemQuad* emitter = nullptr;

    switch (a_cat | b_cat)
    {
    case CCBM_GAME_FIELD | CCBM_PUCK:
        emitter = getParticleVFXPuckCollideWalls();
        break;
    case CCBM_PADDLE1 | CCBM_PUCK:
        emitter = getParticleVFXPuckCollidePaddle(Color4F(m_paddle1->getSprite()->getColor()));
        break;
    case CCBM_PADDLE2 | CCBM_PUCK:
        emitter = getParticleVFXPuckCollidePaddle(Color4F(m_paddle2->getSprite()->getColor()));
        break;
    default: break;
    }

    PhysicsBody* puck_body = (a_cat == CCBM_PUCK) ? a : ((b_cat == CCBM_PUCK) ? b : nullptr);

    if (emitter && puck_body)
    {
        float angle = 180 + CC_RADIANS_TO_DEGREES(puck_body->getVelocity().getAngle());
        emitter->setAngle(angle);
        emitter->setPosition(contact.getContactData()->points[0]);
        this->getChildByTag(TAG_GAME_LAYER)->addChild(emitter, 100);
    }

    last_time = std::chrono::system_clock::now();

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
        if (auto layer = this->getChildByTag(TAG_GAME_END_MENU_LAYER))
        {
            static_cast<GameEndMenuLayer*>(layer)->menuBackToMainMenuCallback(nullptr);
        }
        else if (auto game_menu_layer = this->getChildByTag(TAG_GAME_MENU_LAYER))
        {
            if (game_menu_layer->isVisible())
            {
                onGameMenuClose(event);
            }
            else
            {
                onGameMenuOpen(nullptr);
            }
        }
    }
}

void GameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
}

void GameScene::onGameMenuOpen(Ref* sender)
{
    if (this->getChildByTag(TAG_GAME_END_MENU_LAYER))
        return;

    m_logger->log("GameScene::onGameMenuOpen() enter");

    auto game_menu_layer = this->getChildByTag(TAG_GAME_MENU_LAYER);
    if (game_menu_layer)
    {
        Director::getInstance()->getScheduler()->pauseTarget(this);
        this->getPhysicsWorld()->setAutoStep(false);

        game_menu_layer->setVisible(true);
        game_menu_layer->resume();
        static_cast<GameMenuLayer*>(game_menu_layer)->updateToggleAudioButton();
    }

}

void GameScene::onGameMenuClose(Event* event)
{
    m_logger->log("GameScene::onGameMenuClose() enter");

    auto game_menu_layer = this->getChildByTag(TAG_GAME_MENU_LAYER);
    if (game_menu_layer)
    {
        Director::getInstance()->getScheduler()->resumeTarget(this);
        this->getPhysicsWorld()->setAutoStep(true);

        game_menu_layer->setVisible(false);
        game_menu_layer->pause();
    }

}


void GameScene::onGameEndMenuOpen(Ref* sender)
{
    if (this->getChildByTag(TAG_GAME_END_MENU_LAYER))
        return;

    if (this->getChildByTag(TAG_GAME_MENU_LAYER))
        onGameMenuClose(nullptr);

    m_logger->log("GameScene::onGameEndMenuOpen() enter");

    Director::getInstance()->getScheduler()->pauseTarget(this);
    this->getPhysicsWorld()->setAutoStep(false);

    using namespace airhockey::Sound;
    m_score1 > m_score2 ? AudioEngine::play2d(AUDIOFILE_WIN, false, 0.4f) : AudioEngine::play2d(AUDIOFILE_LOSE, false, 0.4f);

    auto label_result = Label::createWithTTF(m_score1 > m_score2 ? "YOU WIN!" : "YOU LOSE", "fonts/Abduction.ttf", 128);
    label_result->setTextColor(m_score1 > m_score2 ? Color4B::GREEN : Color4B::RED);
    auto game_end_menu_layer = GameEndMenuLayer::create(this, label_result);
    this->addChild(game_end_menu_layer, 255, TAG_GAME_END_MENU_LAYER);
}

void GameScene::onGameEndMenuClose(Event* event)
{
    m_logger->log("GameScene::onGameEndMenuClose() enter");

    Director::getInstance()->getScheduler()->resumeTarget(this);
    this->getPhysicsWorld()->setAutoStep(true);

    auto game_end_menu_layer = this->getChildByTag(TAG_GAME_END_MENU_LAYER);
    if (game_end_menu_layer)
    {
        this->removeChildByTag(TAG_GAME_END_MENU_LAYER);
    }

    onNewGameStart();
}

void GameScene::rethrowPuck(cocos2d::Vec2& puck_rethrow_pos)
{
    m_logger->log("GameScene::rethrowPuck()");

    m_isPuckPlayable = false;

    auto puck_body = static_cast<PhysicsBody*>(m_puck->getComponent("puck_body"));

    puck_body->setVelocity(Vec2::ZERO);
    puck_body->setAngularVelocity(0.0f);
    puck_body->setEnabled(false);

    auto puck_restart_action = [](cocos2d::Vec2& puck_rethrow_pos) {
        auto hide = cocos2d::Hide::create();
        auto move_to_zero = cocos2d::MoveTo::create(0.0f, Vec2::ZERO);
        auto delay = cocos2d::DelayTime::create(3.0f);
        auto move_to_circle = cocos2d::MoveTo::create(0.0f, puck_rethrow_pos);
        auto rotate_to_zero = cocos2d::RotateTo::create(0.0f, 0.0f);
        auto show = cocos2d::Show::create();

        auto seq = cocos2d::Sequence::create(hide, move_to_zero, delay, move_to_circle, rotate_to_zero, show, nullptr);
        return seq;
    };
    m_puck->runAction(puck_restart_action(puck_rethrow_pos));

    auto puck_clone = Sprite::create("puck.png");
    puck_clone->setPosition(Vec2(-1000, -1000));
    this->getChildByTag(TAG_GAME_LAYER)->addChild(puck_clone, 1);
    auto puck_throw_down_effect_action = [](cocos2d::Vec2& puck_rethrow_pos) {
        auto hide = cocos2d::Hide::create();
        auto delay = cocos2d::DelayTime::create(2.0f);
        auto move_to_circle = cocos2d::MoveTo::create(0.0f, puck_rethrow_pos);
        auto scale_up = cocos2d::ScaleTo::create(0.0, 5);
        auto show = cocos2d::Show::create();
        auto scale_down = cocos2d::ScaleTo::create(1.0, 1);
        auto remove = cocos2d::RemoveSelf::create();

        auto seq = cocos2d::Sequence::create(hide, delay, move_to_circle, scale_up, show, scale_down, remove, nullptr);
        return seq;
    };
    puck_clone->runAction(puck_throw_down_effect_action(puck_rethrow_pos));

}

void GameScene::onNewGameStart()
{
    m_logger->log("GameScene::onNewGameStart() enter");

    m_score1 = 0;
    m_score2 = 0;

    if (m_AI)
    {
        m_AI->reset();
        m_AI->setEnabled(false);
    }

    startDelay(3.0f);

    Vec2 puck_rethrow_pos = this->m_field->getCenter();
    rethrowPuck(puck_rethrow_pos);

    onScoreChanged();

    Vec2 p1_pos = m_paddle1->getStartPosition();
    m_logger->log("GameScene::onNewGameStart(): setting paddle1 position immideately to " + CCHelpers::Vec2Str(p1_pos));
    m_paddle1->setPositionImmideately(p1_pos);

    Vec2 p2_pos = m_paddle2->getStartPosition();
    m_logger->log("GameScene::onNewGameStart(): setting paddle2 position immideately to " + CCHelpers::Vec2Str(p2_pos));
    m_paddle2->setPositionImmideately(p2_pos);

}

void GameScene::startDelay(float duration, const std::string& wait_node_name, int action_tag)
{
    m_logger->log("GameScene:: delay started");
    CCHelpers::startDelay(this, duration, wait_node_name, action_tag);
}

bool GameScene::isDelayOver(const std::string& wait_node_name, int action_tag)
{
    bool result = CCHelpers::isDelayOver(this, wait_node_name, action_tag);
    result ? m_logger->log("GameScene:: delay is over") : m_logger->log("GameScene:: delay is NOT over");
    return result;
}

void GameScene::updateTimer(float dt)
{
    m_timeElapsed += dt;
}

void GameScene::drawHUDString(int str_tag, const std::string& str)
{
    auto hud_layer = this->getChildByTag(TAG_HUD_LAYER);
    auto label_node = hud_layer->getChildByTag(str_tag);
    auto label = static_cast<cocos2d::Label*> (label_node);
    label->setString(str);

}

void GameScene::onScoreChanged()
{
    auto hud_layer = this->getChildByTag(TAG_HUD_LAYER);
    auto label_score1 = static_cast<cocos2d::Label*> (hud_layer->getChildByTag(TAG_HUD_LAYER_SCORE1_STRING));
    auto label_score2 = static_cast<cocos2d::Label*> (hud_layer->getChildByTag(TAG_HUD_LAYER_SCORE2_STRING));

    label_score1->stopAllActions();
    label_score2->stopAllActions();
    label_score1->setPosition(m_labelScore1StartPos);
    label_score2->setPosition(m_labelScore2StartPos);

    auto label_score_action = [this](Vec2 start_pos, float y_offset, bool heartbeat) {
        auto move2center = cocos2d::MoveTo::create(0.5f, this->m_field->getCenter() + Vec2(0, y_offset));
        const float SCALE = 5.0f;
        auto scale_up = cocos2d::ScaleTo::create(0.5f, SCALE);
        cocos2d::Action* central_action = nullptr;
        if (heartbeat)
        {
            auto bouncer = cocos2d::ScaleTo::create(0.25f, 1.2f * SCALE);
            auto unbouncer = cocos2d::ScaleTo::create(0.25f, 0.8f * SCALE);
            central_action = cocos2d::Sequence::create(bouncer, unbouncer, bouncer, unbouncer, nullptr);
        }
        else
        {
            central_action = cocos2d::DelayTime::create(1);
        }
        auto move_back = cocos2d::MoveTo::create(0.5f, start_pos);
        auto scale_down = cocos2d::ScaleTo::create(0.5f, 1);

        auto spawn_forward = cocos2d::Spawn::createWithTwoActions(move2center, scale_up);
        auto spawn_backward = cocos2d::Spawn::createWithTwoActions(move_back, scale_down);

        auto seq = cocos2d::Sequence::create(spawn_forward, central_action, spawn_backward, nullptr);
        return seq;
    };
    float y_offset = this->m_field->getPlayRect().size.height / 4;
    label_score1->runAction(label_score_action(label_score1->getPosition(), -y_offset, m_goalHitBy == GoalHitBy::PLAYER1));
    label_score2->runAction(label_score_action(label_score2->getPosition(), y_offset, m_goalHitBy == GoalHitBy::PLAYER2));
}

void GameScene::VFXGoalHit()
{
    if (m_goalHitBy == GoalHitBy::NONE || !airhockey::GlobalSettings::isVFXEnabled)
        return;

    using namespace airhockey;

    bool is_upper_goal = m_goalHitBy == GoalHitBy::PLAYER1;

    auto goal_gate_type = is_upper_goal ? GoalGateLocationType::UPPER : GoalGateLocationType::LOWER;
    const auto& goal_gate = m_field->getGoalGate(goal_gate_type);
    auto goal_rect = goal_gate.getRect();
    
    Color4F color = (is_upper_goal == true) ? Color4F(m_paddle1->getSprite()->getColor()) : Color4F(m_paddle2->getSprite()->getColor());
    auto emitter = airhockey::VFX::getParticleVFXGoalHit(is_upper_goal, goal_rect.size.width, color);
    Vec2 emitter_pos = Vec2(goal_rect.getMidX(), is_upper_goal ? goal_rect.getMinY() : goal_rect.getMaxY());
    emitter->setPosition(emitter_pos);
    this->getChildByTag(TAG_GAME_LAYER)->addChild(emitter, 100);
}


void GameScene::update(float dt)
{
    using namespace airhockey;
    using namespace airhockey::Sound;

    // GAME LOGIC HERE
    m_paddle1->move(dt);

    m_AI->update();

    if (!m_AI)
    {
        m_paddle2->move(dt);
    }

    auto puck_body = static_cast<PhysicsBody*>(m_puck->getComponent("puck_body"));
    
    m_goalHitBy = GoalHitBy::NONE;

    if (!m_isPuckPlayable && isDelayOver())
    {
        m_logger->log("GameScene::update(): delay is over, enabling puck and paddles");

        m_isPuckPlayable = true;
        puck_body->setEnabled(true);
        puck_body->setVelocity(Vec2::ZERO);
        puck_body->setAngularVelocity(0.0f);

        m_paddle1->getPhysicsBody()->setEnabled(true);
        m_paddle2->getPhysicsBody()->setEnabled(true);
        if (m_AI)
        {
            m_AI->setEnabled(true);
            m_AI->reset();
        }
    }

    float puck_y_offset = m_field->getCentralCircleMarking().getSettings().radius;
    // goal to Player1's gate (lower)
    if (m_isPuckPlayable && puck_body->getVelocity() != Vec2::ZERO)
    {
        // goal to Player1's gate (lower)
        if (m_field->getGoalGate(GoalGateLocationType::LOWER).getRect().containsPoint(m_puck->getPosition()))
        {
            m_logger->log("GameScene::update(): goal by Player 2 into Player 1's gate");

            m_goalHitBy = GoalHitBy::PLAYER2;
            ++m_score2;
            puck_y_offset = -puck_y_offset;

            AudioEngine::play2d(AUDIOFILE_BUZZ, false, 0.3f);
        }
        // goal to Player2's gate (upper)
        else if (m_field->getGoalGate(GoalGateLocationType::UPPER).getRect().containsPoint(m_puck->getPosition()))
        {
            m_logger->log("GameScene::update(): goal by Player 1 into Player 2's gate");

            m_goalHitBy = GoalHitBy::PLAYER1;
            ++m_score1;

            switch (rand() % 3)
            {
            case 0:
                AudioEngine::play2d(AUDIOFILE_ORGAN1, false, 0.3f);
                break;
            case 1:
                AudioEngine::play2d(AUDIOFILE_ORGAN2, false, 0.3f);
                break;
            case 2:
            default:
                AudioEngine::play2d(AUDIOFILE_ORGAN3, false, 0.3f);
                break;
            }
        }
    }

    if (m_goalHitBy != GoalHitBy::NONE)
    {
        AudioEngine::play2d(AUDIOFILE_PUCK_FALL_GOAL, false, 0.4f);

        m_isPuckPlayable = false;
        startDelay(3.0f);

        if (m_AI)
        {
            m_AI->reset();
            m_AI->setEnabled(false);
        }
        m_paddle1->getPhysicsBody()->setEnabled(false);
        m_paddle1->setPosition(m_paddle1->getStartPosition());
        
        m_paddle2->getPhysicsBody()->setVelocity(Vec2::ZERO);
        m_paddle2->getPhysicsBody()->setAngularVelocity(0.0f);
        m_paddle2->getPhysicsBody()->setEnabled(false);
        m_paddle2->setPositionImmideately(m_paddle2->getStartPosition());   // <---  причина уезжания ракетки AI за экран!!!

        if (m_score1 < MAX_SCORE && m_score2 < MAX_SCORE)
        {
            Vec2 puck_rethrow_pos = this->m_field->getCenter() + Vec2(0, puck_y_offset);
            rethrowPuck(puck_rethrow_pos);
        }

        VFXGoalHit();
        onScoreChanged();

        m_goalHitBy = GoalHitBy::NONE;

        m_logger->log("Score1: " + std::to_string(m_score1) + "  Score2: " + std::to_string(m_score2));
    }

    if (m_score1 >= MAX_SCORE || m_score2 >= MAX_SCORE)
    {
        if (m_AI)
        {
            m_AI->reset();
            m_AI->setEnabled(false);
        }

        // update total score
        m_totalGames++;
        m_score1 > m_score2 ? ++m_totalScore1 : ++m_totalScore2;

        onGameEndMenuOpen(nullptr);
    }

    drawHUDString(TAG_HUD_LAYER_SCORE1_STRING, std::to_string(m_score1));
    drawHUDString(TAG_HUD_LAYER_SCORE2_STRING, std::to_string(m_score2));

    drawHUDString(TAG_HUD_LAYER_TOTAL_SCORE1_STRING, std::to_string(m_totalScore1));
    drawHUDString(TAG_HUD_LAYER_TOTAL_SCORE2_STRING, std::to_string(m_totalScore2));

    auto p2_p = CCHelpers::Vec2Str(m_paddle2->getSprite()->getPosition());
    auto p2_v = CCHelpers::Vec2Str(m_paddle2->getPhysicsBody()->getVelocity());
    auto p2_s_p = CCHelpers::Vec2Str(m_paddle2->getStick()->getPosition());
    auto p2_s_v = CCHelpers::Vec2Str(m_paddle2->getStickPhysicsBody()->getVelocity());
    m_logger->log("Paddle2 pos = " + p2_p + "  vel = " + p2_v + "  stick_pos = " + p2_s_p + "  stick_vel = " + p2_s_v);

    auto puck_pos = CCHelpers::Vec2Str(m_puck->getPosition());
    auto puck_vel = CCHelpers::Vec2Str(m_puck->getPhysicsBody()->getVelocity());
    m_logger->log("Puck pos = " + puck_pos + " puck vel = " + puck_vel);

}