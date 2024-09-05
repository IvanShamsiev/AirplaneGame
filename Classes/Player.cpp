#include "Player.h"

#include "Enemy.h"
#include "Projectile.h"
#include "GameplayScene.h"

bool Player::initWithFile(const std::string& filename) {
    if (!Sprite::initWithFile(filename)) return false;
    
    auto _mouseListener = EventListenerMouse::create();
    _mouseListener->onMouseMove = CC_CALLBACK_1(Player::onMouseMove, this);
    _mouseListener->onMouseDown = CC_CALLBACK_1(Player::onMouseDown, this);
    _mouseListener->onMouseUp = CC_CALLBACK_1(Player::onMouseUp, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);
    auto visibleSize = Director::getInstance()->getVisibleSize();
    cursor_y = visibleSize.height / 2.0f;

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(Player::onOverlap, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);

    return true;
}

void Player::update(float delta) {
    Sprite::update(delta);
    auto pos = getPhysicsBody()->getPosition();
    auto velocity = cocos2d::Vec2(0, cursor_y - pos.y) * 5.0f;
    getPhysicsBody()->setVelocity(velocity);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    if (pos.y < visibleSize.height / 8.0f)
        gameOver();
}

void Player::onMouseMove(cocos2d::Event* event) {
    if (isGameOver) return;
    cocos2d::EventMouse* e = (cocos2d::EventMouse*)event;
    cursor_y = e->getCursorY();
}

void Player::onMouseDown(cocos2d::Event* event) {
    if (isGameOver) return;
    characterShoot(0.2f);
    schedule(CC_SCHEDULE_SELECTOR(Player::characterShoot), 0.2f);
}

void Player::onMouseUp(cocos2d::Event* event) {
    if (isGameOver) return;
    unschedule(CC_SCHEDULE_SELECTOR(Player::characterShoot));
}

void Player::characterShoot(float dt) {
    auto projectile = Projectile::create("projectile.webp");
    projectile->setContentSize(cocos2d::Size(30.0f, 10.0f));
    projectile->type = ProjectileType::PLAYER;
    
    auto currPos = getPhysicsBody()->getPosition();
    projectile->setPosition(currPos + cocos2d::Vec2(5.0f, 0.0f));

    auto physicsBody = PhysicsBody::createBox(projectile->getContentSize(),
                PhysicsMaterial(0.1f, 1.0f, 0.0f));

    physicsBody->setGravityEnable(true);
    physicsBody->setRotationEnable(false);
    physicsBody->setDynamic(false);
    physicsBody->setVelocity(cocos2d::Vec2(750.0f, 0.0f));
    physicsBody->setContactTestBitmask(0xFFFFFFFF);

    auto contactListener = EventListenerPhysicsContact::create();
    contactListener->onContactBegin = CC_CALLBACK_1(Player::onProjectileOverlap, this);
    projectile->getEventDispatcher()->addEventListenerWithSceneGraphPriority(contactListener, this);
        
    projectile->addComponent(physicsBody);
    projectile->setTag(PLAYER_TAG);

    auto currScene = getScene();
    if (!currScene) return;

    std::hash<Projectile*> hash;
    currScene->scheduleOnce([projectile](float t) {
        projectile->removeFromParentAndCleanup(true);
    }, 1.5f, "remove" + std::to_string(hash(projectile)));

    currScene->addChild(projectile, 0);
}

bool Player::onOverlap(PhysicsContact& contact) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    if (!nodeA || !nodeB) return false;
    if (nodeA != this && nodeB != this) return false;
    if (nodeA->getTag() == nodeB->getTag()) return false;
    
    Projectile* projectile = dynamic_cast<Projectile*>(nodeA);
    if (!projectile) projectile = dynamic_cast<Projectile*>(nodeB);
    if (projectile && !projectile->isActive) return false;

    gameOver();

    return false;
}

bool Player::onProjectileOverlap(PhysicsContact& contact) {
    auto nodeA = contact.getShapeA()->getBody()->getNode();
    auto nodeB = contact.getShapeB()->getBody()->getNode();
    if (!nodeA || !nodeB) return false;
    if (nodeA->getTag() == nodeB->getTag()) return false;

    auto projectileA = dynamic_cast<Projectile*>(nodeA);
    auto projectileB = dynamic_cast<Projectile*>(nodeB);
    if (projectileA && projectileB && projectileA->isActive && projectileB->isActive) {
        projectileA->isActive = false;
        projectileA->setVisible(false);
        projectileB->isActive = false;
        projectileB->setVisible(false);
        return false;
    }
    
    auto projectile = projectileA ? projectileA : projectileB;
    if (!projectile) return false;
    if (!projectile->isActive) return false;
    
    auto scene = getScene();
    if (!scene) return false;
    auto gameplayScene = dynamic_cast<GameplayScene*>(scene);
    if (!gameplayScene) return false;

    gameplayScene->incrementScore();
    projectile->isActive = false;
    projectile->setVisible(false);
    
    return false;
}

void Player::gameOver() {
    auto scene = getScene();
    if (!scene) return;
    auto gameplayScene = dynamic_cast<GameplayScene*>(scene);
    if (!gameplayScene) return;
    gameplayScene->gameOver();
    isGameOver = true;
    unscheduleAllCallbacks();
    setVisible(false);
}
