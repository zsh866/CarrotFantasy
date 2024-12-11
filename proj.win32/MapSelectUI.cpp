#include "MapSelectUI.h"
#include "LevelSelect.hpp"

USING_NS_CC;
using namespace cocos2d::ui;

Scene* MapSelectUI::createScene(const std::string& csbPath)
{
    auto scene = Scene::create();
    auto layer = MapSelectUI::create(csbPath);
    scene->addChild(layer);
    return scene;
}

bool MapSelectUI::init(const std::string& csbPath) {
    if (!Scene::init()) {
        return false;
    }

    _csbPath = csbPath;

    // ���ض�Ӧ�� CSB �ļ�
    auto rootNode = CSLoader::createNode(_csbPath);
    if (rootNode == nullptr) {
        CCLOG("Failed to load %s", _csbPath.c_str());
        return false;
    }
    this->addChild(rootNode);

    initializeUI(rootNode);
    setupMapDisplay();

    return true;
}


void MapSelectUI::initializeUI(Node* rootNode)
{
    _btnBack = static_cast<Button*>(rootNode->getChildByName("btnBack"));
    _btnStart = static_cast<Button*>(rootNode->getChildByName("btnStart"));
    _mapPreview = static_cast<Sprite*>(rootNode->getChildByName("mapPreview"));

    _btnBack->addTouchEventListener([this](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            onBackButtonClicked();
        }
        });

    _btnStart->addTouchEventListener([this](Ref* sender, Widget::TouchEventType type) {
        if (type == Widget::TouchEventType::ENDED) {
            onStartButtonClicked();
        }
        });
}

void MapSelectUI::setupMapDisplay()
{
    // ���ݵ�ǰѡ��Ĺؿ�����������Ӧ����ʾ����
    LevelType currentLevel = SelectedMapInfo::getInstance().getMapLevel();

    // ������Ը��ݲ�ͬ�ؿ��������ò�ͬ��UIԪ��
    // ���米��ͼƬ����ͼԤ����
}

void MapSelectUI::onBackButtonClicked()
{
    Director::getInstance()->replaceScene(LevelSelect::createScene());
}

void MapSelectUI::onStartButtonClicked()
{
    // ���ݲ�ͬ�ؿ����ʹ�����Ӧ����Ϸ����
    LevelType currentLevel = SelectedMapInfo::getInstance().getMapLevel();
    Scene* gameScene = nullptr;

    switch (currentLevel) {
    case LevelType::SKY:
        CCLOG("gameScene = SkyGameScene::createScene()");
        Director::getInstance()->replaceScene(LevelOne::createScene());
        break;
    case LevelType::DESERT:
        CCLOG("gameScene = DesertGameScene::createScene()");
        break;
    case LevelType::FOREST:
        CCLOG("gameScene = ForestGameScene::createScene()");
        break;
    default:
        CCLOG("δ֪�Ĺؿ�����");
        return;
    }

    if (gameScene) {
        // Director::getInstance()->replaceScene(gameScene);
    }
}