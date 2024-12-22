#pragma once
#include "MonsterBase.h"
#include "ui/UILoadingBar.h"

USING_NS_CC;
using namespace ui;

class Monster : public MonsterBase {
public:
    static Monster* createMonster(int _type);
    bool init(int _type);
    void gethit(int damage);

public:
    int hitpoint = 100;
    bool isdie = false;           // «∑ÒÀ¿Õˆ
    LoadingBar* hpBar;
    int rewardmoney = 20;
    int type;

    void initAnimations(std::string filename);
    void updateHealthBar();
    void isDead();
};



