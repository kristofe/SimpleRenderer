#include <string>
#include <iostream>
#include <fstream>
#include "shadersource.h"

namespace renderlib{

ShaderSource::ShaderSource()
{
}

void ShaderSource::parseFile(const std::string filename, const std::string delimeter)
{
  std::string currentShader = "";
  std::string currentShaderName = "";
  std::ifstream  fin(filename.c_str());
  std::string    line;
  
//  std::cout << " good()=" << fin.good();
//  std::cout << " eof()=" << fin.eof();
//  std::cout << " fail()=" << fin.fail();
//  std::cout << " bad()=" << fin.bad() << std::endl;
  
  if(fin.fail())
  {
    std::cout << "Failed to open " << filename << std::endl;
  }
  
  while(std::getline(fin, line))
  {
     if(line.substr(0,delimeter.size()) == delimeter) //Line starts with delimeter
     {
       if(currentShaderName.size() > 0) {
         _shaders[currentShaderName] = currentShader;
         currentShader = "";
       }
       //std::cout << line << std::endl;
       currentShaderName = line.substr(delimeter.size(), line.size());
       currentShaderName = trim(currentShaderName);

     }
     else{
       currentShader += line;
#ifdef  WIN32
	   currentShader += "\r\n";
#else
   	   currentShader += "\n";
#endif
     }
  }
  _shaders[currentShaderName] = currentShader;
  std::cout << currentShader << "has " << currentShaderName.size() << " lines\n";

//  using namespace std;
//  for(map<string, string>::iterator it = _shaders.begin();
//      it != _shaders.end(); ++it)
//  {
//      std::cout << it->first <<"\n" << it->second << "\n";
//  }
}



std::string ShaderSource::getShader(const std::string id)
{
  if(_shaders.find(id) == _shaders.end()){
      return "";
  }
  return _shaders[id];
}




} //namespace renderlib
