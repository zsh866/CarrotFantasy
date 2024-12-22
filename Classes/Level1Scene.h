#ifndef __LEVEL1_SCENE_H__
#define __LEVEL1_SCENE_H__

#include "cocos2d.h"
#include "UiLayer.h"
#include "EntityLayer.h"
#include "Macro.h"
#include "BaseGameScene.h"

class Level1 : public cocos2d::Scene
{
public:
    UiLayer* uiLayer;
    EntityLayer* entityLayer;

    ////地图横着为width,竖着为height，现在只有空格和路径
    //int map[MAPWIDTH][MAPHEIGHT] = {
    //    {},
    //    {0,0,1,1,1,1},
    //    {0,0,1},
    //    {0,0,1},
    //    {0,0,1,1},
    //    {0,0,0,1},
    //    {0,0,0,1},
    //    {0,0,1,1},
    //    {0,0,1},
    //    {0,0,1},
    //    {0,0,1,1,1,1},
    //    {}
    //};
    int map[MAPWIDTH][MAPHEIGHT] = {
        {},
        {0,0,1,1,1,1},
        {0,5,1,0},
        {0,6,1,0,16},
        {0,0,1,1},
        {0,7,0,1,0,8},
        {0,14,0,1},
        {0,0,1,1},
        {0,15,1},
        {0,0,1},
        {0,5,1,1,1,1},
        {}
    };
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(Level1);
};

#endif