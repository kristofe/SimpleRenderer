#include <iostream>
#include <algorithm>
#include <memory>
#include <string>
#include <Rocket/Core.h>

#include "OpenGLHelper.h"
#include "Texture.h"
#include "RenderManager.h"
#include "WindowManager.h"
#include "GLFWTime.h"
#include "ui/simple_rocket_render_interface.h"
#include "ui/simple_ui.h"

#define GL_CLAMP_TO_EDGE 0x812F

#define SAFE_DELETE(x) if (x) { delete x; x = NULL; }

using std::runtime_error;
using std::wstring;
using std::string;

namespace renderlib {

namespace ui {
  
  void RocketRenderInterface::getVertexAttributes(
                            bool hasTextureCoords,
                            std::vector<ShaderAttributeData> &attributes)
  {
    attributes.clear();
    attributes.push_back(ShaderAttributeData(
                                             "position",
                                             GL_FLOAT_VEC2,
                                             Vertex::LocationSlot::POSITION)
                         );
    if(hasTextureCoords)
    {
      attributes.push_back(ShaderAttributeData(
                                               "uv",
                                               GL_FLOAT_VEC2,
                                               Vertex::LocationSlot::UV)
                           );
    }
    attributes.push_back(ShaderAttributeData(
                                             "color",
                                             GL_FLOAT_VEC4,
                                             Vertex::LocationSlot::COLOR)
                         );
    
  }

