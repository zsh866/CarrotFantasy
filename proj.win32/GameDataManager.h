#pragma once
#include "cocos2d.h"
#include <sqlite3.h>

class GameDataManager {
private:
    static GameDataManager* _instance;
    sqlite3* _db;
    bool _initialized;

    GameDataManager()
        : _db(nullptr)
        , _initialized(false)
    {
    }

public:
    static GameDataManager& getInstance()
    {
        static GameDataManager instance;
        return instance;
    }

    // ��ʼ�����ݿ�
    bool init();

    // �ؿ��������
    bool saveLevelProgress(int levelType, int mapIndex, int stars);
    int getLevelStars(int levelType, int mapIndex);
    bool isLevelUnlocked(int levelType, int mapIndex);

    // ��Ϸ���ȱ�����ָ�
    bool saveGameState(int levelType, int mapIndex, const std::string& gameState);
    std::string loadGameState(int levelType, int mapIndex);
    void clearGameState(int levelType, int mapIndex);

    // ��Ƶ����
    bool saveSoundSettings(bool bgmEnabled, bool sfxEnabled, float bgmVolume, float sfxVolume);
    bool getBgmEnabled();
    bool getSfxEnabled();
    float getBgmVolume();
    float getSfxVolume();

    // ������Դ
    void cleanup();

    // ��ֹ����
    GameDataManager(const GameDataManager&) = delete;
    GameDataManager& operator=(const GameDataManager&) = delete;
};