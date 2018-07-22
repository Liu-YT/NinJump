#include "GameScene.h"
#include "GifAnimation.h"
#include "SimpleAudioEngine.h"

USING_NS_CC;

using namespace CocosDenshion;
enum Tag { ROOF, CAVALRY, BIRD, PLAYER };

void GameScene::setPhysicsWorld(PhysicsWorld* world) { m_world = world; }
Scene* GameScene::createScene() {
	srand((unsigned)time(NULL));
	auto scene = Scene::createWithPhysics();
	scene->getPhysicsWorld()->setAutoStep(true);
	scene->getPhysicsWorld()->setGravity(Vec2(0, -300.0f));
	auto layer = GameScene::create();
	layer->setPhysicsWorld(scene->getPhysicsWorld());
	// layer->setJoint();
	scene->addChild(layer);
	return scene;
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
	//////////////////////////////
	// 1. super init first
	dtime = 0;
	if (!Scene::init())
	{
		return false;
	}
	// log("%d %d %d %d", ROOF, CAVALRY, BIRD, PLAYER);
	schedule(schedule_selector(GameScene::updateCustom), 0.01f, kRepeatForever, 0);
	schedule(schedule_selector(GameScene::generateRoofs), 1.0f, kRepeatForever, 0);
	schedule(schedule_selector(GameScene::generateAttacker), 2.0f, kRepeatForever, 0);

	visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	auto bg = Sprite::create("images/newBg.jpg");
	bg->setPosition(Vec2(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));
	bg->setScale(visibleSize.width / bg->getContentSize().width,
		visibleSize.height / bg->getContentSize().height);
	this->addChild(bg, 0);
	
	auto leftWall1 = Sprite::create("images/left_wall.jpg");
	leftWall1->setAnchorPoint(Vec2(0.5,0.5));
	leftWall1->setPosition(Vec2(leftWall1->getContentSize().width * 0.6 / 2, visibleSize.height / 2));
	leftWall1->setScaleY(visibleSize.height / leftWall1->getContentSize().height + 0.1);
	leftWall1->setScaleX(0.6);
	this->addChild(leftWall1, 0);

	auto leftWall2 = Sprite::create("images/left_wall.jpg");
	leftWall2->setAnchorPoint(Vec2(0.5, 0.5));
	leftWall2->setPosition(Vec2(leftWall2->getContentSize().width * 0.6 / 2, visibleSize.height / 2 + visibleSize.height));
	leftWall2->setScaleY(visibleSize.height / leftWall2->getContentSize().height + 0.1);
	leftWall2->setScaleX(0.6);
	this->addChild(leftWall2, 0);

	walls.push_back(leftWall1);
	walls.push_back(leftWall2);

	auto rightWall1 = Sprite::create("images/right_wall.jpg");
	rightWall1->setAnchorPoint(Vec2(0.5, 0.5));
	rightWall1->setPosition(Vec2(visibleSize.width - rightWall1->getContentSize().width  * 0.6 / 2, visibleSize.height / 2));
	rightWall1->setScaleY(visibleSize.height / rightWall1->getContentSize().height + 0.1);
	rightWall1->setScaleX(0.6);
	this->addChild(rightWall1, 0);

	auto rightWall2 = Sprite::create("images/right_wall.jpg");
	rightWall2->setAnchorPoint(Vec2(0.5, 0.5));
	rightWall2->setPosition(Vec2(visibleSize.width - rightWall2->getContentSize().width * 0.6 / 2, visibleSize.height / 2 + visibleSize.height));
	rightWall2->setScaleY(visibleSize.height / rightWall2->getContentSize().height + 0.1);
	rightWall2->setScaleX(0.6);
	this->addChild(rightWall2, 0);

	walls.push_back(rightWall1);
	walls.push_back(rightWall2);

	scoreboard = Sprite::create("images/scoreboard.jpg");
	scoreboard->setPosition(Vec2(visibleSize.width / 2, visibleSize.height - 30));
	scoreboard->setScale(2);
	this->addChild(scoreboard, 1);

	scoreLabel = Label::createWithTTF("0", "fonts/STXINWEI.TTF", 40);
	scoreLabel->setPosition(visibleSize.width / 2, visibleSize.height - 30);
	scoreLabel->setColor(Color3B(255, 255, 255));
	this->addChild(scoreLabel, 1);

	loadMyAnimationsAndSprite();

	player->setPosition(Vec2(leftWall1->getContentSize().width * 0.6 + player->getContentSize().width * 0.7 / 2, visibleSize.height / 3 + origin.y));
	this->addChild(player, 1, 3);

	player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("RunAtLeft")));
	loadMyMusic();
	once = false;
	mutex = false;
	position = false;
	invincible = false;
	invincibleTime = 0;
	// 计分
	score = 0;

	// 添加监听器
	addTouchListener();

	/* 数据库sqlite */
	char * errMsg = NULL;	//错误信息   
	std::string sqlstr;		//SQL指令   
	int result;				//sqlite3_exec返回值   
							//打开一个数据库，如果该数据库不存在，则创建一个数据库文件   
	result = sqlite3_open("NinJump.db", &db);
	//创建表，设置ID为主键，且自动增加   
	result = sqlite3_exec(db, "create table if not exists Scores( ID integer primary key autoincrement, score nvarchar(32) ) ", NULL, NULL, &errMsg);

	// 调度器
	// schedule(schedule_selector(GameScene::attackPlayer), 1.0f, kRepeatForever, 0);
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
	player->setScale(0.7);
	auto playerPhysicsBody = PhysicsBody::createBox(Size(player->getContentSize().width * 0.7, player->getContentSize().height * 0.7), PhysicsMaterial(100.0f, 1.0, 1.0f));
	playerPhysicsBody->setCategoryBitmask(0x3);
	playerPhysicsBody->setCollisionBitmask(0x3);
	playerPhysicsBody->setContactTestBitmask(0x3);
	playerPhysicsBody->setDynamic(false);
	player->setTag(PLAYER);
	player->setPhysicsBody(playerPhysicsBody);
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
	def.loops = 3;
	Sprite* cavalry = Sprite::createWithTexture(GifAnimation::getInstance()->getFristTexture(def.filePath));
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "CavalryLeft");


	def.filePath = "images/cavalry_right.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "CavalryRight");
	def.loops = -1;

	//加载狐狸和狐狸冲锋动画
	def.filePath = "images/fox_left.gif";
	Sprite* fox = Sprite::createWithTexture(GifAnimation::getInstance()->getFristTexture(def.filePath));
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "FoxLeft");

	def.filePath = "images/fox_right.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "FoxRight");

	//加载鸟和鸟飞行动画
	def.filePath = "images/bird_left.gif";
	def.loops = 1;
	// bird = Sprite::createWithTexture(GifAnimation::getInstance()->getFristTexture(def.filePath));
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "BirdLeft");

	def.filePath = "images/bird_right.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "BirdRight");
	
	// 加载死亡动画
	def.filePath = "images/left_fall.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "FallLeft");
	def.filePath = "images/right_fall.gif";
	AnimationCache::getInstance()->addAnimation(GifAnimation::getInstance()->createAnimation(def), "FallRight");

}

