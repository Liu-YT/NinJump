#pragma once
#include <stdio.h>
#include "cocos2d.h"

class MenuScene : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();

	// a selector callback
	void start(Ref * pSender);
    
	void help(Ref * pSender);

    // implement the "static create()" method manually
    CREATE_FUNC(MenuScene);
};
