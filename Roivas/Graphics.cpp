﻿#include "CommonLibs.h"
#include "Graphics.h"


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
		num_lights(0),
		pitch(0.0f),
		accum(0.0f),
		varray_size(8),
		current_rt(0),
		current_lighting(SH_LightingSSM),
		shadows_enabled(true),
		wireframe_enabled(false),
		normal_mapping_enabled(true),
		shadow_size(4.0f),
		shadow_smooth(16.0f),
		SelectedEntity(nullptr)
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
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

		// Shader model - Use this
		glShadeModel(GL_SMOOTH);
 
		//2D rendering
		glMatrixMode(GL_PROJECTION);
	
		// Enable depth checking
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		// Enable alpha blending
		glEnable(GL_BLEND);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnable(GL_ALPHA_TEST);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_CULL_FACE);

		// Create and assign fonts
		SetupFonts();

		// Initialize Camera
		InitializeCamera();
		
		// Build Shaders - automate this somehow
		// Make sure this is in same order as enum list
		CreateShaderProgram("Assets/Shaders/Default.vert",			"Assets/Shaders/Default.frag");			// SH_Default
		CreateShaderProgram("Assets/Shaders/Deferred.vert",			"Assets/Shaders/Deferred.frag");		// SH_Deferred
		CreateShaderProgram("Assets/Shaders/Screen.vert",			"Assets/Shaders/Screen.frag");			// SH_Screen
		CreateShaderProgram("Assets/Shaders/Hud.vert",				"Assets/Shaders/Hud.frag");				// SH_Hud
		CreateShaderProgram("Assets/Shaders/Wireframe.vert",		"Assets/Shaders/Wireframe.frag");		// SH_Wireframe
		CreateShaderProgram("Assets/Shaders/ShadowTex.vert",		"Assets/Shaders/ShadowTex.frag");		// SH_ShadowTex
		CreateShaderProgram("Assets/Shaders/Lighting.vert",			"Assets/Shaders/Lighting.frag");		// SH_Lighting
		CreateShaderProgram("Assets/Shaders/LightingWithSSM.vert",	"Assets/Shaders/LightingWithSSM.frag");	// SH_LightingSSM

		//shadow_size = 1;

		// Preload meshes and textures
		PreloadAssets();

		// Reset default framebuffer
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );

		Level* lvl = new Level("TestLevel.json");
		lvl->Load();

		// Load light data into vectors
		ProcessLights();

		BuildShadows();

		//Level* lvl2 = new Level("TestLevel2.json");
		//lvl2->Load();
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
			//std::cout << "Light pos: " << light->GetTransform()->Position.x << "," << light->GetTransform()->Position.y << "," << light->GetTransform()->Position.z << "," << std::endl;
			fps = 0;
			ticks = SDL_GetTicks();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneLighting], 0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		// Actual framerate value
		++fps;

		// Depth sorting
		SortModels(dt);	

		// Updates current camera
		UpdateCamera(dt);

		// Primary drawing functions; draws the geometry and lighting calculations
		Draw3D(dt);		

		// Post-processing effects
		//DrawPP(dt);

		// Debug drawing
		DrawEditor(dt);

		if( wireframe_enabled == true )
			DrawWireframe(dt);

		// HUD and other 2D drawing
		//Draw2D(dt);	

		// Draw framerate
		DrawDebugText(framerate);	

		// Draw screen texture to screen
		ScreenPass(dt);

		// Clears debug text from previous frame; might change this
		DEBUG_TEXT.clear();
	
		// Update window with OpenGL context; Swap buffers
		SDL_GL_SwapWindow(window);
	}




	void Graphics::PreloadAssets()
	{
		static const GLfloat g_quad_vertex_buffer_data[] = { 
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,
		};

		glGenVertexArrays(1, &meshQuad);
		glBindVertexArray(meshQuad);

		glGenBuffers( 1, &buffQuad );
		glBindBuffer(GL_ARRAY_BUFFER, buffQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

		glGenFramebuffers( 1, &screen_fbo );
		glBindFramebuffer( GL_FRAMEBUFFER, screen_fbo );

		glGenTextures( 1, &screen_tex );
		glBindTexture( GL_TEXTURE_2D, screen_tex );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, screen_width_i, screen_height_i, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_tex, 0 );

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE ) 
		{
			std::cout << "FB error, status: " << status << std::endl;
			return;
		} 



		glGenTextures( 1, &rt_textures[RT_SceneLighting] );
		glBindTexture( GL_TEXTURE_2D, rt_textures[RT_SceneLighting] );

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, screen_width_i, screen_height_i, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneLighting], 0 );

		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE ) 
		{
			std::cout << "FB error, status: " << status << std::endl;
			return;
		} 



		glGenFramebuffers(1, &deferred_fbo);
		glGenRenderbuffers(1, &diffuse_rt);
		glGenRenderbuffers(1, &positions_rt);
		glGenRenderbuffers(1, &normals_rt);
		glGenRenderbuffers(1, &depth_buffer);

		// Bind the FBO so that the next operations will be bound to it
		glBindFramebuffer(GL_FRAMEBUFFER, deferred_fbo);

		// Bind the diffuse render target
		glBindRenderbuffer(GL_RENDERBUFFER, diffuse_rt);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, screen_width_i, screen_height_i);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, diffuse_rt);

		// Bind the position render target
		glBindRenderbuffer(GL_RENDERBUFFER, positions_rt);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA32F, screen_width_i, screen_height_i);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_RENDERBUFFER, positions_rt);

		// Bind the normal render target
		glBindRenderbuffer(GL_RENDERBUFFER, normals_rt);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA16F, screen_width_i, screen_height_i);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_RENDERBUFFER, normals_rt);

		// Bind the depth buffer
		glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screen_width_i, screen_height_i);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_buffer);


		// Generate and bind the OGL texture for diffuse
		glGenTextures(1, &rt_textures[RT_SceneDiffuse]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneDiffuse]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width_i, screen_height_i, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneDiffuse], 0);

		// Generate and bind the OGL texture for positions
		glGenTextures(1, &rt_textures[RT_ScenePositions]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_ScenePositions]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rt_textures[RT_ScenePositions], 0);

		// Generate and bind the OGL texture for normals
		glGenTextures(1, &rt_textures[RT_SceneNormals]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneNormals]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, rt_textures[RT_SceneNormals], 0);

		// Generate and bind the OGL texture for depth
		glGenTextures(1, &rt_textures[RT_SceneDepth]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneDepth]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, rt_textures[RT_SceneDepth], 0);


		// Check if all worked fine and unbind the FBO
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "FB error for deferred, status: " << status << std::endl;
		}



		glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

		glGenRenderbuffers( 1, &rboDepthStencil );
		glBindRenderbuffer( GL_RENDERBUFFER, rboDepthStencil );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screen_width_i, screen_height_i );
		glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil );

		modelMat = mat4();

		current_rt = RT_SceneLighting;
		screen_tex = rt_textures[RT_SceneLighting];
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Graphics::Draw3D(float dt)
	{
		accum += dt;

		glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glEnable( GL_DEPTH_TEST );

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		GeometryPass(dt);

		if( shadows_enabled == true )
			ShadowPass(dt);		

		LightingPass(dt);		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	}

	void Graphics::GeometryPass(float dt)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, deferred_fbo);
		glViewport(0,0,screen_width_i,screen_height_i); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glDisable( GL_BLEND );
		glCullFace(GL_BACK);

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
		glDrawBuffers(4, buffers);
		
		glUseProgram( SHADERS.at(SH_Deferred).ShaderProgram );


		for( unsigned i = 0; i < MODEL_LIST.size(); ++i )
		{
			Transform* t = MODEL_LIST[i]->GetTransform();

			if( t == nullptr )
				continue;

			modelMat = mat4();			

			modelMat = glm::translate( modelMat, t->Position );

			if( MODEL_LIST[i]->Owner->GetBehavior() != nullptr )
				modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );	

			modelMat = glm::rotate( modelMat, t->Rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.z, vec3( 0.0f, 0.0f, 1.0f ) );

			modelMat = glm::scale( modelMat, t->Scale );
			
			SHADERS[SH_Deferred].SetUniform1i( "normal_mapping", normal_mapping_enabled );
			SHADERS[SH_Deferred].SetUniform4fv( "M", &modelMat[0][0]);
			SHADERS[SH_Deferred].SetUniform4fv( "V", &viewMat[0][0]);
			SHADERS[SH_Deferred].SetUniform4fv( "P", &projMat[0][0] );

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, MODEL_LIST.at(i)->DiffuseID);
			SHADERS[SH_Deferred].SetUniform1i( "tex_sampler", 0 );

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, MODEL_LIST.at(i)->NormalID);
			SHADERS[SH_Deferred].SetUniform1i( "norm_sampler", 1 );

			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);

			glBindBuffer(GL_ARRAY_BUFFER, MODEL_LIST.at(i)->VertexBuffer);
				glVertexAttribPointer(
					0,                  // attribute
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					0,                  // stride
					(void*)0            // array buffer offset
				);

		
			glBindBuffer(GL_ARRAY_BUFFER, MODEL_LIST.at(i)->UVBuffer);
				glVertexAttribPointer(
					1,                                // attribute
					2,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);

		
			glBindBuffer(GL_ARRAY_BUFFER, MODEL_LIST.at(i)->NormalBuffer);
				glVertexAttribPointer(
					2,                                // attribute
					3,                                // size
					GL_FLOAT,                         // type
					GL_FALSE,                         // normalized?
					0,                                // stride
					(void*)0                          // array buffer offset
				);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MODEL_LIST.at(i)->ElementBuffer);
				glDrawElements(
					GL_TRIANGLES,      // mode
					//indices.size(),    // count
					MODEL_LIST.at(i)->Indices.size(),
					GL_UNSIGNED_SHORT, // type
					(void*)0           // element array buffer offset
				);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
			glDisableVertexAttribArray(2);
		}			

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		
	}

	void Graphics::ShadowPass(float dt)
	{		
		glViewport(0, 0, (int)(screen_width*shadow_size), (int)(screen_width*shadow_size));
		
		glEnable( GL_DEPTH_TEST );
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT); // Cull back-facing triangles -> draw only front-facing triangles		
		

		glUseProgram( SHADERS.at(SH_ShadowTex).ShaderProgram );
		glBindFramebuffer( GL_FRAMEBUFFER, shadow_fbo );


		for( unsigned j = 0; j < num_lights; ++j )
		{
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, LIGHT_LIST.at(j)->RT_Textures[RT_LightDepth], 0);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			vec3 pos = LIGHT_LIST.at(j)->GetTransform()->Position;

			if( LIGHT_LIST.at(j)->Type == LT_DirectionLight )
			{
				depthProjMat[j] = glm::ortho<float>(-20,20,-20,20,-20,10);
				depthViewMat[j] = glm::lookAt(LIGHT_LIST.at(j)->Direction, vec3(0,0,0), vec3(0,1,0));
			}
			else if( LIGHT_LIST.at(j)->Type == LT_SpotLight )
			{
				depthProjMat[j] = glm::perspective<float>(45.0f, 1.0f, 0.95f, 50.0f);
				depthViewMat[j] = glm::lookAt(pos, pos-LIGHT_LIST.at(j)->Direction, glm::vec3(0,1,0));
			}
			else if( LIGHT_LIST.at(j)->Type == LT_PointLight )
			{
				depthProjMat[j] = glm::perspective<float>(90.0f, 1.0f, 0.95f, 50.0f);
				depthViewMat[j] = glm::lookAt(pos, pos-LIGHT_LIST.at(j)->Direction, glm::vec3(0,1,0));
			}

			for( unsigned i = 0; i < MODEL_LIST.size(); ++i )
			{
				Transform* t = MODEL_LIST[i]->GetTransform();

				if( t == nullptr )
					continue;

				modelMat = mat4();			

				modelMat = glm::translate( modelMat, t->Position );

				if( MODEL_LIST[i]->Owner->GetBehavior() != nullptr )
					modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );	

				modelMat = glm::rotate( modelMat, t->Rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );
				modelMat = glm::rotate( modelMat, t->Rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
				modelMat = glm::rotate( modelMat, t->Rotation.z, vec3( 0.0f, 0.0f, 1.0f ) );

				modelMat = glm::scale( modelMat, t->Scale );

				depthMVP = depthProjMat[j] * depthViewMat[j] * modelMat;

				SHADERS[SH_ShadowTex].SetUniform4fv( "depthMVP", &depthMVP[0][0] );

				glEnableVertexAttribArray(0);

				glBindBuffer(GL_ARRAY_BUFFER, MODEL_LIST.at(i)->VertexBuffer);
					glVertexAttribPointer(
						0,  // The attribute we want to configure
						3,                  // size
						GL_FLOAT,           // type
						GL_FALSE,           // normalized?
						0,                  // stride
						(void*)0            // array buffer offset
					);

				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MODEL_LIST.at(i)->ElementBuffer);
					glDrawElements(
						GL_TRIANGLES,      // mode
						MODEL_LIST.at(i)->Indices.size(),
						GL_UNSIGNED_SHORT, // type
						(void*)0           // element array buffer offset
					);

				glDisableVertexAttribArray(0);
			}
		}
	}

	void Graphics::LightingPass(float dt)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneLighting], 0 );

		glViewport(0,0,screen_width_i,screen_height_i); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glCullFace(GL_BACK);

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		
		glUseProgram( SHADERS.at(current_lighting).ShaderProgram );

		mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0, 
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
		);

		for( unsigned j = 0; j < num_lights; ++j )
		{	
			Light* light = LIGHT_LIST[j];

			if( j > 0 )
			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ONE);
				glDepthFunc(GL_LEQUAL);
			}			
			
			SHADERS[current_lighting].SetUniform1i( "num_lights",	num_lights );
			SHADERS[current_lighting].SetUniform1f( "shadowsmooth", shadow_smooth );
			SHADERS[current_lighting].SetUniform3f( "lightpos",		light->GetTransform()->Position );
			SHADERS[current_lighting].SetUniform3f( "lightcolor",	light->Color );
			SHADERS[current_lighting].SetUniform3f( "lightdir",		light->Direction );
			SHADERS[current_lighting].SetUniform1f( "lightradius",	light->Radius);
			SHADERS[current_lighting].SetUniform1f( "lightcone",	light->Cone );
			SHADERS[current_lighting].SetUniform1i( "lighttype",	light->Type );

			SHADERS[current_lighting].SetUniform4fv( "Bias", &biasMatrix[0][0]);
			SHADERS[current_lighting].SetUniform4fv( "DepthProj", &depthProjMat[j][0][0]);
			SHADERS[current_lighting].SetUniform4fv( "DepthView", &depthViewMat[j][0][0]);
			SHADERS[current_lighting].SetUniform4fv( "M", &modelMat[0][0]);
			SHADERS[current_lighting].SetUniform4fv( "V", &viewMat[0][0]);
			SHADERS[current_lighting].SetUniform4fv( "P", &projMat[0][0] );



			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneDiffuse]);
			SHADERS[current_lighting].SetUniform1i( "tDiffuse", 0 );

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, rt_textures[RT_ScenePositions]);
			SHADERS[current_lighting].SetUniform1i( "tPosition", 1 );

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneNormals]);
			SHADERS[current_lighting].SetUniform1i( "tNormals", 2 );

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, LIGHT_LIST.at(j)->RT_Textures[RT_LightDepth]);
			SHADERS[current_lighting].SetUniform1i( "tShadow", 3 );	

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, buffQuad);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
			glDisableVertexAttribArray(0);

		}

		glDisable(GL_BLEND);

		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_tex, 0 );

	}

	void Graphics::ScreenPass(float dt)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glViewport(0,0,screen_width_i,screen_height_i); 

		// Use our shader
		glUseProgram( SHADERS.at(SH_Screen).ShaderProgram );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, screen_tex);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffQuad);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
		glDisableVertexAttribArray(0);








		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_Lighting], 0 );
		glViewport(screen_width_i/2+screen_width_i/4,screen_height_i/2+screen_height_i/4,screen_width_i/4,screen_height_i/4);
		glCullFace(GL_NONE);

		// Use our shader
		glUseProgram( SHADERS.at(SH_Screen).ShaderProgram );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, LIGHT_LIST.at(0)->RT_Textures[RT_LightDepth]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffQuad);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
		glDisableVertexAttribArray(0);

	}

	void Graphics::ProcessLights()
	{
		num_lights = LIGHT_LIST.size();

		if( num_lights > MAX_LIGHTS )
			num_lights = MAX_LIGHTS;
	}

	void Graphics::BuildShadows()
	{


		/*
		glGenFramebuffers(1, &deferred_fbo);
		glGenRenderbuffers(1, &diffuse_rt);

		// Bind the FBO so that the next operations will be bound to it
		glBindFramebuffer(GL_FRAMEBUFFER, deferred_fbo);

		// Bind the diffuse render target
		glBindRenderbuffer(GL_RENDERBUFFER, diffuse_rt);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, screen_width_i, screen_height_i);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, diffuse_rt);

		// Generate and bind the OGL texture for diffuse
		glGenTextures(1, &rt_textures[RT_LightDiffuse]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_LightDiffuse]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, screen_width_i, screen_height_i, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_LightDiffuse], 0);
		*/



		glGenFramebuffers(1, &shadow_fbo);    			

		for( unsigned i = 0; i < LIGHT_LIST.size(); ++i )
		{
			if( LIGHT_LIST.at(i)->RT_Textures[RT_LightDepth] > 0 )
				continue;

			glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
			glGenTextures(1, &LIGHT_LIST.at(i)->RT_Textures[RT_LightDepth]);
			glBindTexture(GL_TEXTURE_2D, LIGHT_LIST.at(i)->RT_Textures[RT_LightDepth]);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, (int)(screen_width*shadow_size), (int)(screen_width*shadow_size), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	
			glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, LIGHT_LIST.at(i)->RT_Textures[RT_LightDepth], 0);
		}

		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE ) 
		{
			std::cout << "FB error, status: " << status << std::endl;
			return;
		} 

		rt_textures[RT_LightDepth] = LIGHT_LIST.at(0)->RT_Textures[RT_LightDepth];
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
		glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

		glViewport(0,0,screen_width_i,screen_height_i);

		// Draw wireframe geometry
		glDisable( GL_DEPTH_TEST );
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);

		

		for( unsigned i = 0; i < MODEL_LIST.size(); ++i )
		{
			//glBindVertexArray( MODEL_LIST[i]->MeshID );		// Use the cube mesh		
		
			glUseProgram( SHADERS.at(SH_Wireframe).ShaderProgram );		// Activate phong shader

			SHADERS[SH_Wireframe].SetUniform4fv( "view", &viewMat[0][0] );
			SHADERS[SH_Wireframe].SetUniform4fv( "proj", &projMat[0][0] );

			Transform* t = MODEL_LIST[i]->GetTransform();

			if( t == nullptr )
				continue;

			vec3 color = MODEL_LIST[i]->WireColor;

			modelMat = mat4();		

			modelMat = glm::translate( modelMat, t->Position );		

			if( MODEL_LIST[i]->Owner->GetBehavior() != nullptr )
				modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );

			modelMat = glm::rotate( modelMat, t->Rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.z, vec3( 0.0f, 0.0f, 1.0f ) );

			modelMat = glm::scale( modelMat, t->Scale );						

			//glUniform3f( wireColor, color.x, color.y, color.z );
			//glUniformMatrix4fv( wireModel, 1, GL_FALSE, &modelMat[0][0] );		// Pass the locally transformed model matrix to the scene shader	

			SHADERS[SH_Wireframe].SetUniform3f( "wirecolor", color );
			SHADERS[SH_Wireframe].SetUniform4fv( "model", &modelMat[0][0] );
			//glDrawArrays( GL_TRIANGLES, 0, MESH_VERTICES.at(MODEL_LIST[i]->MeshID) );	// Draw first cube

			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, MODEL_LIST.at(i)->VertexBuffer);
				glVertexAttribPointer(
					0,                  // attribute
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					0,                  // stride
					(void*)0            // array buffer offset
				);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MODEL_LIST.at(i)->ElementBuffer);
				glDrawElements(
					GL_TRIANGLES,      // mode
					//indices.size(),    // count
					MODEL_LIST.at(i)->Indices.size(),
					GL_UNSIGNED_SHORT, // type
					(void*)0           // element array buffer offset
				);

			glDisableVertexAttribArray(0);
		}

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);	
	}


	// TODO: Create a second deferred list for "widgets"
	void Graphics::DrawEditor(float dt)
	{		
		glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

		glViewport(0,0,screen_width_i,screen_height_i);

		// Draw wireframe geometry
		glDisable( GL_DEPTH_TEST );			// WE DON'T WANT THIS
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);

		for( unsigned i = 0; i < MODEL_LIST.size(); ++i )
		{
			if( MODEL_LIST[i]->MeshName != "Light.dae" )
				continue;

			//glBindVertexArray( MODEL_LIST[i]->MeshID );		// Use the cube mesh		
		
			glUseProgram( SHADERS.at(SH_Wireframe).ShaderProgram );		// Activate phong shader

			SHADERS[SH_Wireframe].SetUniform4fv( "view", &viewMat[0][0] );
			SHADERS[SH_Wireframe].SetUniform4fv( "proj", &projMat[0][0] );

			Transform* t = MODEL_LIST[i]->GetTransform();

			if( t == nullptr )
				continue;

			vec3 color = MODEL_LIST[i]->WireColor;

			modelMat = mat4();		
			modelMat = glm::translate( modelMat, t->Position );	
			modelMat = glm::rotate( modelMat, t->Rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.z, vec3( 0.0f, 0.0f, 1.0f ) );
			modelMat = glm::scale( modelMat, t->Scale );
			

			SHADERS[SH_Wireframe].SetUniform3f( "wirecolor", color );
			SHADERS[SH_Wireframe].SetUniform4fv( "model", &modelMat[0][0] );		
			
			//glDrawArrays( GL_TRIANGLES, 0, MESH_VERTICES.at(MODEL_LIST[i]->MeshID) );	// Draw first cube

			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, MODEL_LIST.at(i)->VertexBuffer);
				glVertexAttribPointer(
					0,                  // attribute
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					0,                  // stride
					(void*)0            // array buffer offset
				);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MODEL_LIST.at(i)->ElementBuffer);
				glDrawElements(
					GL_TRIANGLES,      // mode
					//indices.size(),    // count
					MODEL_LIST.at(i)->Indices.size(),
					GL_UNSIGNED_SHORT, // type
					(void*)0           // element array buffer offset
				);

			glDisableVertexAttribArray(0);
		}

		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);	
	}

	void Graphics::DrawDebugText(std::string text)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

		if( font->Error() )
			return;

		float offset = font->LineHeight() / screen_height;		// Add this stuff to the function signature

		glUseProgram(0);		// Set current shader to null
		glColor3f(1.0, 1.0, 1.0); 
		//glRasterPos2f(-0.99f, 1-offset*2);
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

		std::string fullpath = "Assets/Textures/" + path;

		image = SOIL_load_image( fullpath.c_str(), &width, &height, 0, SOIL_LOAD_RGB );

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

		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		TEXTURE_LIST[path] = texture;

		return texture;
	}

	void Graphics::LoadMesh(std::string path, GLuint& vbuff, GLuint& uvbuff, GLuint& nbuff, GLuint& ebuff, std::vector<unsigned short>& indices)
	{
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec3> indexed_vertices;
		std::vector<glm::vec2> indexed_uvs;
		std::vector<glm::vec3> indexed_normals;



		if( MESH_LIST.find(path) != MESH_LIST.end() )
		{
			MeshData data = MESH_LIST.at(path);
			vbuff	= data.VertexBuffer;
			uvbuff	= data.UVBuffer;
			nbuff	= data.NormalBuffer;
			ebuff	= data.ElementBuffer;
			indices = data.Indices;
			return;
		}

		std::string fullpath = "Assets/Meshes/" + path;
	
		Assimp::Importer importer;
		scene = importer.ReadFile( fullpath, aiProcess_Triangulate );

		if( !scene)
		{
			std::cout << "Mesh didn't load: ";
			printf("%s\n", importer.GetErrorString());
			return;
		}

		std::cout << "Import of mesh succeeded: <" << path.c_str() << ">" << std::endl;

		// For each mesh
		for( unsigned k = 0; k < scene->mNumMeshes; ++k )
		{
			// For each faces
			for( unsigned i = 0; i < scene->mMeshes[k]->mNumFaces; ++i )
			{
				aiFace face = scene->mMeshes[k]->mFaces[i];

				// For each vert index
				for( unsigned j = 0; j < face.mNumIndices; ++j )
				{
					if( scene->mMeshes[k]->HasPositions() )
					{
						aiVector3D temp = scene->mMeshes[k]->mVertices[face.mIndices[j]];
						vertices.push_back( vec3(temp.x, temp.y, temp.z) );
					}

					if( scene->mMeshes[k]->HasNormals() )
					{
						//vertices[index+3]	= scene->mMeshes[k]->mNormals[face.mIndices[j]].x;
						aiVector3D temp = scene->mMeshes[k]->mNormals[face.mIndices[j]] ;
						normals.push_back( vec3(temp.x, temp.y, temp.z) );
					}

					if( scene->mMeshes[k]->HasTextureCoords(0) )
					{
						aiVector3D temp = scene->mMeshes[k]->mTextureCoords[0][face.mIndices[j]];
						uvs.push_back( vec2( temp.x, temp.y ) );
					}	
				}
			}
		}		

		ProcessVertexData(vertices, uvs, normals, indices, indexed_vertices, indexed_uvs, indexed_normals);
		
		// VBO vertices
		glGenBuffers(1, &vbuff);
		glBindBuffer(GL_ARRAY_BUFFER, vbuff);
		glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(glm::vec3), &indexed_vertices[0], GL_STATIC_DRAW);

		// VBO tex coords
		glGenBuffers(1, &uvbuff);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuff);
		glBufferData(GL_ARRAY_BUFFER, indexed_uvs.size() * sizeof(glm::vec2), &indexed_uvs[0], GL_STATIC_DRAW);

		// VBO normals
		glGenBuffers(1, &nbuff);
		glBindBuffer(GL_ARRAY_BUFFER, nbuff);
		glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);

		// Generate a buffer for the indices as well
		glGenBuffers(1, &ebuff);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuff);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned short), &indices[0], GL_STATIC_DRAW);

		MeshData meshdata;
		meshdata.VertexBuffer	= vbuff;
		meshdata.UVBuffer		= uvbuff;
		meshdata.NormalBuffer	= nbuff;
		meshdata.ElementBuffer	= ebuff;
		meshdata.Indices		= indices;
		MESH_LIST[path] = meshdata;
	}

	void Graphics::ProcessVertexData(
		std::vector<glm::vec3> & in_vertices,
		std::vector<glm::vec2> & in_uvs,
		std::vector<glm::vec3> & in_normals,

		std::vector<unsigned short> & out_indices,
		std::vector<glm::vec3> & out_vertices,
		std::vector<glm::vec2> & out_uvs,
		std::vector<glm::vec3> & out_normals )
	{
		std::map<Attrib,unsigned short> VertexToOutIndex;

		// For each input vertex
		for ( unsigned int i=0; i<in_vertices.size(); i++ )
		{

			Attrib packed;

			if( in_vertices.size() > 0 )
				packed.position = in_vertices.at(i);

			if( in_uvs.size() > 0 )
				packed.uv = in_uvs.at(i);

			if( in_normals.size() > 0 )
				packed.normal = in_normals.at(i);
		

			// Try to find a similar vertex in out_XXXX
			unsigned short index;
			bool found = CompareVertex( packed, VertexToOutIndex, index);

			if ( found )
			{ // A similar vertex is already in the VBO, use it instead !
				out_indices.push_back( index );
			}
			else
			{ // If not, it needs to be added in the output data.
				if( in_vertices.size() > 0 )
					out_vertices.push_back( in_vertices[i]);

				if( in_uvs.size() > 0 )
					out_uvs     .push_back( in_uvs[i]);

				if( in_normals.size() > 0 )
					out_normals .push_back( in_normals[i]);

				unsigned short newindex = (unsigned short)out_vertices.size() - 1;
				out_indices .push_back( newindex );
				VertexToOutIndex[ packed ] = newindex;
			}
		}
	}

	bool Graphics::CompareVertex( Attrib & packed, std::map<Attrib,unsigned short> & vert_out, unsigned short & result )
	{
		std::map<Attrib,unsigned short>::iterator it = vert_out.find(packed);

		if ( it == vert_out.end() )
		{
			return false;
		}
		else
		{
			result = it->second;
			return true;
		}
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

	void Graphics::AddComponent(Model* m)
	{
		MODEL_LIST.push_back(m);
	}

	void Graphics::AddComponent(Light* l)
	{
		LIGHT_LIST.push_back(l);
	}

	void Graphics::RemoveComponent(Model* m)
	{
		auto it_s = MODEL_LIST.begin();
		auto it_e = MODEL_LIST.end();
		for( ; it_s != it_e; ++it_s )
		{
			if( *it_s == m )
			{
				MODEL_LIST.erase(it_s);
				return;
			}
		}
	}

	void Graphics::RemoveComponent(Light* l)
	{
		auto it_s = LIGHT_LIST.begin();
		auto it_e = LIGHT_LIST.end();
		for( ; it_s != it_e; ++it_s )
		{
			if( *it_s == l )
			{
				LIGHT_LIST.erase(it_s);
				return;
			}
		}
	}

	GLint Graphics::CreateShaderProgram(std::string _vertSource, std::string _fragSource)
	{
		GLuint vertShader = LoadShader(_vertSource, GL_VERTEX_SHADER);
		GLuint fragShader = LoadShader(_fragSource, GL_FRAGMENT_SHADER);

		GLuint shaderProgram = glCreateProgram();
		glAttachShader( shaderProgram, vertShader );
		glAttachShader( shaderProgram, fragShader );
		glBindFragDataLocation( shaderProgram, 0, "outColor" );
		glLinkProgram( shaderProgram );	

		SHADERS.push_back( Shader(shaderProgram) );

		return shaderProgram;
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

	void Graphics::SortModels(float dt)
	{
		std::sort( MODEL_LIST.begin(), MODEL_LIST.end(), ZSorting(*this) );
	}

	void Graphics::InitializeCamera()
	{
		cam_pos		= vec3(0.0f, 2.0f, 28.0f);
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

		viewMat = glm::lookAt( cam_pos, cam_pos + cam_look, cam_up );
		projMat = glm::perspective( 45.0f, screen_width / screen_height, 0.1f, 1000.0f );
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

	void Graphics::NextRT()
	{
		++current_rt;

		if( current_rt >= RT_TOTAL )
			current_rt = 0;

		screen_tex = rt_textures[current_rt];
	}

	void Graphics::PrevRT()
	{
		--current_rt;

		if( current_rt < 0 )
			current_rt = RT_TOTAL-1;

		screen_tex = rt_textures[current_rt];
	}

	void Graphics::MouseSelectEntity(float x, float y, bool pressed)
	{
		if( pressed == false )
		{
			SelectedEntity = nullptr;
			return;
		}

		for( unsigned i = 0; i < MODEL_LIST.size(); ++i )
		{
			vec3 position   = MODEL_LIST[i]->GetTransform()->Position;

			vec4 pos;
			pos = viewMat * vec4(position,1.0f);
			pos = projMat * pos;

			pos.x /= pos.z;
			pos.y /= pos.z;

			pos.x = (pos.x + 1.0f) * screen_width / 2.0f;
			pos.y = (pos.y + 1.0f) * screen_height / 2.0f;
			pos.y = screen_height - pos.y;

			if( pos.x < 0 || pos.x > screen_width || pos.y < 0 || pos.y > screen_height )
				continue;

			float dist		= glm::distance(position, cam_pos);
			float radius	= 100.0f;//1.0f / dist * 1000.0f;

			vec2 mouse_xy = vec2(x,y);
			vec2 obj_xy   = vec2(pos.x,pos.y);

			if( glm::distance(mouse_xy,obj_xy) < radius )
			{
				SelectedEntity = MODEL_LIST[i]->Owner;
				return;
			}
		}
	}

	void Graphics::UpdateLightPos(float x = 0, float y = 0)
	{
		if( SelectedEntity == nullptr )
			return;

		vec3 move(0,0,0); 		

		float move_speed = 0.05f*x;
		move += vec3(move_speed,0,0);

		move_speed = 0.05f*y;	

		if( Input::GetInstance()->GetKey(SDLK_RSHIFT) == true || Input::GetInstance()->GetKey(SDLK_LSHIFT) == true )
		{
			move -= vec3(0,move_speed,0);

			SelectedEntity->GetTransform()->Position += move;
		}
		else
		{
			move += vec3(0,0,move_speed);		

			quat turn = cam_rot;
			turn.x = 0;
			turn.z = 0;
			float mag = 1.0f / sqrt(turn.w*turn.w + turn.y*turn.y);
			turn.w *= mag;
			turn.y *= mag;

			SelectedEntity->GetTransform()->Position += turn * move;
		}		
	}

	Graphics::~Graphics()
	{
		for( Shader s : SHADERS )
			glDeleteProgram(s.ShaderProgram);

		glDeleteBuffers( 1, &buffCube );
		glDeleteBuffers( 1, &buffQuad );

		glDeleteVertexArrays( 1, &meshCube );
		glDeleteVertexArrays( 1, &meshQuad );

		glDeleteFramebuffers( 1, &screen_fbo );
		glDeleteFramebuffers( 1, &shadow_fbo );

		glDeleteTextures(RT_TOTAL, rt_textures);

		glDeleteFramebuffers(1, &deferred_fbo);
		glDeleteRenderbuffers(1, &diffuse_rt);
		glDeleteRenderbuffers(1, &positions_rt);
		glDeleteRenderbuffers(1, &normals_rt);
		glDeleteRenderbuffers(1, &depth_buffer);
	}
}