void GameScene::loadMyMusic()
{
	auto audio = SimpleAudioEngine::getInstance();
	audio->preloadBackgroundMusic("sounds/background.mp3");
	audio->playBackgroundMusic("sounds/background.mp3", true);
	audio->preloadEffect("sounds/fall.ogg");
}

void GameScene::updateCustom(float dt) {
	auto leftWall1 = walls[0];
	auto leftWall2 = walls[1];
	auto rightWall1 = walls[2];
	auto rightWall2 = walls[3];

	leftWall1->runAction(MoveBy::create(0.01, Vec2(0, std::max(-8.0, -3-0.02*dtime))));
	leftWall2->runAction(MoveBy::create(0.01, Vec2(0, std::max(-8.0, -3 - 0.02*dtime))));
	rightWall1->runAction(MoveBy::create(0.01, Vec2(0, std::max(-8.0, -3 - 0.02*dtime))));
	rightWall2->runAction(MoveBy::create(0.01, Vec2(0, std::max(-8.0, -3 - 0.02*dtime))));

	if (leftWall1->getPositionY() <= 0 - leftWall1->getContentSize().height * 1.4 / 2) {
		leftWall1->setPosition(Vec2(leftWall1->getContentSize().width * 0.6 / 2, visibleSize.height / 2 + visibleSize.height));
	}
	if (leftWall2->getPositionY() <= 0 - leftWall2->getContentSize().height * 1.4 / 2) {
		leftWall2->setPosition(Vec2(leftWall2->getContentSize().width * 0.6 / 2, visibleSize.height / 2 + visibleSize.height));
	}
	if (rightWall1->getPositionY() <= 0 - rightWall1->getContentSize().height * 1.4 / 2) {
		rightWall1->setPosition(Vec2(visibleSize.width - rightWall1->getContentSize().width * 0.6 / 2, visibleSize.height / 2 + visibleSize.height));
	}
	if (rightWall2->getPositionY() <= 0 - rightWall2->getContentSize().height * 1.4 / 2) {
		rightWall2->setPosition(Vec2(visibleSize.width - rightWall2->getContentSize().width * 0.6 / 2, visibleSize.height / 2 + visibleSize.height));
	}

	for (auto roof : roofs) {
		roof->runAction(MoveBy::create(0.01, Vec2(0, std::max(-8.0, -3 - 0.02*dtime))));
	}

	if (invincible) {
		Galaxy->setPosition(player->getPosition());
	}

	if(score > get()){
		if (once == false) {
			once = true;
			ParticleFireworks* firework = ParticleFireworks::create();
			firework->setPosition(visibleSize.width / 2, visibleSize.height / 3 * 2);
			firework->setDuration(2);
			this->addChild(firework, 1);
		}
	}
} 

