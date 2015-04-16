#include "playerLayer.h"
#include "GameOverScene.h"

playerLayer* playerLayer::sharedPlane = nullptr;

playerLayer::playerLayer(void)
{
	isAlive = true;
	score = 0;
}

playerLayer::~playerLayer(void)
{
}

playerLayer* playerLayer::create() //create this layer
{ 
	playerLayer *pRet = new playerLayer();
	if (pRet && pRet->init())
	{
		pRet->autorelease();
		sharedPlane = pRet;
		return pRet;
	}
	else
	{
		CC_SAFE_DELETE(pRet);
		return nullptr;
	}
}

bool playerLayer::init()
{
	bool bRet = false;
	do 
	{
		CC_BREAK_IF(!Layer::init());

		auto winSize = Director::getInstance()->getWinSize();

		auto plane = Sprite::createWithSpriteFrameName("hero1.png");
		plane->setPosition(Point(winSize.width/2-30, plane->getContentSize().height/2));
		plane->setScale(0.5);
		this->addChild(plane,0,AIRPLANE);

		if (isConnected)
		{
			std::vector<playerInfo> toSend;
			playerInfo infTemp;
			infTemp.posX = plane->getPositionX()-30;
			infTemp.posY = plane->getPositionY();
			//add to vector
			toSend.push_back(infTemp);
			playerInfo* toSendContent = new playerInfo[toSend.size()];
			for (int p = 0; p < toSend.size();p++)
			{
				toSendContent[p] = toSend[p];
			}
			//write on the buffer
			RakNet::BitStream bufferOut;
			bufferOut.Write((RakNet::MessageID)ID_GAME_PLAYER_POS);
			int size = toSend.size();
			bufferOut.Write((const char*)&size,sizeof(int));
			bufferOut.Write((const char*)toSendContent,sizeof(playerInfo) * toSend.size());
			player2->Send(&bufferOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,player2Adress,false);
			delete toSendContent;

		}
		


		auto blink = Blink::create(1,3);
		auto animation = Animation::create();
		animation->setDelayPerUnit(0.1f);
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero1.png"));
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero2.png"));
		auto animate = Animate::create(animation);

		plane->runAction(blink);
		plane->runAction(RepeatForever::create(animate));

		bRet = true;
	} while (0);

	return bRet;
}

void playerLayer::BlowupPlayer1()
{
	if(isAlive)
	{
		//isAlive = false;
		auto animation = Animation::create();
		animation->setDelayPerUnit(0.2f);
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero_blowup_n1.png"));
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero_blowup_n2.png"));
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero_blowup_n3.png"));
		animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("hero_blowup_n4.png"));

		auto animate=Animate::create(animation);
		/*auto removePlane=CallFunc::create(CC_CALLBACK_0(playerLayer::RemovePlane, this));
		auto sequence=Sequence::create(animate,removePlane, nullptr);*/
		this->getChildByTag(AIRPLANE)->stopAllActions();
		this->getChildByTag(AIRPLANE)->runAction(animate);
	}
}

void playerLayer::RemovePlane()
{
	this->removeChildByTag(AIRPLANE,true);
	auto pScene= GameOverScene::createScene(score);
	auto animateScene=TransitionMoveInT::create(0.8f,pScene);
	Director::getInstance()->replaceScene(animateScene);
	//auto scene2 = GameOverScene::createScene(100);
	/*CCTransitionMoveInB *animateScene=CCTransitionMoveInB::create(0.5f, scene2);
	CCDirector::sharedDirector()->replaceScene(animateScene);*/

}
