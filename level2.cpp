#include "level2.h"
#include "GameOverScene.h"

level2::level2(void)
{
	background1 = nullptr;
	background2 = nullptr;
	planeLayer = nullptr;
	/*bulletLayer = nullptr;
	EnemyLayer = nullptr;*/
	boom = false;
	score = 0;
	bigBoomCount = 0;

}


level2::~level2(void)
{
}


Scene* level2::createScene(){

	auto scene = Scene::create();
	auto layer = level2::create();
	scene->addChild(layer);
	return scene;
}


bool level2::init()
{
	if (!Layer::init()) {
		return false;
	}
	//**********************background**************************************************************//
	Size winSize = Director::getInstance()->getWinSize();
	char buff[15];
	int id = random(1, 5);
	sprintf(buff, "img_bg_%d.jpg", id);
	background1 = CCSprite::create(buff);
	background1->setAnchorPoint(Point::ZERO);
	background1->setPosition(Point::ZERO);
	background1->getTexture()->setAliasTexParameters();
	this->addChild(background1);
	background2 = CCSprite::create(buff);
	background2->setAnchorPoint(Point::ZERO);
	background2->setPosition(Point(0,background2->getContentSize().height-2));
	background2->getTexture()->setAliasTexParameters();
	this->addChild(background2);
	this->schedule(schedule_selector(level2::backgroundMove),0.01f);

	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",CC_CALLBACK_1(level2::menuCloseCallback, this));
	closeItem->setPosition(Point(winSize.width-50 ,winSize.height-50));
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 2);
	//**********************background**************************************************************//

	this->planeLayer = playerLayer::create();
	this->addChild(planeLayer);

		/*this->bulletLayer = BulletLayer::create();
		this->addChild(bulletLayer);
		this->bulletLayer->StartShoot();*/

	/*	this->EnemyLayer = enemyLayer::create();
		this->addChild(EnemyLayer);*/

		/*this->ufoLayer = UFOLayer::create();
		this->addChild(ufoLayer);*/
	if (!boom)
	{
		auto listenerKeyboard = EventListenerKeyboard::create();
		listenerKeyboard->onKeyPressed = CC_CALLBACK_2(level2::onKeyPressed, this);
		listenerKeyboard->onKeyReleased = CC_CALLBACK_2(level2::onKeyReleased, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerKeyboard, this);
		this->schedule(schedule_selector(level2::addTarget), 0.4f); //使用时间选择器刷新游戏,自定义调度器(scheduler)
		this->scheduleUpdate();//通过执行schedulerUpdate()调度器每帧执行 update方法
		//this->schedule(schedule_selector(MainScene::update), 0.01f);
	}

	return true;
}

void level2::backgroundMove(float dt)
{
	background1->setPositionY(int(background1->getPositionY())-2);
	background2->setPositionY(int(background1->getPositionY())+int(background1->getContentSize().height)-2);
	if (background2->getPositionY() == 0)
	{
		background1->setPositionY(0);
	}
}

