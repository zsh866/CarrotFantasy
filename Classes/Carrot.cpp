#include "Carrot.h"
#include "GameData.h"
Carrot* Carrot::createCarrot(const cocos2d::Vec2& position) {
	Carrot* carrot = new (std::nothrow)Carrot;
	if (carrot && carrot->init(position)) {
		carrot->autorelease();
		return carrot;
	}
	CC_SAFE_DELETE(carrot);
	return nullptr;
}

bool Carrot::init(const cocos2d::Vec2& position) {
	if (!Sprite::init())
		return false;

	healthpoint = fullhealthpoint =80;
	rewardmoney = 100;

	auto carrot = Sprite::create("/res/Item/Carrot/Carrot_1.png");

	carrot->setPosition(position);
	this->addChild(carrot);

	return true;
}

void Carrot::getAttack(int damage) {
	healthpoint -=10;
	CCLOG("health %d", healthpoint);
	updateAnim();

	if (healthpoint <= 0) {
		healthpoint = 0;
		return;
	}
}

void Carrot::updateAnim() {
	int imageIndex = std::ceil((1.0f - (float)healthpoint / fullhealthpoint) * 8);
	imageIndex = (fullhealthpoint-healthpoint)/10; // 限制在 1-8 范围内

	// 检查文件是否存在
	std::string filename = "res/Item/Carrot/Carrot_" + std::to_string(imageIndex) + ".png";
	if (!FileUtils::getInstance()->isFileExist(filename)) {
		CCLOG("Error: File %s does not exist!", filename.c_str());
		return;
	}

	// 切换图片纹理
	this->setTexture(filename);

	this->setPosition(Vec2(GameData::getInstance()->destination.x * 80 + 40, GameData::getInstance()->destination.y * 80 + 40));

}



