#ifndef _TOWER_H_
#define _TOWER_H_

#include "cocos2d.h"

USING_NS_CC;

class Bullet;
class Monster;

class Tower : public Sprite
{
public:
	std::string type;//��������
	int level;//�ȼ�
	int buildMoney[3];//�����������Ǯ
	int sellMoney[3];//���۽�Ǯ
	int attackNumber[3];//������
	float attackRange;//������Χ
	float attackSpeed;//�����ٶ�
	float bulletFlySpeed;//�ӵ������ٶ�
	bool isChosed;//�Ƿ�ѡ��

	Sprite* target;
public:
	static Tower* buildTower();

	virtual bool init();//��ʼ��

	void sellTower();//����

	void levelUp();//����

	Bullet* aimAndShoot();//��׼�����
};

std::string getTowerFilename(const std::string& type, int level);

#endif