  RocketRenderInterface::RocketRenderInterface(){
	  vmath::Vector2 wnd = WindowManager::_mainWindowSize;
	  resize((const int)wnd.x, (const int)wnd.y);

    std::vector<ShaderAttributeData> attributes;
    // Compile the two types of shaders (with and without texture)
    _shader_programs[RS_T_WITH_TEXTURE] = new Shader();
    _shader_programs[RS_T_WITH_TEXTURE]->registerShader(ROCKET_SHADER_WITH_TEXTURE_VERT,
      ShaderType::VERTEX);
    _shader_programs[RS_T_WITH_TEXTURE]->registerShader(ROCKET_SHADER_WITH_TEXTURE_FRAG,
      ShaderType::FRAGMENT);
    getVertexAttributes(true,attributes);
    _shader_programs[RS_T_WITH_TEXTURE]->setAttributeLocations(attributes);
    _shader_programs[RS_T_WITH_TEXTURE]->compileShaders();
    _shader_programs[RS_T_WITH_TEXTURE]->linkShaders();
    
    _shader_programs[RS_T_NO_TEXTURE] = new Shader();
    _shader_programs[RS_T_NO_TEXTURE]->registerShader(ROCKET_SHADER_NO_TEXTURE_VERT,
      ShaderType::VERTEX);
    _shader_programs[RS_T_NO_TEXTURE]->registerShader(ROCKET_SHADER_NO_TEXTURE_FRAG,
      ShaderType::FRAGMENT);
    getVertexAttributes(false,attributes);
    _shader_programs[RS_T_NO_TEXTURE]->setAttributeLocations(attributes);
    _shader_programs[RS_T_NO_TEXTURE]->compileShaders();
    _shader_programs[RS_T_NO_TEXTURE]->linkShaders();

  }
  void RocketRenderInterface::resize(int width, int height) {
    m_width = width;
    m_height = height;
    ortho_mat = vmath::Matrix4::orthographic(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
  }

  RocketRenderInterface::~RocketRenderInterface() {
    // Note, vao, ibo, etc are already released by libRocket (see
    releaseShaders();
  }

  void RocketRenderInterface::releaseShaders() {
    for (int i = 0; i < ROCKET_SHADER_NUM_SHADERS; i ++) {
      SAFE_DELETE(_shader_programs[i]);
    }
  }

  void RocketRenderInterface::setVertexAttribPointerF(const int id, 
    const int size, const int type, const bool normalized, const int stride, 
    const void* pointer) const {
    if (type == GL_BYTE || type == GL_UNSIGNED_BYTE || type == GL_SHORT ||
      type == GL_UNSIGNED_SHORT || type == GL_INT || type == GL_UNSIGNED_INT) {
      throw runtime_error("setVertexAttribIPointerF() - "
       "ERROR: input type is not float.");
    }
    glVertexAttribPointer(id, size, type, normalized, stride, pointer);
    glEnableVertexAttribArray(id);
  }

  void RocketRenderInterface::setVertexAttribPointerI(const int id, 
    const int size, const int type, const int stride, const void* pointer) 
    const {
    if (type != GL_BYTE && type != GL_UNSIGNED_BYTE && type != GL_SHORT &&
      type != GL_UNSIGNED_SHORT && type != GL_INT && type != GL_UNSIGNED_INT) {
      throw runtime_error("setVertexAttribIPointerI() - "
        "ERROR: input type is not int.");
    }
    glEnableVertexAttribArray(id);
    glVertexAttribIPointer(id, size, type, stride, pointer);
  }

 
  // Called by Rocket when it wants to compile geometry it believes will be 
  // static for the forseeable future.		
  Rocket::Core::CompiledGeometryHandle RocketRenderInterface::CompileGeometry(
    Rocket::Core::Vertex* vertices, int num_vertices, int* indices, 
    int num_indices, Rocket::Core::TextureHandle texture) {

    //For now don't compile anything
    //return (Rocket::Core::CompiledGeometryHandle) NULL;

    unsigned int handle = (unsigned int)_vao.size()+1;
    unsigned int new_vao, new_ibo, new_vbo;


    std::vector<unsigned int> ind(num_indices);
    std::vector<float> verts(num_vertices*2);
    std::vector<float> colors(num_vertices*4);
    std::vector<float> texCoords(num_vertices*2);
    for(int i = 0; i < num_indices; i++) {
      ind[i] = static_cast<unsigned int>(indices[i]);
    }

    for(int i = 0; i < num_vertices; i++) {
      int vidx = 2*i;
      verts[vidx] = vertices[i].position.x;
      verts[vidx + 1] = vertices[i].position.y;

      int cidx = 4*i;
      colors[cidx + 0] = vertices[i].colour.red/255.0f;
      colors[cidx + 1] = vertices[i].colour.green/255.0f;
      colors[cidx + 2] = vertices[i].colour.blue/255.0f;
      colors[cidx + 3] = vertices[i].colour.alpha/255.0f;

      texCoords[vidx] = vertices[i].tex_coord.x;
      texCoords[vidx + 1] = vertices[i].tex_coord.y;
    }

    glGenVertexArrays(1, &new_vao);
    glBindVertexArray(new_vao);

    GetGLError();
    glGenBuffers(1, &new_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, new_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
      sizeof(unsigned int)*ind.size(),&ind[0], GL_STATIC_DRAW);
    GetGLError();

    glGenBuffers(1, &new_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, new_vbo);
    glBufferData(GL_ARRAY_BUFFER, 
      (verts.size()+colors.size()+texCoords.size())*sizeof(float), NULL, 
      GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*verts.size(), &verts[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float)*verts.size(), 
      sizeof(float)*colors.size(), &colors[0]);
    GetGLError();

   if(texture > 0) {
      // Load in the textured gui shader
      glBufferSubData(GL_ARRAY_BUFFER, 
        sizeof(float)*(verts.size()+colors.size()), 
        sizeof(float)*texCoords.size(), &texCoords[0]);
      GetGLError();
      setVertexAttribPointerF(Vertex::LocationSlot::POSITION, 2, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
      setVertexAttribPointerF(Vertex::LocationSlot::COLOR, 4, GL_FLOAT, 
        GL_FALSE, 0, BUFFER_OFFSET(sizeof(float)*verts.size()));
      setVertexAttribPointerF(Vertex::LocationSlot::UV, 2, GL_FLOAT, 
        GL_FALSE, 0, 
        BUFFER_OFFSET(sizeof(float)*(verts.size()+colors.size())));
      _shader_types.push_back(RS_T_WITH_TEXTURE);
    } else {
      _shader_types.push_back(RS_T_NO_TEXTURE);
      setVertexAttribPointerF(Vertex::LocationSlot::POSITION, 2, GL_FLOAT,
        GL_FALSE, 0, BUFFER_OFFSET(0));
      setVertexAttribPointerF(Vertex::LocationSlot::COLOR, 4, GL_FLOAT, 
        GL_FALSE, 0, BUFFER_OFFSET(sizeof(float)*verts.size()));
    }

    // Now store away the indices to the geometry objects
    _vao.push_back(new_vao);
    _ibo.push_back(new_ibo);
    _vbo.push_back(new_vbo);
    _index_size.push_back((unsigned int)ind.size());
    _tex_id.push_back(static_cast<unsigned int>(texture));

    // Unbind the VAO just in case anyone makes changes to it.
    glBindVertexArray(0);
    
//    std::cout << "Compiled libRocket geometry " << new_ibo << " "
//    << new_vbo << " " << new_vao << std::endl;

    return handle;
  }

  // Called by Rocket when it wants to render application-compiled geometry.		
  void RocketRenderInterface::RenderCompiledGeometry(
    Rocket::Core::CompiledGeometryHandle geometry, 
    const Rocket::Core::Vector2f& translation) {

    if (geometry <= 0) {
      throw runtime_error("RocketRenderInterface::"
        "RenderCompiledGeometry() - ERROR: geometry index = 0!");
    }

    uint32_t i_geometry = static_cast<uint32_t>(geometry);
    ROCKET_SHADER_TYPE cur_shader = _shader_types[i_geometry-1];
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    GetGLError();

    _shader_programs[cur_shader]->bind();

    bindTexture(i_geometry-1);
    GetGLError();

    bindUniforms(cur_shader, translation);
    GetGLError();

    glBindVertexArray(_vao[i_geometry-1]);
    GetGLError();

    glDrawElements(GL_TRIANGLES, _index_size[i_geometry-1], 
      GL_UNSIGNED_INT, BUFFER_OFFSET(0));

    GetGLError();
    glBindVertexArray(0);
    GetGLError();

    unbindTexture();
    GetGLError();
    glDisable(GL_BLEND);
  }

  // Called by Rocket when it wants to release application-compiled geometry.		
  void RocketRenderInterface::ReleaseCompiledGeometry(
    Rocket::Core::CompiledGeometryHandle geometry) {

    if (geometry <= 0) {
      throw runtime_error("RocketRenderInterface::"
        "ReleaseCompiledGeometry() - ERROR: geometry index = 0!");
    }
    uint32_t i_geometry = static_cast<uint32_t>(geometry);
//    std::cout << "Releasing " << _ibo[i_geometry-1] << " " << _vbo[i_geometry-1]
//    << " " << _vao[i_geometry-1] << std::endl;
    if (i_geometry < _ibo.size() && _ibo[i_geometry-1]) {
      glDeleteBuffers(1, &_ibo[i_geometry-1]);
      _ibo[i_geometry-1] = 0;
    }
    if (i_geometry < _vbo.size() && _vbo[i_geometry-1]) {
      glDeleteBuffers(1, &_vbo[i_geometry-1]);
      _vbo[i_geometry-1] = 0;
    }
    if (i_geometry < _vao.size() && _vao[i_geometry-1]) {
      glDeleteVertexArrays(1, &_vao[i_geometry-1]);
      _vao[i_geometry-1] = 0;
    }
  }

  void RocketRenderInterface::bindUniforms(ROCKET_SHADER_TYPE rs_type, 
    const Rocket::Core::Vector2f& translation) const {	
    vmath::Matrix4 translate_mat;
   	translate_mat = Matrix4::translation(vmath::Vector3(translation.x, translation.y, 0));

    vmath::Matrix4 mvp_mat;
    mvp_mat = ortho_mat * translate_mat;

    SetUniform("vw_mat", mvp_mat);

    
    if(rs_type == RS_T_WITH_TEXTURE) {
      GLint sampler_target = GL_TEXTURE0 - GL_TEXTURE0;
      SetUniform("f_texture_sampler", sampler_target);
    }
  }

  void RocketRenderInterface::bindTexture(uint32_t i_geometry_internal) const {
      glActiveTexture(GL_TEXTURE0);
      GetGLError();
      glBindTexture(GL_TEXTURE_2D, _tex_id[i_geometry_internal]);
      GetGLError();
  }

  void RocketRenderInterface::unbindTexture() const {
    // Not necessary
  }

  void RocketRenderInterface::SetViewport(int width, int height)
  {
	  m_width = width;
	  m_height = height;
  }


  // Called by Rocket when it wants to enable or disable scissoring to clip 
  // content.		
  void RocketRenderInterface::EnableScissorRegion(bool enable) {
    if (enable) {
      glEnable(GL_SCISSOR_TEST);
    } else {
      glDisable(GL_SCISSOR_TEST);
    }
  }

  // Called by Rocket when it wants to change the scissor region.		
  void RocketRenderInterface::SetScissorRegion(int x, int y, 
    int width, int height) {

	  //glScissor(x, m_height - (y + height), width, height);
	  vmath::Vector2 windowSize = WindowManager::_mainWindowSize;
	  vmath::Vector2 viewportSize = RenderManager::getInstance().getFramebufferSize();
	  
    int scissor_left = x;
    int scissor_bottom = (int)windowSize.y - (y+ height);

    // Unfortunately, the output viewport might not be the same size as the
    // actual UI (and renderer resolution).  An example of when this happens is
    // on Mac OS X when using a retina display.  So we need to adjust for it.
    float viewport_scale_w = viewportSize.x / windowSize.x;
    float viewport_scale_h = viewportSize.y / windowSize.y;
    scissor_left = (int)((float)scissor_left * viewport_scale_w);
    width = (int)((float)width * viewport_scale_w);
    scissor_bottom = (int)((float)scissor_bottom * viewport_scale_h);
    height = (int)((float)height * viewport_scale_h);
    
    //FIXME: I think this might be off by one or two.
    glScissor(scissor_left, scissor_bottom, width, height);
  }

  // Called by Rocket when a texture is required by the library.		
  bool RocketRenderInterface::LoadTexture(Rocket::Core::TextureHandle& texture_handle, 
    Rocket::Core::Vector2i& texture_dimensions,
    const Rocket::Core::String& source) {

//    string filename = string("resource_files/") + source.CString();
    string filename = string("assets/ui/") + source.CString();
	  renderlib::Texture* tex = new renderlib::Texture();
	  tex->loadFile(filename, TextureFilterMode::LINEAR, TextureClampMode::CLAMP_TO_EDGE);
	  tex->uploadData();

	  _textures.push_back(tex);
	  
    texture_dimensions.x = tex->getWidth();
    texture_dimensions.y = tex->getHeight();

    texture_handle = static_cast<Rocket::Core::TextureHandle>(tex->getID());

    return true;
    
  }

  // Called by Rocket when a texture is required to be built from an 
  // internally-generated sequence of pixels.
  bool RocketRenderInterface::GenerateTexture(
    Rocket::Core::TextureHandle& texture_handle, const Rocket::Core::byte* source, 
    const Rocket::Core::Vector2i& source_dimensions) {
	  
	  std::shared_ptr<TextureProxy> tp(new TextureProxy);
	  tp->width = source_dimensions.x;
	  tp->height = source_dimensions.y;
	  tp->pixels = (void*)source;
    
	  Texture* tex = new Texture();
	  tex->setTextureProxy(tp);
	  tex->uploadData();

	  _textures.push_back(tex);

	  texture_handle = static_cast<Rocket::Core::TextureHandle>(tex->getID());
    return true;
  
  }

  // Called by Rocket when a loaded texture is no longer required.		
  void RocketRenderInterface::ReleaseTexture(
    Rocket::Core::TextureHandle texture_handle) {
    // Nothing to do, our texture class will handle this
  }

}  // namespace ui
}  // namespace renderlib
