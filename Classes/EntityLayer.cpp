#include "EntityLayer.h"
#include "GameData.h"
#include "Macro.h"
#include "UiLayer.h"

EntityLayer* EntityLayer::createLayer()
{
	return EntityLayer::create();
}

bool EntityLayer::init()
{
	if (!Layer::init())
	{
		return false;
	}

	visibleSize = Director::getInstance()->getVisibleSize();
	log("%f %f", visibleSize.width, visibleSize.height);



	start = Sprite::create("res/Item/startPoint.png");//路径起点的棋子图标
	start->setPosition(GameData::getInstance()->start.x * 80 + 40, GameData::getInstance()->start.y * 80 + 40);
	this->addChild(start, 1);

	carrot = Carrot::createCarrot(Vec2(GameData::getInstance()->destination.x * 80 + 40, GameData::getInstance()->destination.y * 80 + 40));
	this->addChild(carrot, 1);

	//添加障碍物
	for (int x = 0; x < MAPWIDTH; x++) {
		for (int y = 0; y < MAPHEIGHT; y++) {
			const int posvalue = GameData::getInstance()->map[x][y];
			auto it = Obstacle::getObstacleType().find(posvalue);
			if (it != Obstacle::getObstacleType().end()) {
				std::string type = it->second;
				auto obstacle = Obstacle::createObstacle(Vec2(x * 80 + 40, y * 80 + 40), type);
				if (obstacle) {
					this->addChild(obstacle);
					obstacles.push_back(obstacle);
				}
				else {
					CCLOG("Failed to create obstacle: %s", type.c_str());
				}
			}
		}
	}

	//刷怪，改动后支持5波不同种类的怪物，暂定的是Boss如何完成,以及状态栏显示
	parsePathPoints();

	GameData::getInstance()->wave = 0;
	blank_key_timer(); 

	//暂停
	auto pauselistener = EventListenerCustom::create("togglePause", [this](EventCustom* event) {
		if (!GameData::getInstance()->ispaused) {
			this->scheduleUpdate();
			if (!this->isScheduled("single_wave_update_monster_key")) {
				single_wave_update_monster_timer(GameData::getInstance()->wave);
			}
			if (!this->isScheduled("wave_end_key")) {
				wave_end_timer();
			}
			if (!this->isScheduled("blank_key")) {
				blank_key_timer();
			}

			for (auto& monster : monsters) if (monster) monster->resume();
			for (auto& obstacle : obstacles) if (obstacle) obstacle->resume();
			for (auto& tower : towers) if (tower) tower->resume();
			for (auto& bullet : bullets) if (bullet) bullet->resume();

			_eventDispatcher->resumeEventListenersForTarget(this);
		}
		else {
			this->unscheduleUpdate();
			this->unschedule("single_wave_update_monster_key");
			this->unschedule("wave_end_key");
			this->unschedule("blank_key");

			for (auto& monster : monsters) if (monster) monster->pause();
			for (auto& obstacle : obstacles) if (obstacle) obstacle->pause();
			for (auto& tower : towers) if (tower) tower->pause();
			for (auto& bullet : bullets) if (bullet) bullet->pause();

		}
		});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(pauselistener, this);

	//重新开始游戏，需要清除vector
	auto eventListener = EventListenerCustom::create("clearVector", [=](EventCustom* event) {
		towers.clear();
		monsters.clear();
		bullets.clear();
		obstacles.clear();
		pathPoints.clear();
		});
	_eventDispatcher->addEventListenerWithSceneGraphPriority(eventListener, this);

	auto mouseListener = EventListenerMouse::create();

	mouseListener->onMouseDown = [this](EventMouse* event) {
		if (GameData::getInstance()->ispaused) {
			return;
		}

		auto locationX = event->getCursorX();
		auto locationY = event->getCursorY();
		Vec2 location(locationX, locationY);//鼠标点击位置

		//最上面1行是菜单部分，在entitylayer不应该有响应
		if (locationY > 560)
		{
			return;
		}

		log("%f %f", locationX, locationY);

		//获得鼠标点击位置对应的格点坐标
		int gridX = locationX  / 80;
		int gridY = locationY  / 80;

		Rect buttonRect = Rect(
			visibleSize.width - 200 - 50,
			visibleSize.height - 50 - 25,
			100,
			50
		);

		//检查鼠标是否点击了pause按钮区域
		if (buttonRect.containsPoint(location)) {
			CCLOG("Mouse clicked inside pause button. Ignoring mouse event.");
			return;
		}


		//添加鼠标交互逻辑
		//处理顺序：monster必须最先
		//建造塔，出售塔后需要修改vector
		//建造塔，出售塔后需要修改gamedata的地图


		/*
			如果点击的是怪物
			选中（遍历一遍towers，如果在攻击范围内，把其设为目标）
		*/
		{
			Monster* monster;
			int i;
			for (i = 0; i < monsters.size(); ++i)
			{
				monster = monsters[i];
				Rect boundingBox = monster->getBoundingBox();
				if (boundingBox.containsPoint(location))
				{
					break;
				}
			}
			if (i < monsters.size())//找到了点击的monster
			{
				for (auto& tower : towers)
				{
					Vec2 towerPostion = tower->getPosition();
					Vec2 monsterPostion = monster->getPosition();

					if (towerPostion.distance(monsterPostion) <= tower->towerproperty.attackRange)//如果距离在攻击范围内
					{
						tower->targetMonster = monster;
						tower->targetObstacle = nullptr;
					}
				}
				return;
			}
		}


		/*
			如果点击的是塔
			弹出升级和出售的图标（创建两个按钮）
			获得money值，如果够，升级有色
			如果不够，升级灰色
			点击按钮执行操作
		*/
		if (GameData::getInstance()->map[gridX][gridY] == TOWER)//如果这个格子是塔
		{
			Tower* tower;
			int i;
			for (i = 0; i < towers.size(); ++i)
			{
				tower = towers[i];
				Rect boundingBox = tower->getBoundingBox();
				if (boundingBox.containsPoint(location))
				{
					break;
				}
			}
			if (i < towers.size())//找到了点击的tower
			{
				CCLOG("tower is choosed.");
				popUpgardeAndSellMenu(location, gridX, gridY, tower);
				return;
			}


		}


		/*
			如果点击的是空白
			弹出建造图标（按钮）
			获得money,如果不够，灰色
			如果够，有色
			点击建造，调用auto tower = Tower::buildTower(),并加入towers
			点击出售，调用tower->sellTower(),并移除towers
		*/
		if (GameData::getInstance()->map[gridX][gridY] == EMPTY)
		{
			auto buildmenu = dynamic_cast<Menu*>(this->getChildByName("buildmenu"));
			auto towermenu = dynamic_cast<Menu*>(this->getChildByName("UpgradeAndSellMenu"));
			if (!buildmenu && !towermenu) {
				popBuildMenu(location, gridX, gridY);
				return;
			}
			// 检测是否点击在按钮上
			Menu* menu;
			if (buildmenu)  menu = buildmenu;
			else menu = towermenu;

			bool clickedInsideButton = false;
			for (auto child : menu->getChildren()) {
				auto menuItem = dynamic_cast<MenuItem*>(child);
				if (menuItem && menuItem->isVisible() && menuItem->isEnabled()) {
					Vec2 localLocation = menuItem->convertToNodeSpace(location);
					Rect rect = Rect(0, 0, menuItem->getContentSize().width, menuItem->getContentSize().height);
					if (rect.containsPoint(localLocation)) {
						clickedInsideButton = true;
						break;
					}
				}
			}
			// 如果点击在按钮上，不移除菜单
			if (clickedInsideButton) {
				CCLOG("Mouse clicked inside a menu button.");
				return;
			}
			if (buildmenu)  this->removeChildByName("buildmenu");
			else this->removeChildByName("UpgradeAndSellMenu");
			return;
		}


		/*
			如果点击的是障碍物
			选中（遍历一遍towers，如果在攻击范围内，把其设为目标）
		*/
		if (GameData::getInstance()->map[gridX][gridY] != EMPTY && GameData::getInstance()->map[gridX][gridY] != PATH)
		{
			Obstacle* obstacle;
			int i;
			for (i = 0; i < obstacles.size(); ++i)
			{
				obstacle = obstacles[i];
				Rect boundingBox = obstacle->getBoundingBox();
				if (boundingBox.containsPoint(location))
				{
					break;
				}
			}
			if (i < obstacles.size())//找到了点击的obstacle
			{
				for (auto& tower : towers)
				{
					Vec2 towerPostion = tower->getPosition();
					Vec2 obstaclePostion = obstacle->getPosition();

					if (towerPostion.distance(obstaclePostion) <= tower->towerproperty.attackRange)//如果距离在攻击范围内
					{
						tower->targetMonster = nullptr;
						tower->targetObstacle = obstacle;
					}
				}
				return;
			}
		}

		};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);

	scheduleUpdate();//启用帧更新

	return true;
}

