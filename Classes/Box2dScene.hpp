//
//  Box2dScene.hpp
//  Cocos2dxBox2dSample
//
//  Created by おかひろ on 2016/04/16.
//
//

#ifndef Box2dScene_hpp
#define Box2dScene_hpp

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Box2D/Box2D.h"


static const float PTM_RATIO = 32.0f;
static const float TIME_STEP = 1.0f / 60.0f;
static const int VELOCITY_ITERATIONS = 8;
static const int POSITION_ITERATIONS = 3;

// シーン
class Box2dScene : public cocos2d::Layer
{
private:
	b2World *_world;	// 物理ワールド
	
	int _blockNo = 0;	// ブロックにつける番号
private:
	Box2dScene();
	~Box2dScene();
	
public:
	virtual bool init() override;
	static cocos2d::Scene* createScene();
	CREATE_FUNC(Box2dScene);
	
	void update(float delta) override;
	
	// ブロックを作成
	void createBlock(int x,int y);
	// 地面を作成
	void createGround();
};

#endif /* Box2dScene_hpp */
