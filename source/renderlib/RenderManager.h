//
//  RenderManager.h
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#ifndef __Splash__RenderManager__
#define __Splash__RenderManager__

#include <iostream>
#include <vector>
#include "IRenderable.h"
#include "SimpleCamera.h"
#include "vmath.hpp"

namespace renderlib{
  
using namespace vmath;
  
class RenderManager
{
public:
	typedef enum {
		RES_640_480 = ((640 << 16) | 480),  // 41943520
		RES_800_600 = ((800 << 16) | 600),  // 52429400
		RES_1280_1024 = ((1280 << 16) | 1024),
		RES_1920_1200 = ((1920 << 16) | 1200),
		RES_1280_720 = ((1280 << 16) | 720),
		RES_1920_1080 = ((1920 << 16) | 1080),  // 125830200
		RES_1000_1000 = ((1000 << 16) | 1000),
		RES_960_720 = ((960 << 16) | 720),  // Kinect Resolution * 2
		RES_1280_960 = ((1280 << 16) | 960),  // Kinect Resolution * 2
		RES_1920_1440 = ((1920 << 16) | 1440),  // Kinect Resolution * 3
		NUM_RES = 10,
	} WINDOW_RES;

public:
  static RenderManager& getInstance()
  {
    // Instantiated on first use.
    if(instance == nullptr)
      instance = new RenderManager();
    return *instance;
  }
  void setPrintFPS(bool v) { print_fps_ = v;}
  bool getPrintFPS() {return print_fps_;}
  
  void setPrintFPSInterval(float v) { fps_stat_interval_ = v;}
  bool getPrintFPSInterval() {return fps_stat_interval_;}
  
  void drawObjects();
  void registerObject(IRenderable* ro);
  void deRegisterObject(IRenderable* ro);
  
  void updateFPS(float seconds);
  //TODO: move these to diff class like Renderer
  void setFramebufferSize(int w, int h) { _frameBufferSize.x = (float)w; _frameBufferSize.y = (float)h; }
  Vector2 getFramebufferSize() { return _frameBufferSize; }
  //void setNonHDPIWindowSize(int w, int h) { _nonHDPIWindowSize.x = (float)w; _nonHDPIWindowSize.y = (float)h; }
  //Vector2 getNonHDPIWindowSize() { return _nonHDPIWindowSize; }
  //float getDT() { return _dt;}
  //float getCurrentTime() { return _currentTime;}
  
  Camera& getMainCamera(){return _camera;}
  
  //C++ 11 way of hiding these methods
  //Default constructor, no copy constructor or copy assignment operato
  RenderManager()
  {
    //_currentTime = _dt = _lastTime = 0.0f;
    frame_time_accum_ = 0;
    frame_count_ = 0;
    min_fps_ = std::numeric_limits<double>::infinity();
    max_fps_ = 0;
    fps_ = 0;
  };
  
  RenderManager(const RenderManager&) = delete;
  RenderManager & operator=(const RenderManager&) = delete;
  
  
protected:
  void sortObjects();
  
protected:
  static RenderManager*    instance;
  
  Camera _camera;
  std::vector<IRenderable*> _renderables;
  
  //TODO: move this to diff class like Renderer
  Vector2   _frameBufferSize;
  Vector2   _viewFrameSize;
  //Vector2   _nonHDPIWindowSize;
  
  //FIXME: Put this into a time manager class!
  //float     _currentTime;
  //float     _dt;
  //float     _lastTime;
  
  double frame_time_accum_;
  double min_fps_;
  double max_fps_;
  double fps_;
  char fps_buffer_[256];
  unsigned long frame_count_;
  bool print_fps_ = {false};
  float fps_stat_interval_ = {1.0f};
  
};

}//namespace renderlib
#endif /* defined(__Splash__RenderManager__) */
