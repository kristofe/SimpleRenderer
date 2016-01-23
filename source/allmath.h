#pragma once
#define GLM_FORCE_RADIANS
#include "vmath.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long int uint64;

typedef char int8;
typedef short int16;
typedef int int32;
typedef long long int int64;

#define RAD2DEG 57.295779513082320876798154814105f
#define DEG2RAD 0.01745329251994329576923690768489f
#ifndef PI
#define PI      3.1415926535897932384626433832795f
#endif


static inline float rad2deg(float a)
{
  return a*RAD2DEG;
}

static inline float deg2rad(float a)
{
  return a*DEG2RAD;
}

/*
 static inline float lerp(float a, float b, float t)
 {
 return a + (b-a)*t;
 }
 */

static inline float sqr(float a)
{
  return a*a;
}