void level2::addTarget(float dt)
{
	/////////***************ENEMY sprite******************//////////
	
	if (isConnected)
	{
			Size visibleSize = Director::getInstance()->getVisibleSize();
			SpriteFrame* enemy1SpriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_down1.png");
			auto target = Sprite::createWithSpriteFrame(enemy1SpriteFrame);
				//// Determine where to spawn the target along the Y axis
			int minX = target->getContentSize().width / 2;
			int maxX = visibleSize.width - target->getContentSize().width / 2;
			int rangeX = maxX - minX;
			int actualX = (CCRANDOM_0_1() * rangeX) + minX;
			//int actualX = planeLayer->getChildByTag(AIRPLANE)->getPosition().x;
			target->setPosition(Point(actualX, visibleSize.height + target->getContentSize().height / 2));
			target->setScale(0.2);
			this->addChild(target, 0);
			// Create the target slightly off-screen along the right edge,
			// and along a random position along the Y axis as calculated above
			int minDuration = 2.0;
			int maxDuration = 4.0;
			int rangeDuration = maxDuration - minDuration;
			int actualDuration = (CCRANDOM_0_1() * rangeDuration) + maxDuration;
			target->setTag(1);
			_targets.pushBack(target);

			std::vector<EnemyInfo> toSend;
			EnemyInfo infTemp;
			infTemp.posX = target->getPositionX();
			infTemp.posY = target->getPositionY();
			infTemp.actualX = actualX;
			infTemp.actualDuration = actualDuration;
			//add to vector
			toSend.push_back(infTemp);
			EnemyInfo* toSendContent = new EnemyInfo[toSend.size()];
			for (int p = 0; p < toSend.size();p++)
			{
				toSendContent[p] = toSend[p];
			}
			//write on the buffer
			RakNet::BitStream bufferOut;
			bufferOut.Write((RakNet::MessageID)ID_GAME_ENEMY_POS);
			int size = toSend.size();
			bufferOut.Write((const char*)&size,sizeof(int));
			bufferOut.Write((const char*)toSendContent,sizeof(EnemyInfo) * toSend.size());
			player2->Send(&bufferOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,player2Adress,false);
			delete toSendContent;

			//auto actionMove = MoveTo::create(actualDuration, Point(actualX, -target->getContentSize().height));
			ccBezierConfig bezier;
			bezier.controlPoint_1 = Point(0,0);
			if (isServer)
			{
				bezier.controlPoint_2 = Point(actualX-100, visibleSize.height / 2);
			}else{
				bezier.controlPoint_2 = Point(actualX+100, visibleSize.height / 2);
			}
			//bezier.controlPoint_2 = Point(actualX, visibleSize.height / 8);
			bezier.endPosition = Point(visibleSize.width+1, -visibleSize.height-1);
			auto bezierAction = BezierTo::create(0.1f, bezier);
			//auto actionMove = MoveTo::create(actualDuration, Point(actualX, -target->getContentSize().height));
			auto actionMove = BezierTo::create(actualDuration, bezier);
			//actionMoveDone：精灵移动完成后执行可执行的回调函数为 spriteMoveFinished，当游戏移动到屏幕边界外，即回收该对象
			auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(level2::spriteMoveFinished, this));
			//通过精灵的runAction设置，精灵移动
			target->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
	}
	else
	{
		EnemyMove();
	}
	
	
}

void level2::EnemyMove()
{

	Size visibleSize = Director::getInstance()->getVisibleSize();
	SpriteFrame* enemy1SpriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_down1.png");
	auto target = Sprite::createWithSpriteFrame(enemy1SpriteFrame);
	//// Determine where to spawn the target along the Y axis
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
	//auto actionMove = MoveTo::create(actualDuration, Point(actualX, -target->getContentSize().height));
	ccBezierConfig bezier;
	bezier.controlPoint_1 = Point(0, 0);
	bezier.controlPoint_2 = Point(actualX, visibleSize.height / 8);
	bezier.endPosition = Point(visibleSize.width+1, -visibleSize.height-1);
	auto bezierAction = BezierTo::create(0.1f, bezier);
	//auto actionMove = MoveTo::create(actualDuration, Point(-planeLayer->sharedPlane->getPosition().x, -planeLayer->sharedPlane->getPosition().y));
	auto actionMove = BezierTo::create(actualDuration, bezier);
	//actionMoveDone：精灵移动完成后执行可执行的回调函数为 spriteMoveFinished，当游戏移动到屏幕边界外，即回收该对象
	auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(level2::spriteMoveFinished, this));
	//通过精灵的runAction设置，精灵移动
	target->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
}

