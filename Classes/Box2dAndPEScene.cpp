//
//  Box2dAndPEScene.cpp
//  Cocos2dxBox2dSample
//
//  Created by おかひろ on 2016/04/17.
//
//

#include "Box2dAndPEScene.hpp"

#include "HelloWorldScene.h"
#include "GB2ShapeCache-x.h"

USING_NS_CC;

#pragma mark - Box2dAndPEScene
Box2dAndPEScene::Box2dAndPEScene() : _world(nullptr)
{
	
}
Box2dAndPEScene::~Box2dAndPEScene()
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

Scene* Box2dAndPEScene::createScene()
{
	auto scene = Scene::create();
	
	auto layer = Box2dAndPEScene::create();
	
	scene->addChild(layer);
	
	return scene;
}

// 初期化
bool Box2dAndPEScene::init()
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
	
	// PhysicsEditorから出力したplistを読み込み
	gbox2d::GB2ShapeCache::getInstance()->addShapesWithFile("res/shapes.plist");
	
	// ラベル
	Label *titleLabel = Label::createWithSystemFont("Box2d+PhysicsEditor", "", 48);
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
			   Scene *scene = Box2dAndPEScene::createScene();
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
void Box2dAndPEScene::update(float delta)
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
void Box2dAndPEScene::createBlock(int x,int y)
{
	_blockNo++;
	
	// ブロックの名前（PhysicsEditer上でつけた名前）
	std::string blockName = "";
	// Noによってブロックを変える
	switch(_blockNo % 4)
	{
		case 0:
			blockName = "circle";
			break;
		case 1:
			blockName = "cross";
			break;
		case 2:
			blockName = "square";
			break;
		case 3:
			blockName = "triangle";
			break;
		default:
			blockName = "circle";
			break;
	}
	
	// Sprite
	Sprite *block = Sprite::create(StringUtils::format("res/%s.png",blockName.c_str()));
	block->setTag(_blockNo);		// ブロックに番号をつける
	this->addChild(block);
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x / PTM_RATIO,y / PTM_RATIO);
	
	b2Body *body = this->_world->CreateBody(&bodyDef);
	gbox2d::GB2ShapeCache::getInstance()->addFixturesToBody(body, blockName);	// bodyに、plist上の指定の物理体情報でFixture作成
	body->SetUserData(block);
	
	// AnchorPointを取得して設定
	block->setAnchorPoint(gbox2d::GB2ShapeCache::getInstance()->anchorPointForShape(blockName));
	
	
	CCLOG("■ブロック%dを作成しました。",_blockNo);
}
// 地面を作成する
void Box2dAndPEScene::createGround()
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
