#include "GameScene.h"
#include "KeyboardInputController.h"
#include "AIInputController.h"
#include "MouseInputController.h"
#include "TouchInputController.h"
#include "GameMenuLayer.h"
#include "GameEndLayer.h"
#include "AIIdleState.h"

USING_NS_CC;

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
    TAG_HUD_LAYER_SCORE2_STRING = 2
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

    //////////////////////////////////////////////////
    // GAME LAYER
    //
    auto game_layer = Layer::create();
 
    auto background = cocos2d::LayerGradient::create(cocos2d::Color4B::BLUE, cocos2d::Color4B::ORANGE);
    game_layer->addChild(background, 1);

    Rect GAMEFIELDRECT = Rect(frameCenter.x - m_currLevel.m_width / 2, 
                              frameCenter.y - m_currLevel.m_height / 2, 
                              m_currLevel.m_width, m_currLevel.m_height);
    
    GameFieldBuilder builder = GameFieldBuilder();
    // building steps
    builder.addPlayRect(GAMEFIELDRECT);
    
    const int SIDE_WIDTH = 40;
    const int SIDE_HEIGHT = 40;
    const float GAP = 10;
    const float PUCK_RADIUS = 50;
    const Size GOAL_GATE_SIZE = Size(8 * PUCK_RADIUS, 1000);  // do gate height large enough to catch puck even on high speed intersecting gate
    const Size LONG_PART_SIZE = Size(SIDE_WIDTH, GAMEFIELDRECT.size.height / 2 - 2 * GAP);
    const Size SHORT_PART_SIZE = Size((GAMEFIELDRECT.size.width - GOAL_GATE_SIZE.width)/2 - GAP, SIDE_HEIGHT);

    GameFieldSidePtr left_long_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::UP);
    left_long_side->addSidePart(std::make_unique<GameFieldSidePart>(LONG_PART_SIZE, Color4F::GREEN), GAP);
    left_long_side->addSidePart(std::make_unique<GameFieldSidePart>(LONG_PART_SIZE, Color4F::RED), 2 * GAP);
    builder.addSide(std::move(left_long_side));

    GameFieldSidePtr upper_short_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::RIGHT);
    upper_short_side->addSidePart(std::make_unique<GameFieldSidePart>(SHORT_PART_SIZE, Color4F::YELLOW), GAP);
    upper_short_side->addSidePart(std::make_unique<GameFieldSidePart>(SHORT_PART_SIZE, Color4F::WHITE), GOAL_GATE_SIZE.width);
    builder.addSide(std::move(upper_short_side));

    GameFieldSidePtr right_long_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::DOWN);
    right_long_side->addSidePart(std::make_unique<GameFieldSidePart>(LONG_PART_SIZE, Color4F::GREEN), GAP);
    right_long_side->addSidePart(std::make_unique<GameFieldSidePart>(LONG_PART_SIZE, Color4F::RED), 2 * GAP);
    builder.addSide(std::move(right_long_side));

    GameFieldSidePtr lower_short_side = std::make_unique<GameFieldSide>(GameFieldSide::DIRECTION::LEFT);
    lower_short_side->addSidePart(std::make_unique<GameFieldSidePart>(SHORT_PART_SIZE, Color4F::YELLOW), GAP);
    lower_short_side->addSidePart(std::make_unique<GameFieldSidePart>(SHORT_PART_SIZE, Color4F::WHITE), GOAL_GATE_SIZE.width);
    builder.addSide(std::move(lower_short_side));
    
    const Size CORNER_SIZE = Size(SIDE_WIDTH * 2, SIDE_WIDTH * 2);
    builder.addCorner(std::make_unique<GameFieldSidePart>(CORNER_SIZE, Color4F::BLACK), GameField::GameFieldPlayRectCornerType::BOTTOM_LEFT);
    builder.addCorner(std::make_unique<GameFieldSidePart>(CORNER_SIZE, Color4F::WHITE), GameField::GameFieldPlayRectCornerType::TOP_LEFT);
    builder.addCorner(std::make_unique<GameFieldSidePart>(CORNER_SIZE, Color4F::BLACK), GameField::GameFieldPlayRectCornerType::TOP_RIGHT);
    builder.addCorner(std::make_unique<GameFieldSidePart>(CORNER_SIZE, Color4F::WHITE), GameField::GameFieldPlayRectCornerType::BOTTOM_RIGHT);
    
    builder.addGoalGate(std::make_unique<GoalGate>(GOAL_GATE_SIZE, GoalGateLocationType::LOWER));
    builder.addGoalGate(std::make_unique<GoalGate>(GOAL_GATE_SIZE, GoalGateLocationType::UPPER));

    builder.addCentralCircleMarking(CentralCircleMarkingSettings(GOAL_GATE_SIZE.width / 2, 3, Color4F::MAGENTA, Color4F::GRAY));

    builder.addGoalGateMarking(GoalGateMarkingSettings(GOAL_GATE_SIZE.width, 3, Color4F::MAGENTA, Color4F::GRAY, 
        GoalGateMarkingSettings::GoalGateMarkingShapeType::RECTANGLE), Vec2::ZERO);
    
    //
    m_field = builder.getResult();
    m_field->setParent(game_layer);

    // PUCK
    m_puck = Sprite::create("puck.png");
    PhysicsMaterial puck_material = PhysicsMaterial(0.1f, 1.0f, 0.2f);
    auto puck_body = PhysicsBody::createCircle(m_puck->getBoundingBox().size.width / 2, puck_material);
    puck_body->setName("puck_body");
    m_puck->addComponent(puck_body);
    m_puck->setPosition(m_field->getCenter());
    game_layer->addChild(m_puck, 1);

    const float PADDLE_RADIUS = 1.5 * PUCK_RADIUS;

    // human (lower)
    Rect PLAYER1_FIELDRECT = Rect(GAMEFIELDRECT.getMinX(), GAMEFIELDRECT.getMinY(), GAMEFIELDRECT.size.width, PADDLE_RADIUS + GAMEFIELDRECT.size.height / 2);

    // AI (upper)
    Rect PLAYER2_FIELDRECT = Rect(GAMEFIELDRECT.getMinX(), GAMEFIELDRECT.getMidY() - PADDLE_RADIUS, GAMEFIELDRECT.size.width, PADDLE_RADIUS + GAMEFIELDRECT.size.height / 2);

    _physicsWorld->setGravity(Vec2::ZERO);

    const float PADDLE_START_Y_CENTER_OFFSET = GOAL_GATE_SIZE.width / 2;

    m_paddle1 = std::make_shared<Paddle>("paddle.png", m_field->getCenter().x, m_field->getPlayRect().getMinY() + PADDLE_START_Y_CENTER_OFFSET, 1000, 1000, PADDLE_RADIUS,
        PLAYER1_FIELDRECT, game_layer, this->getPhysicsWorld());

    m_keyboardController = std::make_shared<KeyboardInputController>("KB", m_paddle1);

    m_paddle2 = std::make_shared<Paddle>("paddle.png", m_field->getCenter().x, m_field->getPlayRect().getMaxY() - PADDLE_START_Y_CENTER_OFFSET, 1000, 1000, PADDLE_RADIUS,
        PLAYER2_FIELDRECT, game_layer, this->getPhysicsWorld());

    //m_touchController = std::make_shared<TouchInputController>("TOUCH", m_paddle2);

    //m_AIController = std::make_shared<AIInputController>("AI", m_paddle2);
    const float ATTACK_RADIUS = abs(m_paddle2->getStartPosition().y - m_field->getCenter().y) + PUCK_RADIUS;
    m_AIIdleState = std::make_unique<AIIdleState>(m_field.get(), m_paddle2, m_puck, ATTACK_RADIUS);
    m_AI = std::make_shared<FSMContext>(std::move(m_AIIdleState));
 
    //////////////////////////////////////////////////
    // HUD LAYER
    // non-interactive elements over main game process
    
    auto hud_layer = LayerColor::create(Color4B(0, 0, 0, 0));
    //m_touchController->scheduleDebugOutput(hud_layer);

    auto label_score1 = Label::createWithTTF("0", "fonts/arial.ttf", 72);
    label_score1->setRotation(-90.f);
    label_score1->setAnchorPoint(Vec2(0.5f, 0.5f));
    label_score1->setPosition(frameCenter - Vec2(0, PUCK_RADIUS));
    hud_layer->addChild(label_score1, 1, TAG_HUD_LAYER_SCORE1_STRING);

    auto label_score2 = Label::createWithTTF("0", "fonts/arial.ttf", 72);
    label_score2->setRotation(-90.f);
    label_score2->setAnchorPoint(Vec2(0.5f, 0.5f));
    label_score2->setPosition(frameCenter + Vec2(0, PUCK_RADIUS));
    hud_layer->addChild(label_score2, 1, TAG_HUD_LAYER_SCORE2_STRING);


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

    auto _game_menu_close_listener = EventListenerCustom::create("event_game_menu_close", [=](EventCustom* event) {
        onGameMenuClose(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_game_menu_close_listener, this);

    auto _game_end_menu_close_listener = EventListenerCustom::create("event_game_end_menu_close", [=](EventCustom* event) {
        onGameEndMenuClose(event);
        });
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_game_end_menu_close_listener, this);


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
    Director::getInstance()->getScheduler()->pauseTarget(this);

    auto game_menu_layer = GameMenuLayer::create(this);
    this->addChild(game_menu_layer, 255, TAG_GAME_MENU_LAYER);
}

