#include "GameData.h"

GameData* GameData::getInstance() 
{
    static GameData instance;
    return &instance;
}

void GameData::setMap(int _map[MAPWIDTH][MAPHEIGHT])
{
    for (int i = 0; i < MAPWIDTH; ++i)
    {
        for (int j = 0; j < MAPHEIGHT; ++j)
        {
            map[i][j] = _map[i][j];
        }
    }
}



