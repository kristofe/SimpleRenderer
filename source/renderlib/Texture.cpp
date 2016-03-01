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
#include "mesh.h"
#include "TriangleMesh.h"
#include <thread>
#include <mutex>

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

#include <stdlib.h>
#include <stdio.h>
void Texture::writeDistanceFieldToDisk(int dim,
                                       glm::vec4* data,
                                       //float* data,
                                       std::string const& filename)
{
  size_t num_elems = dim*dim*dim;
  
  FILE *fp = fopen(filename.c_str(), "wb");
  
  
  if(fp == NULL)
    {
    printf("Couldn't open file %s for writing.\n", filename.c_str());
    }
  
  //Set file pointer to be unbuffered
  setbuf(fp, NULL);//This may not be necessary.  fclose calls fflush();
  
  int32_t iDim = dim;
  //write header
  fwrite(&iDim, sizeof(int32_t), 1, fp);
  //Write u,v,w and pressure
  uint32_t num_elems_written;
  //num_elems_written = fwrite(data, sizeof(float), num_elems, fp);
  num_elems_written = fwrite(data, sizeof(glm::vec4), num_elems, fp);
  if(num_elems_written != num_elems)
  {
    printf("Didn't write correct number of elements to file for g_u.  %d != %d\n",(int32_t)num_elems_written, (int32_t)num_elems);
  }
  
  fclose(fp);//Calls fflush() so it should write to disk asap after this.  Can force sooner by calling fflush() manually or using an ubuffered stream.
  
  printf("Wrote %s.\n", filename.c_str());
}
  
void Texture::readDistanceFieldFromDisk(int& dim, glm::vec4** data, std::string const& filename)
{
  FILE *fp = fopen(filename.c_str(), "rb");
  
  if(fp == NULL)
  {
    printf("Couldn't open file %s for reading.\n", filename.c_str());
  }
  
  //Set file pointer to be unbuffered
  setbuf(fp, NULL);//This may not be necessary.  fclose calls fflush();
  
  int32_t iDim;
  size_t num_elems_read;
  //read header
  num_elems_read = fread(&iDim, sizeof(int32_t), 1, fp);
  
  dim = iDim;
  
  size_t num_elems = iDim*iDim*iDim;
	//*data = new float[num_elems];
  *data = new glm::vec4[num_elems];
  
  //num_elems_read = fread(*data, sizeof(float), num_elems, fp);
  num_elems_read = fread(*data, sizeof(glm::vec4), num_elems, fp);
  if (num_elems_read != num_elems)
  {
    printf("Didn't read correct number of elements to file for %s, %d expected got %d.\n",
           filename.c_str(), (int)num_elems, (int)num_elems_read);
  }
  

}

