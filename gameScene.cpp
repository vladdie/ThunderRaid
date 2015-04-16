#include "gameScene.h"
#include "GameOverScene.h"

gameScene::gameScene(void)
{
	boom = false;
}


gameScene::~gameScene(void)
{
}

Scene* gameScene::createScene(){
	// 'scene' is an autorelease object
	auto scene = Scene::create();

	// 'layer' is an autorelease object
	auto layer = gameScene::create();

	// add layer as a child to scene
	scene->addChild(layer);

	// return the scene
	return scene;
}

bool gameScene::init(){
	if (!Layer::init()) {
		return false;
	}

/////////***************main background, random pick******************//////////
	Size winSize = Director::getInstance()->getWinSize();
	char buff[15];
	int id = random(1, 5);
	sprintf(buff, "img_bg_%d.jpg", id);
	/*CCSprite *background = CCSprite::create(buff);
	background->setPosition(Point(winSize.width / 2, winSize.height / 2));
	this->addChild(background);*/
	background1 = CCSprite::create(buff);
	background1->setAnchorPoint(Point::ZERO);
	background1->setPosition(Point::ZERO);
	this->addChild(background1);

	background2 = CCSprite::create(buff);
	background2->setAnchorPoint(Point::ZERO);
	background2->setPosition(Point(0,background2->getContentSize().height-2));
	this->addChild(background2);

	this->schedule(schedule_selector(gameScene::backgroundMove),0.01f);
/////////***************PLAYER sprite******************//////////
	player = CCSprite::create("mplane.png");
	player->setPosition(Point(winSize.width / 2, winSize.height / 3));
	player->setScale(0.5);
	addChild(player);

	auto listenerKeyboard = EventListenerKeyboard::create();
	listenerKeyboard->onKeyPressed = CC_CALLBACK_2(gameScene::onKeyPressed, this);
	listenerKeyboard->onKeyReleased = CC_CALLBACK_2(gameScene::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerKeyboard, this);
	this->schedule(schedule_selector(gameScene::addTarget), 1.0f); //使用时间选择器刷新游戏,自定义调度器(scheduler)
	this->scheduleUpdate();//通过执行schedulerUpdate()调度器每帧执行 update方法
	

	

	return true;
}

void gameScene::backgroundMove(float dt)
{
	background1->setPositionY(int(background1->getPositionY())-2);
	background2->setPositionY(int(background1->getPositionY())+int(background1->getContentSize().height)-2);
	if (background2->getPositionY() == 0)
	{
		background1->setPositionY(0);
	}
}

void gameScene::addTarget(float dt)
{
	/////////***************ENEMY sprite******************//////////
	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto target = Sprite::create("enemy3.png");
	// Determine where to spawn the target along the Y axis
	int minX = target->getContentSize().width / 2;
	int maxX = visibleSize.width - target->getContentSize().width / 2;
	int rangeX = maxX - minX;
	int actualX = (CCRANDOM_0_1() * rangeX) + minX;
	target->setPosition(Point(actualX, visibleSize.height + target->getContentSize().height / 2));
	target->setScale(0.3);
	this->addChild(target, 0);
	target->setTag(1);
	_targets.pushBack(target);
	// Create the target slightly off-screen along the right edge,
	// and along a random position along the Y axis as calculated above
	int minDuration = 2.0;
	int maxDuration = 4.0;
	int rangeDuration = maxDuration - minDuration;
    int actualDuration = (CCRANDOM_0_1() * rangeDuration) + maxDuration;
	// Determine speed of the target
	auto actionMove = MoveTo::create(actualDuration, Point(actualX, -target->getContentSize().height));
	//actionMoveDone：精灵移动完成后执行可执行的回调函数为 spriteMoveFinished，当游戏移动到屏幕边界外，即回收该对象
	auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(gameScene::spriteMoveFinished, this));
	//通过精灵的runAction设置，精灵移动
	target->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
}

void gameScene::spriteMoveFinished(Object* pSender)
{
	Sprite *sprite = (Sprite *)pSender;
	this->removeChild(sprite);
	if (sprite->getTag()==1)
	{
		_targets.eraseObject(sprite);
	}
	else if (sprite->getTag() == 2)
	{
		_bullets.eraseObject(sprite);
	}
}

