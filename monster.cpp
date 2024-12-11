#pragma once

#include "monster.h"
#include "Carrot.h"
USING_NS_CC;

Monster* Monster::createMonster(int type) {
    Monster* monster = new (std::nothrow)Monster;
    
    if (monster && monster->init(type)) {
        monster->autorelease();
        return monster;
    }
    CC_SAFE_DELETE(monster);
    return nullptr;
}

bool Monster::init(int type) {
    if (!Sprite::init()) {
        return false;
    }

    //��ʱ֧��һ������
    hitpoint = 100;
    speed = 10;

    //��������
    auto monster = Sprite::create("monster.png");
    this->addChild(monster);

    //��Ҫ���ѡ�еĹ��ܣ���obstacle����˼·���ƣ���δ���
    
    //Ѫ������ʼ��
    _hp = LoadingBar::create("health_bar_background.png");  
    _hp->setDirection(LoadingBar::Direction::LEFT);  
    _hp->setPercent(100);  

    _hp->setAnchorPoint(Vec2(0.5f, 1.0f));  
    _hp->setPosition(monster->getPosition() + Vec2(0, 30));  
    _hp->setVisible(false);          
    this->addChild(_hp);  

    return true;
}

void Monster::gethit(int damage) {
    hitpoint -= damage;

    if (hitpoint <= 0)
        isdead();
    else {
        updateHealthBar();
        //showexplosionScene();
    }

}

void Monster::movepath(const std::vector<Vec2>& path) {
    if (path.empty()) return;  // ���·���Ƿ�Ϊ��

    this->stopAllActions();                           
    Vector<FiniteTimeAction*> actions;                
    Vec2 currentposition = this->getPosition();       

    for (const Vec2&position:path) {
        auto distance=position-currentposition;
        currentposition=position;

        float duration=distance.length()/speed; 

        auto moveAction=MoveTo::create(duration,position);
        actions.pushBack(moveAction);
    }

    //ִ���ƶ���������
    this->runAction(Sequence::create(actions));

    //�����յ�֮��
    auto reachedEndCallback = CallFunc::create([this]() {
        if (this->hitpoint > 0) {
            //�����ܲ�Ѫ��

        }
        this->removeFromParent();
        });
    actions.pushBack(reachedEndCallback);

    // ִ�ж�������
    this->runAction(Sequence::create(actions));

}
void Monster::updateHealthBar() {
    //�������ڵ�Ѫ��������ʾ������1-2s��������ƶ�

    _hp->setVisible(true);
    _hp->setPercent(hitpoint);

    this->runAction(Sequence::create(
        DelayTime::create(2.0f), 
        FadeOut::create(0.5f),   
        CallFunc::create([this]() {
            _hp->setVisible(false); 
            }),
        nullptr
    ));
}

void Monster::isdead() {
    //������������
    auto deadAnim=Animation::create();
    for (int i=1;i<=3;++i) 
        deadAnim->addSpriteFrameWithFile("dead_"+std::to_string(i)+ ".png");
    deadAnim->setDelayPerUnit(0.1f);
    auto* deadaction = Animate::create(deadAnim);
    auto* func = CallFunc::create([this]() { this->removeFromParentAndCleanup(true);});
    this->runAction(Sequence::create(deadaction,func,nullptr));

}



