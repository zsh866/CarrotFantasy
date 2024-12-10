#include "EntityLayer.h"
#include "GameData.h"

Layer* EntityLayer::createLayer()
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

	auto mouseListener = EventListenerMouse::create();

	mouseListener->onMouseDown = [this](EventMouse* event) {

		auto locationX = event->getCursorX();
		auto locationY = event->getCursorY();
		Vec2 location(locationX, locationY);//�����λ��

		//�����꽻���߼�

		/*
			������������
			���������ͳ��۵�ͼ�꣨����������ť��
			���moneyֵ���������������ɫ
			���������������ɫ
			�����ťִ�в���
		*/
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
			//����
		}
		


		/*
			���������ǿհ�
			��������ͼ�꣨��ť��
			���money,�����������ɫ
			���������ɫ
			������죬����auto tower = Tower::buildTower(),������towers
			������ۣ�����tower->sellTower(),���Ƴ�towers
		*/


		

		/*
			�����������ϰ���
			���������ǹ���
			ѡ�У�����һ��towers������ڹ�����Χ�ڣ�������ΪĿ�꣩
		*/

		};

	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);

	EventCustom event("moneyChange");//�Զ����¼� ��Ǯ�ı���
	/*
	�����ں��ʵط�����
	//���ܵ���ʱ���ַ��¼�
	{
		event.setUserData((void*)100);//��Ϊ���ܵ�monster��õĽ�Ǯ
		_eventDispatcher->dispatchEvent(&event);
	}
	*/

	scheduleUpdate();//����֡����

	return true;
}

void EntityLayer::update(float dt)//֡����
{
	for (auto& monster : monsters)//����monster�ƶ�
	{
		//monster->movePath();
	}

	for (auto& tower : towers)//����tower�Ĺ���
	{
		if (tower->getNumberOfRunningActions() == 0)//���tower�����ж�
		{
			if (!tower->target)//���towerû��Ŀ��
			{
				for (auto& monster : monsters)
				{
					Vec2 towerPostion = tower->getPosition();
					Vec2 monsterPostion = monster->getPosition();

					if (towerPostion.distance(monsterPostion) <= tower->attackRange)//��������ڹ�����Χ��
					{
						tower->target = monster;
						auto bullet = tower->aimAndShoot();
						bullets.push_back(bullet);
						this->addChild(bullet, 3);
					}
				}
			}
			else//���tower��Ŀ��
			{
				Vec2 towerPostion = tower->getPosition();
				Vec2 targetPostion = tower->target->getPosition();

				if (towerPostion.distance(targetPostion) <= tower->attackRange)//���Ŀ���ڹ�����Χ��
				{
					auto bullet = tower->aimAndShoot();
					bullets.push_back(bullet);
					this->addChild(bullet, 3);
				}
				else //���tower��target������Χ
				{
					tower->target = nullptr;
				}
			}
		}
	}

	for (auto& bullet : bullets)//����bullet���ƶ�
	{
		bullet->update(dt);

		Vec2 bulletPosition = bullet->getPosition();
		
		if (bulletPosition.x < 0 || bulletPosition.x > visibleSize.width || bulletPosition.y < 0 || bulletPosition.y > visibleSize.height)//�ӵ�����
		{
			bullet->removeFromParentAndCleanup(true);
			bullets.erase(std::find(bullets.begin(), bullets.end(), bullet));
			continue;
		}

		auto target = bullet->target;
		Vec2 targetPosition = target->getPosition();

		if (bulletPosition.distance(targetPosition) <= hitRange)//�������С����ֵ����Ϊ�ӵ�����
		{
			//�����ӵ�������Ч
			bullet->removeFromParentAndCleanup(true);
			bullets.erase(std::find(bullets.begin(), bullets.end(), bullet));
			//����target,������ˣ�Ҫ������vector
		}
	}
	
}