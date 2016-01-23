
#pragma once

#include <Rocket/Core/RenderInterface.h>

#define ROCKET_SHADER_WITH_TEXTURE_VERT "Shaders/ui/librocket_with_texture.vert"
#define ROCKET_SHADER_WITH_TEXTURE_FRAG "Shaders/ui/librocket_with_texture.frag"
#define ROCKET_SHADER_NO_TEXTURE_VERT "Shaders/ui/librocket_no_texture.vert"
#define ROCKET_SHADER_NO_TEXTURE_FRAG "Shaders/ui/librocket_no_texture.frag"

#define UI_SHADER_TEXTURED_VERT "Shaders/ui/textured_quad.vert"
#define UI_SHADER_TEXTURED_FRAG "Shaders/ui/textured_quad.frag"

namespace renderlib {
  class Texture;
  class Shader;
  struct ShaderAttributeData;

	namespace ui{
		typedef enum {
			RS_T_WITH_TEXTURE = 0,
			RS_T_NO_TEXTURE = 1,
			ROCKET_SHADER_NUM_SHADERS = 2
		} ROCKET_SHADER_TYPE;

       

		class RocketRenderInterface : public Rocket::Core::RenderInterface {
		public:
      void getVertexAttributes(bool hasTextureCoords,
                              std::vector<ShaderAttributeData> &attributes);
			RocketRenderInterface();
			virtual ~RocketRenderInterface();

			void SetViewport(int width, int height);

      virtual void RenderGeometry(Rocket::Core::Vertex* vertices,
        int num_vertices, int* indices, int num_indices,
        Rocket::Core::TextureHandle texture,
        const Rocket::Core::Vector2f& translation) {}

			// Called by Rocket when it wants to compile geometry it believes will be 
			// static for the forseeable future.
			virtual Rocket::Core::CompiledGeometryHandle CompileGeometry(
				Rocket::Core::Vertex* vertices, int num_vertices, int* indices,
				int num_indices, Rocket::Core::TextureHandle texture);

			// Called by Rocket when it wants to render application-compiled geometry.
			virtual void RenderCompiledGeometry(
				Rocket::Core::CompiledGeometryHandle geometry,
				const Rocket::Core::Vector2f& translation);

			// Called by Rocket when it wants to release application-compiled geometry.
			virtual void ReleaseCompiledGeometry(
				Rocket::Core::CompiledGeometryHandle geometry);

			// Called by Rocket when it wants to enable or disable scissoring to clip 
			// content.
			virtual void EnableScissorRegion(bool enable);

			// Called by Rocket when it wants to change the scissor region.
			virtual void SetScissorRegion(int x, int y, int width, int height);

			// Called by Rocket when a texture is required by the library.
			virtual bool LoadTexture(Rocket::Core::TextureHandle& texture_handle,
				Rocket::Core::Vector2i& texture_dimensions,
				const Rocket::Core::String& source);

			// Called by Rocket when a texture is required to be built from an 
			// internally-generated sequence of pixels.
			virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
				const Rocket::Core::byte* source,
				const Rocket::Core::Vector2i& source_dimensions);

			// Called by Rocket when a loaded texture is no longer required.
			virtual void ReleaseTexture(Rocket::Core::TextureHandle texture_handle);

		void resize(const int width, const int height);
			/*
		void renderCrosshairs();
		*/

		private:
			std::vector<unsigned int> _vao, _ibo, _vbo, _index_size, _tex_id;
			std::vector<ROCKET_SHADER_TYPE> _shader_types;
			std::vector<renderlib::Texture*> _textures;
			Shader* _shader_programs[ROCKET_SHADER_NUM_SHADERS];
			/*
			ShaderInfo* _vshaders[ROCKET_SHADER_NUM_SHADERS];
			ShaderInfo* _fshaders[ROCKET_SHADER_NUM_SHADERS];
            */
			vmath::Matrix4 ortho_mat;
			int m_width;
			int m_height;

			void bindTexture(uint32_t i_geometry_internal) const;
			void bindUniforms(ROCKET_SHADER_TYPE rs_type,
				const Rocket::Core::Vector2f& translation) const;
			void unbindTexture() const;
			void releaseShaders();
			void setVertexAttribPointerF(const int id, const int size, const int type,
				const bool normalized, const int stride, const void* pointer) const;
			void setVertexAttribPointerI(const int id, const int size, const int type,
				const int stride, const void* pointer) const;
			void renderTexturedQuad(Texture* tex, const vmath::Vector2& pos,
				const vmath::Vector2& size);
			//renderer::Geometry* makeQuadGeometry(const std::string& name);

			static const vmath::Vector3 pos_quad_[6];

			// Non-copyable, non-assignable.
			RocketRenderInterface(RocketRenderInterface&);
			RocketRenderInterface& operator=(const RocketRenderInterface&);
		};

	}; //namespace ui
};  // namespace renderlib