void GameScene::generateRoofs(float dt) {
	int num = random(1, 2);
	Sprite *roof;
	dtime++;
	score += 20;
	scoreLabel->setString(std::to_string(score));
	if (invincible) {
		invincibleTime--;
		if (invincibleTime == -1)
			invincible = false;
	}
	if (num == 1) {
		roof = Sprite::create("images/left_roof" + std::to_string(random(2, 3)) + ".png");
		roof->setPosition(Vec2(walls[0]->getContentSize().width * 0.6 + roof->getContentSize().width / 3, visibleSize.height));
	}
	if (num == 2) {
		roof = Sprite::create("images/right_roof" + std::to_string(random(2, 3)) + ".png");
		roof->setPosition(Vec2(visibleSize.width - walls[0]->getContentSize().width * 0.6 - roof->getContentSize().width / 3, visibleSize.height));
	}

	auto roofPhysicsBody = PhysicsBody::createBox(Size(roof->getContentSize().width, roof->getContentSize().height), PhysicsMaterial(50.0f, 1.0, 0.0f));
	roofPhysicsBody->setCategoryBitmask(0x2);
	roofPhysicsBody->setCollisionBitmask(0x2);
	roofPhysicsBody->setContactTestBitmask(0x2);
	roofPhysicsBody->setDynamic(false);
	roof->setTag(ROOF);
	roof->setPhysicsBody(roofPhysicsBody);
	roofs.push_back(roof);
	this->addChild(roof, 0);
}

void GameScene::generateAttacker(float dt) {
	int num = random(0, 3);
	if (num == 1 || num == 2) {
		birdAttackPlayer();
	}
	else if (num == 3) {
		cavalryAttackPlayer();
	}
}

// 添加触摸事件监听器
void GameScene::addTouchListener() {
	//事件分发器
	auto dispatcher = Director::getInstance()->getEventDispatcher();

	//创建单点触摸监听器
	auto touchListener = EventListenerTouchOneByOne::create();
	touchListener->onTouchBegan = CC_CALLBACK_2(GameScene::onTouchBegan, this);
	touchListener->onTouchEnded = CC_CALLBACK_2(GameScene::onTouchEnded, this);

	dispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(GameScene::onConcactBegin, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);
}

