#ifndef _GAMEDATA_H_
#define _GAMEDATA_H_

#include "cocos2d.h"
#include "Macro.h"

USING_NS_CC;

class GameData
{
public:
    int level;//关卡

    int money;
    int map[MAPWIDTH][MAPHEIGHT] = {};

    //存放起点和终点的格点坐标，不是像素点坐标
    Vec2 start;
    Vec2 destination;

    int wave=0;//波数
    int allWaves=5;//总波数

    bool ispaused = false;
public:
    static GameData* getInstance();

    void setMap(int _map[MAPWIDTH][MAPHEIGHT]);
};
#endif
