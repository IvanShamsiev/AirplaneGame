#include "MyParallaxNode.h"

void MyParallaxNode::addChild(ImageNode* child, int zOrder, const cocos2d::Vec2& ratio, const cocos2d::Vec2& offset) {
    Node::addChild(child, zOrder);
    child->setPosition(offset);
    child->_ratio = ratio;
}

void MyParallaxNode::move(const cocos2d::Vec2& move) {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    for (auto& child : getChildren()) {
        auto sprite = dynamic_cast<ImageNode*>(child);
        if (!sprite) continue;
        auto& ratio = sprite->_ratio;
        sprite->setPosition(sprite->getPosition() + cocos2d::Vec2(ratio.x * move.x, ratio.y * move.y));
        if (sprite->getPosition().x < -visibleSize.width)
            sprite->setPosition(sprite->getPosition() + cocos2d::Vec2(visibleSize.width, 0.0f));
        if (sprite->getPosition().y < -visibleSize.height)
            sprite->setPosition(sprite->getPosition() + cocos2d::Vec2(0.0f, visibleSize.height));
    }
}

bool MyParallaxNode::init() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto background1 = ImageNode::create("Background/1.png");
    background1->setContentSize(cocos2d::Size(visibleSize.width*2.0f, visibleSize.height));
    background1->setAnchorPoint(Vec2(0,0));
    this->addChild(background1, -4, Vec2(0.4f,0.0f), Vec2::ZERO);

    auto background2 = ImageNode::create("Background/2.png");
    background2->setContentSize(cocos2d::Size(visibleSize.width*2.0f, visibleSize.height));
    background2->setAnchorPoint(Vec2(0,0));
    this->addChild(background2, -3, Vec2(1.4f,0.0f), Vec2(-20.0f, 0.0f));

    auto background3 = ImageNode::create("Background/3.png");
    background3->setContentSize(cocos2d::Size(visibleSize.width*2.0f, visibleSize.height));
    background3->setAnchorPoint(Vec2(0,0));
    this->addChild(background3, -2, Vec2(2.0f,0.0f), Vec2(-30.0f, 0.0f));

    auto background4 = ImageNode::create("Background/4.png");
    background4->setContentSize(cocos2d::Size(visibleSize.width*2.0f, visibleSize.height));
    background4->setAnchorPoint(Vec2(0,0));
    this->addChild(background4, -1, Vec2(3.0f,0.0f), Vec2(-100.0f, 0.0f));

    return true;
}
