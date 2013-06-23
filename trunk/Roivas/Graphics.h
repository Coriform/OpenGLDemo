#pragma once

//#define GLEW_STATIC
//#include <GL/glew.h>

#include "SDL.h"
#include "SDL_opengl.h"
#include <FTGL/ftgl.h>
#include "SOIL.h"
#include "System.h"
#include "Model.h"

namespace Roivas
{
	class Graphics : public System
	{
		public:
			Graphics(SDL_Window* _window, float screenwidth, float screenheight);
			~Graphics();
			void Initialize();
			void PreloadAssets();
			GLuint LoadTexture(std::string path);
			GLuint LoadMesh(std::string path);
			void Update(float dt);
			void UpdateScreenDims(int x, int y, int w, int h);
			void UpdateCameraRotation(float x, float y);

			void AddComponent(Model* m);
			void RemoveComponent(Model* m);

			void CameraPitch(float angle);
			void CameraYaw(float angle);
			void CameraRoll(float angle);			

		private:
			void Draw3D(float dt);
			void DrawPP(float dt);			
			void DrawWireframe(float dt);		
			void Draw2D(float dt);
			void UpdateCamera(float dt);
			void DrawDebugText(std::string path);

			void CreateShaderProgram(std::string _vertSource, std::string _fragSource);			
			void LoadFontmap(std::string path);
			GLint LoadShader(std::string shader_filename, GLenum shader_type);
			void SetupFonts();
			void InitializeCamera();

			GLuint frameBuffer;
			GLuint rboDepthStencil;
			GLuint meshCube, meshQuad;
			GLuint buffCube, buffQuad;
			SDL_Window*		window;

			mat4 modelMat, viewMat, projMat, orthoMat;
			GLuint uniColor, uniModel, uniView, uniProj, uniOrtho;
			GLuint uniLightPos;
			GLuint wireColor, wireModel, wireView, wireProj;
			

			SDL_Surface *HUD;			

			GLuint texColorBuffer;

			std::map<std::string,GLuint> TEXTURE_LIST;
			std::map<std::string,GLuint> MESH_LIST;
			std::vector<GLuint> DEBUG_TEXT;
			std::vector<GLuint> FONTMAPS;
			std::vector<GLuint> SHADER_PROGRAMS;
			std::vector<GLuint> VERTEX_SHADERS;
			std::vector<GLuint> FRAGMENT_SHADERS;

			std::vector<Model*> MODEL_LIST;

			float screen_width, screen_height;
			GLint screen_width_i, screen_height_i;		

			SDL_Surface*	debug_surface;
			std::stringstream debug_string;

			FTGLPixmapFont* font;
			//FTGLPolygonFont* font;

			Uint32 ticks;
			Uint32 fps;
			std::string framerate;
			float accum;

			GLuint	base;
			GLfloat	cnt1;
			GLfloat	cnt2;

			Entity* light;

			vec3 cam_pos;
			vec3 cam_look;
			vec3 cam_up;
			vec3 cam_right;
			quat cam_rot;

			float pitch;
	};
}