#pragma once
#include "cocos2d.h"

using namespace cocos2d;

class selectScene : public Layer
{
public:
	selectScene(void);
	~selectScene(void);

	static Scene* createScene();
	virtual bool init();
	void menuSele1Callback(Ref* pSender);
	void menuSele2Callback(Ref* pSender);
	void menuStartCallback(Ref* pSender);
	void menuCloseCallback(Ref* pSender);
	CREATE_FUNC(selectScene);
};