void level2::spriteMoveFinished(Object* pSender)
{
	Sprite *sprite = (Sprite *)pSender;
	this->removeChild(sprite);
	if (sprite->getTag()==1 || sprite->getTag()==ENEMY22)
	{
		_targets.eraseObject(sprite);
	}
	else if (sprite->getTag() == 2 || sprite->getTag()==BULLET22)
	{
		_bullets.eraseObject(sprite);
	}
}

void level2::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event){
	if( this->planeLayer->isAlive && !Director::getInstance()->isPaused())
	{
		Size visibleSize = Director::getInstance()->getVisibleSize();
		auto plane = this->planeLayer->getChildByTag(AIRPLANE);
		if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW) {
			plane->runAction(MoveTo::create(plane->getPositionX() / 500, Vec2(0, plane->getPositionY())));
		}

		if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW) {
			plane->runAction(MoveTo::create((visibleSize.width-plane->getPositionX()) / 500, Vec2(visibleSize.width, plane->getPositionY())));
		}

		if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW) {
			//determination: Vec2(player->getPositionX(), 0)
			plane->runAction(MoveTo::create((visibleSize.height-plane->getPositionY()) / 500, Vec2(plane->getPositionX(), visibleSize.height)));
		}

		if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW) {
			plane->runAction(MoveTo::create(plane->getPositionY() / 500, Vec2(plane->getPositionX(), 0)));
		}

		else if (keyCode==EventKeyboard::KeyCode::KEY_SPACE)
		{
			////**********HERE is where BULLET is added!!!!!!!!**********************//
			Size visibleSize = Director::getInstance()->getVisibleSize();
			SpriteFrameCache::getInstance()->addSpriteFramesWithFile("bullet.plist");
			//Sprite* bullet
			auto bullet = Sprite::createWithSpriteFrameName("bullet_1.png");
			bullet->setPosition(plane->getPosition());
			this->addChild(bullet);
			bullet->setScale(0.2);
			bullet->setTag(2);
			_bullets.pushBack(bullet);
			bullet->runAction(Sequence::create(MoveTo::create(1, Vec2(plane->getPositionX(), visibleSize.height)),
				CallFuncN::create(CC_CALLBACK_1(level2::spriteMoveFinished,this)),NULL));
			CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("bullet.mp3");
			if (isConnected)
			{
				std::vector<bulletInfo> toSend;
				bulletInfo infTemp;
				infTemp.posX = bullet->getPositionX();
				infTemp.posY = bullet->getPositionY();
				//add to vector
				toSend.push_back(infTemp);
				bulletInfo* toSendContent = new bulletInfo[toSend.size()];
				for (int p = 0; p < toSend.size();p++)
				{
					toSendContent[p] = toSend[p];
				}
				//write on the buffer
				RakNet::BitStream bufferOut;
				bufferOut.Write((RakNet::MessageID)ID_GAME_BULLET_POS);
				int size = toSend.size();
				bufferOut.Write((const char*)&size,sizeof(int));
				bufferOut.Write((const char*)toSendContent,sizeof(bulletInfo) * toSend.size());
				player2->Send(&bufferOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,player2Adress,false);
				delete toSendContent;
			}
			
		}
		
		this->schedule(schedule_selector(level2::updatePlaneInfo), 0.01);
	}
}
void level2::updatePlaneInfo(float dt)
{
	auto plane = this->planeLayer->getChildByTag(AIRPLANE);
	if (isConnected && plane!=NULL)
	{
		std::vector<playerInfo> toSend;
		playerInfo infTemp;
		infTemp.posX = plane->getPositionX();
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
		bufferOut.Write((RakNet::MessageID)ID_GAME_PLAYER_POSUPDATE);
		int size = toSend.size();
		bufferOut.Write((const char*)&size,sizeof(int));
		bufferOut.Write((const char*)toSendContent,sizeof(playerInfo) * toSend.size());
		player2->Send(&bufferOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,player2Adress,false);
		delete toSendContent;
	}
}
void level2::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event){
	if (keyCode != EventKeyboard::KeyCode::KEY_SPACE){
		auto plane = this->planeLayer->getChildByTag(AIRPLANE);
		plane->stopAllActions();
	}
}

