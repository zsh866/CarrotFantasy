#include "Level1Scene.h"
#include "SimpleAudioEngine.h"
#include "GameData.h"
#include "UiLayer.h"
#include "EntityLayer.h"

USING_NS_CC;

Scene* Level1::createScene()
{
    return Level1::create();
}

bool Level1::init()
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
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Item/Tower_Bottle.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Item/Carrot.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Item/ElectricTower.plist");
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Item/Fan.plist");


    //初始化游戏数据
    GameData::getInstance()->level = 1;
    GameData::getInstance()->money = 450;
    GameData::getInstance()->setMap(this->map);
    GameData::getInstance()->start = Vec2(1, 5);
    GameData::getInstance()->destination = Vec2(10, 5);
    GameData::getInstance()->wave = 1;
    GameData::getInstance()->allWaves = 5;

    uiLayer = UiLayer::createLayer();
    this->addChild(uiLayer, 5);

    entityLayer = EntityLayer::createLayer();
    this->addChild(entityLayer, 1);


    //这里可以改成backgroundLayer，因为不需要修改
    auto background = TMXTiledMap::create("/Scene/level1/level1.tmx");
    background->setAnchorPoint(Vec2(0.5, 0.5));
    background->setPosition(visibleSize / 2);
    this->addChild(background, 0);

    return true;
}
