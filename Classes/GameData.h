#ifndef _GAMEDATA_H_
#define _GAMEDATA_H_

#include "cocos2d.h"
#include "Macro.h"

USING_NS_CC;

class GameData
{
public:
    int level;//�ؿ�

    int money;
    int map[MAPWIDTH][MAPHEIGHT] = {};

    //��������յ�ĸ�����꣬�������ص�����
    Vec2 start;
    Vec2 destination;

    int wave=0;//����
    int allWaves=5;//�ܲ���

    bool ispaused = false;
public:
    static GameData* getInstance();

    void setMap(int _map[MAPWIDTH][MAPHEIGHT]);
};
#endif
