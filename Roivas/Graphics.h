#pragma once

//#define GLEW_STATIC
//#include <GL/glew.h>

#include "SDL.h"
#include "SDL_opengl.h"
#include <FTGL/ftgl.h>
#include <assimp/Importer.hpp>
#include <assimp/types.h>
#include <assimp/scene.h> 
#include <assimp/postprocess.h> 
#include "FBORenderTexture.h"
#include "SOIL.h"
#include "System.h"
#include "Model.h"
#include "Light.h"
#include "Transform.h"
#include "Shader.h"


#define MAX_LIGHTS 50

namespace Roivas
{
	struct MeshData
	{		
		GLuint VertexBuffer;
		GLuint UVBuffer;
		GLuint NormalBuffer;
		GLuint ElementBuffer;
		std::vector<unsigned short> Indices;
	};

	struct Attrib
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;
		bool operator<(const Attrib that) const{
			return memcmp((void*)this, (void*)&that, sizeof(Attrib))>0;
		};
	};

	class Graphics : public System
	{
		public:
			Graphics(SDL_Window* _window, float screenwidth, float screenheight);
			~Graphics();
			void Initialize();
			void PreloadAssets();
			GLuint LoadTexture(std::string path);
			void LoadMesh(std::string path, GLuint& vb, GLuint& uvb, GLuint& nb, GLuint& eb, std::vector<unsigned short>& ind);

			void Update(float dt);
			void UpdateScreenDims(int x, int y, int w, int h);
			void UpdateCameraRotation(float x, float y);
			void MouseSelectEntity(float x, float y, bool pressed);
			void UpdateLightPos(float x, float y);

			void AddComponent(Model* m);
			void AddComponent(Light* m);
			void RemoveComponent(Model* m);
			void RemoveComponent(Light* m);

			void CameraPitch(float angle);
			void CameraYaw(float angle);
			void CameraRoll(float angle);

			void NextRT();
			void PrevRT();

			void ProcessLights();
			void BuildShadows();

			GLuint current_lighting;
			bool shadows_enabled;
			bool normal_mapping_enabled;
			bool wireframe_enabled;

		private:
			void Draw3D(float dt);
			void DrawPP(float dt);			
			void DrawEditor(float dt);
			void DrawWireframe(float dt);		
			void Draw2D(float dt);
			void SortModels(float dt);
			void UpdateCamera(float dt);
			void DrawDebugText(std::string path);

			void GeometryPass(float dt);
			void ShadowPass(float dt);
			void LightingPass(float dt);	
			void ScreenPass(float dt);

			GLint CreateShaderProgram(std::string _vertSource, std::string _fragSource);			
			void LoadFontmap(std::string path);
			GLint LoadShader(std::string shader_filename, GLenum shader_type);
			void ProcessVertexData(	std::vector<glm::vec3>& in_vertices,
				std::vector<glm::vec2>& in_uvs,
				std::vector<glm::vec3>& in_normals,	
				std::vector<unsigned short>& out_indices,
				std::vector<glm::vec3>& out_vertices,
				std::vector<glm::vec2>& out_uvs,
				std::vector<glm::vec3>& out_normals );
			bool CompareVertex( Attrib & packed, std::map<Attrib,unsigned short> & VertexToOutIndex,unsigned short & result );

			void SetupFonts();
			void InitializeCamera();

			FBORenderTexture*	m_multipleRenderTarget;

			GLuint rboDepthStencil;
			GLuint meshCube, meshQuad;
			GLuint buffCube, buffQuad;
			SDL_Window*		window;

			mat4 modelMat, viewMat, projMat, MVP;
			mat4 depthViewMat[MAX_LIGHTS], depthProjMat[MAX_LIGHTS], depthMVP;

			SDL_Surface *HUD;		

			GLuint deferred_fbo;
			GLuint screen_fbo;
			GLuint lighting_fbo;
			GLuint shadow_fbo;

			GLuint diffuse_rt;
			GLuint positions_rt;
			GLuint normals_rt;
			GLuint depth_buffer;		

			// Render target textures
			GLuint screen_tex;
			int current_rt;

			GLuint rt_textures[RT_TOTAL];
			//GLuint light_tex;
			//GLuint depth_tex;
			//GLuint diffuse_tex;
			//GLuint positions_tex;
			//GLuint normals_tex;


			std::map<std::string,GLuint> TEXTURE_LIST;
			std::map<std::string,MeshData> MESH_LIST;
			std::vector<GLuint> DEBUG_TEXT;
			std::vector<GLuint> FONTMAPS;
			std::vector<Shader> SHADERS;			

			std::vector<Model*> MODEL_LIST;
			std::vector<Light*> LIGHT_LIST;

			GLuint num_lights;

			float screen_width, screen_height;
			GLint screen_width_i, screen_height_i;		

			SDL_Surface*	debug_surface;
			std::stringstream debug_string;

			FTGLPixmapFont* font;

			const aiScene* scene;

			Uint32 ticks;
			Uint32 fps;
			std::string framerate;
			float accum;

			GLuint	base;
			GLfloat	cnt1;
			GLfloat	cnt2;

			vec3 cam_pos;
			vec3 cam_look;
			vec3 cam_up;
			vec3 cam_right; 
			quat cam_rot;

			unsigned varray_size;

			float pitch;

			int shadow_size;

			Entity* SelectedEntity;


		// Comparator functor for sorting the model list by depth
		struct ZSorting
		{
			ZSorting( const Graphics& g) : graphics(g) {}			
			const Graphics& graphics;
			bool operator() ( Model* a, Model* b ) 
			{ 
				vec3 posa = a->GetTransform()->Position;
				vec3 posb = b->GetTransform()->Position;

				return glm::distance( posa, graphics.cam_pos ) < glm::distance( posb, graphics.cam_pos );
			}
		};

		
	};
}