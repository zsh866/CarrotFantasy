#pragma once
#ifndef _MONSTER_H_
#define _MONSTER_H_

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include <vector>
using namespace cocos2d;
using namespace cocos2d::ui;

class Monster:public Sprite{
public:
	int hitpoint; 
	int speed;
	//int attackrange;
	bool dead=false;

    LoadingBar* _hp;

public:

    //���캯������ʼ������
   /* Monster(int type);*/

    //��ʼ���������
    bool init(int type);

    static Monster* createMonster(int type);

    //�ܵ��˺�
    void gethit(int damage);

    //��·���ƶ�
    void movepath(const std::vector<cocos2d::Vec2>& path);

    //�����������
    void isdead();

    void updateHealthBar();

    //void showexplosionScene();   ��ը��Ч

};

#endif
