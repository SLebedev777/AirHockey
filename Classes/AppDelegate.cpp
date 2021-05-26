/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "AppDelegate.h"
#include "HelloWorldScene.h"

// #define USE_AUDIO_ENGINE 1

#if USE_AUDIO_ENGINE
#include "audio/include/AudioEngine.h"
using namespace cocos2d::experimental;
#endif

USING_NS_CC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
static cocos2d::Size designResolutionSize = cocos2d::Size(1080, 1920);  // resolution of average Android phones
static cocos2d::Size smallResolutionSize = cocos2d::Size(720, 1280);  // resolution of cheap Android phones
static cocos2d::Size mediumResolutionSize = cocos2d::Size(1080, 1920);  // resolution of medium Android phones
static cocos2d::Size largeResolutionSize = cocos2d::Size(1440, 2560);  // resolution of top-notch Android phones
#else
static cocos2d::Size designResolutionSize = cocos2d::Size(1080, 1920);  // 9:16 window size for most popular 1920x1080 PC fullscreen resolution
static cocos2d::Size smallResolutionSize = cocos2d::Size(360, 640);  // 9:16 window size for 768 PC fullscreen height resolution
static cocos2d::Size mediumResolutionSize = cocos2d::Size(405, 768);  // 9:16 window size for 960 PC fullscreen height resolution
static cocos2d::Size largeResolutionSize = cocos2d::Size(540, 960);  // 9:16 window size for 1080 PC fullscreen height resolution
#endif

AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate() 
{
#if USE_AUDIO_ENGINE
    AudioEngine::end();
#endif
}

// if you want a different context, modify the value of glContextAttrs
// it will affect all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributes: red,green,blue,alpha,depth,stencil,multisamplesCount
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8, 0};

    GLView::setGLContextAttrs(glContextAttrs);
}

// if you want to use the package manager to install more packages,  
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
        glview = GLViewImpl::create("airhockey");
        Size monitorSize = reinterpret_cast<GLViewImpl*>(glview)->getMonitorSize();
        Size fittingFrameSize;  // fit window height to user screen resolution height
        if (monitorSize.height >= 1080)
        {
            fittingFrameSize = Size(540, 960);
        }
        else if (monitorSize.height >= 960)
        {
            fittingFrameSize = Size(405, 768);
        }
        else if (monitorSize.height >= 768)
        {
            fittingFrameSize = Size(360, 640);
        }
        else
        {
            fittingFrameSize = Size(270, 480);
        }

        glview->setFrameSize(fittingFrameSize.width, fittingFrameSize.height);
#else
        glview = GLViewImpl::create("airhockey");
#endif
        director->setOpenGLView(glview);
    }

    // turn on display FPS
    director->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0f / 60);

    // Set the design resolution
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::SHOW_ALL);
    
    
    // ContentScaleFactor != 1.0, as it is set in the code below,  makes all sprites 
    // occupy the "physical" area on a screen AS IT IS FOR DESIGN RESOLUTION,
    // no matter what the actual device screen resolution and frame size are.
    // So, the sizes of the objects of the game will be THE SAME on various screen resolution.
    //
    // If ContenScaleFactor == 1.0, and the design resolution is set, the visible size of sprites will be
    // proportionally SCALED according to frameSize/designResolutionSize.
    // The "physical" size of assets will CHANGE from design resolution, but RELATIVE size of assets will be
    // AS FOR DESIGN RESOLUTION.
    
    /*
    auto frameSize = glview->getFrameSize();
    // if the frame's height is larger than the height of medium size.
    if (frameSize.height > mediumResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(largeResolutionSize.height/designResolutionSize.height, largeResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is larger than the height of small size.
    else if (frameSize.height > smallResolutionSize.height)
    {        
        director->setContentScaleFactor(MIN(mediumResolutionSize.height/designResolutionSize.height, mediumResolutionSize.width/designResolutionSize.width));
    }
    // if the frame's height is smaller than the height of medium size.
    else
    {        
        director->setContentScaleFactor(MIN(smallResolutionSize.height/designResolutionSize.height, smallResolutionSize.width/designResolutionSize.width));
    }
    */
    director->setContentScaleFactor(1.0f);

    // add paths to packed art according to design resolution
    std::vector<std::string> searchPaths;
    if (designResolutionSize.height == 1920)
    {
        searchPaths.push_back("HD");
    }
    else
    {
        throw std::invalid_argument("Unsupported design resolution");
    }
    FileUtils::getInstance()->setSearchPaths(searchPaths);

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. Note, when receiving a phone call it is invoked.
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::pauseAll();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

#if USE_AUDIO_ENGINE
    AudioEngine::resumeAll();
#endif
}
