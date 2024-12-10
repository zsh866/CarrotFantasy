#include "UiLayer.h"
#include "GameData.h"

Layer* UiLayer::createLayer()
{
	return UiLayer::create();
}

bool UiLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	int money = GameData::getInstance()->getMoney();
	char moneyString[10] = { 0 };
	sprintf(moneyString, "%d", money);
	auto moneyLabel = Label::createWithTTF(moneyString, "fonts/Marker Felt.ttf", 24);
	//moneyLabel->setPosition();�����ʵ�λ��

	this->addChild(moneyLabel);

	auto eventListener = EventListenerCustom::create("moneyChange", [=](EventCustom* event) {
		int money = (int)(event->getUserData());
		char moneyString[10] = { 0 };
		sprintf(moneyString, "%d", money);
		moneyLabel->initWithTTF(moneyString, "fonts/Marker Felt.ttf", 24);//����money����ʾ
		});

	_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);


	//��Ҫ����˳�����ͣ�ȵȰ�ť


	return true;
}