void level2::update(float dt)//有了这个update函数，我们就可以在GameLayer中对它进行重载，把我们每帧内要执行的动作放到update中，进行调用就可以了。
{
	bulletCollisionEnemy();	
	enemyCollisionPlane();
	//this->schedule(schedule_selector(MainScene::updateNet),0.01);
	updateNet();
}

void level2::updateNet()
{
	if (isConnected)
	{

		RakNet::Packet* packet;
		for (packet = player2->Receive();packet;player2->DeallocatePacket(packet),packet=player2->Receive())
		{
			// only process a packet that came from the other player
			if (packet->systemAddress == player2Adress)
			{
				// player disconnected
				if (packet->data[0] == ID_DISCONNECTION_NOTIFICATION || 
					packet->data[0] == ID_CONNECTION_LOST)
				{
					isConnected = false;
					// put some warning on the screen
					//create a text and make it goes up
					CCLabelBMFont* discWarning = CCLabelBMFont::create("Your opponent just disconnected","badab.fnt");
					discWarning->setColor(ccc3(255,0,0));
					discWarning->setPosition(ccp(400.0f,-20.0f));
					Action* moveWarning=MoveTo::create(2,Vec2(400.0f,200.0f));
					discWarning->runAction(moveWarning);
					discWarning->runAction(CCFadeOut::create(4.0f));//fade
					this->addChild(discWarning,1000);
					this->planeLayer->removeChildByTag(AIRPLANE2);
					//this->removeChild(discWarning);
				}
				else if(packet->data[0] == ID_GAME_PLAYER_POS)
				{
					// received information
					std::vector<playerInfo> received;
					RakNet::BitStream bsIn(packet->data,packet->length,false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					// number of blocks within the package
					int size = 0;
					bsIn.Read((char*)&size,sizeof(int));
					playerInfo* reci = new playerInfo[size];
					bsIn.Read((char*)reci,sizeof(playerInfo) * size);
					// put on the received vector
					for (int p = 0; p < size; p++)
					{
						received.push_back(reci[p]);
					}
					delete reci;

					for (int c = 0; c < received.size(); c++)
					{
						// create block on a random location
						auto plane2 = Sprite::createWithSpriteFrameName("hero1.png");
						plane2->setPositionX(received[c].posX+30);
						plane2->setPositionY(received[c].posY);
						plane2->setScale(0.5);
						this->planeLayer->addChild(plane2,0,AIRPLANE2);
					}
				}
				// other player just lost
				else if (packet->data[0] == ID_GAME_PLAYER_LOST)
				{
					otherGameOver = true;
					
						//this->planeLayer->isAlive = false;
						CCLabelBMFont* congratulation = CCLabelBMFont::create("Good you win!","badab.fnt");
						congratulation->setColor(ccc3(255,0,0));
						congratulation->setPosition(ccp(400.0f,-20.0f));
						Action* moveWarning=MoveTo::create(2,Vec2(400.0f,200.0f));
						congratulation->runAction(moveWarning);
						congratulation->runAction(CCFadeOut::create(4.0f));//fade
						this->addChild(congratulation,1000);
						this->planeLayer->removeChildByTag(AIRPLANE2);
				}

				else if (packet->data[0] == ID_GAME_NEW_POINTS)
				{
					// read new points from the other player
					int rs = 0;
					RakNet::BitStream bsIn(packet->data,packet->length,false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					bsIn.Read((char*)&rs,sizeof(int));
					//pointsManager->SetP2Points(rs);
				}
				
				else if(packet->data[0] == ID_GAME_BULLET_POS)
				{
					// received information
					std::vector<bulletInfo> received;
					RakNet::BitStream bsIn(packet->data,packet->length,false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					// number of blocks within the package
					int size = 0;
					bsIn.Read((char*)&size,sizeof(int));
					bulletInfo* reci = new bulletInfo[size];
					bsIn.Read((char*)reci,sizeof(bulletInfo) * size);
					// put on the received vector
					for (int p = 0; p < size; p++)
					{
						received.push_back(reci[p]);
					}
					delete reci;

					for (int c = 0; c < received.size(); c++)
					{
						// create block on a random location
						SpriteFrameCache::getInstance()->addSpriteFramesWithFile("bullet.plist");
						auto bullet2 = Sprite::createWithSpriteFrameName("bullet_1.png");
						bullet2->setPositionX(received[c].posX);
						bullet2->setPositionY(received[c].posY);
						bullet2->setScale(0.2);
						bullet2->setTag(BULLET22);//bullet in other end tag is 22
						_bullets.pushBack(bullet2);//**************************TAG
						Size visibleSize = Director::getInstance()->getVisibleSize();
						auto plane2 = this->planeLayer->getChildByTag(AIRPLANE2);
						if (plane2!=nullptr)
						{
							bullet2->runAction(Sequence::create(MoveTo::create(1, Vec2(plane2->getPositionX(), visibleSize.height)),CallFuncN::create(CC_CALLBACK_1(level2::spriteMoveFinished,this)),NULL));
						}
						this->addChild(bullet2);
					}
				}
				else if(packet->data[0] == ID_GAME_ENEMY_POS)
				{
						std::vector<EnemyInfo> received;
						RakNet::BitStream bsIn(packet->data,packet->length,false);
						bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
						int size = 0;
						bsIn.Read((char*)&size,sizeof(int));
						EnemyInfo* reci = new EnemyInfo[size];
						bsIn.Read((char*)reci,sizeof(EnemyInfo) * size);
						for (int p = 0; p < size; p++)
						{
							received.push_back(reci[p]);
						}
						delete reci;
						Size visibleSize = Director::getInstance()->getVisibleSize();
						for (int c = 0; c < received.size(); c++)
						{
							SpriteFrameCache::getInstance()->addSpriteFramesWithFile("bullet.plist");
							SpriteFrame* enemy1SpriteFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_down1.png");
							auto target2 = Sprite::createWithSpriteFrame(enemy1SpriteFrame);
							int actualDuration = received[c].actualDuration;
							int actualX = received[c].actualX;
							target2->setPosition(Point(received[c].posX, received[c].posY));
							target2->setScale(0.2);
							this->addChild(target2, 0);
							target2->setTag(ENEMY22);//***************************TAG
							_targets.pushBack(target2);
							ccBezierConfig bezier;
							bezier.controlPoint_1 = Point(0,0);
							if (isServer)
							{
								bezier.controlPoint_2 = Point(actualX+100, visibleSize.height / 2);
							}else{
								bezier.controlPoint_2 = Point(actualX-100, visibleSize.height / 2);
							}
							//bezier.controlPoint_2 = Point(actualX, visibleSize.height / 8);
							bezier.endPosition = Point(visibleSize.width+1, -visibleSize.height-1);
							auto bezierAction = BezierTo::create(0.1f, bezier);
							//auto actionMove = MoveTo::create(actualDuration, Point(-planeLayer->sharedPlane->getPosition().x, -planeLayer->sharedPlane->getPosition().y));
							auto actionMove = BezierTo::create(actualDuration, bezier);
							//auto actionMove = MoveTo::create(actualDuration, Point(actualX, -target2->getContentSize().height));
							auto actionMoveDone = CallFuncN::create(CC_CALLBACK_1(level2::spriteMoveFinished, this));
							target2->runAction(Sequence::create(actionMove, actionMoveDone, NULL));
						}
				}
				else if(packet->data[0] == ID_GAME_PLAYER_POSUPDATE)
				{
					// received information
					std::vector<playerInfo> received;
					RakNet::BitStream bsIn(packet->data,packet->length,false);
					bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
					// number of blocks within the package
					int size = 0;
					bsIn.Read((char*)&size,sizeof(int));
					playerInfo* reci = new playerInfo[size];
					bsIn.Read((char*)reci,sizeof(playerInfo) * size);
					// put on the received vector
					for (int p = 0; p < size; p++)
					{
						received.push_back(reci[p]);
					}
					delete reci;

					for (int c = 0; c < received.size(); c++)
					{
						// create block on a random location
						auto plane2 = this->planeLayer->getChildByTag(AIRPLANE2);
						if (plane2!=nullptr)
						{
							plane2->setPositionX(received[c].posX);
							plane2->setPositionY(received[c].posY);
						}
					}
				}
			}
		}
	}

}


/**
 * 子弹和敌机碰撞检测函数
 */
void level2::bulletCollisionEnemy() //
{
	Vector<cocos2d::Sprite*> targetsToDelete;
	Vector<cocos2d::Sprite*> bulletsToDelete;
	//Vector<cocos2d::Sprite*> targets2ToDelete;
	//Vector<cocos2d::Sprite*> bullets2ToDelete;
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
			this->enemy1Blowup(t);
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
		if (p->getTag() == 2)
		{
  			this->planeLayer->score += 10; 
		}
	}
	bulletsToDelete.clear();
}   

bool level2::enemyCollisionPlane() 
{	
	for (int j = 0; j < _targets.size(); j++)
	{
		auto target = _targets.at(j);
	/*	if (!boom)
		{*/
			if (this->planeLayer->getChildByTag(AIRPLANE)->boundingBox().intersectsRect(target->getBoundingBox()))//本机的碰撞检测
			{
				this->removeChild(target);
				//this->removeChild(player);
				this->planeLayer->BlowupPlayer1();

			//	boom = true;

				//check network
				if (isConnected)
				{
					//send gameover signal through network
					RakNet::BitStream BsOut;
					BsOut.Write((RakNet::MessageID)ID_GAME_PLAYER_LOST);
					player2->Send(&BsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,player2Adress,false);
				}

				//show the end scene
  				auto scene2 = GameOverScene::createScene(this->planeLayer->score);
				CCTransitionMoveInB *animateScene=CCTransitionMoveInB::create(0.5f, scene2);
				CCDirector::sharedDirector()->replaceScene(animateScene);
				this->unscheduleUpdate();
			}
	//	}
		
	}

	return false;
}


void level2::enemy1Blowup(Sprite* t)
{

	auto animation3 = Animation::create();
	animation3->setDelayPerUnit(0.1f);
	animation3->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_down1.png"));
	animation3->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_down2.png"));
	animation3->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_down3.png"));
	animation3->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_down4.png"));
	animation3->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_down5.png"));
	animation3->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("enemy3_down6.png"));
	/*AnimationCache::getInstance()->addAnimation(animation3, "Enemy3Blowup");
	auto animation = AnimationCache::getInstance()->getAnimation("Enemy3Blowup");*/
	auto animate = Animate::create(animation3);
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect("enemy3_down.mp3");
	auto removeTarget = CallFuncN::create(CC_CALLBACK_1(level2::removeEnemy, this));
	auto sequence = Sequence::create(animate, removeTarget, nullptr);
	t->runAction(sequence);
	
}
void level2::removeEnemy(Object* pSender)
{
	Sprite *sprite = (Sprite *)pSender;
	auto enemy =  sprite;
	if (enemy != nullptr)
	{
		enemy->stopAllActions();
		this->removeChild(enemy, true);
	}
}

void level2::DeleteDisctionnectionWarning(CCObject* warning)
{
	this->removeChild((CCLabelBMFont*)warning,true);
}

void level2::menuCloseCallback(Ref* pSender) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
	return;
#endif
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

