#include "UiLayer.h"
#include "GameData.h"
#include "ui/CocosGUI.h"
#include "Level1Scene.h"
#include "MapSelectUI.h"

UiLayer* UiLayer::createLayer()
{
	return UiLayer::create();
}

bool UiLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	int money = GameData::getInstance()->money;
	char moneyString[20] = { 0 };
	sprintf(moneyString, "%s%d", "money: ", money);
	auto moneyLabel = Label::createWithTTF(moneyString, "fonts/Marker Felt.ttf", 24);
	moneyLabel->setAnchorPoint(Vec2(0, 0.5));
	moneyLabel->setPosition(60,600);   //放在适当位置
	this->addChild(moneyLabel, 1);

	auto eventListener = EventListenerCustom::create("moneyChanged", [=](EventCustom* event) {
		int changedMoney = (int)(event->getUserData());
		GameData::getInstance()->money += changedMoney;
		int money = GameData::getInstance()->money;
		char moneyString[20] = { 0 };
		sprintf(moneyString, "%s%d", "money:", money);
		moneyLabel->initWithTTF(moneyString, "fonts/Marker Felt.ttf", 24);//更新money的显示
		});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);

	
	//还要添加波数，暂停，菜单按钮

	showWavecounts();

	gameStatusControl();


	//菜单按钮
	auto menuButton = ui::Button::create("res/ui/menu_normal.png","res/ui/menu_selected.png");
	menuButton->setPosition(Vec2(900, 600));
	menuButton->addClickEventListener(CC_CALLBACK_1(UiLayer::menu, this));
	this->addChild(menuButton, 2);

	return true;
}

bool UiLayer::showWavecounts() {

	std::string data = FileUtils::getInstance()->getStringFromFile("language.json");
	document.Parse(data.c_str());
	if (document.HasParseError()) {
		CCLOG("Failed to parse JSON! Error: %s", document.GetParseError());
		return false;
	}

	std::string waveFormat = document.HasMember("waveInfo") ? document["waveInfo"].GetString() : "Wave %d / %d";
	char waveInfo[50];
	std::snprintf(waveInfo, sizeof(waveInfo), waveFormat.c_str(), 0, GameData::getInstance()->allWaves);
	waveLabel = Label::createWithTTF(waveInfo, "fonts/STXINGKA.ttf", 34);
	waveLabel->setPosition(Vec2(540, 600));
	waveLabel->setAnchorPoint(Vec2(0.5, 0.5));
	this->addChild(waveLabel, 1);

	auto waveeventListener = EventListenerCustom::create("waveAdded", [this](EventCustom* event) {
		if (document.HasMember("waveInfo")) {
			std::string waveFormat = document["waveInfo"].GetString();
			char waveInfo[50];
			std::snprintf(waveInfo, sizeof(waveInfo), waveFormat.c_str(), GameData::getInstance()->wave, GameData::getInstance()->allWaves);
			waveLabel->setString(waveInfo);
		}
		});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(waveeventListener, this);
	return true;
}

bool UiLayer::gameStatusControl() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	pauseButton = ui::Button::create("res/Item/Button/resume.png", "res/Item/Button/resume.png");

	pauseButton->setScale(0.6f);
	pauseButton->setPosition(Vec2(visibleSize.width - 200, visibleSize.height - 40));

	pauseButton->addClickEventListener([this](Ref* sender) {
		GameData::getInstance()->ispaused = !GameData::getInstance()->ispaused;
		EventCustom event("togglePause");
		Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

		if (GameData::getInstance()->ispaused) {
			pauseButton->loadTextures("res/Item/Button/pause.png", "res/Item/Button/resume.png");
		}
		else {
			pauseButton->loadTextures("res/Item/Button/resume.png", "res/Item/Button/pause.png");
		}
		});
	this->addChild(pauseButton, 1, "pauseButton");
	return true;
}

void UiLayer::menu(Ref* sender)
{
	//在游戏中点击菜单也会暂停，需要添加
	bool originalState = GameData::getInstance()->ispaused;

	GameData::getInstance()->ispaused = true;
	EventCustom event("togglePause");
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

	//菜单底板
	auto board = Sprite::create("res/ui/options_bg.png");
	board->setPosition(480, 320);
	board->setName("board");
	this->addChild(board, 10);

	
	//继续游戏
	auto continueGameButton = ui::Button::create("res/ui/resume_normal.png","res/ui/resume_selected.png");
	continueGameButton->setPosition(Vec2(467, 428));
	continueGameButton->setName("continueGameButton");

	continueGameButton->addClickEventListener(CC_CALLBACK_1(UiLayer::continueGame, this, originalState));

	this->addChild(continueGameButton, 11);
	
	
	//重新开始
	auto restartGameButton = ui::Button::create("res/ui/restart_normal.png","res/ui/restart_selected.png");
	restartGameButton->setPosition(Vec2(467, 333));
	restartGameButton->setName("restartGameButton");

	restartGameButton->addClickEventListener(CC_CALLBACK_1(UiLayer::restartGame, this));

	this->addChild(restartGameButton, 11);



	//选择关卡
	auto selectLevelButton = ui::Button::create("res/ui/return_normal.png", "res/ui/return_selected.png");
	selectLevelButton->setPosition(Vec2(467, 238));
	selectLevelButton->setName("selectLevelButton");

	//连接到外部选关
	selectLevelButton->addClickEventListener(CC_CALLBACK_1(UiLayer::selectGame, this));

	this->addChild(selectLevelButton, 11);
	
}

void UiLayer::continueGame(Ref* sender, bool originalState)
{
	GameData::getInstance()->ispaused = originalState;
	EventCustom event("togglePause");
	Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

	this->removeChildByName("board");
	this->removeChildByName("continueGameButton");
	this->removeChildByName("restartGameButton");
	this->removeChildByName("selectLevelButton");
}

void UiLayer::restartGame(Ref* sender)
{

	//清除entityLayer的vector
	EventCustom event("clearVector");
	_eventDispatcher->dispatchEvent(&event);


	auto newScene = Level1::createScene();
	Director::getInstance()->replaceScene(newScene);
}


void UiLayer::selectGame(Ref* sender)
{
	Director::getInstance()->replaceScene(MapSelectUI::createScene("res/SkylineScene.csb"));
}