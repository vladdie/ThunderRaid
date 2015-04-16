#include "selectScene.h"
#include "WelcomeScene.h"
#include "WaitingConnectionScene.h"

selectScene::selectScene(void)
{
}


selectScene::~selectScene(void)
{
}

Scene* selectScene::createScene(){

	auto scene = Scene::create();
	auto layer = selectScene::create();
	scene->addChild(layer);
	return scene;
}

bool selectScene::init()
{
	if (!Layer::init()) {
		return false;
	}

	Size winSize = Director::getInstance()->getWinSize();
	//**********************background**************************************************************//
	 CCSprite* background = CCSprite::create("background.png");
	 background->setAnchorPoint(Point::ZERO);
	 background->setPosition(Point::ZERO);
	 this->addChild(background);

//**********************MENU**************************************************************//
	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",CC_CALLBACK_1(selectScene::menuCloseCallback, this));
	closeItem->setPosition(Point(winSize.width-50 ,winSize.height-50));
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	auto sele1Item = MenuItemImage::create("select1.png", "selected1.png",CC_CALLBACK_1(selectScene::menuSele1Callback, this));
	sele1Item->setPosition(Point(winSize.width / 2 ,winSize.height /2 + 30));
	auto menu1 = Menu::create(sele1Item, NULL);
	menu1->setPosition(Vec2::ZERO);
	this->addChild(menu1, 2);

	auto sele2Item = MenuItemImage::create("select2.png", "selected2.png",CC_CALLBACK_1(selectScene::menuSele2Callback, this));
	sele2Item->setPosition(Point(winSize.width / 2 ,winSize.height /2 - 30));
	auto menu2 = Menu::create(sele2Item, NULL);
	menu2->setPosition(Vec2::ZERO);
	this->addChild(menu2, 3);
	
	return true;
}

void selectScene::menuSele1Callback(Ref* pSender)
{
	auto pScene = WelcomeScene::createScene();
	auto animateScene = TransitionSlideInL::create(1.0f, pScene);
	Director::getInstance()->replaceScene(animateScene);
}

void selectScene::menuSele2Callback(Ref* pSender)
{
	//auto pScene = WelcomeScene::createScene();
	auto pScene = WaitingConnectionScene::scene();
	auto animateScene = TransitionSlideInL::create(1.0f, pScene);
	Director::getInstance()->replaceScene(animateScene);
}

void selectScene::menuCloseCallback(Ref* pSender) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
	return;
#endif
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}