void EntityLayer::update(float dt)//帧更新
{
	std::vector<Monster*> deadMonsters;
	std::vector<Obstacle*>destroyedObstacles;

	//游戏胜利和失败的逻辑还没有完成
	if (GameData::getInstance()->wave > GameData::getInstance()->allWaves && monsters.empty())
	{
		//游戏胜利
		EventCustom event1("win");
		_eventDispatcher->dispatchEvent(&event1);

		EventCustom event2("togglePause");
		GameData::getInstance()->ispaused = true;
		_eventDispatcher->dispatchEvent(&event2);
	}

	if (carrot->healthpoint <= 0)
	{
		//游戏失败
		EventCustom event1("lose");
		_eventDispatcher->dispatchEvent(&event1);

		EventCustom event2("togglePause");
		GameData::getInstance()->ispaused = true;
		_eventDispatcher->dispatchEvent(&event2);
	}

	for (auto& monster : monsters)
	{
		if (monster->getPosition()== Vec2(GameData::getInstance()->destination.x * 80 + 40, GameData::getInstance()->destination.y * 80 + 40)) {

			CCLOG("danger");
			carrot->getAttack(10);
			deadMonsters.push_back(monster);   //萝卜生命值减为0时，应该要结束游戏
			
		}
	}

	for (auto& tower : towers)//处理tower的攻击
	{
		if (tower == nullptr)
			continue;
		if (tower->getNumberOfRunningActions() == 0)//如果tower不在行动
		{
			if (!tower->targetMonster && !tower->targetObstacle)//如果tower没有目标
			{
				for (auto& monster : monsters)
				{
					Vec2 towerPostion = tower->getPosition();
					Vec2 monsterPostion = monster->getPosition();

					if (towerPostion.distance(monsterPostion) <= tower->towerproperty.attackRange)  //如果距离在攻击范围内
					{
						tower->targetMonster = monster;
						auto bullet = tower->attack();
						bullets.push_back(bullet);
						this->addChild(bullet, 3);
						CCLOG("Bullet added to the scene.");
						break;
					}
				}
			}
			else//如果tower有目标
			{
				Vec2 towerPostion = tower->getPosition();
				Vec2 targetPostion;
				if (tower->targetMonster)
				{
					targetPostion = tower->targetMonster->getPosition();
					//CCLOG("Tower targeting monster at position: (%f, %f)", tower->targetMonster->getPosition().x, tower->targetMonster->getPosition().y);
				}
				else
				{
					targetPostion = tower->targetObstacle->getPosition();
				}

				if (towerPostion.distance(targetPostion) <= tower->towerproperty.attackRange)//如果目标在攻击范围内
				{
					//CCLOG("Monster in range of tower.");
					auto bullet = tower->attack();
					bullets.push_back(bullet);
					this->addChild(bullet, 3);
				}
				else   //如果tower和target距离超出范围
				{
					tower->targetMonster = nullptr;
					tower->targetObstacle = nullptr;
				}
			}
		}
	}

	//处理子弹逻辑,改用迭代器，之前的方法erase可能会出现问题
	for (auto it = bullets.begin(); it != bullets.end();) {
		auto bullet = *it;

		if (bullet == nullptr || (bullet->targetMonster == nullptr && bullet->targetObstacle == nullptr)) {
			bullet->removeFromParentAndCleanup(true);
			it = bullets.erase(it);
			continue;
		}

		bullet->update(dt);
		Vec2 bulletPosition = bullet->getPosition();

		if (bulletPosition.x < 0 || bulletPosition.x > visibleSize.width || bulletPosition.y < 0 || bulletPosition.y > visibleSize.height) {
			bullet->removeFromParentAndCleanup(true);
			it = bullets.erase(it);
			continue;
		}

		if (bullet->targetMonster) {
			auto target = bullet->targetMonster;
			if (!target->isdie && bulletPosition.distance(target->getPosition()) <= HITRANGE) {

				auto explosionSprite = Sprite::create("/res/Item/bottle/Bottle_bullet_level1_5.png");
				explosionSprite->setPosition(bullet->getPosition());
				this->addChild(explosionSprite, 2);
				explosionSprite->runAction(Sequence::create(
					FadeOut::create(0.5f),
					RemoveSelf::create(),
					nullptr
				));


				bullet->removeFromParentAndCleanup(true);
				it = bullets.erase(it);

				target->gethit(bullet->attackNumber);
				if (target->isdie) {
					deadMonsters.push_back(target);
				}
				continue;
			}
		}
			if (bullet->targetObstacle) {
				auto target = bullet->targetObstacle;
				if (bulletPosition.distance(target->getPosition()) <= HITRANGE) {

					auto explosionSprite = Sprite::create("/res/Item/bottle/Bottle_bullet_level1_5.png");
					explosionSprite->setPosition(bullet->getPosition());
					this->addChild(explosionSprite, 2);
					explosionSprite->runAction(Sequence::create(
						FadeOut::create(0.5f),
						RemoveSelf::create(),
						nullptr
					));

					bullet->removeFromParentAndCleanup(true);
					it = bullets.erase(it);

					target->getAttack(bullet->attackNumber);
					if (target->isdie) {
						destroyedObstacles.push_back(target);
					}
					continue;
				}
			}
		++it;
	}

	//统一清理死亡的 monster, obstacle和所有相关子弹
	for (auto& target : deadMonsters) {
		for (auto it = bullets.begin(); it != bullets.end();) {
			if ((*it)->targetMonster == target) {
				(*it)->removeFromParentAndCleanup(true);
				it = bullets.erase(it);
			}
			else {
				++it;
			}
		}

		for (auto& tower : towers) {
			if (tower->targetMonster == target) {
				tower->targetMonster = nullptr;
			}
		}
		target->isDead();
		monsters.erase(std::find(monsters.begin(), monsters.end(), target));
	}
	for (auto& target:destroyedObstacles) {
		for (auto it = bullets.begin(); it != bullets.end();) {
			if ((*it)->targetObstacle == target) {
				(*it)->removeFromParentAndCleanup(true);
				it = bullets.erase(it);
			}
			else {
				++it;
			}
		}
		for (auto& tower : towers) {
			if (tower->targetObstacle == target) {
				tower->targetObstacle = nullptr;
			}
		}

		Vec2 pos = target->getPosition();
		int girdX = pos.x / 80;
		int girdY = pos.y / 80;
		GameData::getInstance()->map[girdX][girdY] = EMPTY;
		target->dead();
		obstacles.erase(std::find(obstacles.begin(), obstacles.end(), target));
	}
}

