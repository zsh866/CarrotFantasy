#include "Tower.h"
#include "Bullet.h"
#include "Monster.h"
#include "Obstacle.h"
#include "GameData.h"

USING_NS_CC;

Tower* Tower::buildTower(int _type)
{
	auto tower = new Tower;
	if (tower && tower->init(_type))
	{
		tower->autorelease();
		return tower;
	}
	CC_SAFE_DELETE(tower);
	return nullptr;
}

bool Tower::init(int _type)
{
	auto it = towerProperties.find(_type);
	if (it == towerProperties.end()) {
		CCLOG("Invalid tower type: %d", _type);
		return false;
	}

	towerproperty = it->second;
	level = 1;

	if (!Sprite::initWithFile(towerproperty.baseTexture)) {
		return false;
	}

	isChosed = false;
	targetMonster = nullptr;
	targetObstacle = nullptr;

	return true;
}

void Tower::sellTower()
{

	EventCustom event("moneyChanged");
	event.setUserData((void*)towerproperty.sellMoney[level-1]);
	_eventDispatcher->dispatchEvent(&event);

	this->removeFromParentAndCleanup(true);
}

void Tower::levelUp()
{
	if (level < 3)
	{
		EventCustom event("moneyChanged");
		event.setUserData((void*)-towerproperty.buildMoney[level]);
		_eventDispatcher->dispatchEvent(&event);

		++level;
		std::string newTexture = "/res/Item/" + towerproperty.type + "/" + towerproperty.type + "_level" + std::to_string(level) + "_1.png";
		this->setTexture(newTexture);
		CCLOG("Tower upgrade to %d", level);
		//this->setTexture(getTowerFilename(towerproperty.type, towerproperty.level));//升级，换贴图即可
	}
}

Bullet* Tower::attack()
{
	CCLOG("Tower is trying to attack.");

	Vec2 towerPosition = getPosition();
	Vec2 targetPosition;

	if (!targetMonster && !targetObstacle) {
		CCLOG("No target available for attack.");
		return nullptr;
	}

	if (targetMonster)
	{
		targetPosition = targetMonster->getPosition();
	}
	else
	{
		targetPosition = targetObstacle->getPosition();
	}

	Vec2 direction = targetPosition - towerPosition;
	float angle = CC_RADIANS_TO_DEGREES(direction.getAngle());//角度

	auto rotate = RotateTo::create(0.1,360 - angle);
	auto delay = DelayTime::create(0.4);
	this->runAction(Sequence::create(rotate, delay, nullptr));

	//需要补充发射动画
	std::string bulletTexture;
	if (level <= 2)
	{
		bulletTexture = "/res/Item/" + towerproperty.type + "/" + towerproperty.type + "_bullet_level" + std::to_string(level) + "_1.png";
	}
	else
	{
		bulletTexture = "/res/Item/" + towerproperty.type + "/" + towerproperty.type + "_bullet_level" + std::to_string(level - 1) + "_1.png";
	}
	CCLOG("Bullet Texture Path: %s", bulletTexture.c_str());
	auto bullet = Bullet::create(bulletTexture, this, targetMonster, targetObstacle);

	if (bullet) {
		bullet->setPosition(this->getPosition());
		CCLOG("Bullet creation failed for texture: %s", bulletTexture.c_str());
	}
	else
		CCLOG("Bullet creation failed for texture: %s", bulletTexture.c_str());
	return bullet;
}

std::string getTowerFilename(const std::string& type, int level)
{
	std::string filename = type;
	filename += char(level + '0');
	filename += ".png";
	return filename;
}