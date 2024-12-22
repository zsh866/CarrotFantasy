#ifndef _OBSTACLE_H_
#define _OBSTACLE_H_

#include "cocos2d.h"

USING_NS_CC;

class Obstacle : public cocos2d::Sprite {
public:
	std::string obstacleType;
	int rewardmoney;
	int hp;
	int fullHp;
	bool isdie = false;
public:
	static Obstacle* createObstacle(const Vec2& position, std::string type);
	virtual bool init(const Vec2& position, std::string type);
	void getAttack(int damage);
	void dead();
	std::string getObstacleName(int type);
	static const std::unordered_map<int, std::string>& getObstacleType();
};


#endif


