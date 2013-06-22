#include "CommonLibs.h"
#include "Graphics.h"
#include "Core.h"
#include "Body.h"		// DELETE
#include "Transform.h"	// DELETE
#include "Entity.h"		// DELETE
#include "FileIO.h"		// DELETE

namespace Roivas
{
	Graphics::Graphics(SDL_Window* _window, float s_width, float s_height) : System(SYS_Graphics, "Graphics"), 
		window(_window), 
		screen_width(s_width),
		screen_height(s_height),
		screen_width_i((GLint)s_width),
		screen_height_i((GLint)s_height),
		debug_surface(nullptr),
		ticks(0),
		fps(0),
		pitch(0.0f),
		accum(0.0f)
	{
		// Initialize ticks counted for framerate
		ticks = SDL_GetTicks();

		// Vsync = 1; Immediate = 0
		SDL_GL_SetSwapInterval(0);

	}

	void Graphics::Initialize()
	{
		// Reset matrix
		glLoadIdentity();

		// Clear color
		glClearColor ( 1.0, 1.0, 1.0, 1.0 );

		// Shader model - Use this
		glShadeModel(GL_SMOOTH);
 
		//2D rendering
		glMatrixMode(GL_PROJECTION);
		
 
		// Enable depth checking
		glEnable(GL_DEPTH_TEST);

		// Enable alpha blending
		glEnable(GL_BLEND);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_ALPHA_TEST);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Create and assign fonts
		SetupFonts();

		// Initialize Camera
		InitializeCamera();

		// Build Shaders - automate this somehow
		// Make sure this is in same order as enum list
		CreateShaderProgram("Assets/Shaders/Default.vert",	"Assets/Shaders/Default.frag");
		CreateShaderProgram("Assets/Shaders/Phong.vert",	"Assets/Shaders/Phong.frag");
		CreateShaderProgram("Assets/Shaders/Screen.vert",	"Assets/Shaders/Screen.frag");
		CreateShaderProgram("Assets/Shaders/Hud.vert",		"Assets/Shaders/Hud.frag");
		CreateShaderProgram("Assets/Shaders/Wireframe.vert","Assets/Shaders/Wireframe.frag");

		// Preload meshes and textures
		PreloadAssets();


		// DELETE; function used to create temporary geometry, VAOs, VBOs, and specify data layout for shaders
		TempCreate();

