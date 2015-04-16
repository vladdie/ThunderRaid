#include "WelcomeScene.h"
//#include "HelloWorldScene.h"
#include "gameScene.h"
#include "MainScene.h"
#include "level2.h"
#include "SimpleAudioEngine.h"

WelcomeScene::WelcomeScene(void)
{
}

WelcomeScene::~WelcomeScene(void)
{
}

Scene* WelcomeScene::createScene()
{
	auto scene = Scene::create();
	auto layer = WelcomeScene::create();
	scene->addChild(layer);
	return scene;
}

bool WelcomeScene::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	PreloadMusic();
	//DO YOUR OWN STUFF	*******************************************
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("shoot_background.plist");
	CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("shoot.plist");

	Size winSize = Director::getInstance()->getWinSize();
	CCSprite* background=CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("background.png"));
	background->setPosition(Point(winSize.width/2,winSize.height/2));
	this->addChild(background);

		CCSprite* copyright=CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("shoot_copyright.png"));
		copyright->setAnchorPoint(ccp(0.5,0));
		copyright->setPosition(ccp(winSize.width/2,winSize.height/2));
		this->addChild(copyright);

	CCSprite* loading=CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game_loading1.png"));
	loading->setPosition(ccp(winSize.width/2,winSize.height/2-40));
	this->addChild(loading);

	CCAnimation* animation=CCAnimation::create();
	animation->setDelayPerUnit(0.2f);
	animation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game_loading1.png"));
	animation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game_loading2.png"));
    animation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game_loading3.png"));
    animation->addSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("game_loading4.png"));

	CCAnimate* animate=CCAnimate::create(animation);
	CCRepeat* repeat=CCRepeat::create(animate,2);
	CallFuncN* repeatdone=CallFuncN::create(CC_CALLBACK_1(WelcomeScene::loadingDone, this));
	CCSequence* sequence=CCSequence::create(repeat,repeatdone,NULL);
	loading->runAction(sequence);
	//*********************************************************

	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",CC_CALLBACK_1(WelcomeScene::menuCloseCallback, this));
	closeItem->setPosition(Point(winSize.width-50 ,winSize.height-50));
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);
	
	return true;
}

void WelcomeScene::loadingDone(CCNode* pNode)
{
	//CCScene* pScene= level2::createScene();
	CCScene* pScene= MainScene::createScene();
	CCTransitionMoveInB *animateScene=CCTransitionMoveInB::create(0.5f,pScene);
	CCDirector::sharedDirector()->replaceScene(animateScene);
}

//void WelcomeScene::getHighestHistorySorce()
//{
//	if (isHaveSaveFile())
//	{
//		GameOverLayer::highestHistoryScore=CCUserDefault::sharedUserDefault()->getIntegerForKey("HighestScore",0);
//	}
//}

void WelcomeScene::PreloadMusic()
{
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("sound/game_music.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/bullet.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/enemy1_down.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/enemy2_down.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/enemy3_down.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/game_over.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/get_bomb.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/get_double_laser.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/use_bomb.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/big_spaceship_flying.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/achievement.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/out_porp.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("sound/button.mp3");

	CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic("sound/game_music.mp3",true);
}

void WelcomeScene::menuCloseCallback(Ref* pSender)
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