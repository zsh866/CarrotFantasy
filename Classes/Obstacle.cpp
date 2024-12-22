#include "Obstacle.h"
#include "GameData.h"
Obstacle* Obstacle::createObstacle(const Vec2& position, std::string type) {
	Obstacle* obstacle = new (std::nothrow)Obstacle;
	if (obstacle && obstacle->init(position, type)) {
		obstacle->autorelease();
		return obstacle;
	}
	CC_SAFE_DELETE(obstacle);
	return nullptr;
}

bool Obstacle::init(const Vec2& position, std::string type) {
	if (!Sprite::initWithSpriteFrameName(type))
		return false;

	hp = fullHp = 50;
	obstacleType = type;
	rewardmoney = 20;

	this->setPosition(position);
	return true;
}

//障碍物损坏时显示残损图片，待添加
void Obstacle::getAttack(int damage) {
	hp -= damage;
	if (hp <= 0)
		isdie = true;
}

std::string Obstacle::getObstacleName(int type) {
	std::string filename = "cloud0";
	filename += type;
	filename += ".png";
	return filename;
}

const std::unordered_map<int, std::string>& Obstacle::getObstacleType() {
	static const std::unordered_map<int, std::string> obstacleMap = {
		{5, "cloud01.png"},{6, "cloud02.png"},{7, "cloud03.png"},
        {8, "cloud04.png"},{9, "cloud05.png"},{10, "cloud06.png"},
        {11, "cloud07.png"},{14, "cloud10.png"},{15, "cloud11.png"},{16, "cloud12.png"}
	};
	return obstacleMap;
}
void Obstacle::dead() {
	EventCustom event("moneyChanged");
	event.setUserData((void*)rewardmoney);
	_eventDispatcher->dispatchEvent(&event);
    this->removeFromParentAndCleanup(true);
    //消失特效
}
