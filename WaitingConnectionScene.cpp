#include "WaitingConnectionScene.h"

#define SERVER_PORT 59999
#define CLIENT_PORT 49999
#define MAX_CLIENTS 10
#define SERVER_ADD "255.255.255.255"
using namespace cocos2d;

CCScene* WaitingConnectionScene::scene()
{
    CCScene *scene = CCScene::create();
    WaitingConnectionScene *layer = WaitingConnectionScene::create();
    scene->addChild(layer);
    return scene;
}

WaitingConnectionScene::~WaitingConnectionScene()
{
        player2->Shutdown(100);
        RakNet::RakPeerInterface::DestroyInstance(player2);
}

bool WaitingConnectionScene::init()
{
        // super init first of all
        if (!CCLayer::init())
        {
                return false;
        }
		Size winSize = Director::getInstance()->getWinSize();
        //connectedSign = CCLabelBMFont::create("","font.fnt");
		connectedSign = CCLabelBMFont::create("","badab.fnt");
		connectedSign->setAnchorPoint(Point::ZERO);
        connectedSign->setPosition(Point(winSize.width/2-20,winSize.height/2-100));
        this->addChild(connectedSign);

       // // create instance here using global pointer
       // // create instance of a client and try to connect
        player2 = RakNet::RakPeerInterface::GetInstance();
        RakNet::SocketDescriptor sd(CLIENT_PORT,0);
        player2->Startup(1, &sd, 1);
        RakNet::Packet* packet;
		player2->SetMaximumIncomingConnections(1);
        isServer = true;//第一个打开游戏的作为server
		
       // //start broadcast
        if (player2->Ping("255.255.255.255",SERVER_PORT,false))//broadcast寻找有没有可以连接的server
        {
                Sleep(500);
                for (packet = player2->Receive();packet;player2->DeallocatePacket(packet),packet=player2->Receive())
                {
                        //DefaultMessageIDTypes i = (DefaultMessageIDTypes)packet->data[0];
                        if (packet->data[0] == ID_UNCONNECTED_PONG)//没有收到ping的返回信息
                        {
                                //try to connect
                                player2->Connect(packet->systemAddress.ToString(false),SERVER_PORT,0,0);//向server port请求连接
                                Sleep(200);
                        }
                        else if (packet->data[0] == ID_GAME_MESSAGE_1)//收到ping的返回信息ID_GAME_MESSAGE_1
                        {
                                //reply
                                RakNet::BitStream bsOut;
                                bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
                                player2->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,false);
                                isServer = false;
                                isConnected = true;
                                // change warning
                                connectedSign->setColor(ccc3(0,0,255));
                                connectedSign->setString("Player Connected!");
                                // copy player2 adress
                                player2Adress = packet->systemAddress;
                        }
                }
        }
       // // if didn't find anything, create a server peer完全没收到任何信息，未执行上面任何一句话
        if (isServer)
        {
                //first delete old one
                RakNet::RakPeerInterface::DestroyInstance(player2);
                //create a new
                player2 = RakNet::RakPeerInterface::GetInstance();
                RakNet::SocketDescriptor sd_server(SERVER_PORT,0);
                player2->Startup(MAX_CLIENTS,&sd_server,1);
                player2->SetMaximumIncomingConnections(MAX_CLIENTS);
                connectedSign->setColor(ccc3(255,0,0));
                connectedSign->setString("No Player Connected");
        }

       player2->SetTimeoutTime(30000,RakNet::UNASSIGNED_SYSTEM_ADDRESS);
       //// setup scene, only the server can start the game
        if (isServer)
        {
                //jump button
                CCMenuItemFont* jumpButton = CCMenuItemFont::create("START",this,menu_selector(WaitingConnectionScene::JumpCallback));
                // put the item on the middle of screen
                jumpButton->setPosition(winSize.width/2.0f,winSize.height/2.0f);
                //// create menu to put button
                CCMenu* mainMenu = CCMenu::create(jumpButton, NULL);
                mainMenu->setPosition(Vec2::ZERO);
                this->addChild(mainMenu);
        }
        else
        {
                CCLabelBMFont* awatingPlayerSign = CCLabelBMFont::create("","badab.fnt");
				//CCLabelBMFont* awatingPlayerSign = LabelBMFont::create("","font.fnt");
                awatingPlayerSign->setPosition(winSize.width/2.0f,winSize.height/2.0f);
                awatingPlayerSign->setString("Awating for the other player start the game...");
                this->addChild(awatingPlayerSign);              
        }

       // // put update method to work
        this->scheduleUpdate();

       // //init random seed
        srand(time(0));
        
        return true;
}


void WaitingConnectionScene::update(float dt)
{
        // keep checking for incomming connections

        RakNet::Packet* packet;
        for (packet = player2->Receive();packet;player2->DeallocatePacket(packet),packet=player2->Receive())
        {
                if (packet->data[0] == ID_NEW_INCOMING_CONNECTION)
                {
                        //reply
                        RakNet::BitStream bsOut;
                        bsOut.Write((RakNet::MessageID)ID_GAME_MESSAGE_1);
                        player2->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,packet->systemAddress,false);
                }
                else if (packet->data[0] == ID_GAME_MESSAGE_1)
                {
                        // copy player2 adress
                        player2Adress = packet->systemAddress;
                        connectedSign->setColor(Color3B::BLUE);
                        connectedSign->setString("Player Connected!");
						//auto label = Label::createWithTTF("Player Connected!", "fonts/Marker Felt.ttf", 24);
						//label->setColor(Color3B::BLUE);
						//Size winSize = Director::getInstance()->getWinSize();
						//label->setPosition(Point(winSize.width/2,winSize.height/2-50));
                        isConnected = true;
                }
                else if (packet->data[0] == ID_GAME_START)
                {
                        // start game
                        CCDirector::sharedDirector()->pushScene(CCTransitionProgressOutIn::create(1.0f,WelcomeScene::createScene()));
                }
                // player disconnected
                else if (packet->data[0] == ID_DISCONNECTION_NOTIFICATION || 
                                packet->data[0] == ID_CONNECTION_LOST)
                {
                        // change warning
                        connectedSign->setColor(ccc3(255,0,0));
                        connectedSign->setString("No Player Connected");
                        isConnected = false;
                }
        }

}

void WaitingConnectionScene::JumpCallback(CCObject* pSender)
{
        if (isServer && isConnected)
        {
                // send signal to start the game
                RakNet::BitStream bsOut;
                bsOut.Write((RakNet::MessageID)ID_GAME_START);
                player2->Send(&bsOut,HIGH_PRIORITY,RELIABLE_ORDERED,0,player2Adress,false);
         CCDirector::sharedDirector()->pushScene(CCTransitionProgressOutIn::create(1.0f,WelcomeScene::createScene()));
		}
		if(!isConnected)
		{
			Size winSize = Director::getInstance()->getWinSize();
			CCLabelBMFont* warning = CCLabelBMFont::create("Wait for other player","badab.fnt");
			warning->setPosition(winSize.width/2.0f,winSize.height/2.0f-150);
			warning->setColor(Color3B::ORANGE);
			this->addChild(warning);  
			warning->runAction(CCFadeOut::create(4.0f));//fade
			//this->removeChild(warning);
		}
        // calls next scene
	
		//CCScene* pScene= MainScene::createScene();
		////CCTransitionMoveInB *animateScene=CCTransitionMoveInB::create(0.5f,pScene);
		//CCDirector::sharedDirector()->replaceScene(pScene);
		
        //CCDirector::sharedDirector()->pushScene(CCTransitionProgressOutIn::create(1.0f,WelcomeScene::createScene()));
}