void EntityLayer::single_wave_update_monster_timer(int wave)
{
	this->schedule([this, wave](float dt) {
		int monsterType = wave;
		if (wave > 3) {
			monsterType = (rand() % 3) + 1;
		}

		auto monster = Monster::createMonster(monsterType);
		monster->setPosition(start->getPosition());
		monster->setPointsVector(pathPoints);

		monsters.push_back(monster);
		this->addChild(monster, 2);
		}, 2.0f, "single_wave_update_monster_key");
}



void EntityLayer::wave_end_timer()
{
	this->schedule([this](float dt) {
		this->unschedule("single_wave_update_monster_key");
		this->unschedule("wave_end_key");
		blank_key_timer();
		}, 30.0f, "wave_end_key");
}


void EntityLayer::blank_key_timer()
{
	if (GameData::getInstance()->wave <= GameData::getInstance()->allWaves)
	{
		++GameData::getInstance()->wave;
		this->schedule([this](float dt) {
			single_wave_update_monster_timer(GameData::getInstance()->wave);
			wave_end_timer();

			EventCustom event("waveAdded");
			int wave = GameData::getInstance()->wave;  //获取当前波数
			event.setUserData((void*)&wave);
			_eventDispatcher->dispatchEvent(&event);

			this->unschedule("blank_key");
			}, 10.0f, "blank_key");

	}
}

