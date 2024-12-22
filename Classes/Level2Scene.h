#pragma once
#ifndef __LEVEL2_SCENE_H__
#define __LEVEL2_SCENE_H__

#include "cocos2d.h"
#include "UiLayer.h" 
#include "EntityLayer.h"
#include "Macro.h"
#include "BaseGameScene.h"

class Level2 : public cocos2d::Scene
{
public:
    UiLayer* uiLayer;
    EntityLayer* entityLayer;
    int map[MAPWIDTH][MAPHEIGHT] = {
        {},
        {0,0,6},
        {0,1,1,1},
        {0,1,0,1,5},
        {8,1,16,1,0,1},
        {0,1,0,1,0,1},
        {0,1,0,1,0,1},
        {0,1,0,1,0,1},
        {6,0,0,1,0,1 },
        {0,5,0,1,1,1},
        {0,0,15},
        {}
    };
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

    CREATE_FUNC(Level2);
};

#endif
