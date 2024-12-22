#pragma once
#ifndef _TOWER_PROPERTY_H_
#define _TOWER_PROPERTY_H_

#include <string>
#include <unordered_map>

struct TowerProperty {
    std::string type;          // 塔的类型
    std::string baseTexture;   // 塔的基础贴图
    int buildMoney[3];         // 建造费用
    int sellMoney[3];          // 出售价格
    int attackNumber[3];       // 攻击力
    float attackRange;         // 攻击范围
    float attackSpeed;         // 攻击速度
    float bulletFlySpeed;      // 子弹飞行速度
    int level;                 // 当前等级
};

extern std::unordered_map<int, TowerProperty> towerProperties;

#endif