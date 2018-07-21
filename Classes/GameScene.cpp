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

	loadMyAnimationsAndSprite();

	player->setPosition(Vec2(leftWall->getContentSize().width * 0.6 + player->getContentSize().width / 2, visibleSize.height * 0.6 / 2 + origin.y));
	this->addChild(player, 1);

	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("RunAtLeft")));
	loadMyMusic();

	// һ��ֻ�ܲ���һ������
	mutex = false;
	attack = false;
	position = false;

	// ��Ӽ�����
	addTouchListener();

	// ������
	schedule(schedule_selector(GameScene::attackPlayer), 1.0f, kRepeatForever, 0);

	return true;
}

void GameScene::loadMyAnimationsAndSprite()
{
	GifAnimationDef def;
	def.loops = -1;						  // ѭ������
	def.delayPerUnit = 0.1f;			  // ÿ֡���
	def.restoreOriginalFrame = false;	  // ��ԭ��ʼ״̬

	def.filePath = "images/left_run.gif"; // �ļ�·��
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

	//������ʿ����ʿ��涯��
	def.filePath = "images/cavalry_left.gif";
	cavalry = Sprite::createWithTexture(GifAnimation::getInstance()->getFristTexture(def.filePath));
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "CavalryLeft");

	def.filePath = "images/cavalry_right.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "CavalryRight");

	//���غ���ͺ����涯��
	def.filePath = "images/fox_left.gif";
	cavalry = Sprite::createWithTexture(GifAnimation::getInstance()->getFristTexture(def.filePath));
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "FoxLeft");

	def.filePath = "images/fox_right.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "FoxRight");

	//�����������ж���
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

// ��Ӵ����¼�������
void GameScene::addTouchListener() {
	// Todo

	//�¼��ַ���
	auto dispatcher = Director::getInstance()->getEventDispatcher();

	//�������㴥��������
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);

	dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

bool GameScene::onTouchBegan(Touch *touch, Event *event) {
	if (!mutex) {
		mutex = true;
		player->getActionManager()->removeAllActions();
		//���Ŷ���ʱ��Ķ���
		Size visibleSize = Director::getInstance()->getVisibleSize();
		auto playerMove = MoveTo::create(0.5, Vec2(visibleSize.width - player->getPosition().x, player->getPosition().y));
		Animate* move = Animate::create(AnimationCache::getInstance()->getAnimation("Move"));
		auto set = CallFunc::create(([this]() {
			player->getActionManager()->removeAllActions();
			position = !position;
			if(position)
				player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("RunAtRight")));
			else 
				player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("RunAtLeft")));
			mutex = false;
		}));
		//�������궯���Ժ������mutex��ֵ
		Spawn* spawn = Spawn::create(move, playerMove, NULL);
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
			bird->setPosition(visibleSize.width * 1 / 4, visibleSize.height * 1 / 4);
			birdAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("BirdRight"));
		}
		this->addChild(bird, 1);
		auto move = MoveTo::create(0.8, player->getPosition());
		Spawn* birdSpawn = Spawn::create(birdAnimate, move, NULL);
		Sequence* birdSeq = Sequence::create(birdAnimate, birdSpawn, NULL);
		bird->runAction(birdSeq);
	}
}