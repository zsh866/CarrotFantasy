#ifndef _UILAYER_H_
#define _UILAYER_H_

#include "cocos2d.h"
#include "json/document.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

class UiLayer : public Layer
{
private:
	rapidjson::Document document;
	Label* waveLabel;
public:
	cocos2d::ui::Button* pauseButton;

	static UiLayer* createLayer();

	virtual bool init();

	bool showWavecounts();

	bool gameStatusControl();

	void menu(Ref* sender);

	void continueGame(Ref* sender, bool originalState);

	void restartGame(Ref* sender);

	void selectGame(Ref* sender);

	void win(Ref* sender);

	void lose(Ref* sender);

	CREATE_FUNC(UiLayer);

	cocos2d::ui::Button* getPauseButton() const { return pauseButton; }
};

#endif
