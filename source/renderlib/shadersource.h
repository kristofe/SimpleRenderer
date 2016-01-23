#ifndef SHADERSOURCE_H
#define SHADERSOURCE_H

#include "OpenGLHelper.h"
#include <map>

namespace renderlib{

class ShaderSource
{
public:
  ShaderSource();

  void parseFile(const std::string filename, const std::string delimeter);
  std::string getShader(const std::string id);

private:
  DISALLOW_COPY_AND_ASSIGN(ShaderSource);

//Data Members

protected:
   GLint _programID;
   std::map<std::string, std::string> _shaders;



};



}//namespace renderlib

#endif // SHADERSOURCE_H
