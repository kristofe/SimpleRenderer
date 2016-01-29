#include <cstring>
#include "OpenGLHelper.h"
#include "TriangleMesh.h"
#include "UniformGrid.h"
#include "UniformHGrid.h"

namespace renderlib
{
  ///////////////////////////////////////////////////////////////////////////////
  // UniformGrid Class Methods
  ///////////////////////////////////////////////////////////////////////////////
  UniformHGrid::UniformHGrid()
  {
  }


  UniformHGrid::UniformHGrid(uint32_t n, glm::vec3 origin){
    bool powerOfTwo = !(n == 0) && !(n & (n - 1));
    assert(powerOfTwo);
    
    uint32_t currN = n;
    //Grids reduce resolution as you go into vector
    while( currN > 0)
    {
      UniformGrid grid(currN, origin);
      _grids.push_back(grid);
      currN *= 0.5f;
    }
    
  }
  
  void UniformHGrid::storeTriangleMesh(TriangleMesh& triMesh)
  {
    for(UniformGrid& grid : _grids)
    {
      grid.storeTriangleMesh(triMesh);
    }
  }
  
  //BIG ASSUMPTION: That if there is anything in the grid cells searched that
  //They will be the closest.
  //Is there a situation where a bounding box intersects the region but another
  //one doesn't but the one that doesn't is closer?
  //The answer is yes.  But how do I overcome it?
  std::vector<uint32_t>& UniformHGrid::getTrianglesNearPosition(glm::vec3 const& pos)
	{
    //Grids reduce resolution as you go into vector
    for(UniformGrid& grid : _grids)
    {
      std::vector<uint32_t>& results = grid.getTrianglesNearPosition(pos);
      if(results.size() > 0)
      {
        return results;
      }
    }
    std::vector<uint32_t> r;
    return r;
    
  }
} //namespace renderlib
