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

	auto text = Label::createWithTTF("Play Guide", "fonts/arial.TTF", 40);
	text->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - text->getContentSize().height));
	text->setColor(Color3B(0, 0, 0));
	this->addChild(text, 1);

	auto first = Label::createWithTTF("1. You can kill the bird when you click and \n   the player attacks the bird, then you'll\n get 100 score.", "fonts/STXINWEI.TTF", 20);
	first->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 140));
	first->setColor(Color3B(0, 0, 0));
	this->addChild(first, 1);

	auto second = Label::createWithTTF("2. When you kill 3 birds, you get 5 seconds \n   of invincible time.", "fonts/STXINWEI.TTF", 20);
	second->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 210));
	second->setColor(Color3B(0, 0, 0));
	this->addChild(second, 1);

	auto third = Label::createWithTTF("3. When you collide a cavalry or a roof, you \n will die.", "fonts/STXINWEI.TTF", 20);
	third->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 280));
	third->setColor(Color3B(0, 0, 0));
	this->addChild(third, 1);

	auto fourth = Label::createWithTTF("4. In database, We will store your best score. \n", "fonts/STXINWEI.TTF", 20);
	fourth->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 350));
	fourth->setColor(Color3B(0, 0, 0));
	this->addChild(fourth, 1);

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

