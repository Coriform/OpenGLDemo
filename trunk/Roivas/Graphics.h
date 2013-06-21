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
			void Update(float dt);
			void UpdateScreenDims(int x, int y, int w, int h);
			void UpdateCameraRotation(float x, float y);


			void CameraPitch(float angle);
			void CameraYaw(float angle);
			void CameraRoll(float angle);			
			

			void TempCreate();

		private:
			void Draw3D(float dt);
			void DrawPP(float dt);			
			void DrawWireframe(float dt);		
			void Draw2D(float dt);
			void UpdateCamera(float dt);

			void DrawDebugText(std::string path);
			void CreateShaderProgram(std::string _vertSource, std::string _fragSource);
			void LoadTexture(std::string path);
			void LoadFontmap(std::string path);
			GLint LoadShader(std::string shader_filename, GLenum shader_type);
			void SetupFonts();
			void InitializeCamera();

			GLuint frameBuffer;
			GLuint rboDepthStencil;
			GLuint vaoCube, vaoQuad, vaoText;
			GLuint vboCube, vboQuad, vboText;
			SDL_Window*		window;

			mat4 model, view, proj, ortho;
			GLuint uniColor, uniModel, uniView, uniProj, uniOrtho;
			GLuint uniLightPos;
			GLuint wireColor, wireModel, wireView, wireProj;
			

			SDL_Surface *HUD;			

			GLuint texColorBuffer;

			std::vector<GLuint> TEXTURES;
			std::vector<GLuint> DEBUG_TEXT;
			std::vector<GLuint> FONTMAPS;
			std::vector<GLuint> SHADER_PROGRAMS;
			std::vector<GLuint> VERTEX_SHADERS;
			std::vector<GLuint> FRAGMENT_SHADERS;

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

			vec3 light_pos;

			vec3 cam_pos;
			vec3 cam_look;
			vec3 cam_up;
			vec3 cam_right;
			quat cam_rot;

			vec3 obj_position;

			float pitch;

			Model* testmodel;
	};
}