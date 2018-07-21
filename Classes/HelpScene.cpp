#include "HelpScene.h"

USING_NS_CC;

Scene* HelpScene::createScene()
{
	return HelpScene::create();
}

// on "init" you need to initialize your instance
bool HelpScene::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Scene::init())
	{
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("images/newBg.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	bg->setScale(visibleSize.width / bg->getContentSize().width,
		visibleSize.height / bg->getContentSize().height);
	this->addChild(bg, 0);

	auto text = Label::createWithTTF("Player Guide", "fonts/arial.TTF", 40);
	text->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - text->getContentSize().height));
	text->setColor(Color3B(0, 0, 0));
	this->addChild(text, 1);

	auto backButton = MenuItemFont::create("Back", [](Ref* pSender) {
		Director::getInstance()->popScene();
	});
	if (backButton) {
		float x = origin.x + visibleSize.width / 2;
		float y = origin.y + backButton->getContentSize().height / 2 + 10;
		backButton->setPosition(Vec2(x, y));
		backButton->setColor(Color3B(255, 0, 0));
	}
	auto menu = Menu::create(backButton, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 1);

	return true;
}

