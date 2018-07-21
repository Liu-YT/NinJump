#pragma once
#include <stdio.h>
#include "cocos2d.h"

USING_NS_CC;

class GameScene : public cocos2d::Scene
{
public:
	static cocos2d::Scene* createScene();

	virtual bool init();

	CREATE_FUNC(GameScene);

private:
	
	cocos2d::Sprite* player;
	cocos2d::Sprite* cavalry;
	cocos2d::Sprite* fox;

	void loadMyAnimationsAndSprite();
	void loadMyMusic();

};