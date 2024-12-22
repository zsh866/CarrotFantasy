#include "Bullet.h"
#include "Tower.h"
#include "Monster.h"
#include "Obstacle.h"

USING_NS_CC;


//添加了很多调试语句，可删除
Bullet* Bullet::create(const std::string& filename, Tower* tower, Monster* _targetMonster, Obstacle* _targetObstacle)
{
	CCLOG("Bullet::create called with texture: %s", filename.c_str());

	Bullet* ret = new Bullet;
	if (ret && ret->initWithFile(filename)) {
		ret->autorelease();
		if (ret->init(tower, _targetMonster, _targetObstacle)) {
			CCLOG("Bullet initialized successfully.");
			return ret;
		}
		else {
			CCLOG("Bullet initialization failed.");
		}
	}
	else {
		CCLOG("Bullet::initWithFile failed for texture: %s", filename.c_str());
	}

	CC_SAFE_DELETE(ret);
	return nullptr;
}


bool Bullet::init(Tower* tower, Monster* _targetMonster, Obstacle* _targetObstacle)
{
	if (!tower) {
		CCLOG("Tower pointer is null.");
		return false;
	}

	type = tower->towerproperty.type;
	attackNumber = tower->towerproperty.attackNumber[tower->level - 1];
	flySpeed = tower->towerproperty.bulletFlySpeed;

	CCLOG("Bullet properties: Type=%s, AttackNumber=%d, FlySpeed=%f",
		type.c_str(), attackNumber, flySpeed);

	targetMonster = _targetMonster;
	targetObstacle = _targetObstacle;

	if (targetMonster) {
		CCLOG("Target Monster assigned.");
	}
	else if (targetObstacle) {
		CCLOG("Target Obstacle assigned.");
	}
	else {
		CCLOG("No valid target assigned.");
		return false;
	}

	return true;
}

void Bullet::update(float dt)
{
	//在每一帧，子弹检查位置，直射向目标

	Vec2 bulletPosition = getPosition();
	Vec2 targetPosition;

	if (targetMonster == nullptr && targetObstacle == nullptr)
	{
		this->removeFromParentAndCleanup(true);
		return;
	}

	if (targetMonster)
	{
		if (!targetMonster->getParent()) {
			targetMonster = nullptr;
			this->removeFromParentAndCleanup(true);
			return;
		}

		targetPosition = targetMonster->getPosition();
	}
	else
	{
		targetPosition = targetObstacle->getPosition();
	}


	Vec2 direction = targetPosition - bulletPosition;
	float angle = direction.getAngle();//单位是弧度

	auto move = MoveBy::create(dt, Vec2(flySpeed * dt * cos(angle), flySpeed * dt * sin(angle)));
	this->runAction(move);
}