bool GameScene::onTouchBegan(Touch *touch, Event *event) {
	if (!mutex) {
		status = "attacking";
		mutex = true;
		player->getActionManager()->removeAllActionsFromTarget(this->getChildByTag(3));
		Size visibleSize = Director::getInstance()->getVisibleSize();
		float moveX = -1.0f * (2 * player->getPosition().x - visibleSize.width);
		float backX = player->getPosition().x;
		float backY = player->getPosition().y;

		auto playerMove0 = MoveTo::create(0.06, Vec2(backX + moveX * 1 / 8, backY + 12.0f));
		auto playerMove1 = MoveTo::create(0.06, Vec2(backX + moveX * 2 / 8, backY + 20.0f));
		auto playerMove2 = MoveTo::create(0.06, Vec2(backX + moveX * 3 / 8, backY + 27.0f));
		auto playerMove3 = MoveTo::create(0.06, Vec2(backX + moveX * 4 / 8, backY + 31.5f));
		auto playerMove4 = MoveTo::create(0.06, Vec2(backX + moveX * 5 / 8, backY + 27.0f));
		auto playerMove5 = MoveTo::create(0.06, Vec2(backX + moveX * 6 / 8, backY + 20.0f));
		auto playerMove6 = MoveTo::create(0.06, Vec2(backX + moveX * 7 / 8, backY + 12.0f));
		auto playerMove7 = MoveTo::create(0.06, Vec2(backX + moveX * 8 / 8, backY + 0.0f));

		Animate* moveAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("Move"));
		auto set = CallFunc::create(([this]() {
			position = !position;
			if (position)
				player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("RunAtRight")));
			else
				player->runAction(Animate::create(AnimationCache::getInstance()->getAnimation("RunAtLeft")));
			mutex = false;
			status = "running";
		}));
		Sequence* moveSeq = Sequence::create(playerMove0, playerMove1, playerMove2, playerMove3, playerMove4, playerMove5, playerMove6, playerMove7, NULL);

		Spawn* spawn = Spawn::create(moveAnimate, moveSeq, NULL);
		Sequence* seq = Sequence::create(spawn, set, NULL);
		player->runAction(seq);
	}
	return true;
}

void GameScene::onTouchEnded(Touch *touch, Event *event) {

}

// 鸟攻击者
void GameScene::birdAttackPlayer() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Sprite* bird;
	Animate* birdAnimate;

	if (!position) {
		// left
		bird = Sprite::create("images/bird_l.png");
		bird->setScale(0.8);
		bird->setPosition(visibleSize.width * 4 / 5 + 20, visibleSize.height * 3.5 / 4);
		birdAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("BirdLeft"));
	}
	else {
		// right
		bird = Sprite::create("images/bird_r.png");
		bird->setScale(0.8);
		bird->setPosition(visibleSize.width * 1 / 5 - 20, visibleSize.height * 3.5 / 4);
		birdAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("BirdRight"));
	}
	auto birdPhysicsBody = PhysicsBody::createBox(Size(bird->getContentSize().width, bird->getContentSize().height), PhysicsMaterial(100.0f, 1.0, 1.0f));
	birdPhysicsBody->setCategoryBitmask(0x1);
	birdPhysicsBody->setCollisionBitmask(0x1);
	birdPhysicsBody->setContactTestBitmask(0x1);
	birdPhysicsBody->setDynamic(false);
	bird->setTag(BIRD);
	bird->setPhysicsBody(birdPhysicsBody);
	this->addChild(bird, 1);
	auto set = CallFunc::create(([this, bird]() {
		bird->removeFromParentAndCleanup(true);
	}));
	auto move = MoveTo::create(0.8, Vec2(visibleSize.width - bird->getPositionX(), visibleSize.height / 4));
	Spawn* birdSpawn = Spawn::create(birdAnimate, move, NULL);
	Sequence* birdSeq = Sequence::create(birdSpawn,set, NULL);
	bird->runAction(birdSeq);
}