void Texture::createDistanceFieldFromMesh(int n, const TriangleMesh& mesh, bool writeToFile,
                                          std::string const& filename)
{
	_is3D = true;
  GLuint handle;
  if(!writeToFile)
  {
  
    glGenTextures(1, &handle);
  	glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, handle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  }
  	glm::vec4 * data = new glm::vec4[n*n*n];
  	//float * data = new float[n*n*n];

    float dim = (float)n;
    int sliceNum =0;
    std::mutex lock;
  
  float cellRadius = 0.5f/dim;
  glm::vec3 offset(cellRadius,cellRadius,cellRadius);
  
    unsigned numWorkers = 4;
	unsigned concurentThreadsSupported = std::thread::hardware_concurrency();
	if (concurentThreadsSupported > numWorkers)
		numWorkers = concurentThreadsSupported;

    std::thread* workers = new std::thread[numWorkers];
  
    auto calcClosetstPoints = [&](int n0, int n1)
    {
      for (int z = n0; z < n1; ++z) {
        for (int y = 0; y < n; ++y) {
          for (int x = 0; x < n; ++x) {
            glm::vec3 p((x)/dim,(y)/dim,(z)/dim);
      	  //Storing distance
            glm::vec3 closestPoint, closestNormal;
            p = p + offset;
            float dist = mesh.getClosestPoint(p, closestPoint, closestNormal);

			//3D texture memory layout is layers of 2D textures.  
			//So z*n*n is the stride in z. z images of n*n
			//y*n is the stride in y, y rows of x
			//x is the row offset
            int idx = z*n*n + y*n + x;
            data[idx].w = dist;
      		  //Storing normal too
            data[idx].x = closestNormal.x;
            data[idx].y = closestNormal.y;
            data[idx].z = closestNormal.z;
             //ptr[idx].x = ptr[idx].y = ptr[idx].z = 0.0f;
          }
        }
        lock.lock();
        sliceNum++;
        lock.unlock();
        fprintf(stdout,"Slice %d of %d\n", sliceNum, n);
        
      }
    };
  
  for(int i = 0; i < numWorkers; i++)
  {
    float size = n/((float)numWorkers);
    int n0 = glm::round(i * size);
    int n1 = glm::round((i+1) * size);
    if( i == numWorkers -1)
      n1 = n;
    fprintf(stdout,"Starting thread with x range %d - %d\n", n0, n1);
    workers[i] = std::thread(calcClosetstPoints, n0, n1);
  }
  for(int i = 0; i < numWorkers; i++)
  {
    workers[i].join();
  }
  
  delete[] workers;

  //Write data to file?
  if(writeToFile)
  {
    writeDistanceFieldToDisk(n, data, filename);
  }
  
  
  if(!writeToFile)
  {
    _textureProxy->target = GL_TEXTURE_3D;
	//Weird artifacts happen if you don't specify precision of the float
    _textureProxy->internalFormat = GL_RGBA32F;
    //_textureProxy->internalFormat = GL_R32F;
    _textureProxy->width = n;
    _textureProxy->height = n;
    _textureProxy->depth = n;
    _textureProxy->format = GL_RGBA;
    //_textureProxy->format = GL_RED;
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
  }
    delete[] data;
    //return handle;
}
  
void Texture::loadDistanceFieldFromDisk( std::string const& filename)
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

	glm::vec4 * data = nullptr;
    //float * data = nullptr;
    int n;
  
    readDistanceFieldFromDisk(n, &data, filename);
    printf("read %s, dim %d\n", filename.c_str(), n);
  
    _textureProxy->target = GL_TEXTURE_3D;
	//Weird artifacts happen if you don't specify precision of the float
    _textureProxy->internalFormat = GL_RGBA32F;
    //_textureProxy->internalFormat = GL_R32F;
    _textureProxy->width = n;
    _textureProxy->height = n;
    _textureProxy->depth = n;
    _textureProxy->format = GL_RGBA;
    //_textureProxy->format = GL_RED;
    _textureProxy->type = TextureDataType::TDT_FLOAT;
    _textureProxy->pixels = nullptr;
    _textureProxy->glID = handle;

    glTexImage3D(_textureProxy->target, 0,
      _textureProxy->internalFormat,
      _textureProxy->width, _textureProxy->height, _textureProxy->depth, 0,
      _textureProxy->format,
      _textureProxy->type,
      data);
    delete[] data;
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
  
void Texture::debugDraw(glm::ivec4 viewport)
{

  if(_debugMesh == nullptr) return;
  
  _debugShader->bind();

  bindToChannel(0);
  
  _debugShader->setUniform("uTexture0",0);
  _debugShader->setUniform("iGlobalTime", GLFWTime::getCurrentTime());
  _debugShader->setUniform("uTimeScale", 0.5f);
  
  int cachedViewport[4];
  glGetIntegerv(GL_VIEWPORT, cachedViewport);
  glViewport(viewport.x,viewport.y, viewport.z, viewport.w);
  GetGLError();
  _debugMesh->drawBuffers();
  glViewport(cachedViewport[0], cachedViewport[1],
             cachedViewport[2], cachedViewport[3]);
  GetGLError();
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
