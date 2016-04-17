//
//  Box2dAndPEScene.hpp
//  Cocos2dxBox2dSample
//
//  Created by おかひろ on 2016/04/17.
//
//

#ifndef Box2dAndPEScene_hpp
#define Box2dAndPEScene_hpp

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "Box2D/Box2D.h"

// シーン
class Box2dAndPEScene : public cocos2d::Layer
{
private:
	b2World *_world;	// 物理ワールド
	
	const float PTM_RATIO = 32.0f;
	const float TIME_STEP = 1.0f / 60.0f;
	const int VELOCITY_ITERATIONS = 8;
	const int POSITION_ITERATIONS = 3;
	
	int _blockNo = 0;	// ブロックにつける番号
private:
	Box2dAndPEScene();
	~Box2dAndPEScene();
	
public:
	virtual bool init() override;
	static cocos2d::Scene* createScene();
	CREATE_FUNC(Box2dAndPEScene);
	
	void update(float delta) override;
	
	// ブロックを作成
	void createBlock(int x,int y);
	// 地面を作成
	void createGround();
};

#endif /* Box2dAndPEScene_hpp */
