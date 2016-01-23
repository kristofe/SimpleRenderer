#include "MarchingCubesRenderer.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "RenderManager.h"
#include "GLFWTime.h"
#include "ObjectIDGenerator.h"

#include "OpenGLHelper.h"

namespace renderlib{

  int edgeTable[256]={
    0x0  , 0x109, 0x203, 0x30a, 0x406, 0x50f, 0x605, 0x70c,
    0x80c, 0x905, 0xa0f, 0xb06, 0xc0a, 0xd03, 0xe09, 0xf00,
    0x190, 0x99 , 0x393, 0x29a, 0x596, 0x49f, 0x795, 0x69c,
    0x99c, 0x895, 0xb9f, 0xa96, 0xd9a, 0xc93, 0xf99, 0xe90,
    0x230, 0x339, 0x33 , 0x13a, 0x636, 0x73f, 0x435, 0x53c,
    0xa3c, 0xb35, 0x83f, 0x936, 0xe3a, 0xf33, 0xc39, 0xd30,
    0x3a0, 0x2a9, 0x1a3, 0xaa , 0x7a6, 0x6af, 0x5a5, 0x4ac,
    0xbac, 0xaa5, 0x9af, 0x8a6, 0xfaa, 0xea3, 0xda9, 0xca0,
    0x460, 0x569, 0x663, 0x76a, 0x66 , 0x16f, 0x265, 0x36c,
    0xc6c, 0xd65, 0xe6f, 0xf66, 0x86a, 0x963, 0xa69, 0xb60,
    0x5f0, 0x4f9, 0x7f3, 0x6fa, 0x1f6, 0xff , 0x3f5, 0x2fc,
    0xdfc, 0xcf5, 0xfff, 0xef6, 0x9fa, 0x8f3, 0xbf9, 0xaf0,
    0x650, 0x759, 0x453, 0x55a, 0x256, 0x35f, 0x55 , 0x15c,
    0xe5c, 0xf55, 0xc5f, 0xd56, 0xa5a, 0xb53, 0x859, 0x950,
    0x7c0, 0x6c9, 0x5c3, 0x4ca, 0x3c6, 0x2cf, 0x1c5, 0xcc ,
    0xfcc, 0xec5, 0xdcf, 0xcc6, 0xbca, 0xac3, 0x9c9, 0x8c0,
    0x8c0, 0x9c9, 0xac3, 0xbca, 0xcc6, 0xdcf, 0xec5, 0xfcc,
    0xcc , 0x1c5, 0x2cf, 0x3c6, 0x4ca, 0x5c3, 0x6c9, 0x7c0,
    0x950, 0x859, 0xb53, 0xa5a, 0xd56, 0xc5f, 0xf55, 0xe5c,
    0x15c, 0x55 , 0x35f, 0x256, 0x55a, 0x453, 0x759, 0x650,
    0xaf0, 0xbf9, 0x8f3, 0x9fa, 0xef6, 0xfff, 0xcf5, 0xdfc,
    0x2fc, 0x3f5, 0xff , 0x1f6, 0x6fa, 0x7f3, 0x4f9, 0x5f0,
    0xb60, 0xa69, 0x963, 0x86a, 0xf66, 0xe6f, 0xd65, 0xc6c,
    0x36c, 0x265, 0x16f, 0x66 , 0x76a, 0x663, 0x569, 0x460,
    0xca0, 0xda9, 0xea3, 0xfaa, 0x8a6, 0x9af, 0xaa5, 0xbac,
    0x4ac, 0x5a5, 0x6af, 0x7a6, 0xaa , 0x1a3, 0x2a9, 0x3a0,
    0xd30, 0xc39, 0xf33, 0xe3a, 0x936, 0x83f, 0xb35, 0xa3c,
    0x53c, 0x435, 0x73f, 0x636, 0x13a, 0x33 , 0x339, 0x230,
    0xe90, 0xf99, 0xc93, 0xd9a, 0xa96, 0xb9f, 0x895, 0x99c,
    0x69c, 0x795, 0x49f, 0x596, 0x29a, 0x393, 0x99 , 0x190,
    0xf00, 0xe09, 0xd03, 0xc0a, 0xb06, 0xa0f, 0x905, 0x80c,
    0x70c, 0x605, 0x50f, 0x406, 0x30a, 0x203, 0x109, 0x0   };

