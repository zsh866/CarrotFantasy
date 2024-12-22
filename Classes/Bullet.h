#ifndef _BULLET_H_
#define _BULLET_H_

#include "cocos2d.h"

class Tower;
class Monster;
class Obstacle;

USING_NS_CC;

class Bullet : public Sprite
{
public:
	std::string type;
	int attackNumber;
	float flySpeed;

	Monster* targetMonster;
	Obstacle* targetObstacle;
public:
	static Bullet* create(const std::string& filename, Tower* tower, Monster* _targetMonster, Obstacle* _targetObstacle);

	virtual bool init(Tower* tower, Monster* _targetMonster, Obstacle* _targetObstacle);

	void update(float dt);
};

#endif