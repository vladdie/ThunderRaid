#pragma once

#include "cocos2d.h"
#include "playerLayer.h"
#include "RaknetGlobals.h"
#include "SimpleAudioEngine.h"
#include "level2.h"
//#include "enemyLayer.h"
#define NEXTLEVELSCORE 50
const int BULLET2=22;
const int ENEMY2 = 11;


class MainScene : public Layer
{
public:
	MainScene(void);
	~MainScene(void);

	static Scene* createScene();
	CREATE_FUNC(MainScene);

	virtual bool init();

	void backgroundMove(float dt);
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void addTarget(float dt);//自定义调度器(scheduler)
	void spriteMoveFinished(Object* pSender);
	void onFire();
	void update(float t);//通过执行schedulerUpdate()调度器每帧执行 update方法
	void bulletCollisionEnemy();//子弹碰撞检测
	bool enemyCollisionPlane(); //飞机碰撞检测
	void menuCloseCallback(Ref* pSender);
	//void updateCollision(float dt);
	void updatePlaneInfo(float dt);
	void updateNet();
	bool checkPoint();
	void enemy1Blowup(Sprite* t);
	void removeEnemy(Object* pSender);
	void loadingDone(CCNode* pNode);

	void EnemyMove();
	bool sceneChange;
	bool peerControl;
	//void menuBigBoomCallback(Ref* pSender);
	void DeleteDisctionnectionWarning(CCObject* warning);
private:
	cocos2d::Vector<cocos2d::Sprite*> _targets;
	cocos2d::Vector<cocos2d::Sprite*> _bullets;
	cocos2d::Vector<cocos2d::Sprite*> _players;
	
	
	bool boom;
	
	Sprite* background1;
	Sprite* background2;
	playerLayer* planeLayer;
	//enemyLayer* EnemyLayer;
	int score;
	int bigBoomCount;
};

