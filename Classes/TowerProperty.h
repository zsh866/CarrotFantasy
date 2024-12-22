#pragma once
#ifndef _TOWER_PROPERTY_H_
#define _TOWER_PROPERTY_H_

#include <string>
#include <unordered_map>

struct TowerProperty {
    std::string type;          // ��������
    std::string baseTexture;   // ���Ļ�����ͼ
    int buildMoney[3];         // �������
    int sellMoney[3];          // ���ۼ۸�
    int attackNumber[3];       // ������
    float attackRange;         // ������Χ
    float attackSpeed;         // �����ٶ�
    float bulletFlySpeed;      // �ӵ������ٶ�
    int level;                 // ��ǰ�ȼ�
};

extern std::unordered_map<int, TowerProperty> towerProperties;

#endif