void EntityLayer::parsePathPoints()
{
	pathPoints.clear();
	int visited[MAPWIDTH][MAPHEIGHT] = { 0 };

	const int dx[4] = { 0, 0, -1, 1 };
	const int dy[4] = { 1, -1, 0, 0 };

	std::queue<Vec2> q;

	Vec2 start = GameData::getInstance()->start;
	q.push(start);
	visited[int(start.x)][int(start.y)] = true;

	while (!q.empty()) {
		Vec2 current = q.front();
		q.pop();

		auto point = Node::create();
		point->setPosition(Vec2(current.x * 80 + 40, current.y * 80 + 40));
		pathPoints.pushBack(point);

		for (int dir = 0; dir < 4; ++dir) {
			int newX = current.x + dx[dir];
			int newY = current.y + dy[dir];

			if (newX >= 0 && newX < MAPWIDTH && newY >= 0 && newY < MAPHEIGHT &&
				GameData::getInstance()->map[newX][newY] == PATH && !visited[newX][newY]) {
				visited[newX][newY] = true;
				q.push(Vec2(newX, newY));
			}
		}
	}

}

//弹出建造按钮
void EntityLayer::popBuildMenu(Vec2 position, int gridX, int gridY) {
	// 如果已存在菜单，移除
	if (this->getChildByName("buildMenu")) {
		this->removeChildByName("buildMenu");
	}

	struct TowerInfo {
		int type;
		std::string normalImage;
		std::string disabledImage;
		int cost;
	};

	std::vector<TowerInfo> towersInfo = {
		{1, "res/Item/bottle/Bottle_build_button.png", "res/Item/bottle/Bottle_build_gray.png", 100},
		{2, "res/Item/Shit/Shit_build_button.png", "res/Item/Shit/Shit_build_gray.png", 160},
		{3, "res/Item/fan/Fan_build_button.png", "res/Item/fan/Fan_build_gray.png", 160}
	};

	// 创建按钮
	cocos2d::Vector<MenuItem*> buildButtons;
	float offsetX = 80;
	float totalWidth = (towersInfo.size() - 1) * offsetX;
	float startX = -totalWidth / 2;

	for (size_t i = 0; i < towersInfo.size(); ++i) {
		const auto& towerInfo = towersInfo[i];
		std::string image = (GameData::getInstance()->money < towerInfo.cost)
			? towerInfo.disabledImage
			: towerInfo.normalImage;

		auto buildButton = MenuItemImage::create(
			image,
			image,
			[=](Ref* sender) {
				CCLOG("Building tower type: %d", towerInfo.type);
				if (GameData::getInstance()->money >= towerInfo.cost) {

					EventCustom event("moneyChanged");
					event.setUserData((void*)-towerInfo.cost);
					_eventDispatcher->dispatchEvent(&event);

					auto tower = Tower::buildTower(towerInfo.type);

					tower->setPosition(gridX * 80 + 40, gridY * 80 + 40);
					towers.push_back(tower);
					this->addChild(tower, 2);
					GameData::getInstance()->map[gridX][gridY] = TOWER;

					CCLOG("Tower built at position: %.2f, %.2f", position.x, position.y);
					this->removeChildByName("buildmenu");
				}
			});

		buildButton->setAnchorPoint(Vec2(0.5, 0.5));
		buildButton->setPosition(Vec2(startX + i * offsetX, 0));

		if (GameData::getInstance()->money < towerInfo.cost) {
			buildButton->setEnabled(false);
		}

		buildButtons.pushBack(buildButton);
	}

	auto menu = Menu::createWithArray(buildButtons);
	menu->setName("buildmenu");
	menu->setPosition(position);

	this->addChild(menu, 10);
}

