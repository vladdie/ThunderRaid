#ifndef __WAITING_CONNECTION_SCENE__
#define __WAITING_CONNECTION_SCENE__

#include "cocos2d.h"

#include "MainScene.h"
#include "RaknetGlobals.h"
#include "gameScene.h"
#include "WelcomeScene.h"


using namespace cocos2d;
// this scene just keeps checking if the other player is ready to play, like a lobby
class WaitingConnectionScene : public Layer
{
public:

	// setup scene
	virtual bool init();

	virtual ~WaitingConnectionScene();
	static Scene* scene();

	// jump menu item callback
	void JumpCallback(CCObject* pSender);

	//// called each frame
	void update(float dt);

	// implement the "static node()" method manually
	CREATE_FUNC(WaitingConnectionScene);


	/* RakNet::RakPeerInterface* player2;
	RakNet::SystemAddress player2Adress;*/
private:
	// emits a warning when there's a connected peer
	CCLabelBMFont* connectedSign;

};

#endif // __WAITING_CONNECTION_SCENE__