  int triTable[256][16] =
  {{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
    {3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
    {1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
      {3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
      {3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
      {9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
      {9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
      {2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
      {8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
      {9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
      {4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
      {3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
      {1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
      {4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
      {4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
      {5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
      {2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
      {9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
      {0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
      {2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
      {10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
      {4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
      {5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
      {5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
      {9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
      {0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
      {1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
      {10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
      {8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
      {2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
      {7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
      {2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
      {11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
      {5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
      {11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
      {11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
      {1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
      {9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
      {2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
      {0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
      {6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
      {3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
      {6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
      {5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
      {1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
      {10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
      {6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
      {8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
      {7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
      {3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
      {5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
      {0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
      {9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
      {8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
      {5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
      {0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
      {6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
      {10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
      {10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
      {8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
      {1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
      {0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
      {10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
      {3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
      {6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
      {9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
      {8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
      {3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
      {6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
      {0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
      {10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
      {10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
      {2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
      {7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
      {7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
      {2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
      {1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
      {11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
      {8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
      {0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
      {7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
      {10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
      {2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
      {6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
      {7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
      {2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
      {1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
      {10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
      {10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
      {0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
      {7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
      {6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
      {8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
      {9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
      {6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
      {4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
      {10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
      {8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
      {0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
      {1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
      {8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
      {10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
      {4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
      {10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
      {5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
      {11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
      {9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
      {6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
      {7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
      {3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
      {7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
      {9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
      {3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
      {6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
      {9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
      {1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
      {4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
      {7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
      {6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
      {3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
      {0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
      {6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
      {0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
      {11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
      {6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
      {5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
      {9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
      {1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
      {1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
      {10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
      {0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
      {5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
      {10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
      {11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
      {9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
      {7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
      {2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
      {8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
      {9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
      {9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
      {1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
      {9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
      {9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
      {5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
      {0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
      {10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
      {2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
      {0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
      {0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
      {9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
      {5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
      {3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
      {5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
      {8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
      {0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
      {9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
      {0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
      {1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
      {3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
      {4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
      {9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
      {11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
      {11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
      {2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
      {9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
      {3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
      {1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
      {4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
      {4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
      {0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
      {3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
      {3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
      {0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
      {9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
      {1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
      {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}};

MarchingCubesRenderer::MarchingCubesRenderer()
{
  _id = ObjectIDGenerator::getInstance().getNextID();
  MeshBufferInfo* mbi = new MeshBufferInfo(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, GL_TRIANGLES);
  _mesh = new Mesh(mbi);
  _meshGS = new Mesh();
  viewOrient = vec3(1, 0, 1.5);

  cubeSize = vec3(48, 48, 48);
  cubeStep = vec3(2.0f, 2.0f, 2.0f) / cubeSize;

  dataSize = ivec3(128, 128, 128);

  isolevel = 40.5f;
  animate = true;
  autoWay = true;
  curData = 0;
  mode = 1;
  enableSwizzledWalk = false;

  wireframe = false;
  gridData = nullptr;

  _renderSortValue = IRenderable::DefaultSortValue;
}

MarchingCubesRenderer::~MarchingCubesRenderer()
{

  delete _mesh;
  delete _meshGS;
  delete _shader;
  //delete _trackball;
}

/*
Linearly interpolate the position where an isosurface cuts
an edge between two vertices, each with their own scalar value
*/
vec3 MarchingCubesRenderer::VertexInterp(float isolevel, vec3 p1, vec3 p2, float valp1, float valp2) {
  float mu;
  vec3 p;

  mu = (isolevel - valp1) / (valp2 - valp1);
  p = p1 + (p2 - p1) * mu;

  return(p);
}

/*
Given a grid cell and an isolevel, calculate the triangular
facets required to represent the isosurface through the cell.
Return the number of triangular facets, the array "triangles"
will be loaded up with the vertices at most 5 triangular facets.
0 will be returned if the grid cell is either totally above
of totally below the isolevel.
*/
int  MarchingCubesRenderer::Polygonise(GridCell &grid, float isolevel, vec3 *triangles)
{
  int i, ntriang;
  int cubeindex;
  vec3 vertlist[12];
  /*
  Determine the index into the edge table which
  tells us which vertices are inside of the surface
  */
  cubeindex = 0;
  if (grid.val[0] < isolevel) cubeindex |= 1;
  if (grid.val[1] < isolevel) cubeindex |= 2;
  if (grid.val[2] < isolevel) cubeindex |= 4;
  if (grid.val[3] < isolevel) cubeindex |= 8;
  if (grid.val[4] < isolevel) cubeindex |= 16;
  if (grid.val[5] < isolevel) cubeindex |= 32;
  if (grid.val[6] < isolevel) cubeindex |= 64;
  if (grid.val[7] < isolevel) cubeindex |= 128;

  /* Cube is entirely in/out of the surface */
  if (edgeTable[cubeindex] == 0)
    return(0);

  /* Find the vertices where the surface intersects the cube */
  if (edgeTable[cubeindex] & 1)
    vertlist[0] =
    VertexInterp(isolevel, grid.pos[0], grid.pos[1], grid.val[0], grid.val[1]);
  if (edgeTable[cubeindex] & 2)
    vertlist[1] =
    VertexInterp(isolevel, grid.pos[1], grid.pos[2], grid.val[1], grid.val[2]);
  if (edgeTable[cubeindex] & 4)
    vertlist[2] =
    VertexInterp(isolevel, grid.pos[2], grid.pos[3], grid.val[2], grid.val[3]);
  if (edgeTable[cubeindex] & 8)
    vertlist[3] =
    VertexInterp(isolevel, grid.pos[3], grid.pos[0], grid.val[3], grid.val[0]);
  if (edgeTable[cubeindex] & 16)
    vertlist[4] =
    VertexInterp(isolevel, grid.pos[4], grid.pos[5], grid.val[4], grid.val[5]);
  if (edgeTable[cubeindex] & 32)
    vertlist[5] =
    VertexInterp(isolevel, grid.pos[5], grid.pos[6], grid.val[5], grid.val[6]);
  if (edgeTable[cubeindex] & 64)
    vertlist[6] =
    VertexInterp(isolevel, grid.pos[6], grid.pos[7], grid.val[6], grid.val[7]);
  if (edgeTable[cubeindex] & 128)
    vertlist[7] =
    VertexInterp(isolevel, grid.pos[7], grid.pos[4], grid.val[7], grid.val[4]);
  if (edgeTable[cubeindex] & 256)
    vertlist[8] =
    VertexInterp(isolevel, grid.pos[0], grid.pos[4], grid.val[0], grid.val[4]);
  if (edgeTable[cubeindex] & 512)
    vertlist[9] =
    VertexInterp(isolevel, grid.pos[1], grid.pos[5], grid.val[1], grid.val[5]);
  if (edgeTable[cubeindex] & 1024)
    vertlist[10] =
    VertexInterp(isolevel, grid.pos[2], grid.pos[6], grid.val[2], grid.val[6]);
  if (edgeTable[cubeindex] & 2048)
    vertlist[11] =
    VertexInterp(isolevel, grid.pos[3], grid.pos[7], grid.val[3], grid.val[7]);

  /* Create the triangle */
  ntriang = 0;
  for (i = 0; triTable[cubeindex][i] != -1; i += 3) {
    triangles[ntriang] = vertlist[triTable[cubeindex][i]];
    triangles[ntriang + 1] = vertlist[triTable[cubeindex][i + 1]];
    triangles[ntriang + 2] = vertlist[triTable[cubeindex][i + 2]];
    ntriang += 3;
  }

  return(ntriang);
}


//Software marching cubes
//VERY far to be optimal !
void MarchingCubesRenderer::RenderMarchCube(float *data, ivec3 size, ivec3 gridsize, float isolevel){
  vec3 gridStep = vec3(2.0, 2.0, 2.0) / vec3(gridsize.x, gridsize.y, gridsize.z);

  ivec3 dataGridStep = size / gridsize;

  vec3 *triangles = new vec3[16];
  
  std::vector<Vector3>& positions = _mesh->getPositionVector();
  positions.clear();

  for (int k = 0; k < gridsize.z - 1; k++)
  for (int j = 0; j < gridsize.y - 1; j++)
  for (int i = 0; i < gridsize.x - 1; i++){
    GridCell cell;
    vec3 vcurf(i, j, k);
    ivec3 vcuri(i, j, k);

    cell.pos[0] = vcurf*gridStep - 1.0f;
    ivec3 valPos0 = vcuri*dataGridStep;
    cell.val[0] = data[valPos0.x + valPos0.y*size.x + valPos0.z*size.x*size.y];

    ivec3 valPos;

    cell.pos[1] = cell.pos[0] + vec3(gridStep.x, 0, 0);
    if (i == gridsize.x - 1)
      valPos = valPos0;
    else
      valPos = valPos0 + ivec3(dataGridStep.x, 0, 0);
    cell.val[1] = data[valPos.x + valPos.y*size.x + valPos.z*size.x*size.y];

    cell.pos[2] = cell.pos[0] + vec3(gridStep.x, gridStep.y, 0);
    valPos = valPos0 + ivec3(i == gridsize.x - 1 ? 0 : dataGridStep.x, j == gridsize.y - 1 ? 0 : dataGridStep.y, 0);
    cell.val[2] = data[valPos.x + valPos.y*size.x + valPos.z*size.x*size.y];

    cell.pos[3] = cell.pos[0] + vec3(0, gridStep.y, 0);
    valPos = valPos0 + ivec3(0, j == gridsize.y - 1 ? 0 : dataGridStep.y, 0);
    cell.val[3] = data[valPos.x + valPos.y*size.x + valPos.z*size.x*size.y];


    cell.pos[4] = cell.pos[0] + vec3(0, 0, gridStep.z);
    valPos = valPos0 + ivec3(0, 0, k == gridsize.z - 1 ? 0 : dataGridStep.z);
    cell.val[4] = data[valPos.x + valPos.y*size.x + valPos.z*size.x*size.y];


    cell.pos[5] = cell.pos[0] + vec3(gridStep.x, 0, gridStep.z);
    valPos = valPos0 + ivec3(i == gridsize.x - 1 ? 0 : dataGridStep.x, 0, k == gridsize.z - 1 ? 0 : dataGridStep.z);
    cell.val[5] = data[valPos.x + valPos.y*size.x + valPos.z*size.x*size.y];

    cell.pos[6] = cell.pos[0] + vec3(gridStep.x, gridStep.y, gridStep.z);
    valPos = valPos0 + ivec3(i == gridsize.x - 1 ? 0 : dataGridStep.x, j == gridsize.y - 1 ? 0 : dataGridStep.y, k == gridsize.z - 1 ? 0 : dataGridStep.z);
    cell.val[6] = data[valPos.x + valPos.y*size.x + valPos.z*size.x*size.y];

    cell.pos[7] = cell.pos[0] + vec3(0, gridStep.y, gridStep.z);
    valPos = valPos0 + ivec3(0, j == gridsize.y - 1 ? 0 : dataGridStep.y, k == gridsize.z - 1 ? 0 : dataGridStep.z);
    cell.val[7] = data[valPos.x + valPos.y*size.x + valPos.z*size.x*size.y];


    int numvert = Polygonise(cell, isolevel, triangles);

    //Put the triangles into the mesh
    for (int n = 0; n < numvert; n++){
      positions.push_back(Vector3(triangles[n].x, triangles[n].y, triangles[n].z));
    }


  }
  if (positions.empty() == false)
  {
    GetGLError();
    _mesh->constructBuffer();
    GetGLError();
    _mesh->drawBuffers();
    GetGLError();
  }
}

void MarchingCubesRenderer::resize()
{
  _screenDim = RenderManager::getInstance().getFramebufferSize();
}
  
void MarchingCubesRenderer::update(float time)
{


}

void MarchingCubesRenderer::preRender()
{


}

void MarchingCubesRenderer::draw()
{
	//States setting
	//glEnable(GL_DEPTH_TEST);
	//glDisable(GL_ALPHA_TEST);

  /*
  vec3 eye = vec3(0, 0, 3);
  vec3 up(0, 1, 0); vec3 target(0, 0, 0);
  mat4 mv =glm::lookAt(eye, target, up);

	//Activate modelview
  mv = mv * glm::rotate(GLFWTime::getCurrentTime()*0.5f, vec3(0, 1, 0));
  //mv = glm::rotate(this->viewOrient.y * 10, vec3(0, 1, 0));

  Vector2 size = RenderManager::getInstance().getFramebufferSize();
  float aspect = size.x / size.y;

  float n = 0.01f;
  float f = 100.0f;
  float fieldOfView = 0.7f;// 1.74532925f;//100 degrees

  mat4 projection = glm::perspective(fieldOfView, aspect, n, f);

  mat4 mvp = projection*mv;

  */
  mat4 m = glm::rotate(GLFWTime::getCurrentTime()*0.5f, vec3(0, 1, 0));
  //FROM RENDERER
  Camera& cam = RenderManager::getInstance().getMainCamera();
  mat4 view = cam.getModelview();
  mat4 proj = cam.getProjection();
  mat4 mv = view *m;
  mat4 mvp = proj * mv;

  GetGLError();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, this->dataFieldTex[curData]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, this->edgeTableTex);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, this->triTableTex);
  
  GetGLError();

  isolevel = (sin(GLFWTime::getCurrentTime() * 1.5f) + 1.0f) * 0.02f + 0.45f;
  //isolevel = (sin(GLFWTime::getCurrentTime() * 0.5f) + 1.0f) * 0.2f + 0.4f;
  //printf("%3.4f\n", isolevel);

  if(mode != 0){
		if(mode==1){
			//Shader program binding

      programObject.bind();


      _texture.bindToChannel(0);
      programObject.setUniform("dataFieldTex", 0);
      programObject.setUniform("edgeTableTex", 1);
      programObject.setUniform("triTableTex", 2);
			programObject.setUniform("isolevel", isolevel); 
      programObject.setUniform("mvp", mvp);
      programObject.setUniform("ModelViewProjection", mvp);
      //Decal for each vertex in a marching cube
      programObject.setUniform("vertDecals[0]", vec3(0.0f, 0.0f, 0.0f));
      programObject.setUniform("vertDecals[1]", vec3(cubeStep.x, 0.0f, 0.0f));
      programObject.setUniform("vertDecals[2]", vec3(cubeStep.x, cubeStep.y, 0.0f));
      programObject.setUniform("vertDecals[3]", vec3(0.0f, cubeStep.y, 0.0f));
      programObject.setUniform("vertDecals[4]", vec3(0.0f, 0.0f, cubeStep.z));
      programObject.setUniform("vertDecals[5]", vec3(cubeStep.x, 0.0f, cubeStep.z));
      programObject.setUniform("vertDecals[6]", vec3(cubeStep.x, cubeStep.y, cubeStep.z));
      programObject.setUniform("vertDecals[7]", vec3(0.0f, cubeStep.y, cubeStep.z));
      programObject.setUniform("vertDecals[0]", vec3(0.0f, 0.0f, 0.0f));
		}else{
			//Shader program binding
      programObjectGS.bind();
      programObjectGS.setUniform("dataFieldTex", 0);
      programObjectGS.setUniform("edgeTableTex", 1);
      programObjectGS.setUniform("triTableTex", 2);
      programObjectGS.setUniform("isolevel", isolevel);
      programObjectGS.setUniform("mvp", mvp);
      programObjectGS.setUniform("ModelViewProjection", mvp);
		}	

		//glEnable(GL_LIGHTING);

		//Switch to wireframe or solid rendering mode
		//if(wireframe)
		//	glPolygonMode(GL_FRONT_AND_BACK , GL_LINE );
		//else
		//	glPolygonMode(GL_FRONT_AND_BACK , GL_FILL );


    _meshGS->drawBuffers();
   
  	}else{
      //THIS IS SOFTWARE MARCHING CUBES
  		programObjectFS.bind();
      glActiveTexture(GL_TEXTURE0 );
      glBindTexture(GL_TEXTURE_3D, this->dataFieldTex[curData]);
      programObjectFS.setUniform("dataFieldTex", 0);
      programObjectFS.setUniform("mvp", mvp);
      programObjectFS.setUniform("ModelViewProjection", mvp);
      programObjectFS.setUniform("ModelView", mv);
      GetGLError();
  		RenderMarchCube(dataField[curData], ivec3(128,128,128), ivec3(cubeSize.x, cubeSize.y, cubeSize.z), isolevel);
      GetGLError();
      programObjectFS.unbind();
  	}
  }


	
}

void MarchingCubesRenderer::postRender()
{


}
void MarchingCubesRenderer::handleKey(KeyInfo& key)
{
  
  if(key.key >= '0' && key.key <= '9' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    //char idx = key.key - '0';
  }
  if(key.key == 'R' && key.action == KeyInfo::KeyAction::RELEASE)
  {
  }
  if(key.key == 'G' && key.action == KeyInfo::KeyAction::RELEASE)
  {
  }
  if(key.key == 'B' && key.action == KeyInfo::KeyAction::RELEASE)
  {
  }
  if(key.key == 'D' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    // _showDebug = !_showDebug;
  }
  if(key.key == ' ' && key.action == KeyInfo::KeyAction::PRESS)
  {
  }
  
  
  
  if(key.action == KeyInfo::KeyAction::PRESS)
  {
    switch (key.key)
    {
      case ',':
      case '<':
        break;
      case '.':
      case '>':
        break;
      case '/':
      case '?':
        break;
    }
  }
}
  
void MarchingCubesRenderer::handlePointer(std::vector<PointerInfo>& pointers)
{

}

float distance(const vec3& a, const vec3& b)
{
  vec3 diff = b - a;
  float lensqr = glm::dot(diff,diff);
  return sqrtf(lensqr);
  
}
  
void swizzledWalk(int &n, float *gridData, ivec3 pos, ivec3 size, const vec3 &cubeSize){
	if(size.x>1){
		ivec3 newSize=size/2;

		swizzledWalk(n, gridData, pos, newSize, cubeSize);
		swizzledWalk(n, gridData, pos+ivec3(newSize.x, 0, 0), newSize, cubeSize);
		swizzledWalk(n, gridData, pos+ivec3(0, newSize.y,0), newSize, cubeSize);
		swizzledWalk(n, gridData, pos+ivec3(newSize.x, newSize.y, 0), newSize, cubeSize);

		swizzledWalk(n, gridData, pos+ivec3(0, 0, newSize.z), newSize, cubeSize);
		swizzledWalk(n, gridData, pos+ivec3(newSize.x, 0, newSize.z), newSize, cubeSize);
		swizzledWalk(n, gridData, pos+ivec3(0, newSize.y, newSize.z), newSize, cubeSize);
		swizzledWalk(n, gridData, pos+ivec3(newSize.x, newSize.y, newSize.z), newSize, cubeSize);
	}else{
		gridData[n]=(pos.x/cubeSize.x)*2.0f-1.0f;
		gridData[n+1]=(pos.y/cubeSize.y)*2.0f-1.0f;
		gridData[n+2]=(pos.z/cubeSize.z)*2.0f-1.0f;
		n+=3;
	}
}


void MarchingCubesRenderer::init(){
  //_texture.createPyroclasticVolume(64, 0.025f);
  _texture.loadRaw3DData("assets/Data/512x512x512x_uint16.raw", 512, 512, 512, TextureDataType::TDT_USHORT);

  //Form multi-face view
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  GetGLError();
  /////GLSL/////
  programObject.registerShader("shaders/MarchingCubes/TestG80_GS2.glsl", ShaderType::GEOMETRY);
  programObject.registerShader("shaders/MarchingCubes/TestG80_VS.glsl", ShaderType::VERTEX);
  programObject.registerShader("shaders/MarchingCubes/TestG80_FS.glsl", ShaderType::FRAGMENT);
  programObject.compileShaders();
  programObject.linkShaders();
  programObject.bind();

  GetGLError();
  glGenTextures(1, &(this->edgeTableTex));
  glActiveTexture(GL_TEXTURE1);
  GetGLError();

  GetGLError();
  glBindTexture(GL_TEXTURE_2D, this->edgeTableTex);
  GetGLError();
  //Integer textures must use nearest filtering mode
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  GetGLError();
  //We create an integer texture with new GL_EXT_texture_integer formats
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16I, 256, 1, 0, GL_RED_INTEGER, GL_INT, &edgeTable);

  GetGLError();

  //Triangle Table texture//
  //This texture store the vertex index list for 
  //generating the triangles of each configurations.
  glGenTextures(1, &(this->triTableTex));
  glActiveTexture(GL_TEXTURE2);

  glBindTexture(GL_TEXTURE_2D, this->triTableTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_R16I, 16, 256, 0, GL_RED_INTEGER, GL_INT, &triTable);


  GetGLError();
  //Datafield//
  //Store the volume data to polygonise
  glGenTextures(3, (this->dataFieldTex));

  glActiveTexture(GL_TEXTURE0);
  //glEnable(GL_TEXTURE_3D);
  glBindTexture(GL_TEXTURE_3D, this->dataFieldTex[0]);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  //Generate a distance field to the center of the cube
  dataField[0] = new float[dataSize.x*dataSize.y*dataSize.z];
  for (int k = 0; k < dataSize.z; k++)
  for (int j = 0; j < dataSize.y; j++)
  for (int i = 0; i < dataSize.x; i++){
    float d = distance(vec3(i, j, k), vec3(dataSize.x / 2, dataSize.y / 2, dataSize.z / 2)) / (float)(dataSize.length()*0.4);
    dataField[0][i + j*dataSize.x + k*dataSize.x*dataSize.y] = d;//+(rand()%100-50)/200.0f*d;
  }

  glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, dataSize.x, dataSize.y, dataSize.z, 0, GL_RED, GL_FLOAT, dataField[0]);

  GetGLError();

  //Datafield Perturbated//
  dataField[1] = new float[dataSize.x*dataSize.y*dataSize.z];
  //perturb
  for (int k = 0; k < dataSize.z; k++)
  for (int j = 0; j < dataSize.y; j++)
  for (int i = 0; i < dataSize.x; i++){
    float d = dataField[0][i + j*dataSize.x + k*dataSize.x*dataSize.y];
    dataField[1][i + j*dataSize.x + k*dataSize.x*dataSize.y] = d + (rand() % 100 - 50) / 100.0f*d;
  }

  //Smooth
  for (int l = 0; l < 4; l++)
  for (int k = 1; k < dataSize.z - 1; k++)
  for (int j = 1; j < dataSize.y - 1; j++)
  for (int i = 1; i < dataSize.x - 1; i++){
    dataField[1][i + j*dataSize.x + k*dataSize.x*dataSize.y] = (dataField[1][i + 1 + j*dataSize.x + k*dataSize.x*dataSize.y] + dataField[1][i - 1 + j*dataSize.x + k*dataSize.x*dataSize.y] + dataField[1][i + (j + 1)*dataSize.x + k*dataSize.x*dataSize.y] + dataField[1][i + (j - 1)*dataSize.x + k*dataSize.x*dataSize.y] + dataField[1][i + j*dataSize.x + (k + 1)*dataSize.x*dataSize.y] + dataField[1][i + j*dataSize.x + (k - 1)*dataSize.x*dataSize.y]) / 6.0f;
  }

  //Store the volume data to polygonise
  glBindTexture(GL_TEXTURE_3D, this->dataFieldTex[1]);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, dataSize.x, dataSize.y, dataSize.z, 0, GL_RED, GL_FLOAT, dataField[1]);


  GetGLError();
  //Cayley-polynomial//
  dataField[2] = new float[dataSize.x*dataSize.y*dataSize.z];

  for (int k = 0; k < dataSize.z; k++)
  for (int j = 0; j < dataSize.y; j++)
  for (int i = 0; i < dataSize.x; i++){
    float x = 2.0f / dataSize.x*i - 1.0f;
    float y = 2.0f / dataSize.y*j - 1.0f;
    float z = 2.0f / dataSize.z*k - 1.0f;
    dataField[2][i + j*dataSize.x + k*dataSize.x*dataSize.y] = 16.0f*x*y*z + 4.0f*x*x + 4.0f*y*y + 4.0f*z*z - 1.0f;
  }

  glBindTexture(GL_TEXTURE_3D, this->dataFieldTex[2]);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, dataSize.x, dataSize.y, dataSize.z, 0, GL_RED, GL_FLOAT, dataField[2]);

  GetGLError();
  //Set current texture//
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, this->dataFieldTex[curData]);

  GetGLError();

  programObject.setUniform("dataFieldTex", 0);
  programObject.setUniform("edgeTableTex", 1);
  programObject.setUniform("triTableTex", 2);

  GetGLError();
  ////Uniforms parameters////
  //Initial isolevel
  programObject.setUniform("isoLevel", isolevel);
  //Step in data 3D texture for gradient computation (lighting)
  programObject.setUniform("dataStep", vec3(1.0f / dataSize.x, 1.0f / dataSize.y, 1.0f / dataSize.z));
  GetGLError();

  //Decal for each vertex in a marching cube
  programObject.setUniform("vertDecals[0]", vec3(0.0f, 0.0f, 0.0f));
  programObject.setUniform("vertDecals[1]", vec3(cubeStep.x, 0.0f, 0.0f));
  programObject.setUniform("vertDecals[2]", vec3(cubeStep.x, cubeStep.y, 0.0f));
  programObject.setUniform("vertDecals[3]", vec3(0.0f, cubeStep.y, 0.0f));
  programObject.setUniform("vertDecals[4]", vec3(0.0f, 0.0f, cubeStep.z));
  programObject.setUniform("vertDecals[5]", vec3(cubeStep.x, 0.0f, cubeStep.z));
  programObject.setUniform("vertDecals[6]", vec3(cubeStep.x, cubeStep.y, cubeStep.z));
  programObject.setUniform("vertDecals[7]", vec3(0.0f, cubeStep.y, cubeStep.z));
  programObject.setUniform("vertDecals[0]", vec3(0.0f, 0.0f, 0.0f));
  GetGLError();
 



  ////////////////////////////////////////////////////////////////////////////////////////////
  ///////Two others versions of the program////////
  //Geometry Shader only version (plus VS needed)//
  //Program object creation

  ////shaders loading////
  //programObjectGS.registerShader("shaders/MarchingCubes/TestG80_GS2.glsl", ShaderType::GEOMETRY);
  //programObjectGS.registerShader("shaders/MarchingCubes/TestG80_VS.glsl", ShaderType::VERTEX);
  //programObjectGS.registerShader("shaders/MarchingCubes/TestG80_FS_Simple.glsl", ShaderType::FRAGMENT);
  //programObjectGS.compileShaders();
  //programObjectGS.linkShaders();
  //programObjectGS.bind();
  //GetGLError();

  ////Samplers assignment///
  //programObjectGS.setUniform("dataFieldTex", 0);
  //programObjectGS.setUniform("edgeTableTex", 1);
  //programObjectGS.setUniform("triTableTex", 2);

  GetGLError();
  ////Uniforms parameters////
  //Initial isolevel
  //programObjectGS.setUniform("isolevel", isolevel);
  //Decal for each vertex in a marching cube
  //programObjectGS.setUniform("vertDecals[0]", vec3(0.0f, 0.0f, 0.0f));
  //programObjectGS.setUniform("vertDecals[1]", vec3(cubeStep.x, 0.0f, 0.0f));
  //programObjectGS.setUniform("vertDecals[2]", vec3(cubeStep.x, cubeStep.y, 0.0f));
  //programObjectGS.setUniform("vertDecals[3]", vec3(0.0f, cubeStep.y, 0.0f));
  //programObjectGS.setUniform("vertDecals[4]", vec3(0.0f, 0.0f, cubeStep.z));
  //programObjectGS.setUniform("vertDecals[5]", vec3(cubeStep.x, 0.0f, cubeStep.z));
  //programObjectGS.setUniform("vertDecals[6]", vec3(cubeStep.x, cubeStep.y, cubeStep.z));
  //programObjectGS.setUniform("vertDecals[7]", vec3(0.0f, cubeStep.y, cubeStep.z));
  /////

  GetGLError();
  //Fragment Shader only version for software marching cubes lighting//
  //Program object creation

  //Vertex Shader for software MC
  //programObjectFS.registerShader("shaders/MarchingCubes/TestG80_VS2.glsl", ShaderType::VERTEX);
  //programObjectFS.registerShader("shaders/MarchingCubes/TestG80_FS.glsl", ShaderType::FRAGMENT);
  //programObjectFS.setAttributeLocations(Mesh::getShaderAttributeLocations());//Not needed when they are in the shader
  //programObjectFS.compileShaders();
  //programObjectFS.linkShaders();
  //programObjectFS.bind();
  //programObjectFS.setUniform("dataFieldTex", 0);

  //Step in data 3D texture for gradient computation (lighting)
  //programObjectFS.setUniform("dataStep", vec3(1.0f / dataSize.x, 1.0f / dataSize.y, 1.0f / dataSize.z));

  GetGLError();


  //////Grid data construction
  _meshGS->createPointGrid(Vector3(cubeSize.x, cubeSize.y, cubeSize.z));
  _meshGS->bindAttributesToVAO();

  /*
  //Linear Walk
  gridData = new float[(int)(cubeSize.x*cubeSize.y*cubeSize.z * 3)];
  int ii = 0;
  for (float k = -1; k < 1.0f; k += cubeStep.z)
  for (float j = -1; j < 1.0f; j += cubeStep.y)
  for (float i = -1; i < 1.0f; i += cubeStep.x){
    gridData[ii] = i;
    gridData[ii + 1] = j;
    gridData[ii + 2] = k;

    ii += 3;
  }

  //VBO configuration for marching grid linear walk
  glGenBuffers(1, &gridDataBuffId);
  glBindBuffer(GL_ARRAY_BUFFER, gridDataBuffId);
  glBufferData(GL_ARRAY_BUFFER, cubeSize.x*cubeSize.y*cubeSize.z * 3 * 4, gridData, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  GetGLError();
  //Swizzled Walk 
  int n = 0;
  swizzledWalk(n, gridData, ivec3(0, 0, 0), ivec3(cubeSize.x, cubeSize.y, cubeSize.z), cubeSize);


  //VBO configuration for marching grid Swizzled walk
  glGenBuffers(1, &gridDataSwizzledBuffId);
  glBindBuffer(GL_ARRAY_BUFFER, gridDataSwizzledBuffId);
  glBufferData(GL_ARRAY_BUFFER, cubeSize.x*cubeSize.y*cubeSize.z * 3 * 4, gridData, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  */
  GetGLError();


  _mesh->createCube();
  _mesh->bindAttributesToVAO();
}



