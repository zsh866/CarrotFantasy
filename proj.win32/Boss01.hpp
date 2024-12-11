#pragma once
#include "MonsterBase.hpp"
class Monster : public MonsterBase {
public:
    CREATE_FUNC(Monster);
    bool init() override {
        if (!MonsterBase::init()) {
            return false;
        }

        // ���ؾ���ͼ��
        SpriteFrameCache::getInstance()->addSpriteFramesWithFile("/res/Item/Monsters01-hd.plist");

        // ʹ�õ�һ֡��ʼ������
        initWithSpriteFrameName("boss_big01.png");

        // ��ʼ������
        initAnimations();

        // �����ƶ��ٶȣ�������Ҫ������
        setRunSpeed(2.0f);

        // ��ӷ���仯�Ķ�ʱ��
        schedule(CC_SCHEDULE_SELECTOR(MonsterBase::changeDirection), 0.1f);

        return true;
    }
    void playAnimation(const std::string& animName) {
        // ֹͣ��ǰ����
        stopAllActions();

        // �����¶���
        auto animation = AnimationCache::getInstance()->getAnimation(animName);
        if (animation) {
            runAction(RepeatForever::create(Animate::create(animation)));
        }
    }
private:
    void initAnimations() {
        // ����plist�е�������������
        // ����boss_big01.png��boss_big08.png��һ�鶯��
        auto bossAnim = createAnimation("boss_big", 8, 0.1f);
        AnimationCache::getInstance()->addAnimation(bossAnim, "boss_run");

        // ��Ӹ�������Ķ���
        auto rightAnim = createAnimation("boss_big", 8, 0.1f);
        AnimationCache::getInstance()->addAnimation(rightAnim, "runright");

        auto leftAnim = createAnimation("boss_big", 8, 0.1f);
        AnimationCache::getInstance()->addAnimation(leftAnim, "runleft");

        auto upAnim = createAnimation("boss_big", 8, 0.1f);
        AnimationCache::getInstance()->addAnimation(upAnim, "runup");

        auto downAnim = createAnimation("boss_big", 8, 0.1f);
        AnimationCache::getInstance()->addAnimation(downAnim, "rundown");
    }
};