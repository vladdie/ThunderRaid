#include "GameOverScene.h"
//#include "gameScene.h"
#include "MainScene.h"
#include "selectScene.h"


int highestHistoryScore=0;

GameOverScene::GameOverScene(void)
{
	score=0;
	highestScore=NULL;
}


GameOverScene::~GameOverScene(void)
{
}

Scene* GameOverScene::createScene(int passScore)
{
	auto scene = Scene::create();
	auto layer = GameOverScene::create(passScore);//own defined create()
	scene->addChild(layer);
	return scene;
}

bool GameOverScene::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	Size winSize=CCDirector::sharedDirector()->getWinSize();
	CCSprite* background=CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("gameover.png"));
	background->setPosition(ccp(winSize.width/2,winSize.height/2));
	this->addChild(background);

	CCSprite* normalBackToGame=CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("btn_finish.png"));
	CCSprite* pressedBackToGame=CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("btn_finish.png"));
	//CCMenuItemImage* pBackItem=CCMenuItemImage::create();
	auto pBackItem = MenuItemSprite::create(normalBackToGame, pressedBackToGame, CC_CALLBACK_1(GameOverScene::menuBackCallback, this));
	pBackItem->setPosition(Point(winSize.width-normalBackToGame->getContentSize().width/2-10, normalBackToGame->getContentSize().height/2+10));
	auto menuBack = Menu::create(pBackItem, NULL);
	menuBack->setPosition(Vec2::ZERO);
	this->addChild(menuBack, 1);

	std::string strScore = std::to_string(score);
	auto finalScore = Label::createWithBMFont("font.fnt", strScore);
	finalScore->setColor(Color3B(143,146,147));
	finalScore->setPosition(Point(winSize.width/2, winSize.height/2));
	this->addChild(finalScore);

	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("achievement.mp3");
	
	auto delay = DelayTime::create(1.0f);
	auto scalebig = ScaleTo::create(1.0f,3.0f);
	auto scalelittle = ScaleTo::create(0.3f,2.0f);
	auto sequence = Sequence::create(delay, scalebig, scalelittle, nullptr);
	finalScore->runAction(sequence);

	std::string strHighestScore = std::to_string(score);
	auto highestScore = Label::createWithBMFont("font.fnt", strHighestScore);
	highestScore->setColor(Color3B(143,146,147));
	highestScore->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
	highestScore->setPosition(Point(140,winSize.height-50));
	this->addChild(highestScore);

	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",CC_CALLBACK_1(GameOverScene::menuCloseCallback, this));
	closeItem->setPosition(Point(winSize.width-50 ,winSize.height-50));
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);

	/*if (score>highestHistoryScore)
	{
		UserDefault::getInstance()->setIntegerForKey("HighestScore", score);
		highestHistoryScore = score;
		auto delayChange = DelayTime::create(1.3f);
		auto moveOut = MoveBy::create(0.1f, Point(0,100));
		auto beginChange = CallFuncN::create(CC_CALLBACK_1(GameOverScene::beginChangeHighestScore, this));
		auto moveIn = MoveBy::create(0.1f,Point(0,-100));
		auto sequence = Sequence::create(delayChange, moveOut, beginChange, moveIn, nullptr);
		highestScore->runAction(sequence);
	}*/


	return true;
}


GameOverScene* GameOverScene::create(int passScore)
{
	GameOverScene *pRet = new GameOverScene();
	pRet->score = passScore;
	if (pRet && pRet->init())
	{ 
		pRet->autorelease();
	} 
	else 
	{ 
		delete pRet; 
		pRet = nullptr;
	}
	return pRet;
}

void GameOverScene::menuBackCallback(Ref* pSender)
{
	
		auto pScene = selectScene::createScene();
		auto animateScene = TransitionSlideInL::create(1.0f, pScene);
		Director::getInstance()->replaceScene(animateScene);

}

//void GameOverScene::beginChangeHighestScore(Node* pNode)
//{
//	//CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("achievement.mp3");
//	Value changeScore(score);
//	highestScore->setString(changeScore.asString());
//}
//

void GameOverScene::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
	return;
#endif

	Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}