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



	start = Sprite::create("res/Item/startPoint.png");//·����������ͼ��
	start->setPosition(GameData::getInstance()->start.x * 80 + 40, GameData::getInstance()->start.y * 80 + 40);
	this->addChild(start, 1);

	carrot = Carrot::createCarrot(Vec2(GameData::getInstance()->destination.x * 80 + 40, GameData::getInstance()->destination.y * 80 + 40));
	this->addChild(carrot, 1);

	//����ϰ���
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

	//ˢ�֣��Ķ���֧��5����ͬ����Ĺ���ݶ�����Boss������,�Լ�״̬����ʾ
	parsePathPoints();

	GameData::getInstance()->wave = 0;
	blank_key_timer(); 

	//��ͣ
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

	//���¿�ʼ��Ϸ����Ҫ���vector
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
		Vec2 location(locationX, locationY);//�����λ��

		//������1���ǲ˵����֣���entitylayer��Ӧ������Ӧ
		if (locationY > 560)
		{
			return;
		}

		log("%f %f", locationX, locationY);

		//��������λ�ö�Ӧ�ĸ������
		int gridX = locationX  / 80;
		int gridY = locationY  / 80;

		Rect buttonRect = Rect(
			visibleSize.width - 200 - 50,
			visibleSize.height - 50 - 25,
			100,
			50
		);

		//�������Ƿ�����pause��ť����
		if (buttonRect.containsPoint(location)) {
			CCLOG("Mouse clicked inside pause button. Ignoring mouse event.");
			return;
		}


		//�����꽻���߼�
		//����˳��monster��������
		//������������������Ҫ�޸�vector
		//������������������Ҫ�޸�gamedata�ĵ�ͼ


		/*
			���������ǹ���
			ѡ�У�����һ��towers������ڹ�����Χ�ڣ�������ΪĿ�꣩
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
			if (i < monsters.size())//�ҵ��˵����monster
			{
				for (auto& tower : towers)
				{
					Vec2 towerPostion = tower->getPosition();
					Vec2 monsterPostion = monster->getPosition();

					if (towerPostion.distance(monsterPostion) <= tower->towerproperty.attackRange)//��������ڹ�����Χ��
					{
						tower->targetMonster = monster;
						tower->targetObstacle = nullptr;
					}
				}
				return;
			}
		}


		/*
			������������
			���������ͳ��۵�ͼ�꣨����������ť��
			���moneyֵ���������������ɫ
			���������������ɫ
			�����ťִ�в���
		*/
		if (GameData::getInstance()->map[gridX][gridY] == TOWER)//��������������
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
			if (i < towers.size())//�ҵ��˵����tower
			{
				CCLOG("tower is choosed.");
				popUpgardeAndSellMenu(location, gridX, gridY, tower);
				return;
			}


		}


		/*
			���������ǿհ�
			��������ͼ�꣨��ť��
			���money,�����������ɫ
			���������ɫ
			������죬����auto tower = Tower::buildTower(),������towers
			������ۣ�����tower->sellTower(),���Ƴ�towers
		*/
		if (GameData::getInstance()->map[gridX][gridY] == EMPTY)
		{
			auto buildmenu = dynamic_cast<Menu*>(this->getChildByName("buildmenu"));
			auto towermenu = dynamic_cast<Menu*>(this->getChildByName("UpgradeAndSellMenu"));
			if (!buildmenu && !towermenu) {
				popBuildMenu(location, gridX, gridY);
				return;
			}
			// ����Ƿ����ڰ�ť��
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
			// �������ڰ�ť�ϣ����Ƴ��˵�
			if (clickedInsideButton) {
				CCLOG("Mouse clicked inside a menu button.");
				return;
			}
			if (buildmenu)  this->removeChildByName("buildmenu");
			else this->removeChildByName("UpgradeAndSellMenu");
			return;
		}


		/*
			�����������ϰ���
			ѡ�У�����һ��towers������ڹ�����Χ�ڣ�������ΪĿ�꣩
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
			if (i < obstacles.size())//�ҵ��˵����obstacle
			{
				for (auto& tower : towers)
				{
					Vec2 towerPostion = tower->getPosition();
					Vec2 obstaclePostion = obstacle->getPosition();

					if (towerPostion.distance(obstaclePostion) <= tower->towerproperty.attackRange)//��������ڹ�����Χ��
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

	scheduleUpdate();//����֡����

	return true;
}

void EntityLayer::update(float dt)//֡����
{
	std::vector<Monster*> deadMonsters;
	std::vector<Obstacle*>destroyedObstacles;

	//��Ϸʤ����ʧ�ܵ��߼���û�����
	if (GameData::getInstance()->wave > GameData::getInstance()->allWaves && monsters.empty())
	{
		//��Ϸʤ��
		EventCustom event1("win");
		_eventDispatcher->dispatchEvent(&event1);

		EventCustom event2("togglePause");
		GameData::getInstance()->ispaused = true;
		_eventDispatcher->dispatchEvent(&event2);
	}

	if (carrot->healthpoint <= 0)
	{
		//��Ϸʧ��
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
			deadMonsters.push_back(monster);   //�ܲ�����ֵ��Ϊ0ʱ��Ӧ��Ҫ������Ϸ
			
		}
	}

	for (auto& tower : towers)//����tower�Ĺ���
	{
		if (tower == nullptr)
			continue;
		if (tower->getNumberOfRunningActions() == 0)//���tower�����ж�
		{
			if (!tower->targetMonster && !tower->targetObstacle)//���towerû��Ŀ��
			{
				for (auto& monster : monsters)
				{
					Vec2 towerPostion = tower->getPosition();
					Vec2 monsterPostion = monster->getPosition();

					if (towerPostion.distance(monsterPostion) <= tower->towerproperty.attackRange)  //��������ڹ�����Χ��
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
			else//���tower��Ŀ��
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

				if (towerPostion.distance(targetPostion) <= tower->towerproperty.attackRange)//���Ŀ���ڹ�����Χ��
				{
					//CCLOG("Monster in range of tower.");
					auto bullet = tower->attack();
					bullets.push_back(bullet);
					this->addChild(bullet, 3);
				}
				else   //���tower��target���볬����Χ
				{
					tower->targetMonster = nullptr;
					tower->targetObstacle = nullptr;
				}
			}
		}
	}

	//�����ӵ��߼�,���õ�������֮ǰ�ķ���erase���ܻ��������
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

	//ͳһ���������� monster, obstacle����������ӵ�
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
			int wave = GameData::getInstance()->wave;  //��ȡ��ǰ����
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

//�������찴ť
void EntityLayer::popBuildMenu(Vec2 position, int gridX, int gridY) {
	// ����Ѵ��ڲ˵����Ƴ�
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

	// ������ť
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

//�����ͳ��۰�ť
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

	// ����������ť
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
	// �������۰�ť
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
	// ���ְ�ť
	float offsetY = 50.0f;
	upgradeButton->setPosition(Vec2(0, offsetY));
	sellButton->setPosition(Vec2(0, -offsetY));

	cocos2d::Vector<MenuItem*> Buttons;
	Buttons.pushBack(upgradeButton);
	Buttons.pushBack(sellButton);
	auto menu = Menu::createWithArray(Buttons);
	menu->setName("UpgradeAndSellMenu");
	menu->setPosition(position);

	// ��Ӳ˵�������
	this->addChild(menu, 10);
}

