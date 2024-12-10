#include "Bullet.h"
#include "Tower.h"
#include "Monster.h"

USING_NS_CC;

Bullet* Bullet::create(const std::string& filename, Tower* tower, Sprite* _target)
{
	Bullet* ret = new Bullet;
	if (ret && ret->initWithFile(filename)) {
		ret->autorelease();
		ret->init(tower, _target);
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool Bullet::init(Tower* tower, Sprite* _target)
{
	type = tower->type;
	attackNumber = tower->attackNumber[tower->level];
	flySpeed = tower->bulletFlySpeed;
	target = _target;
	return true;
}

void Bullet::update(float dt)
{
	//��ÿһ֡���ӵ����λ�ã�ֱ�������
	
	Vec2 bulletPosition = getPosition();
	Vec2 targetPosition = target->getPosition();

	Vec2 direction = targetPosition - bulletPosition;
	float angle = direction.getAngle();//����

	auto move = MoveBy::create(dt, Vec2(flySpeed * dt * cos(angle), flySpeed * dt * sin(angle)));
	this->runAction(move);
}