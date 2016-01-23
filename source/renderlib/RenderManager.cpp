//
//  RenderManager.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//
#include <algorithm>
#include "RenderManager.h"
#include "ObjectIDGenerator.h"

namespace renderlib {
RenderManager* RenderManager::instance = nullptr;

void RenderManager::drawObjects()
{
  for(IRenderable* r : _renderables)
  {
    r->draw();
  }
}

void RenderManager::deRegisterObject(IRenderable *ro)
{
  std::vector<IRenderable*>::iterator it;
  it = std::find(_renderables.begin(), _renderables.end(), ro);
  if(it != _renderables.end())
  {
    _renderables.erase(it);
  }
  
}

void RenderManager::registerObject(IRenderable *ro)
{
  _renderables.push_back(ro);
  sortObjects();
  
}

bool sortMethod(IRenderable* a, IRenderable* b)
{
  return a->getRenderSortValue() > b->getRenderSortValue();
}

void RenderManager::sortObjects()
{
  std::sort(_renderables.begin(), _renderables.end(), sortMethod);
}
  
void RenderManager::updateFPS(float dt)
{
  // Update the framerate counter
  frame_time_accum_ += dt;
  frame_count_++;
  double cur_frame_fps = 1.0 / dt;
  min_fps_ = std::min<double>(min_fps_, cur_frame_fps);
  max_fps_ = std::max<double>(max_fps_, cur_frame_fps);
  if (frame_time_accum_ >= fps_stat_interval_) {
    fps_ = frame_count_ / frame_time_accum_;
    if(print_fps_){
      sprintf(fps_buffer_, "FPS: %.1f (AVE) %.1f (MIN) %.1f (MAX)",
              fps_, min_fps_, max_fps_);
      std::cout << fps_buffer_ << std::endl;
    }
    min_fps_ = std::numeric_limits<double>::infinity();
    max_fps_ = 0;
    frame_count_ = 0;
    frame_time_accum_ = 0;
  }
  
  
}

}//namespace renderlib