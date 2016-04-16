//
//  Box2dScene.cpp
//  Cocos2dxBox2dSample
//
//  Created by おかひろ on 2016/04/16.
//
//

#include "Box2dScene.hpp"
#include "HelloWorldScene.h"

USING_NS_CC;

#pragma mark - Box2dScene
Box2dScene::Box2dScene() : _world(nullptr)
{
	
}
Box2dScene::~Box2dScene()
{
	// b2Worldを解放
	if(_world)
	{
		delete _world;
		_world = nullptr;
		
		CCLOG("b2World has been safely deleted.");
	}
}

#pragma mark - シーン関連

Scene* Box2dScene::createScene()
{
	auto scene = Scene::create();
	
	auto layer = Box2dScene::create();
	
	scene->addChild(layer);
	
	return scene;
}

// 初期化
bool Box2dScene::init()
{
	if ( !Layer::init() )
	{
		return false;
	}
	
	Size winSize = Director::getInstance()->getWinSize();
	
	// 物理設定
	b2Vec2 gravity;
	gravity.Set(0.0f, -25.0f);	// 重力の値は動きを見ながら調整
	
	// World作成
	_world = new b2World(gravity);
	_world->SetAllowSleeping(true);
	_world->SetContinuousPhysics(true);
	
	// ラベル
	Label *titleLabel = Label::createWithSystemFont("Box2d基礎", "", 48);
	titleLabel->setPosition(Vec2(winSize.width * 0.5f,winSize.height * 0.95f));
	this->addChild(titleLabel);
	
	// もどるボタン
	ui::Button *menuButton = ui::Button::create();
	menuButton->setTitleText("もどる");
	menuButton->setTitleFontSize(26);
	menuButton->setPosition(Vec2(winSize.width * 0.9f,winSize.height * 0.9f));
	menuButton->addTouchEventListener([=](Ref* pSender,ui::Widget::TouchEventType type)
	  {
		  if(type == ui::Widget::TouchEventType::ENDED)
		  {
			  Scene *scene = HelloWorld::createScene();
			  TransitionFade *transition = TransitionFade::create(1.0f, scene, Color3B::WHITE);
			  Director::getInstance()->replaceScene(transition);
		  }
	  });
	this->addChild(menuButton);
	
	// リセットボタン
	ui::Button *resetButton = ui::Button::create();
	resetButton->setTitleText("リセット");
	resetButton->setTitleFontSize(26);
	resetButton->setPosition(Vec2(winSize.width * 0.9f,winSize.height * 0.85f));
	resetButton->addTouchEventListener([=](Ref* pSender,ui::Widget::TouchEventType type)
	   {
		   if(type == ui::Widget::TouchEventType::ENDED)
		   {
			   Scene *scene = Box2dScene::createScene();
			   TransitionFade *transition = TransitionFade::create(1.0f, scene, Color3B::WHITE);
			   Director::getInstance()->replaceScene(transition);
		   }
	   });
	this->addChild(resetButton);
	
	
	// 画面をタップしたら
	auto listenerForSprite = EventListenerTouchOneByOne::create();
	listenerForSprite->setSwallowTouches(true);
	listenerForSprite->onTouchBegan = [=](Touch* touch, Event* event)
	{
		Vec2 pos = touch->getLocation();
		// ブロックを作成
		this->createBlock(pos.x, pos.y);
		
		return true;
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listenerForSprite, this);
	
	// 地面を作成
	this->createGround();
	
	// updateメソッドを開始
	this->scheduleUpdate();
	
	return true;
}

// updateメソッドでBox2dWorldにある物理オブジェクトとNodeの位置と角度を合わせる
void Box2dScene::update(float delta)
{
	_world->Step(TIME_STEP, VELOCITY_ITERATIONS, POSITION_ITERATIONS);
	
	for (b2Body *b = _world->GetBodyList(); b; b = b->GetNext())
	{
		if (b->GetUserData() != nullptr)
		{
			auto myActor = (Node *)b->GetUserData();
			myActor->setPosition(b->GetPosition().x * PTM_RATIO,b->GetPosition().y * PTM_RATIO);
			myActor->setRotation(-1 * CC_RADIANS_TO_DEGREES(b->GetAngle()));
		}
	}
	
	// 画面外に落ちたブロックを削除
	b2Body* node = this->_world->GetBodyList();
	
	while (node)
	{
		b2Body* b = node;
		node = node->GetNext();
		
		Node *n = (Node*)b->GetUserData();
		
		// 物理オブジェクトの高さが一定以下になったら削除
		if (n->getPositionY() < -100)
		{
			this->_world->DestroyBody(b);
			CCLOG("■ブロック%dが削除されました。",n->getTag());
			this->removeChild(n);
		}
	}
}

#pragma mark - 物理オブジェクト
// ブロックを作成する
void Box2dScene::createBlock(int x,int y)
{
	_blockNo++;
	
	// Sprite
	Sprite *block = Sprite::create("res/circle.png");
	block->setTag(_blockNo);		// ブロックに番号をつけてみる
	this->addChild(block);
	
	// 物理オブジェクトBody
	b2Body *blockBody;
	// 物理オブジェクトBody定義
	b2BodyDef blockBodyRef;
	blockBodyRef.type = b2_dynamicBody;
	blockBodyRef.position.Set(x / PTM_RATIO, y / PTM_RATIO);
	blockBodyRef.userData = block;
	blockBody = _world->CreateBody(&blockBodyRef);
	
	// 形状は円
	b2CircleShape circleShape;
	circleShape.m_radius = block->getContentSize().width * 0.5f / PTM_RATIO;
	
	b2FixtureDef circleShapeRef;
	circleShapeRef.shape = &circleShape;
	circleShapeRef.density = 10.0f;		// 密度
	circleShapeRef.friction = 1.0f;		// 摩擦
	circleShapeRef.restitution = 0.0f;	// 反発
	
	// Fixture（これがないと、ブロック同士が衝突判定されない）
	blockBody->CreateFixture(&circleShapeRef);
	
	
	CCLOG("■ブロック%dを作成しました。",_blockNo);
}
// 地面を作成する
void Box2dScene::createGround()
{
	Size winSize = Director::getInstance()->getWinSize();
	
	// Node
	Node *ground = Node::create();
	ground->setPosition(Vec2::ZERO);
	this->addChild(ground);
	
	// 物理オブジェクトBody
	b2Body *groundBody;
	// 物理オブジェクトBody定義
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(ground->getPositionX() / PTM_RATIO,ground->getPositionY() / PTM_RATIO);
	groundBodyDef.userData = ground;
	groundBody = _world->CreateBody(&groundBodyDef);
	
	// 地面　両端は落ちるようになっている
	b2EdgeShape groundShape;
	groundShape.Set(b2Vec2(winSize.width * 0.2f / PTM_RATIO,0),b2Vec2(winSize.width * 0.8f / PTM_RATIO,0));
	groundBody->CreateFixture(&groundShape, 0);
}

