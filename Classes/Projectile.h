#pragma once

#include "cocos2d.h"

USING_NS_CC;

enum ProjectileType { PLAYER, ENEMY, NEUTRAL };

class Projectile : public cocos2d::Sprite {
public:
    ProjectileType type = ProjectileType::NEUTRAL;
    bool isActive = true;

    
    static Projectile* Projectile::create(const std::string& filename)
    {
        Projectile *projectile = new (std::nothrow) Projectile();
        if (projectile && projectile->initWithFile(filename))
        {
            projectile->autorelease();
            return projectile;
        }
        CC_SAFE_DELETE(projectile);
        return nullptr;
    }
};
