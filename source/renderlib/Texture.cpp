//
//  Texture.cpp
//  VolumeRenderer
//
//  Created by Kristofer Schlachter on 6/10/14.
//
//
#include <algorithm>
#include "Texture.h"
#include "glutil.h"
#include "GLFWTime.h"
#include "ImageUtils.h"
#include <thread>

namespace renderlib {

Texture::Texture()
{
  _textureProxy = std::make_shared<TextureProxy>();
  _debugMesh = nullptr;
  _debugShader = nullptr;
  _is3D = false;
}
  
Texture::~Texture()
{
  if(_debugMesh != nullptr)
  {
    delete _debugMesh;
    _debugMesh = nullptr;
  }
  
  if(_debugShader != nullptr)
  {
    delete _debugShader;
    _debugShader = nullptr;
  }
  
  _textureProxy.reset();
  
}
  
void Texture::setTextureProxy(std::shared_ptr<TextureProxy> tp)
{
  _textureProxy.reset();
	if (tp != nullptr)
	{
	  _textureProxy = tp;
	  if (tp->target == GL_TEXTURE_3D) 
	  {
		  _is3D = true;
	  }
	  else
	  {
		  _is3D = false;
	  }
	}
}
  
void Texture::loadBlank()
{
  std::shared_ptr<int> pixel(new int[1]);
  pixel.get()[0] = 0xFF000000;
  _textureProxy->pixels = pixel;
  _textureProxy->width = 1;
  _textureProxy->height = 1;
  _textureProxy->numColorChannels = 4;
  _textureProxy->hasAlpha = true;
  //tex.bitsPerPixel = tex.numColorChannels * bit_depth;
  _textureProxy->format = GL_RGBA;
  _textureProxy->internalFormat = GL_RGBA8;
  _textureProxy->type = GL_UNSIGNED_BYTE;
  _textureProxy->uploadData();
  //_textureProxy->pixels = nullptr;
}



void Texture::createDistanceFieldFromMesh(int n, Mesh& mesh)
{
	_is3D = true;
    GLuint handle;
    glGenTextures(1, &handle);
  	glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, handle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  	glm::vec4 * data = new glm::vec4[n*n*n];
    glm::vec4 *ptr = data;

    float center = n / 2.0f + 0.5f;
    float dim = (float)n;
  
    const int numWorkers = 8;
    std::thread workers[numWorkers];
  
    auto calcClosetstPoints = [&](int n0, int n1)
    {
      for (int x = n0; x < n1; ++x) {
        for (int y = 0; y < n; ++y) {
          for (int z = 0; z < n; ++z) {
            glm::vec3 p(x/dim,y/dim,z/dim);
      	  //Storing distance
            glm::vec3 closestPoint, closestNormal;
            float dist = mesh.getClosestPoint(p, closestPoint, closestNormal);
            
            int idx = x*n*n + y*n + z;
            ptr[idx].w = dist;
            //(*ptr).w = glm::length(p- glm::vec3(0.5f)) - 0.3f;//radius 0.3
      		  //Storing normal too
  		  /*
            (*ptr).x = closestNormal.x;
            (*ptr).y = closestNormal.y;
            (*ptr).z = closestNormal.z;
  		  (*ptr).x = dist; (*ptr).y = dist; (*ptr).z = dist;
  		  */
            ptr[idx].x = ptr[idx].y = ptr[idx].z = 0.0f;
          }
        }
        fprintf(stdout,"Slice %d of %d\n", x, n);
      }
    };
  
  for(int i = 0; i < numWorkers; i++)
  {
    int size = n/numWorkers;
    int n0 = i * size;
    int n1 = n0 + size;
    if( i == numWorkers -1)
      n1 = n;
    
   fprintf(stdout,"Starting thread with x range %d - %d\n", n0, n1);
    workers[i] = std::thread(calcClosetstPoints, n0, n1);
  }
  for(int i = 0; i < numWorkers; i++)
  {
    workers[i].join();
  }
    _textureProxy->target = GL_TEXTURE_3D;
    _textureProxy->internalFormat = GL_RGBA;
    _textureProxy->width = n;
    _textureProxy->height = n;
    _textureProxy->depth = n;
    _textureProxy->format = GL_RGBA;
    _textureProxy->type = TextureDataType::TDT_FLOAT;
    _textureProxy->pixels = nullptr;
    _textureProxy->glID = handle;

  //Should go into the glUtil class... for now keep it here
    glTexImage3D(_textureProxy->target, 0,
      _textureProxy->internalFormat,
      _textureProxy->width, _textureProxy->height, _textureProxy->depth, 0,
      _textureProxy->format,
      _textureProxy->type,
      data);
    delete[] data;
    //return handle;
}
/*
void Texture::loadFile(std::string path, TextureFilterMode tfm, TextureClampMode tcm)
{
		std::string ext = path.substr(path.size() - 3);
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		if (ext.compare("png") == 0)
		{
			loadPNG(path, tfm, tcm);
			return;
		}
		if (ext.compare("tga") == 0)
		{
			loadTGA(path, tfm, tcm);
			return;
		}
    
		std::cerr << "Texture::loadFile() - file extension not supported."
			<< " Only png and tga are supported. " << ext<< std::endl;

}
*/

void Texture::loadFile(std::string path, std::string directory, TextureFilterMode tfm, TextureClampMode tcm)
{
  std::string filename = std::string(path);
  filename = directory + "/" + filename;
  loadFile(filename, tfm, tcm);
}

void Texture::loadFile(std::string path, TextureFilterMode tfm, TextureClampMode tcm)
{
  GetGLError();
  _path = path;
  int width, height, channels;
  std::shared_ptr<unsigned char> image(SOIL_load_image(path.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO));
  if (image.get() == nullptr)
    printf("FAILED TO LOAD IMAGE %s\n", path.c_str());
  _textureProxy->pixels = image;
  _textureProxy->anisoValue = 1.0f;
  _textureProxy->clampMode = tcm;
  _textureProxy->filterMode = tfm;


  _textureProxy->width = width;
  _textureProxy->height = height;
  _textureProxy->numColorChannels = channels;
  _textureProxy->hasAlpha = _textureProxy->numColorChannels == 4 ? true : false;
  if (_textureProxy->numColorChannels == 4)
  {
    _textureProxy->format = GL_RGBA;
    _textureProxy->internalFormat = GL_RGBA;
  }
  else if (_textureProxy->numColorChannels == 3)
  {
    _textureProxy->format = GL_RGB;
    _textureProxy->internalFormat = GL_RGB8;
  }
  else if (_textureProxy->numColorChannels == 2)
  {
    _textureProxy->format = GL_RG;
    _textureProxy->internalFormat = GL_RG8;
  }
  else if (_textureProxy->numColorChannels == 1)
  {
    _textureProxy->format = GL_RED;
    _textureProxy->internalFormat = GL_R8;
  }

  _textureProxy->uploadData();
  //SOIL_free_image_data(image);
}
void Texture::loadPNG(std::string path, TextureFilterMode tfm, TextureClampMode tcm)
{
  _path = path;
  read_png_file(path.c_str(), *_textureProxy);
  _textureProxy->anisoValue = 1.0f;
  _textureProxy->clampMode = tcm;
  _textureProxy->filterMode = tfm;
  _textureProxy->uploadData();
}
  
void Texture::loadCubemapPNG(std::string basepath)
{
  _textureProxy->loadCubemap(basepath, ".png");
}

void Texture::loadTGA(std::string path, TextureFilterMode tfm, TextureClampMode tcm)
{
  _path = path;
	read_tga_file(path.c_str(), *_textureProxy);
	_textureProxy->anisoValue = 1.0f;
	_textureProxy->clampMode = tcm;
	_textureProxy->filterMode = tfm;
	_textureProxy->uploadData();;
}

void Texture::createPyroclasticVolume(int n, float r)
{
  _textureProxy->createPyroclasticVolume(n,r);
  _is3D = true;
}
  
void Texture::createPyroclasticDistanceField(int n, float r, float strength)
{
  _textureProxy->createPyroclasticDistanceField(n,r, strength);
  _is3D = true;
}

void Texture::loadRaw3DData(const std::string& filename, int dx, int dy, int dz, TextureDataType textureDataType)
{
  _path = filename;
  _textureProxy->loadRaw3DData(filename, dx, dy, dz, textureDataType);
  _is3D = true;
}

void Texture::bindToChannel(int textureChannel)
{
  _textureProxy->bindToChannel(textureChannel);
}

void Texture::debugDraw()
{

  if(_debugMesh == nullptr) return;
  
  _debugShader->bind();

  bindToChannel(0);
  
  _debugShader->setUniform("uTexture0",0);
  _debugShader->setUniform("iGlobalTime", GLFWTime::getCurrentTime());
  _debugShader->setUniform("uTimeScale", 0.5f);
  _debugMesh->drawBuffers();

  _debugShader->unbind();
}

void Texture::setupDebugData(Vector2 min, Vector2 max)
{
	if (_debugShader != nullptr)
		return;

	std::stringstream fsSource;
	if (!_is3D)
	{
	fsSource
		<< "#version 410\n"
		<< "  in vec2 vUV;\n"
		<< "  out vec4 color;\n"
		<< "\n"
		<< "  uniform sampler2D uTexture0;\n"
		<< "  uniform float iGlobalTime;\n"
		<< "  uniform float uTimeScale;\n"
		<< "\n"
		<< "  void main(void) {\n"
		<< "    vec3 c = texture(uTexture0, vUV).rgb;\n"
		<< "    color = vec4(c, 1.0);\n"
		<< "  }\n";
	}
	else
	{
		glEnable(GL_TEXTURE_3D);
		fsSource
			<< "#version 410\n"
			<< "  in vec2 vUV;\n"
			<< "  out vec4 color;\n"
			<< "\n"
			<< "  uniform sampler3D uTexture0;\n"
			<< "  uniform float iGlobalTime;\n"
			<< "  uniform float uTimeScale;\n"
			<< "\n"
			<< "  void main(void) {\n"
			<< "    float v = mod(iGlobalTime*uTimeScale,  1.0);\n"
			<< "    vec3 tc = vec3(vUV.xy,v);\n"
			<< "    vec3 c = texture(uTexture0, tc).aaa;\n"//WARNING:Assuming the interesting values are in the alpha channel
			<< "    color = vec4(c, 1.0);\n"
			<< "  }\n";
	}
  std::stringstream vsSource;
  vsSource
	  << "#version 410\n"
	  << "  in vec3 position;\n"
	  << "  in vec3 normal;\n"
	  << "  in vec2 uv;\n"
	  << "  in vec4 tangent;\n"
	  << "  in vec4 color;\n"
	  << "\n"
	  << "  out vec2 vUV;\n"
	  << "\n"
	  << "  void main(void) {\n"
//	  << "    vUV = position.xy;\n"//This works as a test case.  UV's were not mapping correctly. So this was a hack
	  << "    vUV = uv.xy;\n"
	  << "    gl_Position = vec4(position,1.0);\n"
	  << "  }\n";

  _debugMesh = new Mesh();
  _debugMesh->createScreenQuad(min, max);

  _debugShader = new Shader();
  _debugShader->registerShaderSource(vsSource.str(), ShaderType::VERTEX, "textureDebugVertShader");
  _debugShader->registerShaderSource(fsSource.str(), ShaderType::FRAGMENT, "textureDebugFragShader");
  _debugShader->compileShaders();
  _debugShader->linkShaders();

  _debugMesh->bindAttributesToVAO(*_debugShader);
}

 

} // namespace renderlib
