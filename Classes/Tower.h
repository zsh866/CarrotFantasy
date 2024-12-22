#ifndef _TOWER_H_
#define _TOWER_H_

#include "cocos2d.h"
#include "TowerProperty.h"

USING_NS_CC;

class Bullet;
class Monster;
class Obstacle;

//添加了towerproperty结构体，方便构造不同类型的防御塔
class Tower : public Sprite
{
public:
	TowerProperty towerproperty;
	int level;
	bool isChosed;         //是否被选中

	Monster* targetMonster;
	Obstacle* targetObstacle;

public:
	static Tower* buildTower(int _type);

	virtual bool init(int _type);//初始化

	void sellTower();//出售

	void levelUp();//升级

	Bullet* attack();//瞄准并射击
};

std::string getTowerFilename(const std::string& type, int level);//获得需要的塔的图片的名称

#endif
