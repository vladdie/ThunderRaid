#pragma once
#include "cocos2d.h"

using namespace cocos2d;
class GameOverScene : public CCLayer
{
public:
	GameOverScene(void);
	~GameOverScene(void);

	static Scene* createScene(int passScore);
	//static Scene* createScene();
	virtual bool init();
	void menuBackCallback(Ref* pSender);
	static GameOverScene* create(int passScore);
	//CREATE_FUNC(GameOverScene);
	void menuCloseCallback(Ref* pSender);
	//virtual void keyBackClicked();
	void beginChangeHighestScore(CCNode* pNode);

public:

	int score;

	static int highestHistoryScore;

	CCLabelBMFont* highestScore;
};

