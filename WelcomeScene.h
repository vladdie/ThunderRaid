#pragma once
#include "cocos2d.h"

using namespace cocos2d;

class WelcomeScene : public CCLayer
{
public:
	WelcomeScene(void);
	~WelcomeScene(void);

	static Scene* createScene();
	virtual bool init();
	/*void menuSeleCallback(Ref* pSender);
	void menuStartCallback(Ref* pSender);*/
	void menuCloseCallback(Ref* pSender);
	CREATE_FUNC(WelcomeScene);
	void PreloadMusic();
	void loadingDone(CCNode* pNode);
	bool isHaveSaveFile();
	void getHighestHistorySorce();
};

