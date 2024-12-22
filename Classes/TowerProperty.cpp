#include "TowerProperty.h"
std::unordered_map<int, TowerProperty> towerProperties = {
    {1, {"Bottle", "/res/Item/bottle/Bottle_level1_1.png",
         {100, 180, 320}, {80, 144, 304}, {10, 12, 14}, 300.0f, 1.0,100 }},
    {2, {"Shit", "/res/Item/Shit/Shit_level1_1.png",
         {160, 220, 380}, {96, 176, 336}, {20, 25, 30}, 320.0f, 0.8f, 120.0f}},
    {3, {"Fan", "/res/Item/fan/Fan_level1_1.png",
         {160, 260, 480}, {120, 218, 512}, {15, 18, 21}, 350.0f, 0.6f, 150.0f}}
};