// 人马攻击者
void GameScene::cavalryAttackPlayer() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	Sprite* cavalry;
	Animate* cavalryAnimate;

	if (!position) {
		// left
		cavalry = Sprite::create("images/cavalry_l.png");
		cavalry->setScale(0.8);
		cavalry->setPosition(walls[0]->getContentSize().width * 0.6 + cavalry->getContentSize().width / 2 - 10, visibleSize.height);
		cavalryAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("CavalryLeft"));
	}
	else {
		// right
		cavalry = Sprite::create("images/cavalry_r.png");
		cavalry->setScale(0.8);
		cavalry->setPosition(visibleSize.width - walls[0]->getContentSize().width * 0.6 - cavalry->getContentSize().width / 2 + 10, visibleSize.height);
		cavalryAnimate = Animate::create(AnimationCache::getInstance()->getAnimation("CavalryRight"));
	}
	auto cavalryPhysicsBody = PhysicsBody::createBox(Size(cavalry->getContentSize().width, cavalry->getContentSize().height), PhysicsMaterial(50.0f, 1.0, 0.0f));
	cavalryPhysicsBody->setCategoryBitmask(0x2);
	cavalryPhysicsBody->setCollisionBitmask(0x2);
	cavalryPhysicsBody->setContactTestBitmask(0x2);
	cavalryPhysicsBody->setDynamic(false);
	cavalry->setTag(CAVALRY);
	cavalry->setPhysicsBody(cavalryPhysicsBody);
	this->addChild(cavalry, 1);
	auto set = CallFunc::create(([this, cavalry]() {
		cavalry->removeFromParentAndCleanup(true);
	}));
	auto move = MoveTo::create(1.6f, Vec2(cavalry->getPositionX(), -cavalry->getContentSize().height * 0.8));
	Spawn* cavalrySpawn = Spawn::create(cavalryAnimate, move, NULL);
	Sequence* cavalrySeq = Sequence::create(cavalrySpawn, set, NULL);
	cavalry->runAction(cavalrySeq);
}

bool GameScene::onConcactBegin(PhysicsContact & contact) {
	//auto c1 = contact.getShapeA(), c2 = contact.getShapeB();
	auto nodeA = contact.getShapeA()->getBody()->getNode();
	auto nodeB = contact.getShapeB()->getBody()->getNode();

	if (nodeA->getTag() == PLAYER) {
		if (nodeB->getTag() == ROOF || nodeB->getTag() == CAVALRY) {
			if (!invincible)
				gameOver();
		}
		else if (nodeB->getTag() == BIRD) {
			if (status == "attacking") {
				killBird();
				beInvincible();
				score += 100;
				nodeB->removeFromParentAndCleanup(true);
			}
			else if(!invincible)
				gameOver();
		}
	}
	else if (nodeB->getTag() == PLAYER) {
		if (nodeA->getTag() == ROOF || nodeA->getTag() == CAVALRY) {
			if (!invincible)
				gameOver();
		}
		else if (nodeA->getTag() == BIRD) {
			if (status == "attacking") {
				killBird();
				beInvincible();
				score += 100;
				nodeA->removeFromParentAndCleanup(true);
			}
			else if (!invincible)
				gameOver();
		}
	}
	return true;
}

