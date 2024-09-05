#pragma once

#include "cocos2d.h"

USING_NS_CC;

#define PLAYER_TAG 11

class Player : public cocos2d::Sprite {
protected:
    float cursor_y;
    bool isGameOver = false;

public:

    bool initWithFile(const std::string& filename) override;
    void update(float delta) override;

    void onMouseMove(cocos2d::Event *event);
    void onMouseDown(cocos2d::Event *event);
    void onMouseUp(cocos2d::Event *event);
    
    void characterShoot(float dt);

    bool onOverlap(PhysicsContact& contact);
    bool onProjectileOverlap(PhysicsContact& contact);
    void gameOver();


    static Player* Player::create()
    {
        Player *player = new (std::nothrow) Player();
        if (player && player->initWithFile("Player.png"))
        {
            player->autorelease();
            return player;
        }
        CC_SAFE_DELETE(player);
        return nullptr;
    }
};
