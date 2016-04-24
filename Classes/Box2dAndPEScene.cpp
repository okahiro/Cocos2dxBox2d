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
Box2dAndPEScene::Box2dAndPEScene() : _world(nullptr),_debugDraw(nullptr)
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
	// GLESDebugDrawを解放
	if(_debugDraw)
	{
		delete _debugDraw;
		_debugDraw = nullptr;
		
		CCLOG("DebugDraw has been safely deleted.");
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
	
	// DebugDrawを設定
	_debugDraw = new GLESDebugDraw(PTM_RATIO);
	_world->SetDebugDraw(_debugDraw);
	
	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	//        flags += b2Draw::e_jointBit;
	//        flags += b2Draw::e_aabbBit;
	flags += b2Draw::e_pairBit;
	//        flags += b2Draw::e_centerOfMassBit;
	this->_debugDraw->SetFlags(flags);
	
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
	
	// 接触情報取得ボタン
	ui::Button *infoButton = ui::Button::create();
	infoButton->setTitleText("接触情報取得");
	infoButton->setTitleFontSize(26);
	infoButton->setPosition(Vec2(winSize.width * 0.85f,winSize.height * 0.75f));
	infoButton->addTouchEventListener([=](Ref* pSender,ui::Widget::TouchEventType type)
	  {
		  if(type == ui::Widget::TouchEventType::ENDED)
		  {
			  std::string logString = "■接触情報■\n";
			  
			  for (b2Body *b = _world->GetBodyList(); b; b = b->GetNext())
			  {
				  if (b->GetUserData() != nullptr && b->GetLinearVelocity().Length() < 2.0f)
				  {
					  auto node = (Node *)b->GetUserData();
					  
					  // タグ。0は地面、1移行はブロック
					  int tag = node->getTag();
					  logString.append(StringUtils::format("物理体No%d\n",tag));
						  
					  // 物理体から、現時点のコンタクトのリストを取得する。同じ物理体に二箇所以上で接触している場合、その箇所分
					  for (b2ContactEdge* ce = b->GetContactList(); ce; ce = ce->next)
					  {
						  b2Contact* c = ce->contact;
						  // 接触していなければ終了
						  if(!c->IsTouching())
						  {
							  continue;
						  }
						  
						  // b2Contactから、２つのオブエジェクトを取得する。
						  b2Body *bA = c->GetFixtureA()->GetBody();
						  b2Body *bB = c->GetFixtureB()->GetBody();
						  Node *nodeA = (Node*)bA->GetUserData();
						  Node *nodeB = (Node*)bB->GetUserData();
						  
						  int contactNodeTag = -1;
						  
						  if(nodeA->getTag() == tag)
						  {
							  contactNodeTag = nodeB->getTag();
						  }
						  else if(nodeB->getTag() == tag)
						  {
							  contactNodeTag = nodeA ->getTag();
						  }
						  else
						  {
							  continue;
						  }
						  
						  logString.append(StringUtils::format("┗接触物理体No%d\n",contactNodeTag));
					  }
				  }
			  }
			  
			  // ログに表示
			  CCLOG("%s",logString.c_str());
			  
			  // ラベルがあれば削除
			  this->removeChildByTag(999);
			  // ラベルを作って表示
			  Label *infoLabel = Label::createWithSystemFont(logString, "", 20);
			  infoLabel->setTag(999);
			  //infoLabel->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
			  infoLabel->setAnchorPoint(Vec2(0.0f,1.0f));
			  infoLabel->setPosition(Vec2(5,winSize.height * 0.85f));
			  this->addChild(infoLabel);
		  }
	  });
	this->addChild(infoButton);
	
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
	block->setOpacity(80);
	this->addChild(block);
	
	// NoLabel
	Label *blockNoLabel = Label::createWithSystemFont(StringUtils::format("No%d",_blockNo), "", 36);
	blockNoLabel->setPosition(block->getContentSize() * 0.5f);
	blockNoLabel->enableOutline(Color4B::BLACK,2);
	block->addChild(blockNoLabel);
	
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(x / PTM_RATIO,y / PTM_RATIO);
	
	b2Body *body = this->_world->CreateBody(&bodyDef);
	gbox2d::GB2ShapeCache::getInstance()->addFixturesToBody(body, blockName);	// bodyに、plist上の指定の物理体情報でFixture作成
	body->SetUserData(block);
	
	// AnchorPointを取得して設定
	block->setAnchorPoint(gbox2d::GB2ShapeCache::getInstance()->anchorPointForShape(blockName));
	
	// ブロックのサイズ変更
	// 大きさ 0.75倍、等倍、1.25倍
	float scale = 0.75f + ((int)(CCRANDOM_0_1() * 100) % 3) * 0.25f;
	
	if(scale != 1.0f)
	{
		// 物理構造のスケーリング
		for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
		{
			b2Shape *shape = f->GetShape();
			if(shape->GetType() == b2Shape::e_circle)
			{
				// 円の場合は半径をスケール
				body->GetFixtureList()->GetShape()->m_radius *= scale;
			}
			else
			{
				// 円以外の場合
				b2PolygonShape *poly = (b2PolygonShape*)shape;
				int vCount = poly->GetVertexCount();	// 頂点の数
				b2Vec2 vertices[b2_maxPolygonVertices];
				// 全ての頂点にscaleをかける
				for(int i = 0; i < vCount; i++)
				{
					b2Vec2 v = poly->GetVertex(i);
					vertices[i].x = v.x * scale;
					vertices[i].y = v.y * scale;
				}
				// 新しい頂点を設定
				poly->Set(vertices, vCount);
			}
		}
		
		// スプライトもサイズ変更
		block->setScale(scale);
	}
	
	
	CCLOG("■ブロック%dを作成しました。サイズ：%f",_blockNo,scale);
}
// 地面を作成する
void Box2dAndPEScene::createGround()
{
	Size winSize = Director::getInstance()->getWinSize();
	
	// Node
	Node *ground = Node::create();
	ground->setPosition(Vec2::ZERO);
	ground->setTag(0);
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


// デバッグ情報表示
void Box2dAndPEScene::draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& transform, uint32_t flags) {
	Layer::draw(renderer, transform, flags);
	Director* director = Director::getInstance();
	
	GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION );
	director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
	this->_world->DrawDebugData();
	director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