void GameScene::gameOver() {
	log("gameover");
	_eventDispatcher->removeAllEventListeners();
	unschedule(schedule_selector(GameScene::updateCustom));
	unschedule(schedule_selector(GameScene::generateAttacker));
	unschedule(schedule_selector(GameScene::generateRoofs));
	scoreboard->removeFromParentAndCleanup(true);
	scoreLabel->removeFromParentAndCleanup(true);
	player->getActionManager()->removeAllActionsFromTarget(this->getChildByTag(3));
	auto move = MoveTo::create(2.0f, Vec2(player->getPositionX(), -player->getContentSize().height));
	Spawn* playerSpawn;
	if(position)
		playerSpawn = Spawn::create(Animate::create(AnimationCache::getInstance()->getAnimation("FallRight")), move, NULL);
	else
		playerSpawn = Spawn::create(Animate::create(AnimationCache::getInstance()->getAnimation("FallLeft")), move, NULL);
	player->runAction(playerSpawn);
	SimpleAudioEngine::getInstance()->playEffect("sounds/fall.ogg");
	SimpleAudioEngine::getInstance()->stopBackgroundMusic("sounds/background.mp3");
	auto label1 = Label::createWithTTF("Game Over~", "fonts/STXINWEI.TTF", 60);
	label1->setColor(Color3B(0, 0, 0));
	label1->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	this->addChild(label1);

	Sprite* scoreImage = Sprite::create("images/score.png");
	scoreImage->setPosition(visibleSize.width / 2, visibleSize.height / 4 * 3);
	scoreImage->setScale(0.5);
	this->addChild(scoreImage);

	auto scoreLabel = Label::createWithTTF(std::to_string(score), "fonts/STXINWEI.TTF", 40);
	scoreLabel->setPosition(visibleSize.width / 2, visibleSize.height / 4 * 3 + 40);
	this->addChild(scoreLabel);

	Sprite* historyScoreImage = Sprite::create("images/score2.png");
	historyScoreImage->setPosition(visibleSize.width / 5 * 4 - 40, 70);
	historyScoreImage->setScale(0.5);
	this->addChild(historyScoreImage);

	auto historyInfoLabel = Label::createWithTTF("History", "fonts/STXINWEI.TTF", 17);
	historyInfoLabel->setPosition(visibleSize.width / 5 * 4 + 20, 100);
	this->addChild(historyInfoLabel);

	auto historyScoreLabel = Label::createWithTTF(std::to_string(get()), "fonts/STXINWEI.TTF", 17);
	historyScoreLabel->setPosition(visibleSize.width / 5 * 4 - 70, 100);
	this->addChild(historyScoreLabel);

	auto label2 = Label::createWithTTF("Again", "fonts/STXINWEI.TTF", 40);
	label2->setColor(Color3B(0, 0, 0));
	auto replayBtn = MenuItemLabel::create(label2, CC_CALLBACK_1(GameScene::replayCallback, this));
	Menu* replay = Menu::create(replayBtn, NULL);
	replay->setPosition(visibleSize.width / 2 - 80, visibleSize.height / 2 - 100);
	this->addChild(replay);

	auto label3 = Label::createWithTTF("Quit", "fonts/STXINWEI.TTF", 40);
	label3->setColor(Color3B(0, 0, 0));
	auto exitBtn = MenuItemLabel::create(label3, CC_CALLBACK_1(GameScene::exitCallback, this));
	Menu* exit = Menu::create(exitBtn, NULL);
	exit->setPosition(visibleSize.width / 2 + 90, visibleSize.height / 2 - 100);
	this->addChild(exit);
	if (get() < score)
		store();
} 
// 继续或重玩按钮响应函数
void GameScene::replayCallback(Ref * pSender) {
	Director::getInstance()->replaceScene(GameScene::createScene());
}

// 退出
void GameScene::exitCallback(Ref * pSender) {
	Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

// 忍者斩杀鸟
void GameScene::killBird() {
	Sprite* bird = Sprite::create("images/bird.png");
	bird->setScale(0.2);
	bird->setPosition(Vec2(birds.size() * bird->getContentSize().width * 0.2 + 20, bird->getContentSize().height * 0.2));
	this->addChild(bird);
	birds.push_back(bird);
}

// 无敌状态
void GameScene::beInvincible() {
	if (birds.size() == 3) {
		for (auto it : birds)
			it->removeFromParentAndCleanup(true);
		birds.clear();
		//无敌
		invincible = true;
		invincibleTime = 5;
		Galaxy = ParticleGalaxy::create();
		Galaxy->setPosition(player->getPosition());
		Galaxy->setDuration(5);
		this->addChild(Galaxy, 1);
	}
}

// datebase
//存
void GameScene::store() {

	/* sqlite */
	char * errMsg = NULL;	//错误信息   
	std::string sqlstr = " insert into Scores( score ) values ( '" + Value(score).asString() + "' ) ";
	sqlite3_exec(db, sqlstr.c_str(), NULL, NULL, &errMsg);
	//关闭数据库   
	sqlite3_close(db);

	/* userdefault */
	//检测xml文件是否存在（非必须）
	if (!database->getBoolForKey("isExist")) {
		database->setBoolForKey("isExist", true);
	}

	//存
	int kill = std::atoi(Value(score).asString().c_str());
	if (kill > database->getIntegerForKey("bestScore", 0))
		database->setIntegerForKey("bestScore", kill);

	int round = database->getIntegerForKey("round", 0) + 1;
	database->setIntegerForKey("round", round);

	std::string newKey = "round" + Value(round).asString();
	database->setIntegerForKey(newKey.c_str(), kill);
}

//取
int GameScene::get() {
	int bestScore = database->getIntegerForKey("bestScore", 0);

	return bestScore;
}

