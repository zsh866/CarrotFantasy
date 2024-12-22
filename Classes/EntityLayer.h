#ifndef _TOWERLAYER_H_
#define _TOWERLAYER_H_

#include "cocos2d.h"
#include "Tower.h"
#include "Bullet.h"
#include "Monster.h"
#include "Obstacle.h"
#include "Carrot.h"

USING_NS_CC;

#define HITRANGE 10

class EntityLayer : public Layer
{
public:
	Sprite* start;

	std::vector<Tower*> towers;
	std::vector<Bullet*> bullets;
	std::vector<Monster*> monsters;
	std::vector<Obstacle*> obstacles;
	Carrot* carrot;

	Vector<Node*> pathPoints;  //��ӣ���¼·��

	Size visibleSize;
public:
	static EntityLayer* createLayer();

	virtual bool init();

	virtual void update(float dt) override;

	CREATE_FUNC(EntityLayer);

	void single_wave_update_monster_timer(int wave);

	void wave_end_timer();

	void blank_key_timer();

	void parsePathPoints();  //��·������monster

	void popBuildMenu(Vec2 position, int gridX, int gridY);  //�������찴ť

	void popUpgardeAndSellMenu(Vec2 position, int gridX, int gridY, Tower* tower);  //���������ͳ��۰�ť
};

#endif