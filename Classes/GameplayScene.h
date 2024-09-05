#pragma once

#include "cocos2d.h"
#include "MyParallaxNode.h"
#include "Player.h"

USING_NS_CC;

class GameplayScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    void update(float delta) override;

    void menuCloseCallback(cocos2d::Ref* pSender);

    void incrementScore();
    void spawnEnemies(float dt);
    void gameOver();

    int score = 0;
    float playTime = 0.0f;
    bool gameEnd = false;
    
    Player* player;
    Label* scoreLabel;
    MyParallaxNode* paraNode;
    
    // implement the "static create()" method manually
    CREATE_FUNC(GameplayScene);
};
