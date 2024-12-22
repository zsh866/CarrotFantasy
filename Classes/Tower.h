#ifndef _TOWER_H_
#define _TOWER_H_

#include "cocos2d.h"
#include "TowerProperty.h"

USING_NS_CC;

class Bullet;
class Monster;
class Obstacle;

//�����towerproperty�ṹ�壬���㹹�첻ͬ���͵ķ�����
class Tower : public Sprite
{
public:
	TowerProperty towerproperty;
	int level;
	bool isChosed;         //�Ƿ�ѡ��

	Monster* targetMonster;
	Obstacle* targetObstacle;

public:
	static Tower* buildTower(int _type);

	virtual bool init(int _type);//��ʼ��

	void sellTower();//����

	void levelUp();//����

	Bullet* attack();//��׼�����
};

std::string getTowerFilename(const std::string& type, int level);//�����Ҫ������ͼƬ������

#endif
