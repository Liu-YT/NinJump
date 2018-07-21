#include "GameScene.h"
#include "GifAnimation.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

using namespace CocosDenshion;

Scene* GameScene::createScene()
{
	return GameScene::create();
}

// on "init" you need to initialize your instance
bool GameScene::init()
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
	
	auto leftWall = Sprite::create("images/left_wall.jpg");
	leftWall->setPosition(Vec2(leftWall->getContentSize().width * 0.6 / 2, visibleSize.height / 2));
	leftWall->setScaleY(visibleSize.height / leftWall->getContentSize().height);
	leftWall->setScaleX(0.6);
	this->addChild(leftWall, 0);

	auto rightWall = Sprite::create("images/right_wall.jpg");
	rightWall->setPosition(Vec2(visibleSize.width - rightWall->getContentSize().width * 0.6 / 2, visibleSize.height / 2));
	rightWall->setScaleY(visibleSize.height / rightWall->getContentSize().height);
	rightWall->setScaleX(0.6);
	this->addChild(rightWall, 0);

	wallWidth = leftWall->getContentSize().width * 0.6 / 2;

	loadMyAnimationsAndSprite();

	player->setPosition(Vec2(leftWall->getContentSize().width * 0.6 + player->getContentSize().width / 2, visibleSize.height * 0.6 / 2 + origin.y));
	this->addChild(player, 1, 1);

	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("RunAtLeft")));
	loadMyMusic();

	// 一次只能播放一个动画
	mutex = false;

	// 地图上是否已经存在攻击者
	attack = false;

	// 玩家位置
	position = false;

	// 添加监听器
	addTouchListener();

	// 调度器
	schedule(schedule_selector(GameScene::attackPlayer), 0.5f, kRepeatForever, 0);

	return true;
}

void GameScene::loadMyAnimationsAndSprite()
{
	GifAnimationDef def;
	def.loops = -1;						  // 循环次数
	def.delayPerUnit = 0.1f;			  // 每帧间隔
	def.restoreOriginalFrame = false;	  // 还原初始状态

	def.filePath = "images/left_run.gif"; // 文件路径
	player = Sprite::createWithTexture(GifAnimation::getInstance()->getFristTexture(def.filePath));
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "RunAtLeft");

	def.filePath = "images/right_run.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "RunAtRight");

	def.filePath = "images/move.gif";
	def.loops = 1;
	def.delayPerUnit = 0.05f;
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "Move");
	def.loops = -1;
	def.delayPerUnit = 0.1f;

	//加载骑士和骑士冲锋动画
	def.filePath = "images/cavalry_left.gif";
	cavalry = Sprite::createWithTexture(GifAnimation::getInstance()->getFristTexture(def.filePath));
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "CavalryLeft");

	def.filePath = "images/cavalry_right.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "CavalryRight");

	//加载狐狸和狐狸冲锋动画
	def.filePath = "images/fox_left.gif";
	cavalry = Sprite::createWithTexture(GifAnimation::getInstance()->getFristTexture(def.filePath));
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "FoxLeft");

	def.filePath = "images/fox_right.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "FoxRight");

	//加载鸟和鸟飞行动画
	def.filePath = "images/bird_left.gif";
	def.loops = 1;
	fox = Sprite::createWithTexture(GifAnimation::getInstance()->getFristTexture(def.filePath));
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "BirdLeft");

	def.filePath = "images/bird_right.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "BirdRight");
}

void GameScene::loadMyMusic()
{
	auto audio = SimpleAudioEngine::getInstance();
	
	audio->preloadBackgroundMusic("sounds/background.mp3");
	audio->playBackgroundMusic("sounds/background.mp3", true);
}

// 添加触摸事件监听器
void GameScene::addTouchListener() {
	// Todo

	//事件分发器
	auto dispatcher = Director::getInstance()->getEventDispatcher();

	//创建单点触摸监听器
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);

	dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

bool GameScene::onTouchBegan(Touch *touch, Event *event) {
	if (!mutex) {
		mutex = true;
		//播放动作时候的动画
		player->getActionManager()->removeAllActionsFromTarget(this->getChildByTag(1));
		Size visibleSize = Director::getInstance()->getVisibleSize();
		float moveX = -1.0f * (2 * player->getPosition().x - visibleSize.width);
		float backX = player->getPosition().x;
		float backY = player->getPosition().y;

		auto playerMove0 = MoveTo::create(0.1, Vec2(backX + moveX * 1 / 8, backY + 12.0f));
		auto playerMove1 = MoveTo::create(0.1, Vec2(backX + moveX * 2 / 8, backY + 20.0f));
		auto playerMove2 = MoveTo::create(0.1, Vec2(backX + moveX * 3 / 8, backY + 27.0f));
		auto playerMove3 = MoveTo::create(0.1, Vec2(backX + moveX * 4 / 8, backY + 31.5f));
		auto playerMove4 = MoveTo::create(0.1, Vec2(backX + moveX * 5 / 8, backY + 27.0f));
		auto playerMove5 = MoveTo::create(0.1, Vec2(backX + moveX * 6 / 8, backY + 20.0f));
		auto playerMove6 = MoveTo::create(0.1, Vec2(backX + moveX * 7 / 8, backY + 12.0f));
		auto playerMove7 = MoveTo::create(0.1, Vec2(backX + moveX * 8 / 8, backY + 0.0f));

		
		Animate* moveAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("Move"));
		auto set = CallFunc::create(([this]() {
			position = !position;
			if(position)
				player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("RunAtRight")));
			else 
				player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("RunAtLeft")));
			mutex = false;
		}));
		//当播放完动画以后才设置mutex的值
		Sequence* moveSeq = Sequence::create(playerMove0, playerMove1, playerMove2, playerMove3, playerMove4, playerMove5, playerMove6, playerMove7, NULL);

		Spawn* spawn = Spawn::create(moveAnimate, moveSeq, NULL);
		Sequence* seq = Sequence::create(spawn, set, NULL);
		player->runAction(seq);
	}
	return true;
}

void GameScene::onTouchEnded(Touch *touch, Event *event) {

}

void GameScene::attackPlayer(float f) {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	if (!attack) {
		attack = true;
		Sprite* bird;
		Animate* birdAnimate;
		if (!position) {
			// left
			bird = Sprite::create("images/bird_l.png");
			bird->setScale(0.8);
			bird->setPosition(visibleSize.width * 3 / 4, visibleSize.height * 3 / 4);
			birdAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("BirdLeft"));
		} 
		else {
			// right
			bird = Sprite::create("images/bird_r.png");
			bird->setScale(0.8);
			bird->setPosition(visibleSize.width * 1 / 4, visibleSize.height * 3 / 4);
			birdAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("BirdRight"));
		}
		this->addChild(bird, 1);
		auto set = CallFunc::create(([this]() {
			attack = false;
		}));
		auto move = MoveTo::create(0.8, player->getPosition());
		Spawn* birdSpawn = Spawn::create(birdAnimate, move, set, NULL);
		Sequence* birdSeq = Sequence::create(birdAnimate, birdSpawn, NULL);
		bird->runAction(birdSeq);
	}
}