void GameScene::onGameMenuClose(Event* event)
{
    Director::getInstance()->getScheduler()->resumeTarget(this);

    auto game_menu_layer = this->getChildByTag(TAG_GAME_MENU_LAYER);
    if (game_menu_layer)
    {
        this->removeChildByTag(TAG_GAME_MENU_LAYER);
    }

}


void GameScene::onGameEndMenuOpen(Ref* sender)
{
    Director::getInstance()->getScheduler()->pauseTarget(this);

    auto game_end_menu_layer = GameEndMenuLayer::create(this);
    this->addChild(game_end_menu_layer, 255, TAG_GAME_END_MENU_LAYER);
}

void GameScene::onGameEndMenuClose(Event* event)
{
    Director::getInstance()->getScheduler()->resumeTarget(this);

    auto game_end_menu_layer = this->getChildByTag(TAG_GAME_END_MENU_LAYER);
    if (game_end_menu_layer)
    {
        this->removeChildByTag(TAG_GAME_END_MENU_LAYER);
    }

    m_score1 = 0;
    m_score2 = 0;
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

void GameScene::update(float dt)
{
    using namespace airhockey;

    // GAME LOGIC HERE
    m_paddle1->move(dt);

    m_AI->update();

    if (!m_AI)
    {
        m_paddle2->move(dt);
    }

    auto puck_body = static_cast<PhysicsBody*>(m_puck->getComponent("puck_body"));
    
    enum class GoalHitBy
    {
        NONE = 0,
        PLAYER1,
        PLAYER2
    };
    GoalHitBy goal_hit_by = GoalHitBy::NONE;

    if (!m_isPuckPlayable && !this->getChildByName("WaitNode")->getActionByTag(12345))
    {
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
    if (m_isPuckPlayable)
    {
        // goal to Player1's gate (lower)
        if (m_field->getGoalGate(GoalGateLocationType::LOWER).getRect().containsPoint(m_puck->getPosition()))
        {
            goal_hit_by = GoalHitBy::PLAYER2;
            ++m_score2;
            puck_y_offset = -puck_y_offset;
        }
        // goal to Player2's gate (upper)
        else if (m_field->getGoalGate(GoalGateLocationType::UPPER).getRect().containsPoint(m_puck->getPosition()))
        {
            goal_hit_by = GoalHitBy::PLAYER1;
            ++m_score1;
        }
    }

    if (goal_hit_by != GoalHitBy::NONE)
    {
        m_isPuckPlayable = false;
        auto wait_action = DelayTime::create(3.0f);
        wait_action->setTag(12345);
        auto wait_node = Node::create();
        wait_node->runAction(wait_action);
        this->addChild(wait_node, 2, "WaitNode");

        puck_body->setVelocity(Vec2::ZERO);
        puck_body->setAngularVelocity(0.0f);
        puck_body->setEnabled(false);
        
        if (m_AI)
        {
            m_AI->reset();
            m_AI->setEnabled(false);
        }
        m_paddle1->getPhysicsBody()->setEnabled(false);
        m_paddle2->getPhysicsBody()->setEnabled(false);
        m_paddle1->setPosition(m_paddle1->getStartPosition());
        //m_paddle2->setPosition(m_paddle2->getStartPosition());   <--- ������� �������� ������� AI �� �����!!!

        auto puck_restart_action = [this, &puck_y_offset]() {
            auto hide = cocos2d::Hide::create();
            auto move_to_zero = cocos2d::MoveTo::create(0.0f, Vec2::ZERO);
            auto delay = cocos2d::DelayTime::create(3.0f);
            auto move_to_circle = cocos2d::MoveTo::create(0.0f, this->m_field->getCenter() + Vec2(0, puck_y_offset));
            auto rotate_to_zero = cocos2d::RotateTo::create(0.0f, 0.0f);
            auto show = cocos2d::Show::create();

            auto seq = cocos2d::Sequence::create(hide, move_to_zero, delay, move_to_circle, rotate_to_zero, show, nullptr);
            return seq;
        };
        m_puck->runAction(puck_restart_action());

        auto puck_clone = Sprite::create("puck.png");
        this->getChildByTag(TAG_GAME_LAYER)->addChild(puck_clone, 1);
        auto puck_throw_down_effect_action = [this, &puck_y_offset]() {
            auto hide = cocos2d::Hide::create();
            auto delay = cocos2d::DelayTime::create(2.0f);
            auto move_to_circle = cocos2d::MoveTo::create(0.0f, this->m_field->getCenter() + Vec2(0, puck_y_offset));
            auto scale_up = cocos2d::ScaleTo::create(0.0, 5);
            auto show = cocos2d::Show::create();
            auto scale_down = cocos2d::ScaleTo::create(1.0, 1);
            auto remove = cocos2d::RemoveSelf::create();

            auto seq = cocos2d::Sequence::create(hide, delay, move_to_circle, scale_up, show, scale_down, remove, nullptr);
            return seq;
        };
        puck_clone->runAction(puck_throw_down_effect_action());


        auto hud_layer = this->getChildByTag(TAG_HUD_LAYER);
        auto label_score1 = static_cast<cocos2d::Label*> (hud_layer->getChildByTag(TAG_HUD_LAYER_SCORE1_STRING));
        auto label_score2 = static_cast<cocos2d::Label*> (hud_layer->getChildByTag(TAG_HUD_LAYER_SCORE2_STRING));

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
        label_score1->runAction(label_score_action(label_score1->getPosition(), -y_offset, goal_hit_by == GoalHitBy::PLAYER1));
        label_score2->runAction(label_score_action(label_score2->getPosition(), y_offset, goal_hit_by == GoalHitBy::PLAYER2));
        goal_hit_by = GoalHitBy::NONE;

    }

    const int MAX_SCORE = 7;
    if (m_score1 >= MAX_SCORE || m_score2 >= MAX_SCORE)
    {
        onGameEndMenuOpen(nullptr);
    }

    drawHUDString(TAG_HUD_LAYER_SCORE1_STRING, std::to_string(m_score1));
    drawHUDString(TAG_HUD_LAYER_SCORE2_STRING, std::to_string(m_score2));

}