void gameScene::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event){
	Size visibleSize = Director::getInstance()->getVisibleSize();

	//log("Key with Keycode %d pressed",keyCode);
	if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
		player->runAction(MoveTo::create(player->getPositionX() / 500, Vec2(0, player->getPositionY())));
	}

	if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
		player->runAction(MoveTo::create((visibleSize.width-player->getPositionX()) / 500, Vec2(visibleSize.width, player->getPositionY())));
	}

	if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
		//determination: Vec2(player->getPositionX(), 0)
		player->runAction(MoveTo::create((visibleSize.height-player->getPositionY()) / 500, Vec2(player->getPositionX(), visibleSize.height)));
	}

	if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
		player->runAction(MoveTo::create(player->getPositionY() / 500, Vec2(player->getPositionX(), 0)));
	}

	else if (keyCode==EventKeyboard::KeyCode::KEY_SPACE)
	{
		////**********HERE is where BULLET is added!!!!!!!!**********************//
		Size visibleSize = Director::getInstance()->getVisibleSize();
		SpriteFrameCache::getInstance()->addSpriteFramesWithFile("bullet.plist");
		//Sprite* bullet
		auto bullet = Sprite::createWithSpriteFrameName("bullet_1.png");
		bullet->setPosition(player->getPosition());
		this->addChild(bullet);
		bullet->setScale(0.2);
		bullet->setTag(2);
		_bullets.pushBack(bullet);
		bullet->runAction(Sequence::create(MoveTo::create(1, Vec2(player->getPositionX(), visibleSize.height)),
		CallFuncN::create(CC_CALLBACK_1(gameScene::spriteMoveFinished,this)),NULL));
	}
}

void gameScene::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event){
	if (keyCode != EventKeyboard::KeyCode::KEY_SPACE){
		player->stopAllActions();
	}
}

void gameScene::update(float dt)//有了这个update函数，我们就可以在GameLayer中对它进行重载，把我们每帧内要执行的动作放到update中，进行调用就可以了。
{
	bulletCollisionEnemy();	
	enemyCollisionPlane();
}

/**
 * 子弹和敌机碰撞检测函数
 */
void gameScene::bulletCollisionEnemy() //
{
	Vector<cocos2d::Sprite*> targetsToDelete;
	Vector<cocos2d::Sprite*> bulletsToDelete;
	for (int i = 0; i < _bullets.size(); i++)
	{
		auto bullet = _bullets.at(i); //get all the bullet from the scene
		auto bulletRect = bullet->getBoundingBox();
		for (int j = 0; j < _targets.size(); j++)
		{
			auto target = _targets.at(j);
			auto targetRect = target->getBoundingBox();
			if (bulletRect.intersectsRect(targetRect))
			{
				targetsToDelete.pushBack(target);
			}
		}
		//C++11 range-based for loop
		for (Sprite* t : targetsToDelete)
		{
			_targets.eraseObject(t);
			this->removeChild(t);
		}
		if (targetsToDelete.size() >0)
		{
			bulletsToDelete.pushBack(bullet);
		}
		targetsToDelete.clear();
	}
	for (const auto& p : bulletsToDelete)
	{
		_bullets.eraseObject(p);
		this->removeChild(p);
	}
	bulletsToDelete.clear();
}   

bool gameScene::enemyCollisionPlane() 
{	
	for (int j = 0; j < _targets.size(); j++)
	{
		auto target = _targets.at(j);
		if (!boom)
		{
			if (player->boundingBox().intersectsRect(target->getBoundingBox()))
			{
				this->removeChild(target);
				this->removeChild(player);
				boom = true;
				//show the end scene
				auto scene2 = GameOverScene::createScene(100);
				CCTransitionMoveInB *animateScene=CCTransitionMoveInB::create(0.5f, scene2);
				CCDirector::sharedDirector()->replaceScene(animateScene);
				//CCDirector::sharedDirector()->replaceScene(CCTransitionScene::create(2, HelloWorld::createScene()));
			}
		}
		
	}

	return false;
}

  

void gameScene::menuCloseCallback(Ref* pSender) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
	return;
#endif
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

