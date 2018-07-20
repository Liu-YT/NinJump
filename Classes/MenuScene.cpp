#include "MenuScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"
#include "HelpScene.h"

USING_NS_CC;

Scene* MenuScene::createScene()
{
    return MenuScene::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//background
	auto bg = Sprite::create("images/newBg.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	bg->setScale(visibleSize.width / bg->getContentSize().width,
		visibleSize.height / bg->getContentSize().height);
	this->addChild(bg, 0);

	//player
	auto player = Sprite::create("images/left_ninjia.png");
	player->setPosition(Vec2(player->getContentSize().width + origin.x, visibleSize.height / 2 + origin.y));
	player->setScale(2);
	this->addChild(player, 0);

	//bird
	auto bird = Sprite::create("images/bird.png");
	bird->setPosition(Vec2(visibleSize.width / 3, visibleSize.height * 2 / 3));
	bird->setScale(0.35);
	this->addChild(bird, 0);

	//title
	auto title = Sprite::create("images/title.png");
	title->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height - title->getContentSize().height / 2));
	this->addChild(title, 2);

	//start MenuItem
	auto startMenuItem = MenuItemImage::create(
		"images/Play.png",
		"images/Play.png",
		CC_CALLBACK_1(MenuScene::start, this));
	startMenuItem->setPosition(Point(visibleSize.width - startMenuItem->getContentSize().width * 2 / 3 , 100 + startMenuItem->getContentSize().height));

	//help MenuItem
	auto helpMenuItem = MenuItemImage::create(
		"images/Help.png",
		"images/Help.png",
		CC_CALLBACK_1(MenuScene::help, this));
	helpMenuItem->setPosition(Point(visibleSize.width - helpMenuItem->getContentSize().width * 2 / 3, 50 + helpMenuItem->getContentSize().height));

	auto menu = Menu::create(startMenuItem, helpMenuItem, NULL);
	menu->setPosition(Vec2::ZERO);
	this->addChild(menu, 3);
	return true;
}

void MenuScene::start(Ref* pSender) {
	auto gameScene = GameScene::createScene();
	Director::getInstance()->pushScene(TransitionFade::create(0.5, gameScene, Color3B(255,255,255)));
}

void MenuScene::help(Ref* pSender) {
	auto helpScene = HelpScene::createScene();
	Director::getInstance()->pushScene(TransitionFade::create(0.5, helpScene, Color3B(255, 255, 255)));
}
