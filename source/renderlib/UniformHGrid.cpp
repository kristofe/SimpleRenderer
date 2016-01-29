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
    while( currN > 2)
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
  
  std::vector<uint32_t> UniformHGrid::getTrianglesNearPosition(glm::vec3 const& pos)
	{
    std::vector<uint32_t> results;
    //Grids reduce resolution as you go into vector
    for(UniformGrid& grid : _grids)
    {
      std::vector<uint32_t>& r = grid.getTrianglesNearPosition(pos);
      if(r.size() > 0)
      {
        //Append results and break out
        results.insert(results.end(), r.begin(), r.end());
        break;
      }
    }
    return results;
    
  }
} //namespace renderlib

