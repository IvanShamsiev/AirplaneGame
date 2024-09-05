#pragma once

#include "cocos2d.h"

USING_NS_CC;

#define ENEMY_TAG 12

enum EnemyType { BIRD, BOMBER, FIGHTER };

class Enemy : public cocos2d::Sprite {
protected:
    Enemy(EnemyType type) : _type(type) {}
    
public:
    EnemyType _type;
    bool isAlive = true;

    bool onOverlap(PhysicsContact& contact);
    
    static Enemy* makeEnemy(EnemyType type);
};

class EnemyBird : public Enemy {
public:
    EnemyBird() : Enemy(EnemyType::BIRD) {}
    
    bool initWithFile(const std::string& filename) override;

    static EnemyBird* create()
    {
        EnemyBird *enemyBird = new (std::nothrow) EnemyBird();
        if (enemyBird && enemyBird->initWithFile("EnemyBird.png"))
        {
            enemyBird->autorelease();
            return enemyBird;
        }
        CC_SAFE_DELETE(enemyBird);
        return nullptr;
    }
};


class EnemyBomber : public Enemy {
public:
    EnemyBomber() : Enemy(EnemyType::BOMBER) {}
    
    bool initWithFile(const std::string& filename) override;

    static EnemyBomber* create()
    {
        EnemyBomber *enemyBomber = new (std::nothrow) EnemyBomber();
        if (enemyBomber && enemyBomber->initWithFile("EnemyBomber.png"))
        {
            enemyBomber->autorelease();
            return enemyBomber;
        }
        CC_SAFE_DELETE(enemyBomber);
        return nullptr;
    }
};


class EnemyFighter : public Enemy {
public:
    EnemyFighter() : Enemy(EnemyType::FIGHTER) {}
    
    bool initWithFile(const std::string& filename) override;

    void shoot();

    void update(float delta) override;

    static EnemyFighter* create()
    {
        EnemyFighter *enemyFighter = new (std::nothrow) EnemyFighter();
        if (enemyFighter && enemyFighter->initWithFile("EnemyFighter.png"))
        {
            enemyFighter->autorelease();
            return enemyFighter;
        }
        CC_SAFE_DELETE(enemyFighter);
        return nullptr;
    }
};