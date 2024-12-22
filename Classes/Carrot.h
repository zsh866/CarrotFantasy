#ifndef _CARROT_H_
#define _CARROT_H_

#include "cocos2d.h"

USING_NS_CC;

class Carrot : public cocos2d::Sprite {
public:
	int healthpoint;      //生命值
	int fullhealthpoint;  //最大生命值
	int rewardmoney;      //奖励金币
public:
	static Carrot* createCarrot(const cocos2d::Vec2& position);
	virtual bool init(const cocos2d::Vec2& position);
	void getAttack(int damage);
	void updateAnim();
	//void shakeAnim();

};

#endif // _CARROT_H_