//升级和出售按钮
void EntityLayer::popUpgardeAndSellMenu(Vec2 position, int gridX, int gridY, Tower* tower) {
	if (this->getChildByName("UpgradeAndSellMenu")) {
		this->removeChildByName("UpgradeAndSellMenu");
	}

	std::string upgradeImage;
	std::string sellImage = "/res/Item/Button/" + tower->towerproperty.type + "_sell_level" + std::to_string(tower->level) + ".png";

	if (tower->level < 3) {
		if (GameData::getInstance()->money >= tower->towerproperty.buildMoney[tower->level])
			upgradeImage = "/res/Item/Button/" + tower->towerproperty.type + "_upgrade_level" + std::to_string(tower->level + 1) + ".png";
		else
			upgradeImage = "/res/Item/Button/" + tower->towerproperty.type + "_upgrade_level" + std::to_string(tower->level + 1) + "_gray.png";
	}
	else {
		upgradeImage = "/res/Item/Button/level_3.png";
	}

	// 创建升级按钮
	auto upgradeButton = MenuItemImage::create(
		upgradeImage,
		upgradeImage,
		[=](Ref* sender) {
			if (tower->level < 3 && GameData::getInstance()->money >= tower->towerproperty.buildMoney[tower->level]) {
				tower->levelUp();
				CCLOG("Tower upgraded to level %d", tower->level);

				this->removeChildByName("UpgradeAndSellMenu");
			}
			else {
				CCLOG("Not enough money or max level reached!");
			}
		}
	);
	upgradeButton->setScale(0.6f);

	if (tower->level >= 3 || GameData::getInstance()->money < tower->towerproperty.buildMoney[tower->level]) {
		upgradeButton->setEnabled(false);
	}
	// 创建出售按钮
	auto sellButton = MenuItemImage::create(
		sellImage,
		sellImage,
		[=](Ref* sender) {
			tower->sellTower();
			towers.erase(std::remove(towers.begin(), towers.end(), tower), towers.end());
			GameData::getInstance()->map[gridX][gridY] = EMPTY;

			this->removeChildByName("UpgradeAndSellMenu");
			CCLOG("Tower sold!");
		}
	);
	sellButton->setScale(0.6f);
	// 布局按钮
	float offsetY = 50.0f;
	upgradeButton->setPosition(Vec2(0, offsetY));
	sellButton->setPosition(Vec2(0, -offsetY));

	cocos2d::Vector<MenuItem*> Buttons;
	Buttons.pushBack(upgradeButton);
	Buttons.pushBack(sellButton);
	auto menu = Menu::createWithArray(Buttons);
	menu->setName("UpgradeAndSellMenu");
	menu->setPosition(position);

	// 添加菜单到场景
	this->addChild(menu, 10);
}