		// Reset default framebuffer
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );


		Entity* e = Factory::AddEntity("Assets/Objects/test2.json");

		Factory::AddEntity("Assets/Objects/test2.json");
		Factory::AddEntity("Assets/Objects/test2.json");
		Factory::AddEntity("Assets/Objects/test2.json");

		Factory::AddEntity("Assets/Objects/test2.json");
	}

	void Graphics::Update(float dt)
	{
		// Calculate framerate, based on an increment every 1000 milliseconds
		if( SDL_GetTicks() - ticks >= 1000 )
		{
			std::stringstream ss;
			ss << "FPS: " << fps;
			framerate = ss.str();
			std::cout << "Frame rate frames per second: " << framerate << std::endl;
			fps = 0;
			ticks = SDL_GetTicks();
		}

		// Actual framerate value
		++fps;

		// Updates current camera
		UpdateCamera(dt);

		// Primary drawing functions; draws the geometry and lighting calculations
		Draw3D(dt);		

		// Post-processing effects
		DrawPP(dt);

		// Debug drawing
		DrawWireframe(dt);

		// HUD and other 2D drawing
		Draw2D(dt);	

		// Draw framerate
		DrawDebugText(framerate);	

		// Clears debug text from previous frame; might change this
		DEBUG_TEXT.clear();

	
		// Update window with OpenGL context; Swap buffers
		SDL_GL_SwapWindow(window);

		Transform* t = new Transform();

		
		Entity* ent = new Entity();

		ent->AddComponent(t);

		testmodel->Owner = ent;
		Body* b = testmodel->GetBody();

		Transform* t2 = testmodel->GetTransform();

		//FileIO fio;
		//fio.Test();

		int iii = 0;
	}

	void Graphics::PreloadAssets()
	{
		testmodel = new Model();

		// Load textures - automate this somehow
		testmodel->DiffuseID = LoadTexture("Assets/Textures/sample.png");

		// Cube vertices
		float cubeVertices[] = {
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

			-1.0f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			 1.0f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
			-1.0f, -0.5f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
		};

		// Quad vertices
		float quadVertices[] = {
			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f,  1.0f,  1.0f, 1.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,

			 1.0f, -1.0f,  1.0f, 0.0f,
			-1.0f, -1.0f,  0.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 1.0f
		};

		// Create VAOs
		glGenVertexArrays( 1, &meshCube );
		glGenVertexArrays( 1, &meshQuad );

		// Load vertex data
		glGenBuffers( 1, &buffCube ); // Generate 1 buffer
		glGenBuffers( 1, &buffQuad );

		glBindBuffer( GL_ARRAY_BUFFER, buffCube );
		glBufferData( GL_ARRAY_BUFFER, sizeof( cubeVertices ), cubeVertices, GL_DYNAMIC_DRAW );

		glBindBuffer( GL_ARRAY_BUFFER, buffQuad );
		glBufferData( GL_ARRAY_BUFFER, sizeof( quadVertices ), quadVertices, GL_DYNAMIC_DRAW );

		testmodel->MeshID = meshCube;
	}


	void Graphics::TempCreate()
	{
		

		//SHADER_PROGRAMS[SH_PHONG];

		// Specify the layout of the vertex data
		glBindVertexArray( meshCube );
		glBindBuffer( GL_ARRAY_BUFFER, buffCube );

			GLint posAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Default), "position" );
			glEnableVertexAttribArray( posAttrib );
			glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0 );

			GLint colAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Default), "color" );
			glEnableVertexAttribArray( colAttrib );
			glVertexAttribPointer( colAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)( 3*sizeof(float) ) );

			GLint texAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Default), "texcoord" );
			glEnableVertexAttribArray( texAttrib );
			glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)( 6*sizeof(float) ) );


		glBindVertexArray( meshQuad );
		glBindBuffer( GL_ARRAY_BUFFER, buffQuad );

			posAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Screen), "position" );
			glEnableVertexAttribArray( posAttrib );
			glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );

			texAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Screen), "texcoord" );
			glEnableVertexAttribArray( texAttrib );
			glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), (void*)( 2 * sizeof( float ) ) );

			//

			posAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Hud), "position" );		// HUD shader
			glEnableVertexAttribArray( posAttrib );
			glVertexAttribPointer( posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof( float ), 0 );



		////
		
		glGenFramebuffers( 1, &frameBuffer );
		glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );

		glGenTextures( 1, &texColorBuffer );
		glBindTexture( GL_TEXTURE_2D, texColorBuffer );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, screen_width_i, screen_height_i, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0 );

		glGenRenderbuffers( 1, &rboDepthStencil );
		glBindRenderbuffer( GL_RENDERBUFFER, rboDepthStencil );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width_i, screen_height_i );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil );

		////


		uniView = glGetUniformLocation( SHADER_PROGRAMS.at(SH_Default), "view" );
		uniProj = glGetUniformLocation( SHADER_PROGRAMS.at(SH_Default), "proj" );
		uniModel = glGetUniformLocation( SHADER_PROGRAMS.at(SH_Default), "model" );
		uniColor = glGetUniformLocation( SHADER_PROGRAMS.at(SH_Phong), "overrideColor" );
		uniLightPos = glGetUniformLocation( SHADER_PROGRAMS.at(SH_Phong), "lightpos" );

		wireView = glGetUniformLocation( SHADER_PROGRAMS.at(SH_Wireframe), "view" );
		wireProj = glGetUniformLocation( SHADER_PROGRAMS.at(SH_Wireframe), "proj" );
		wireModel = glGetUniformLocation( SHADER_PROGRAMS.at(SH_Wireframe), "model" );
		wireColor = glGetUniformLocation( SHADER_PROGRAMS.at(SH_Wireframe), "wirecolor" );

		modelMat = mat4();

		obj_position = vec3();

		light_pos = vec3(0,4,0);
	}

	void Graphics::Draw3D(float dt)
	{
		obj_position = vec3(1.0f,0,0);

		glPolygonMode(GL_FRONT, GL_FILL);

		accum += dt;

		glBindFramebuffer( GL_FRAMEBUFFER, frameBuffer );	// RW; uses frameBuffer object to store and read from
		// If I put this ABOVE glBindFrameBuffer, acts like it doesn't clear
		// Maybe: I bind the buffer, then I clear THE BUFFER???

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glEnable( GL_DEPTH_TEST );			// Enable z buffering / occlusion testing

		glBindVertexArray( testmodel->MeshID );		// Use the cube mesh		
		
		glUseProgram( SHADER_PROGRAMS.at(SH_Phong) );		// Activate phong shader

		glUniformMatrix4fv( uniView, 1, GL_FALSE, MatToArray( viewMat ) );
		glUniformMatrix4fv( uniProj, 1, GL_FALSE, MatToArray( projMat ) );

		//if( TEXTURES.size() > 0 && TEXTURES.at("Assets/Textures/sample.png") )
		//{
			glActiveTexture( GL_TEXTURE0 );					
			glBindTexture( GL_TEXTURE_2D, testmodel->DiffuseID );		// Stores TEXTURES.at(0) into texture unit 0
		//}


		modelMat = mat4();		
		modelMat = glm::translate( modelMat, obj_position );
		modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );		
		glUniform3f( uniColor, 1.0f, 1.0f, 1.0f );
		glUniform3f( uniLightPos, light_pos.x, light_pos.y, light_pos.z );
		glUniformMatrix4fv( uniModel, 1, GL_FALSE, MatToArray( modelMat ) );		// Pass the locally transformed model matrix to the scene shader		
		glDrawArrays( GL_TRIANGLES, 0, 36 );	// Draw first cube


		glEnable( GL_STENCIL_TEST );			// Enable stencil teesting

		// Draw floor
		glStencilFunc( GL_ALWAYS, 1, 0xFF ); // Set any stencil to 1
		glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
		glStencilMask( 0xFF ); // Write to stencil buffer

		glClear( GL_STENCIL_BUFFER_BIT ); // Clear stencil buffer (0 by default)

		glDepthMask( GL_FALSE ); // Don't write to depth buffer		

		modelMat = mat4();	
		modelMat = glm::translate( modelMat, obj_position );
		modelMat = glm::translate( modelMat, vec3(0, -EPSILON, 0));		
		glUniformMatrix4fv( uniModel, 1, GL_FALSE, MatToArray( modelMat ) );	
		glDrawArrays( GL_TRIANGLES, 36, 6 );		//  Reflection plane
		
		glDepthMask( GL_TRUE ); // Write to depth buffer


		// Draw cube reflection
		glStencilFunc( GL_EQUAL, 1, 0xFF ); // Pass test if stencil value is 1
		glStencilMask( 0x00 ); // Don't write anything to stencil buffer		
		
		modelMat = mat4();		
		modelMat = glm::translate( modelMat, obj_position );
		modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );
		modelMat = glm::scale( glm::translate( modelMat, vec3( 0, -1, 0 ) ), vec3( 1, -1, 1 ) );		
		glUniformMatrix4fv( uniModel, 1, GL_FALSE, MatToArray( modelMat ) );	
		glUniform3f( uniColor, 0.3f, 0.3f, 0.3f );
		glDrawArrays( GL_TRIANGLES, 0, 36 );			// Draw inverted cube
		

		glDisable( GL_STENCIL_TEST );


		
		// Bind default framebuffer and draw contents of our framebuffer
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );		// Default system frame buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glDisable( GL_DEPTH_TEST );

		glBindVertexArray( meshQuad );
		
		glUseProgram( SHADER_PROGRAMS.at(SH_Screen) );

		glActiveTexture( GL_TEXTURE0 );

		glBindTexture( GL_TEXTURE_2D, texColorBuffer );
		//glBindTexture( GL_TEXTURE_2D, TEXTURES.at(2) );

		// Screen quad; This contains the entire scene
		glDrawArrays( GL_TRIANGLES, 0, 6 );



		

	}

	void Graphics::DrawPP(float dt)
	{
		// GUI drawing

	}

	void Graphics::Draw2D(float dt)
	{
		// GUI drawing


		
	}

	void Graphics::DrawWireframe(float dt)
	{		
		// Draw wireframe geometry
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);

		//glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		glBindVertexArray( testmodel->MeshID );		// Use the cube mesh		
		
		glUseProgram( SHADER_PROGRAMS.at(SH_Wireframe) );		// Activate phong shader

		glUniformMatrix4fv( wireView, 1, GL_FALSE, MatToArray( viewMat ) );
		glUniformMatrix4fv( wireProj, 1, GL_FALSE, MatToArray( projMat ) );

		modelMat = mat4();		
		modelMat = glm::translate( modelMat, obj_position );
		modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );		
		glUniform3f( wireColor, 1.0f, 0.0f, 0.0f );
		glUniformMatrix4fv( wireModel, 1, GL_FALSE, MatToArray( modelMat ) );		// Pass the locally transformed model matrix to the scene shader		
		glDrawArrays( GL_TRIANGLES, 0, 36 );	// Draw first cube

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);	
	}

	void Graphics::DrawDebugText(std::string text)
	{
		if( font->Error() )
			return;

		float offset = font->LineHeight() / screen_height;		// Add this stuff to the function signature

		glUseProgram(0);		// Set current shader to null
		glColor3f(0.0, 0.0, 0.0); 
		glRasterPos2f(-0.99f, 1-offset*2);
		font->Render(text.c_str());	
	}

	GLuint Graphics::LoadTexture(std::string path)
	{
		// Need to add code to assign texture GLuint to model

		if( TEXTURE_LIST.find(path) != TEXTURE_LIST.end() )
		{
			return TEXTURE_LIST.at(path);
		}

		int width, height;
		unsigned char* image = nullptr;

		image = SOIL_load_image( path.c_str(), &width, &height, 0, SOIL_LOAD_RGB );

		if( image == nullptr )
		{
			std::cout << "Texture didn't load properly" << std::endl;
			return 0;
		}

		GLuint texture;
		glGenTextures( 1, &texture );

		glBindTexture( GL_TEXTURE_2D, texture );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
		SOIL_free_image_data( image );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );;
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		TEXTURE_LIST[path] = texture;

		return texture;
	}

	GLuint Graphics::LoadMesh(std::string path)
	{
		// Need to add code to assign texture GLuint to model

		if( MESH_LIST.find(path) != MESH_LIST.end() )
		{
			return MESH_LIST.at(path);
		}

		float vertices[10];
		GLuint buff, mesh;


		glGenVertexArrays( 1, &mesh );
		glGenBuffers( 1, &buff );

		glBindBuffer( GL_ARRAY_BUFFER, buff );
		glBufferData( GL_ARRAY_BUFFER, sizeof( vertices ), vertices, GL_DYNAMIC_DRAW );

		// Specify the layout of the vertex data
		glBindVertexArray( mesh );
		glBindBuffer( GL_ARRAY_BUFFER, buff );

			GLint posAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Default), "position" );
			glEnableVertexAttribArray( posAttrib );
			glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0 );

			GLint colorAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Default), "color" );
			glEnableVertexAttribArray( colorAttrib );
			glVertexAttribPointer( colorAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)( 3*sizeof(float) ) );

			GLint normAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Default), "normal" );
			glEnableVertexAttribArray( normAttrib );
			glVertexAttribPointer( normAttrib, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)( 6*sizeof(float) ) );

			GLint texAttrib = glGetAttribLocation( SHADER_PROGRAMS.at(SH_Default), "texcoord" );
			glEnableVertexAttribArray( texAttrib );
			glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)( 8*sizeof(float) ) );


		MESH_LIST[path] = mesh;

		return mesh;
	}

	void Graphics::LoadFontmap(std::string path)
	{
		GLuint texture;
		glGenTextures( 1, &texture );
	
		int width, height;
		unsigned char* image;
	
		glBindTexture( GL_TEXTURE_2D, texture );
		image = SOIL_load_image( path.c_str(), &width, &height, 0, SOIL_LOAD_RGB );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image );
		SOIL_free_image_data( image );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		FONTMAPS.push_back(texture);
	}

	void Graphics::CreateShaderProgram(std::string _vertSource, std::string _fragSource)
	{
		GLuint vertShader = LoadShader(_vertSource, GL_VERTEX_SHADER);
		GLuint fragShader = LoadShader(_fragSource, GL_FRAGMENT_SHADER);

		VERTEX_SHADERS.push_back(vertShader);
		FRAGMENT_SHADERS.push_back(fragShader);

		GLuint shaderProgram = glCreateProgram();
		glAttachShader( shaderProgram, vertShader );
		glAttachShader( shaderProgram, fragShader );
		glBindFragDataLocation( shaderProgram, 0, "outColor" );
		glLinkProgram( shaderProgram );	

		SHADER_PROGRAMS.push_back(shaderProgram);
	}


	GLint Graphics::LoadShader(std::string shader_filename, GLenum shader_type)
	{
		std::ifstream shaderFile( shader_filename );
		std::stringstream shaderData;
		shaderData << shaderFile.rdbuf();  //Loads the entire string into a string stream.
		shaderFile.close();
		const std::string &shaderString = shaderData.str(); //Get the string stream as a std::string.

		const char* source = shaderString.c_str(); 

		GLuint shader_resource = glCreateShader( shader_type );
		glShaderSource( shader_resource, 1, (const GLchar**)(&source), NULL );

		glCompileShader( shader_resource );

		GLint status;
		glGetShaderiv( shader_resource, GL_COMPILE_STATUS, &status ); 

		if( status != GL_TRUE )
		{
			char buffer[512];
			glGetShaderInfoLog( shader_resource, 512, NULL, buffer ); 
			std::cout << buffer << std::endl;
		}

		return shader_resource;
	}

	void Graphics::SetupFonts()
	{
		font = new FTGLPixmapFont("Assets/Fonts/arial.ttf");

		if( font->Error() )
		{
			std::cout << "Font didn't load properly" << std::endl;
			return;
		}
		
		font->FaceSize(32);
	}

	void Graphics::UpdateScreenDims(int x, int y, int w, int h)
	{
		screen_width_i	= w;
		screen_height_i	= h;
		screen_width	= (float)w;
		screen_height	= (float)h;

		glViewport(0, 0, w, h);
	}

	void Graphics::InitializeCamera()
	{
		cam_pos		= vec3(0.0f,0.0f,4.0f);
		cam_look	= vec3(0.0f, 0.0f, -1.0f);
		cam_up		= vec3(0.0f, 1.0f, 0.0f);
		cam_rot		= quat();
	}

	void Graphics::UpdateCamera(float dt)
	{
		vec3 move(0,0,0); 
		float move_speed = 0.01f*dt;
		
		if( Input::GetInstance()->GetKey(SDLK_w) == true )
			move -= vec3(0.0f,0,move_speed);

		if( Input::GetInstance()->GetKey(SDLK_s) == true )
			move += vec3(0.0f,0,move_speed);

		if( Input::GetInstance()->GetKey(SDLK_a) == true )
			move -= vec3(move_speed,0,0);

		if( Input::GetInstance()->GetKey(SDLK_d) == true )
			move += vec3(move_speed,0,0);

		if( Input::GetInstance()->GetKey(SDLK_r) == true )
			move += vec3(0,move_speed,0);

		if( Input::GetInstance()->GetKey(SDLK_f) == true )
			move -= vec3(0,move_speed,0);

		cam_pos += cam_rot * move;

		cam_look = cam_rot * vec3(0,0,-1);

		//glUseProgram( SHADER_PROGRAMS.at(SH_PHONG) );

		viewMat = glm::lookAt( cam_pos, cam_pos + cam_look, cam_up );
		//glUniformMatrix4fv( uniView, 1, GL_FALSE, MatToArray( view ) );

		projMat = glm::perspective( 45.0f, screen_width / screen_height, 1.0f, 1000.0f );
		//glUniformMatrix4fv( uniProj, 1, GL_FALSE, MatToArray( proj ) );
	}

	void Graphics::UpdateCameraRotation(float x, float y)
	{
		CameraYaw(x);
		CameraPitch(y);				
	}

	void Graphics::CameraPitch(float angle)
	{
		pitch += angle;

		if( pitch < -90.0f )
			pitch = -90.0f + EPSILON;
		else if( pitch > 90.0f )
			pitch = 90.0f - EPSILON;

		cam_rot = glm::normalize( glm::rotate(cam_rot, pitch, vec3(-1,0,0)) );
	}

	void Graphics::CameraYaw(float angle)
	{
		cam_rot = glm::normalize( glm::rotate(cam_rot, angle, vec3(0,-1,0)) );

		cam_rot.x = 0;
		cam_rot.z = 0;
		float mag = 1.0f / sqrt(cam_rot.w*cam_rot.w + cam_rot.y*cam_rot.y);
		cam_rot.w *= mag;
		cam_rot.y *= mag;
	}

	void Graphics::CameraRoll(float angle)
	{
		cam_rot = glm::normalize( glm::rotate(cam_rot, angle, cam_look) );
	}

	Graphics::~Graphics()
	{
		for( GLuint i : SHADER_PROGRAMS )
			glDeleteProgram(i);

		for( GLuint i : VERTEX_SHADERS )
			glDeleteProgram(i);

		for( GLuint i : FRAGMENT_SHADERS )
			glDeleteProgram(i);

		glDeleteBuffers( 1, &buffCube );
		glDeleteBuffers( 1, &buffQuad );

		glDeleteVertexArrays( 1, &meshCube );
		glDeleteVertexArrays( 1, &meshQuad );

		glDeleteFramebuffers( 1, &frameBuffer );
	}
}