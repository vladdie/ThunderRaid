#pragma once
#include "cocos2d.h"
using namespace cocos2d;

class gameScene : public Layer
{
public:
	gameScene(void);
	~gameScene(void);

	CCSprite* player;
	virtual bool init();
	static Scene* createScene();
	void addTarget(float dt);//自定义调度器(scheduler)
	void spriteMoveFinished(Object* pSender);
	void onFire();
	void update(float t);//通过执行schedulerUpdate()调度器每帧执行 update方法
	void bulletCollisionEnemy();//子弹碰撞检测
	bool enemyCollisionPlane(); //飞机碰撞检测
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);
	void backgroundMove(float dt);
	void menuCloseCallback(Ref* pSender);
	CREATE_FUNC(gameScene);

	


private:
	cocos2d::Vector<cocos2d::Sprite*> _targets;
	cocos2d::Vector<cocos2d::Sprite*> _bullets;
	Sprite* background1;
	Sprite* background2;
    bool boom;
};
