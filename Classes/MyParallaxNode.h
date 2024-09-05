#pragma once

#include "cocos2d.h"

USING_NS_CC;

class ImageNode : public cocos2d::Sprite {
public:
    cocos2d::Vec2 _ratio;

    static ImageNode* create(const std::string& filename)
    {
        ImageNode *imageNode = new (std::nothrow) ImageNode();
        if (imageNode && imageNode->initWithFile(filename))
        {
            imageNode->autorelease();
            return imageNode;
        }
        CC_SAFE_DELETE(imageNode);
        return nullptr;
    }
};

class MyParallaxNode : public cocos2d::Node {
public:

    bool init() override;
    void addChild(ImageNode* child, int zOrder, const cocos2d::Vec2& ratio, const cocos2d::Vec2& offset);

    void move(const cocos2d::Vec2& move);



    static MyParallaxNode* create()
    {
        MyParallaxNode *node = new (std::nothrow) MyParallaxNode();
        if (node && node->init())
        {
            node->autorelease();
            return node;
        }
        CC_SAFE_DELETE(node);
        return nullptr;
    }
};
