#include "Monster.h"
#include  "GameData.h"

//fat_green02.png
//fly_blue01.png
//fly_yellow01.png

Monster* Monster::createMonster(int _type)
{
    Monster* monster = new (std::nothrow) Monster;
    if (monster && monster->init(_type))
    {
        monster->autorelease();
        return monster;
    }
    CC_SAFE_DELETE(monster);
    return nullptr;
}

bool Monster::init(int _type) {
    if (!MonsterBase::init())
        return false;

    type = _type;
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("res/Item/Monsters01-hd.plist");

    switch (type) {
    case(1): {
        initWithSpriteFrameName("fat_green01.png");
        initAnimations("fat_green");
        setRunSpeed(1.0f); //设置较慢的速度
        rewardmoney = 8;
        break;
    }

    case(2): {
        initWithSpriteFrameName("fly_blue01.png");
        initAnimations("fly_blue");
        setRunSpeed(2.5f); //设置较快的速度
        rewardmoney = 10;
        break;
    }

    case(3): {
        initWithSpriteFrameName("fly_yellow01.png");
        initAnimations("fly_yellow");
        setRunSpeed(2.0f); //设置中等速度
        rewardmoney = 14;
        break;
    }

    default:
        break;
    }
    // 添加血量条
    hpBar = LoadingBar::create("res/Item/health_bar_background.jpg");
    hpBar->setPercent(100);
    hpBar->setPosition(Vec2(0, getContentSize().height / 2 + 10));
    hpBar->setVisible(false);
    addChild(hpBar);

    return true;
}

void Monster::initAnimations(std::string filename) {
    auto runAnim = createAnimation(filename,2, 0.1f);
    AnimationCache::getInstance()->addAnimation(runAnim, "runright");
    AnimationCache::getInstance()->addAnimation(runAnim, "runleft");
    AnimationCache::getInstance()->addAnimation(runAnim, "runup");
    AnimationCache::getInstance()->addAnimation(runAnim, "rundown");
}

void Monster::gethit(int damage) {
    hitpoint -= damage;
    updateHealthBar();
    if (hitpoint <= 0) {
        //isDead();
        isdie= true;
        return;
    }
}

void Monster::updateHealthBar() {
    //显示一段时间
    hpBar->setPercent(hitpoint);
}

void Monster::isDead() {
    if (this->getPosition() < Vec2(60 + GameData::getInstance()->destination.x * 80 + 40, 40 + GameData::getInstance()->destination.y * 80 + 40)) {
        EventCustom event("moneyChanged");
        event.setUserData((void*)rewardmoney);
        _eventDispatcher->dispatchEvent(&event);
    }
    this->removeFromParentAndCleanup(true);

}