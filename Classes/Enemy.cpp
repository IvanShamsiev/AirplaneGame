#include "Enemy.h"
#include "GameplayScene.h"
#include "Projectile.h"

Enemy* Enemy::makeEnemy(EnemyType type) {
    switch (type) {
    case EnemyType::BIRD:
        return EnemyBird::create();
    case EnemyType::BOMBER:
        return EnemyBomber::create();
    case EnemyType::FIGHTER:
        return EnemyFighter::create();
    }
    return nullptr;
}

bool Enemy::onOverlap(PhysicsContact& contact) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    if (!nodeA || !nodeB) return false;
    if (nodeA != this && nodeB != this) return false;
    
    Projectile* projectile = dynamic_cast<Projectile*>(nodeA);
    if (!projectile) projectile = dynamic_cast<Projectile*>(nodeB);
    if (projectile && !projectile->isActive) return false;

    if (nodeA->getTag() != nodeB->getTag()) {
        setVisible(false);
        isAlive = false;
        unscheduleAllCallbacks();
        auto physicalBody = getPhysicsBody();
        if (physicalBody) physicalBody->setContactTestBitmask(0x0);
    }

    return false;
}

bool EnemyBird::initWithFile(const std::string& filename) {
    if (!Enemy::initWithFile(filename)) return false;
    auto visibleSize = Director::getInstance()->getVisibleSize();
        
    auto physicsBody = PhysicsBody::createBox(getContentSize(),
        PhysicsMaterial(0.1f, 1.0f, 0.0f));
    physicsBody->setGravityEnable(false);
    physicsBody->setRotationEnable(false);
    physicsBody->setDynamic(false);
    physicsBody->setVelocity(cocos2d::Vec2(-200.0f + cocos2d::random(-400.0f, 0.0f), 0.0f));
    addComponent(physicsBody);
        
    setPosition(visibleSize.width, cocos2d::random(0.0f, visibleSize.height/4.0f));
    return true;
}

bool EnemyBomber::initWithFile(const std::string& filename) {
    if (!Enemy::initWithFile(filename)) return false;

    auto physicsBody = PhysicsBody::createBox(getContentSize(),
        PhysicsMaterial(0.1f, 1.0f, 0.0f));
    physicsBody->setGravityEnable(false);
    physicsBody->setRotationEnable(false);
    physicsBody->setDynamic(false);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    addComponent(physicsBody);
    setTag(ENEMY_TAG);

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(Enemy::onOverlap, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    auto visibleSize = Director::getInstance()->getVisibleSize();
    if (cocos2d::rand_0_1() - 0.5f < 0) { // High and fast
        setPosition(visibleSize.width, cocos2d::random(visibleSize.height * 0.75f, visibleSize.height - getContentSize().height));
        getPhysicsBody()->setVelocity(cocos2d::Vec2(-600.0f + cocos2d::random(-150.0f, 0.0f), 0.0f));
    } else { // Average high and average speed
        setPosition(visibleSize.width, cocos2d::random(visibleSize.height * 0.4f, visibleSize.height * 0.6f));
        getPhysicsBody()->setVelocity(cocos2d::Vec2(-300.0f + cocos2d::random(-150.0f, 0.0f), 0.0f));
    }
        
    return true;
}

bool EnemyFighter::initWithFile(const std::string& filename) {
    if (!Enemy::initWithFile(filename)) return false;

    auto physicsBody = PhysicsBody::createBox(getContentSize(),
                                              PhysicsMaterial(0.1f, 1.0f, 0.0f));
    physicsBody->setGravityEnable(false);
    physicsBody->setRotationEnable(false);
    physicsBody->setDynamic(false);
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
    addComponent(physicsBody);
    setTag(ENEMY_TAG);

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(Enemy::onOverlap, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
        
    auto visibleSize = Director::getInstance()->getVisibleSize();
    setPosition(visibleSize.width, cocos2d::random(visibleSize.height * 0.2f, visibleSize.height * 0.6f));
        
    if (cocos2d::rand_0_1() - 0.5f < 0) { // fast
        getPhysicsBody()->setVelocity(cocos2d::Vec2(-450.0f + cocos2d::random(-150.0f, 0.0f), 0.0f));
    } else { // average speed
        getPhysicsBody()->setVelocity(cocos2d::Vec2(-300.0f + cocos2d::random(-150.0f, 0.0f), 0.0f));
    }

    schedule([this](float) {shoot();}, 0.5f, "enemyShoot");
        
    return true;
}

void EnemyFighter::shoot() {
    auto projectile = Projectile::create("projectile.webp");
    projectile->setContentSize(cocos2d::Size(30.0f, 10.0f));
    projectile->type = ProjectileType::ENEMY;
    
    auto currPos = getPhysicsBody()->getPosition();
    projectile->setPosition(currPos + cocos2d::Vec2(-5.0f, 0.0f));

    auto physicsBody = PhysicsBody::createBox(projectile->getContentSize(),
                                              PhysicsMaterial(0.1f, 1.0f, 0.0f));

    physicsBody->setGravityEnable(true);
    physicsBody->setRotationEnable(false);
    physicsBody->setDynamic(false);
    physicsBody->setVelocity(getPhysicsBody()->getVelocity() + cocos2d::Vec2(-300.0f, 0.0f));
    physicsBody->setContactTestBitmask(0xFFFFFFFF);
        
    projectile->addComponent(physicsBody);
    projectile->setTag(ENEMY_TAG);

    auto currScene = getScene();
    if (!currScene) return;
    
    std::hash<Projectile*> hash;
    currScene->scheduleOnce([projectile](float t) {
        projectile->removeFromParentAndCleanup(true);
    }, 5.0f, "remove" + std::to_string(hash(projectile)));

    currScene->addChild(projectile, 0);
}

void EnemyFighter::update(float delta) {
    auto scene = getScene();
    if (!scene) return;
    auto gameplayScene = dynamic_cast<GameplayScene*>(scene);
    if (!gameplayScene) return;
    auto player = gameplayScene->player;
    if (!player) return;
    auto playerPos = player->getPosition();
    auto pos = getPosition();
    getPhysicsBody()->setVelocity(cocos2d::Vec2(getPhysicsBody()->getVelocity().x, playerPos.y - pos.y));
}
