#pragma once
#include "Config.h"
#include "GameStart.hpp"
#include "SimpleAudioEngine.h"
#include "cocos2d.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;
using namespace cocostudio;
using namespace cocos2d::ui;
extern ChineseWord Words;
extern cocos2d::SpriteFrameCache* spritecache;
extern CocosDenshion::SimpleAudioEngine* audio;

class LevelSelect :public cocos2d::Scene {
public:
	CREATE_FUNC(LevelSelect);
	static cocos2d::Scene* createScene();
    virtual bool init();

    private:
        Button* _btnLeft;
        Button* _btnRight;
        PageView* _pageView;
        void pageEvent(Ref* page, PageView::EventType type);
        void layputEvent(Layout* layout, Widget::TouchEventType eventType);
        void handlePageTouch(Layout* layout, Widget::TouchEventType eventType);
        void changePage(int index);
        void btnClick(Button* btn, Widget::TouchEventType eventType);
        void initializeUIControls(Node* rootNode);
        void setupPageView(Node* rootNode);
};