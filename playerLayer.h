#pragma once
#include "cocos2d.h"
#include "RaknetGlobals.h"
using namespace cocos2d;
const int AIRPLANE=111; //111=player1,222=player2
const int AIRPLANE2=222; 

struct playerInfo
{
	float posX;
	float posY;
	//bool isAlive;
	//int score;
};

struct bulletInfo
{
	float posX;
	float posY;
	//bool isAlive;
	//int score;
};

struct EnemyInfo
{
	float posX;
	float posY;
	int actualX;
	int actualDuration;
	//bool isAlive;
	//int score;
};

class playerLayer : public Layer
{
public:
	playerLayer(void);
	~playerLayer(void);

	static playerLayer* create();
//	CREATE_FUNC(playerLayer);
	virtual bool init();

	void BlowupPlayer1();

	void RemovePlane();

	/*void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);*/
	/*void MoveTo(Point location);

	void Blowup(int passScore);

	void RemovePlane();*/

public:

	static playerLayer* sharedPlane;

	bool isAlive;

	int score;
};

