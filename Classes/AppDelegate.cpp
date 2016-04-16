#include "AppDelegate.h"
#include "HelloWorldScene.h"

USING_NS_CC;

static Size designResolutionSize = Size(640,960);
static Size designResolutionSize4inch = Size(640,1136);
static Size designResolutionSize4_7inch = Size(750,1334);
static Size designResolutionSize5_5inch = Size(1242,2208);

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() 
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages, 
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_MAC) || (CC_TARGET_PLATFORM == CC_PLATFORM_LINUX)
        glview = GLViewImpl::createWithRect("Cocos2dxBox2dSample", Rect(0, 0, designResolutionSize.width, designResolutionSize.height));
#else
        glview = GLViewImpl::create("Cocos2dxBox2dSample");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

	Size winSize = Director::getInstance()->getWinSize();
	Size visibleSize = Director::getInstance()->getVisibleSize();
	CCLOG("%f,%f , %f,%f",winSize.width,winSize.height,visibleSize.width,visibleSize.height);
	// iPhoneの4、4.7、5.5インチは640x1136に合わせる。それ以外は640x960
	if((winSize.width == designResolutionSize4inch.width && winSize.height == designResolutionSize4inch.height) ||
	   (winSize.width == designResolutionSize4_7inch.width && winSize.height == designResolutionSize4_7inch.height) ||
	   (winSize.width == designResolutionSize5_5inch.width && winSize.height == designResolutionSize5_5inch.height))
	{
		// for 4inch.
		glview->setDesignResolutionSize(designResolutionSize4inch.width, designResolutionSize4inch.height, ResolutionPolicy::SHOW_ALL);
	}
	else
	{
		// for the others.
		glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
	}

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
