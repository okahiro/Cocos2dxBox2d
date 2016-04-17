#include "HelloWorldScene.h"
#include "Box2dScene.hpp"
#include "Box2dAndPEScene.hpp"

USING_NS_CC;

Scene* HelloWorld::createScene()
{
    auto scene = Scene::create();
    auto layer = HelloWorld::create();
    scene->addChild(layer);
	
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size winSize = Director::getInstance()->getWinSize();
	
	// Box2dサンプル画面へ
	ui::Button *box2dButton = ui::Button::create();
	box2dButton->setTitleText("Box2D基礎");
	box2dButton->setTitleFontSize(42);
	box2dButton->setPosition(Vec2(winSize.width * 0.5f,winSize.height * 0.8f));
	box2dButton->addTouchEventListener([=](Ref* pSender,ui::Widget::TouchEventType type)
	   {
		   if(type == ui::Widget::TouchEventType::ENDED)
		   {
			   Scene *scene = Box2dScene::createScene();
			   TransitionFade *transition = TransitionFade::create(1.0f, scene, Color3B::WHITE);
			   Director::getInstance()->replaceScene(transition);
		   }
	   });
	this->addChild(box2dButton);
	
	// Box2d+PhysicsEditor
	ui::Button *box2dPEButton = ui::Button::create();
	box2dPEButton->setTitleText("Box2D+PhysicsEditor");
	box2dPEButton->setTitleFontSize(42);
	box2dPEButton->setPosition(Vec2(winSize.width * 0.5f,winSize.height * 0.6f));
	box2dPEButton->addTouchEventListener([=](Ref* pSender,ui::Widget::TouchEventType type)
	   {
		   if(type == ui::Widget::TouchEventType::ENDED)
		   {
			   Scene *scene = Box2dAndPEScene::createScene();
			   TransitionFade *transition = TransitionFade::create(1.0f, scene, Color3B::WHITE);
			   Director::getInstance()->replaceScene(transition);
		   }
	   });
	this->addChild(box2dPEButton);
	
    
    return true;
}
