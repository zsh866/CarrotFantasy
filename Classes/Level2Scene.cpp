#include "Level2Scene.h"
#include "SimpleAudioEngine.h"
#include "GameData.h"
#include "UiLayer.h"
#include "EntityLayer.h"
 
USING_NS_CC;

Scene* Level2::createScene()
{
    return Level2::create();
}

bool Level2::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    //加载plist图片
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Item/Monsters01-hd.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Item/Object01-hd.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Item/Carrot.plist");


    //初始化游戏数据
    GameData::getInstance()->level = 2;
    GameData::getInstance()->money = 430;
    GameData::getInstance()->setMap(this->map);
    GameData::getInstance()->start = Vec2(4, 5);
    GameData::getInstance()->destination = Vec2(7, 1);
    GameData::getInstance()->wave = 1;
    GameData::getInstance()->allWaves = 5;

    uiLayer = UiLayer::createLayer();
    this->addChild(uiLayer, 5);

    entityLayer = EntityLayer::createLayer();
    this->addChild(entityLayer, 1);


    //这里可以改成backgroundLayer，因为不需要修改
    auto background = Sprite::create("Scene/level2/Level_1_2_bg.png");
    background->setAnchorPoint(Vec2(0.5, 0.5));
    background->setPosition(visibleSize / 2);
    this->addChild(background, 0);

    return true;
}
