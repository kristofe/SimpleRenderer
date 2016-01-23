#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

namespace renderlib{

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

#define BUFFER_OFFSET(i) ((void*)(i))
#ifndef PI
#define PI 3.14159265359f
#endif


struct InputPointer
{
  float x, y;
  int button;
  bool up;
  bool down;
  bool dragging;
  float force;
};

struct InputKey
{
  char key;
  bool up;
  bool down;
  bool held;
  
};
  
// trim from start
static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
}

} //namespace renderlib

#endif // UTILS_H
