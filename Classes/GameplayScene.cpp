#include "GameplayScene.h"

#include "Enemy.h"

USING_NS_CC;

#define METEOR_TAG 13

Scene* GameplayScene::createScene()
{
    return GameplayScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

bool GameplayScene::init() {

    if(!Scene::initWithPhysics()) return false;

    //auto world = getPhysicsWorld();
    //if (world) world->setDebugDrawMask(true);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto closeItem = MenuItemImage::create(
        "CloseNormal.png",
        "CloseSelected.png",
        CC_CALLBACK_1(GameplayScene::menuCloseCallback, this));

    if (closeItem == nullptr ||
        closeItem->getContentSize().width <= 0 ||
        closeItem->getContentSize().height <= 0) {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    } else {
        float x = visibleSize.width - closeItem->getContentSize().width/2;
        float y = closeItem->getContentSize().height/2;
        closeItem->setPosition(Vec2(x,y));
    }
    
    auto restartItem = MenuItemImage::create(
        "RestartNormal.png",
        "RestartSelected.png",
        CC_CALLBACK_1(GameplayScene::menuRestartCallback, this));

    if (restartItem == nullptr ||
        restartItem->getContentSize().width <= 0 ||
        restartItem->getContentSize().height <= 0) {
        problemLoading("'RestartNormal.png' and 'RestartSelected.png'");
    } else {
        float x = visibleSize.width - restartItem->getContentSize().width/2 - closeItem->getContentSize().width;
        float y = restartItem->getContentSize().height/2;
        restartItem->setPosition(Vec2(x,y));
    }

    auto menu = Menu::create(closeItem, restartItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    scoreLabel = Label::createWithTTF("Score: " + std::to_string(score), "fonts/Marker Felt.ttf", 24);
    if (scoreLabel == nullptr) {
        problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        scoreLabel->setPosition(Vec2(visibleSize.width/2,
            visibleSize.height - scoreLabel->getContentSize().height));
        this->addChild(scoreLabel, 1);
    }
    
    paraNode = MyParallaxNode::create();
    
    this->addChild(paraNode, -1);

    //uto go = MoveBy::create(8, Vec2(-visibleSize.width,0) );
    //paraNode->runAction(RepeatForever::create(go));
    //schedule([&](float dt) { paraNode->setPosition(0.0f, 0.0f); }, 8.0f, "parallaxLoop");

    player = Player::create();
    if (player) {
        auto physicsBody = PhysicsBody::createBox(player->getContentSize(),
                    PhysicsMaterial(0.1f, 1.0f, 0.0f));

        physicsBody->setGravityEnable(false);
        physicsBody->setRotationEnable(false);
        physicsBody->setDynamic(false);
        physicsBody->setContactTestBitmask(0xFFFFFFFF);
        player->setTag(PLAYER_TAG);

        player->setPosition(Vec2(visibleSize.width/4, visibleSize.height/2));
        player->addComponent(physicsBody);
        
        this->addChild(player, 0);
    }

    scheduleOnce(CC_SCHEDULE_SELECTOR(GameplayScene::spawnEnemies), 2.0f);
    scheduleUpdate();
    
    return true;
}

void GameplayScene::update(float delta) {
    paraNode->move(cocos2d::Vec2(-delta * 100.0f, 0.0f));
}


void GameplayScene::menuCloseCallback(Ref* pSender) {
    Director::getInstance()->end();
}

void GameplayScene::menuRestartCallback(cocos2d::Ref* pSender) {
    auto director = Director::getInstance();
    director->replaceScene(GameplayScene::createScene());
}

void GameplayScene::incrementScore() {
    scoreLabel->setString("Score: " + std::to_string(++score));
}

void GameplayScene::spawnEnemies(float dt) {
    if (gameEnd) return;

    if (cocos2d::random(0.0f, 100.0f) < 5.0f) { // METEOR
        auto meteorSprite = Sprite::create("meteor.png");
        if (meteorSprite) {
            auto visibleSize = Director::getInstance()->getVisibleSize();
            auto physicsBody = PhysicsBody::createBox(player->getContentSize(),
                        PhysicsMaterial(0.1f, 1.0f, 0.0f));

            meteorSprite->setContentSize(cocos2d::Size(100.0f, 100.0f));
            physicsBody->setGravityEnable(true);
            physicsBody->setRotationEnable(false);
            physicsBody->setDynamic(false);
            physicsBody->setContactTestBitmask(0xFFFFFFFF);
            meteorSprite->addComponent(physicsBody);
            meteorSprite->setTag(METEOR_TAG);
            meteorSprite->setAnchorPoint(cocos2d::Vec2(0.0f, 0.0f));
            physicsBody->setVelocity(cocos2d::Vec2(500.0f, -600.0f));
            meteorSprite->setPosition(cocos2d::random(visibleSize.width * 0.1f, visibleSize.width * 0.4f), visibleSize.height);
            
            std::hash<Sprite*> hash;
            scheduleOnce([meteorSprite](float t) {
                meteorSprite->removeFromParentAndCleanup(true);
            }, 4.0f, "remove" + std::to_string(hash(meteorSprite)));
            this->addChild(meteorSprite);
        }
    } else { // Spawn normal enemy
        auto rand = cocos2d::random(0.0f, 3.0f);
        EnemyType type = EnemyType::BIRD;
        if (rand < 1.0f) type = EnemyType::BIRD;
        else if (rand < 2.0f) type = EnemyType::BOMBER;
        else if (rand <= 3.0f) type = EnemyType::FIGHTER;
        auto enemy = Enemy::makeEnemy(type);
    
        if (enemy) {
            std::hash<Enemy*> hash;
            scheduleOnce([enemy](float t) {
                enemy->removeFromParentAndCleanup(true);
            }, 10.0f, "remove" + std::to_string(hash(enemy)));
            this->addChild(enemy);
        }
    }
    
    playTime += dt;
    float nextSpawnDelay = 0.5f + (100.0f - dt) / 100.0f;
    nextSpawnDelay = nextSpawnDelay < 0.3f ? 0.3f : nextSpawnDelay;
    scheduleOnce(CC_SCHEDULE_SELECTOR(GameplayScene::spawnEnemies), nextSpawnDelay);
}

void GameplayScene::gameOver() {
    unschedule(CC_SCHEDULE_SELECTOR(GameplayScene::spawnEnemies));
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto gameOverLabel = Label::createWithTTF("GAME OVER", "fonts/Marker Felt.ttf", 72);
    if (!gameOverLabel) return;
    gameOverLabel->setPosition(Vec2(visibleSize.width/2, visibleSize.height/2));
    this->addChild(gameOverLabel, 1);
}
