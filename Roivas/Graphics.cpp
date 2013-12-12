#include "CommonLibs.h"
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
		current_lighting(SH_Lighting),
		current_fog(0),
		wireframe_enabled(false),
		normal_mapping_enabled(true),
		bloom_enabled(true),
		volumelight_enabled(true),
		fog_density(1.0f),
		shadow_size(2.0f),
		shadow_smooth(1.0f),
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
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

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
		CreateShaderProgram("Assets/Shaders/Ambient.vert",			"Assets/Shaders/Ambient.frag");			// SH_Ambient
		CreateShaderProgram("Assets/Shaders/Deferred.vert",			"Assets/Shaders/Deferred.frag");		// SH_Deferred
		CreateShaderProgram("Assets/Shaders/Screen.vert",			"Assets/Shaders/Screen.frag");			// SH_Screen
		CreateShaderProgram("Assets/Shaders/Hud.vert",				"Assets/Shaders/Hud.frag");				// SH_Hud
		CreateShaderProgram("Assets/Shaders/Wireframe.vert",		"Assets/Shaders/Wireframe.frag");		// SH_Wireframe
		CreateShaderProgram("Assets/Shaders/ShadowTex.vert",		"Assets/Shaders/ShadowTex.frag");		// SH_ShadowTex
		CreateShaderProgram("Assets/Shaders/Lighting.vert",			"Assets/Shaders/Lighting.frag");		// SH_Lighting
		CreateShaderProgram("Assets/Shaders/LightingWithSSM.vert",	"Assets/Shaders/LightingWithSSM.frag");	// SH_LightingSSM
		CreateShaderProgram("Assets/Shaders/LightingWithESM.vert",	"Assets/Shaders/LightingWithESM.frag");	// SH_LightingESM
		CreateShaderProgram("Assets/Shaders/LightingWithDSM.vert",	"Assets/Shaders/LightingWithDSM.frag");	// SH_LightingDSM
		CreateShaderProgram("Assets/Shaders/BlurDSM.vert",			"Assets/Shaders/BlurDSM.frag");			// SH_BlurDSM
		CreateShaderProgram("Assets/Shaders/LogBlur.vert",			"Assets/Shaders/LogBlur.frag");			// SH_LogBlur
		CreateShaderProgram("Assets/Shaders/GaussBlur.vert",		"Assets/Shaders/GaussBlur.frag");		// SH_GaussBlur
		CreateShaderProgram("Assets/Shaders/DepthFog.vert",			"Assets/Shaders/DepthFog.frag");		// SH_DepthFog
		CreateShaderProgram("Assets/Shaders/FogRadiance.vert",		"Assets/Shaders/FogRadiance.frag");		// SH_FogRadiance
		CreateShaderProgram("Assets/Shaders/FogBlur.vert",			"Assets/Shaders/FogBlur.frag");			// SH_FogBlur
		CreateShaderProgram("Assets/Shaders/Blend.vert",			"Assets/Shaders/Blend.frag");			// SH_Blend
		CreateShaderProgram("Assets/Shaders/Skybox.vert",			"Assets/Shaders/Skybox.frag");			// SH_Skybox
		CreateShaderProgram("Assets/Shaders/VolumeLight.vert",		"Assets/Shaders/VolumeLight.frag");		// SH_VolumeLight
		CreateShaderProgram("Assets/Shaders/VolumeSampling.vert",	"Assets/Shaders/VolumeSampling.frag");	// SH_VolumeSampling

		//shadow_size = 1;

		// Preload meshes and textures
		PreloadAssets();

		LoadSkybox("sky_ft.jpg","sky_bk.jpg","sky_lf.jpg","sky_rt.jpg","sky_tp.jpg","sky_bt.jpg");

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
		DrawPP(dt);

		// Debug drawing
		//DrawEditor(dt);

		// HUD and other 2D drawing
		//Draw2D(dt);	

		// Draw screen texture to screen
		ScreenPass(dt);	

		if( wireframe_enabled == true )
		{
			DrawWireframe(dt);
			DrawLightShape(dt);
		}		

		// Draw framerate
		//DrawDebugText(framerate);	

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

		static const GLfloat g_skybox_vertex_buffer_data[] = {
		  -10.0f,  10.0f, -10.0f,
		  -10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
  
		  -10.0f, -10.0f,  10.0f,
		  -10.0f, -10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f, -10.0f,
		  -10.0f,  10.0f,  10.0f,
		  -10.0f, -10.0f,  10.0f,
  
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
   
		  -10.0f, -10.0f,  10.0f,
		  -10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f, -10.0f,  10.0f,
		  -10.0f, -10.0f,  10.0f,
  
		  -10.0f,  10.0f, -10.0f,
		   10.0f,  10.0f, -10.0f,
		   10.0f,  10.0f,  10.0f,
		   10.0f,  10.0f,  10.0f,
		  -10.0f,  10.0f,  10.0f,
		  -10.0f,  10.0f, -10.0f,
  
		  -10.0f, -10.0f, -10.0f,
		  -10.0f, -10.0f,  10.0f,
		   10.0f, -10.0f, -10.0f,
		   10.0f, -10.0f, -10.0f,
		  -10.0f, -10.0f,  10.0f,
		   10.0f, -10.0f,  10.0f
		};

		glGenVertexArrays(1, &meshSkybox);
		glBindVertexArray(meshSkybox);

		glGenBuffers(1, &buffSkybox);
		glBindBuffer(GL_ARRAY_BUFFER, buffSkybox);
		glBufferData(GL_ARRAY_BUFFER, 3*36*sizeof(float), &g_skybox_vertex_buffer_data, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, buffSkybox);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte*)NULL);






		glGenVertexArrays(1, &meshQuad);
		glBindVertexArray(meshQuad);

		glGenBuffers( 1, &buffQuad );
		glBindBuffer(GL_ARRAY_BUFFER, buffQuad);
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);

		glGenFramebuffers( 1, &screen_fbo );
		glBindFramebuffer( GL_FRAMEBUFFER, screen_fbo );

		glGenTextures(1, &screen_tex);
		glBindTexture( GL_TEXTURE_2D, screen_tex );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, screen_width_i, screen_height_i, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL );
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screen_tex, 0 );

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE ) 
		{
			std::cout << "FB error, status: " << status << std::endl;
			return;
		} 


		glGenFramebuffers(1, &deferred_fbo);

		glGenRenderbuffers(1, &depth_buffer);

		// Bind the FBO so that the next operations will be bound to it
		glBindFramebuffer(GL_FRAMEBUFFER, deferred_fbo);

		// Bind the depth buffer
		glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, screen_width_i, screen_height_i);
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

		// Generate and bind the OGL texture for specular mapping
		glGenTextures(1, &rt_textures[RT_SceneSpecular]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneSpecular]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, rt_textures[RT_SceneSpecular], 0);

		// Generate and bind the OGL texture for depth
		glGenTextures(1, &rt_textures[RT_SceneDepth]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneDepth]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, rt_textures[RT_SceneDepth], 0);	

		// Generate and bind the OGL texture for depth
		glGenTextures(1, &rt_textures[RT_SceneGlow]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneGlow]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, rt_textures[RT_SceneGlow], 0);	


		// Check if all worked fine and unbind the FBO
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "FB error for deferred, status: " << status << std::endl;
		}



		// Lighting and shadowing fbo
		glGenFramebuffers(1, &lighting_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, lighting_fbo);

		// Generate and bind the OGL texture for diffuse
		glGenTextures(1, &rt_textures[RT_SceneLighting]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneLighting]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneLighting], 0);

		// Generate and bind the OGL texture for positions
		glGenTextures(1, &rt_textures[RT_SceneShadows]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneShadows]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rt_textures[RT_SceneShadows], 0);

		// Generate and bind the OGL texture for depth
		glGenTextures(1, &rt_textures[RT_SceneBloom]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneBloom]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, rt_textures[RT_SceneBloom], 0);	

		// Check if all worked fine and unbind the FBO
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "FB error for deferred, status: " << status << std::endl;
		}



		glGenFramebuffers( 1, &blur_fbo );
		glBindFramebuffer( GL_FRAMEBUFFER, blur_fbo );




		glGenFramebuffers( 1, &fog_fbo );
		glBindFramebuffer( GL_FRAMEBUFFER, fog_fbo );

		// Generate and bind the OGL texture for diffuse
		glGenTextures(1, &rt_textures[RT_SceneAttRadiance]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneAttRadiance]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneAttRadiance], 0);

		// Generate and bind the OGL texture for positions
		glGenTextures(1, &rt_textures[RT_SceneSctRadiance]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneSctRadiance]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rt_textures[RT_SceneSctRadiance], 0);

		// Check if all worked fine and unbind the FBO
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "FB error for deferred, status: " << status << std::endl;
		}



		glGenFramebuffers( 1, &volume_fbo );
		glBindFramebuffer( GL_FRAMEBUFFER, volume_fbo );

		// Generate and bind the OGL texture for positions
		glGenTextures(1, &rt_textures[RT_VolumeLight]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_VolumeLight]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_VolumeLight], 0);

		// Generate and bind the OGL texture for positions
		glGenTextures(1, &rt_textures[RT_CoordTex]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_CoordTex]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rt_textures[RT_CoordTex], 0);

		// Generate and bind the OGL texture for positions
		glGenTextures(1, &rt_textures[RT_SceneVolumeLight]);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneVolumeLight]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screen_width_i, screen_height_i, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// Attach the texture to the FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneVolumeLight], 0);

		

		// Check if all worked fine and unbind the FBO
		status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "FB error for deferred, status: " << status << std::endl;
		}


		


		glGenFramebuffers( 1, &blend_fbo );
		glBindFramebuffer( GL_FRAMEBUFFER, blend_fbo );




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

		if( current_lighting > SH_Lighting )
			ShadowPass(dt);		

		LightingPass(dt);	

		if( current_lighting == SH_LightingDSM )
		{
			BlurDSM();
			Blend(rt_textures[RT_SceneLighting], rt_textures[RT_SceneShadows], rt_textures[RT_SceneLighting], 1, true);
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	}

	void Graphics::DrawSkybox(float dt)
	{
		glDepthRangef(0.999f, 1.0f);	
		glDisable(GL_CULL_FACE);

		glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneDiffuse], 0);

		glDepthMask(GL_FALSE);

		glUseProgram(SHADERS[SH_Skybox].ShaderProgram);



		modelMat = mat4();
		modelMat = glm::translate( modelMat, cam_pos );
		modelMat = glm::scale( modelMat, vec3(10.0f,10.0f,10.0f) );
		


		SHADERS[SH_Skybox].SetUniform4fv( "V", &viewMat[0][0]);
		SHADERS[SH_Skybox].SetUniform4fv( "P", &projMat[0][0]);
		SHADERS[SH_Skybox].SetUniform4fv( "M", &modelMat[0][0]);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, sky_tex);
	
		glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, buffSkybox);
			glVertexAttribPointer(
				0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
				3,                  // size
				GL_FLOAT,           // type
				GL_FALSE,           // normalized?
				0,                  // stride
				(void*)0            // array buffer offset
			);

			glDrawArrays(GL_TRIANGLES, 0, 36); // 2*3 indices starting at 0 -> 2 triangles

		glDisableVertexAttribArray(0);


		glEnable(GL_CULL_FACE);
		glDepthRangef(0.0f, 1.0f);

		glDepthMask(GL_TRUE);
	}

	void Graphics::GeometryPass(float dt)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, deferred_fbo);
		glViewport(0,0,screen_width_i,screen_height_i); // Render on the whole framebuffer, complete from the lower left corner to the upper right
		glDepthRangef(0.0f, 0.999f);

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glDisable( GL_BLEND );
		glCullFace(GL_BACK);

		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		DrawSkybox(dt);	

		glBindFramebuffer(GL_FRAMEBUFFER, deferred_fbo);

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, 
			GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };
		glDrawBuffers(6, buffers);
		
		glUseProgram( SHADERS.at(SH_Deferred).ShaderProgram );


		for( unsigned i = 0; i < MODEL_LIST.size(); ++i )
		{
			Transform* t = MODEL_LIST[i]->GetTransform();

			if( t == nullptr )
				continue;

			modelMat = mat4();			

			modelMat = glm::translate( modelMat, t->Position );

			modelMat = glm::rotate( modelMat, t->Rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.z, vec3( 0.0f, 0.0f, 1.0f ) );

			if( MODEL_LIST[i]->Owner->GetBehavior() != nullptr )
				modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );	

			modelMat = glm::scale( modelMat, t->Scale );
			
			SHADERS[SH_Deferred].SetUniform1i( "normal_mapping", normal_mapping_enabled );
			SHADERS[SH_Deferred].SetUniform4fv( "M", &modelMat[0][0]);
			SHADERS[SH_Deferred].SetUniform4fv( "V", &viewMat[0][0]);
			SHADERS[SH_Deferred].SetUniform4fv( "P", &projMat[0][0] );

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, MODEL_LIST.at(i)->DiffuseID);
			//glBindTexture(GL_TEXTURE_2D, sky_tex);
			SHADERS[SH_Deferred].SetUniform1i( "tex_sampler", 0 );

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, MODEL_LIST.at(i)->NormalID);
			SHADERS[SH_Deferred].SetUniform1i( "norm_sampler", 1 );

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, MODEL_LIST.at(i)->SpecID);
			SHADERS[SH_Deferred].SetUniform1i( "spec_sampler", 2 );

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, MODEL_LIST.at(i)->GlowID);
			SHADERS[SH_Deferred].SetUniform1i( "glow_sampler", 3 );


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

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
	}

	void Graphics::ShadowPass(float dt)
	{		
		glBindFramebuffer( GL_FRAMEBUFFER, shadow_fbo );

		glViewport(0, 0, (int)(screen_width*shadow_size), (int)(screen_width*shadow_size));
		
		glEnable( GL_DEPTH_TEST );
		glEnable(GL_CULL_FACE);

		if( current_lighting == SH_LightingESM )
			glCullFace(GL_BACK);
		else
			glCullFace(GL_FRONT); 	

		glUseProgram( SHADERS.at(SH_ShadowTex).ShaderProgram );		


		for( unsigned j = 0; j < num_lights; ++j )
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			vec3 pos = LIGHT_LIST.at(j)->GetTransform()->Position;

			if( LIGHT_LIST.at(j)->Type == LT_DirectionLight )
			{
				float dim = 30.0f;
				depthProjMat[j] = glm::ortho<float>(-dim,dim,-dim,dim,-20.0f,36.0f);

				vec3 direction = LIGHT_LIST.at(j)->Direction;

				if( LIGHT_LIST.at(j)->Orbital )
					direction = glm::normalize(LIGHT_LIST.at(j)->GetTransform()->Position - cam_pos);
	
				depthViewMat[j] = glm::lookAt(direction, vec3(0,0,0), vec3(0,1,0));
			}
			else if( LIGHT_LIST.at(j)->Type == LT_SpotLight )
			{
				depthProjMat[j] = glm::perspective<float>(45.0f, 1.0f, 0.95f, 50.0f);
				depthViewMat[j] = glm::lookAt(pos, pos-LIGHT_LIST.at(j)->Direction, glm::vec3(0,1,0));
			}
			else if( LIGHT_LIST.at(j)->Type == LT_PointLight )
			{
				//depthProjMat[j] = glm::perspective<float>(90.0f, 1.0f, 0.95f, 50.0f);
				//depthViewMat[j] = glm::lookAt(pos, pos-LIGHT_LIST.at(j)->Direction, glm::vec3(0,1,0));
				depthProjMat[j] = glm::perspective<float>(90.0f, 1.0f, 0.95f, 50.0f);
				depthViewMat[j] = glm::lookAt(pos, pos-LIGHT_LIST.at(j)->Direction, glm::vec3(0,1,0));


			}

			for( unsigned i = 0; i < MODEL_LIST.size(); ++i )
			{
				if( MODEL_LIST[i]->GetLight() )
					continue;

				Transform* t = MODEL_LIST[i]->GetTransform();

				if( t == nullptr )
					continue;

				modelMat = mat4();			

				modelMat = glm::translate( modelMat, t->Position );

				modelMat = glm::rotate( modelMat, t->Rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );
				modelMat = glm::rotate( modelMat, t->Rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
				modelMat = glm::rotate( modelMat, t->Rotation.z, vec3( 0.0f, 0.0f, 1.0f ) );

				if( MODEL_LIST[i]->Owner->GetBehavior() != nullptr )
					modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );	

				modelMat = glm::scale( modelMat, t->Scale );

				depthMVP = depthProjMat[j] * depthViewMat[j] * modelMat;

				SHADERS[SH_ShadowTex].SetUniform4fv( "depthMVP", &depthMVP[0][0] );
				SHADERS[SH_ShadowTex].SetUniform4fv( "M", &modelMat[0][0] );
				SHADERS[SH_ShadowTex].SetUniform4fv( "V", &depthViewMat[j][0][0] );

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

		if( current_lighting == SH_LightingESM )
			BlurESM();

		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	void Graphics::LightingPass(float dt)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, lighting_fbo);

		glViewport(0,0,screen_width_i,screen_height_i); // Render on the whole framebuffer, complete from the lower left corner to the upper right

		glDisable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glCullFace(GL_BACK);


		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, buffers);

		glUseProgram( SHADERS.at(SH_Ambient).ShaderProgram );


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneDiffuse]);
		SHADERS[SH_Ambient].SetUniform1i( "tDiffuse", 0 );

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneDepth]);
		SHADERS[SH_Ambient].SetUniform1i( "tDepth", 1 );


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
	


		glCullFace( GL_FRONT );
		glEnable( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ONE );


		
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
			
			Transform* t = light->GetTransform();

			if( t == nullptr )
				continue;

			modelMat = mat4();	

			vec3 direction = light->Direction;

			if( light->Orbital )
				direction = glm::normalize(light->GetTransform()->Position - cam_pos);

			SHADERS[current_lighting].SetUniform1f( "shadowsmooth", shadow_smooth );
			SHADERS[current_lighting].SetUniform3f( "lightpos",		light->GetTransform()->Position );
			SHADERS[current_lighting].SetUniform3f( "lightcolor",	light->Color );
			SHADERS[current_lighting].SetUniform3f( "lightdir",		direction );
			SHADERS[current_lighting].SetUniform1f( "lightradius",	light->Radius);
			SHADERS[current_lighting].SetUniform1f( "lightcone",	light->Cone );
			SHADERS[current_lighting].SetUniform1i( "lighttype",	light->Type );
			SHADERS[current_lighting].SetUniform2f( "screensize",	vec2(screen_width,screen_height) );

			SHADERS[current_lighting].SetUniform4fv( "Bias", &biasMatrix[0][0]);
			SHADERS[current_lighting].SetUniform4fv( "DepthProj", &depthProjMat[j][0][0]);
			SHADERS[current_lighting].SetUniform4fv( "DepthView", &depthViewMat[j][0][0]);


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
			glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneSpecular]);
			SHADERS[current_lighting].SetUniform1i( "tSpecular", 3 );

			glActiveTexture(GL_TEXTURE5);
			if( current_lighting == SH_LightingESM )
				glBindTexture(GL_TEXTURE_2D, LIGHT_LIST.at(j)->RT_Textures[RT_ExpLightBlur]);
			else
				glBindTexture(GL_TEXTURE_2D, LIGHT_LIST.at(j)->RT_Textures[RT_LightDepth]);
			SHADERS[current_lighting].SetUniform1i( "tShadow", 5 );	

			

			
			if( light->Type == LT_PointLight )
			{
				glCullFace( GL_FRONT );

				modelMat = glm::translate( modelMat, t->Position );

				modelMat = glm::rotate( modelMat, t->Rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );
				modelMat = glm::rotate( modelMat, t->Rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
				modelMat = glm::rotate( modelMat, t->Rotation.z, vec3( 0.0f, 0.0f, 1.0f ) );

				modelMat = glm::scale( modelMat, t->Scale*t->GetLight()->Radius*10.0f );

				SHADERS[current_lighting].SetUniform4fv( "M", &modelMat[0][0]);
				SHADERS[current_lighting].SetUniform4fv( "V", &viewMat[0][0]);
				SHADERS[current_lighting].SetUniform4fv( "P", &projMat[0][0] );

				glEnableVertexAttribArray(0);

					glBindBuffer(GL_ARRAY_BUFFER, light->GetModel()->VertexBuffer);
						glVertexAttribPointer(
							0,  // The attribute we want to configure
							3,                  // size
							GL_FLOAT,           // type
							GL_FALSE,           // normalized?
							0,                  // stride
							(void*)0            // array buffer offset
						);

					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, light->GetModel()->ElementBuffer);
						glDrawElements(
							GL_TRIANGLES,      // mode
							light->GetModel()->Indices.size(),
							GL_UNSIGNED_SHORT, // type
							(void*)0           // element array buffer offset
						);

				glDisableVertexAttribArray(0);

			}
			else
			{
				
				glCullFace(GL_BACK);

				modelMat = mat4();

				SHADERS[current_lighting].SetUniform4fv( "M", &modelMat[0][0] );
				SHADERS[current_lighting].SetUniform4fv( "V", &viewMat[0][0]);
				SHADERS[current_lighting].SetUniform4fv( "P", &projMat[0][0] );

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

		}
		

		glDisable(GL_BLEND);

		glEnable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glCullFace( GL_BACK );

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Graphics::ScreenPass(float dt)
	{
		glDisable( GL_DEPTH_TEST );	

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



		if( screen_tex == rt_textures[RT_SceneLighting] && NULL )
		{
			glViewport(screen_width_i/2+screen_width_i/4,screen_height_i/2+screen_height_i/4,screen_width_i/4,screen_height_i/4);
			glCullFace(GL_NONE);

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
		
		glEnable( GL_DEPTH_TEST );	
	}

	void Graphics::Blend(GLint in1, GLint in2, GLint out, int amount = 1, bool mult = true)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blend_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, out, 0);

		glViewport(0,0,screen_width_i,screen_height_i); 

		// Use our shader
		glUseProgram( SHADERS.at(SH_Blend).ShaderProgram );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, in1);
		SHADERS[SH_Blend].SetUniform1i( "tSampleA", 0 );

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, in2);
		SHADERS[SH_Blend].SetUniform1i( "tSampleB", 1 );

		SHADERS[SH_Blend].SetUniform1i( "amount", amount );
		SHADERS[SH_Blend].SetUniform1i( "mult", mult );


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
	}

	void Graphics::Blur(GLint tex, float w, float h, float numpixels)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

		glViewport(0,0,screen_width_i,screen_height_i); 

		// Use our shader
		glUseProgram( SHADERS.at(SH_GaussBlur).ShaderProgram );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		SHADERS[SH_GaussBlur].SetUniform1i( "tBlur", 0 );

		SHADERS[SH_GaussBlur].SetUniform2f( "blurSize", vec2(w*1.0f/screen_width, h*1.0f/screen_height));
		SHADERS[SH_GaussBlur].SetUniform1f( "blurAmount", 5.0f);
		SHADERS[SH_GaussBlur].SetUniform1f( "blurPixels", numpixels);


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
	}

	void Graphics::Fog(GLint radiance, GLint depth)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fog_fbo);
		glViewport(0,0,screen_width_i,screen_height_i); 

		if( current_fog == 1 )
		{
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, radiance, 0);

			glUseProgram( SHADERS.at(SH_DepthFog).ShaderProgram );

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, radiance);
			SHADERS[SH_DepthFog].SetUniform1i( "tInRadiance", 0 );

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, depth);
			SHADERS[SH_DepthFog].SetUniform1i( "tDepth", 1 );

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

			return;
		}

		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneAttRadiance], 0);

		GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, buffers);

		// Use our shader
		glUseProgram( SHADERS.at(SH_FogRadiance).ShaderProgram );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, radiance);
		SHADERS[SH_FogRadiance].SetUniform1i( "tInRadiance", 0 );

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depth);
		SHADERS[SH_FogRadiance].SetUniform1i( "tDepth", 1 );

		SHADERS[SH_FogRadiance].SetUniform1f( "fog_density", fog_density );

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

		FogBlur();
		Blend(rt_textures[RT_SceneSctRadiance], rt_textures[RT_SceneAttRadiance], rt_textures[RT_SceneSctRadiance], 2, false);
		FogBlur();
		//Blend(rt_textures[RT_SceneSctRadiance], rt_textures[RT_SceneLighting], rt_textures[RT_SceneLighting], 1, false);


		glBindFramebuffer(GL_FRAMEBUFFER, screen_fbo);

		// Use our shader
		glUseProgram( SHADERS.at(SH_Screen).ShaderProgram );
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneLighting], 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneSctRadiance]);

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
		
	}

	void Graphics::FogBlur()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fog_fbo);
		glViewport(0,0,screen_width_i,screen_height_i);

		// Use our shader
		glUseProgram( SHADERS.at(SH_FogBlur).ShaderProgram );

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneSctRadiance], 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneLighting], 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneSctRadiance]);
		//glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneLighting]);
		SHADERS[SH_FogBlur].SetUniform1i( "tBlur", 0 );

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneDepth]);
		SHADERS[SH_FogBlur].SetUniform1i( "tDepth", 1 );

		SHADERS[SH_FogBlur].SetUniform2f( "blurSize", vec2(1.0f/screen_width, 0));
		SHADERS[SH_FogBlur].SetUniform1f( "blurAmount", 6.0f);
		SHADERS[SH_FogBlur].SetUniform1f( "blurPixels", 4.0f);
		SHADERS[SH_FogBlur].SetUniform1f( "fog_density", fog_density);

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

		SHADERS[SH_FogBlur].SetUniform2f( "blurSize", vec2(0, 1.0f/screen_height));

		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

		glDisableVertexAttribArray(0);

	}

	void Graphics::BlurESM()
	{
		glDisable(GL_BLEND);
		glDisable( GL_DEPTH_TEST );
		glEnable( GL_CULL_FACE );
		glCullFace(GL_BACK);


		// bind blur FBO
		glBindFramebuffer(GL_FRAMEBUFFER, exp_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_ExpLightBlur], 0);
		// save our old viewport so we can restore it
		//glPushAttrib( GL_VIEWPORT_BIT );
		glViewport(0, 0, (int)(screen_width*shadow_size), (int)(screen_width*shadow_size));
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 

		glUseProgram( SHADERS.at(SH_LogBlur).ShaderProgram );

		glActiveTexture( GL_TEXTURE0 );
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_ExpLightDepth]);

		
		SHADERS[SH_LogBlur].SetUniform1i( "tBlur", 0 );


		glEnableVertexAttribArray(0);

		SHADERS[SH_LogBlur].SetUniform2f( "blurSize", vec2(1.0f/screen_width, 0));
		
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

		SHADERS[SH_LogBlur].SetUniform2f( "blurSize", vec2(0, 1.0f/screen_height));

		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_ExpLightBlur]);

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

		glEnable(GL_BLEND);
		glEnable( GL_DEPTH_TEST );


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Graphics::BlurDSM()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blur_fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneShadows], 0);

		glViewport(0,0,screen_width_i,screen_height_i); 

		// Use our shader
		glUseProgram( SHADERS.at(SH_BlurDSM).ShaderProgram );

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneShadows]);
		SHADERS[SH_BlurDSM].SetUniform1i( "tBlur", 0 );

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneNormals]);
		SHADERS[SH_BlurDSM].SetUniform1i( "tNormals", 1 );



		SHADERS[SH_BlurDSM].SetUniform2f( "blurSize", vec2(1.2f/screen_width, 0));
		SHADERS[SH_BlurDSM].SetUniform1f( "blurAmount", 6.0f);
		SHADERS[SH_BlurDSM].SetUniform1f( "blurPixels", 6.0f);


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

		SHADERS[SH_BlurDSM].SetUniform2f( "blurSize", vec2(0, 1.2f/screen_height));
		glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles

		glDisableVertexAttribArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Graphics::Glow()
	{
		Blur(rt_textures[RT_SceneGlow],1,0,4);
		Blur(rt_textures[RT_SceneGlow],0,1,4);
		Blend(rt_textures[RT_SceneGlow], rt_textures[RT_SceneGlow],rt_textures[RT_SceneGlow], 1, false);
		Blend(rt_textures[RT_SceneLighting], rt_textures[RT_SceneGlow],rt_textures[RT_SceneLighting], 1, false);
	}

	void Graphics::Bloom()
	{
		//Blend(rt_textures[RT_SceneBloom], rt_textures[RT_SceneBloom],rt_textures[RT_SceneBloom], 0.5f, false);
		Blur(rt_textures[RT_SceneBloom],1,0,4);
		Blur(rt_textures[RT_SceneBloom],0,1,4);
		Blend(rt_textures[RT_SceneBloom], rt_textures[RT_SceneBloom],rt_textures[RT_SceneBloom], 1, false);
		Blend(rt_textures[RT_SceneBloom], rt_textures[RT_SceneBloom],rt_textures[RT_SceneBloom], 1, true);
		Blend(rt_textures[RT_SceneBloom], rt_textures[RT_SceneBloom],rt_textures[RT_SceneBloom], 1, false);
		Blend(rt_textures[RT_SceneLighting], rt_textures[RT_SceneBloom],rt_textures[RT_SceneLighting], 1, false);
	}

	void Graphics::VolumeLight()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, volume_fbo);

		glViewport(0,0,screen_width_i,screen_height_i); 
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


		GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, buffers);


		glUseProgram( SHADERS.at(SH_VolumeSampling).ShaderProgram );
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneVolumeLight], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, rt_textures[RT_SceneVolumeLight], 0);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_VolumeLight]);
		SHADERS[SH_VolumeSampling].SetUniform1i( "tLighting", 0 );

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rt_textures[RT_SceneGlow]);
		SHADERS[SH_VolumeSampling].SetUniform1i( "tEpipolar", 1 );

		Light* light = LIGHT_LIST[0];

		SHADERS[SH_VolumeSampling].SetUniform3f( "light_pos", light->GetTransform()->Position);
		SHADERS[SH_VolumeSampling].SetUniform3f( "light_color", light->Color);
		SHADERS[SH_VolumeSampling].SetUniform2f( "screen_dims", vec2(screen_width,screen_height));

		mat4 m = mat4();

		modelMat = glm::translate( modelMat, LIGHT_LIST[0]->GetTransform()->Position );
		m = viewMat * modelMat;

		SHADERS[SH_VolumeSampling].SetUniform4fv( "V", &viewMat[0][0] );
		SHADERS[SH_VolumeSampling].SetUniform4fv( "P", &projMat[0][0] );


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

		
		Blend( rt_textures[RT_SceneVolumeLight], rt_textures[RT_SceneLighting], rt_textures[RT_SceneLighting], 1, false );

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Graphics::ProcessLights()
	{
		num_lights = LIGHT_LIST.size();

		if( num_lights > MAX_LIGHTS )
			num_lights = MAX_LIGHTS;
	}

	void Graphics::BuildShadows()
	{
		for( unsigned i = 0; i < LIGHT_LIST.size(); ++i )
		{
			if( LIGHT_LIST.at(i)->RT_Textures[RT_LightDepth] > 0 )
				continue;

			glGenTextures(1, &LIGHT_LIST.at(i)->RT_Textures[RT_LightDepth]);
			glBindTexture(GL_TEXTURE_2D, LIGHT_LIST.at(i)->RT_Textures[RT_LightDepth]);			

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);	
			// luminance is 1 channel (R/red in our case). We only need one since depth is 1 channel
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, (int)(screen_width*shadow_size), (int)(screen_width*shadow_size), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);


			glGenTextures(1, &LIGHT_LIST.at(i)->RT_Textures[RT_ExpLightDepth]);
			glBindTexture(GL_TEXTURE_2D, LIGHT_LIST.at(i)->RT_Textures[RT_ExpLightDepth]);	

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

			GLfloat border[] = {1.0f, 1.0f, 1.0f, 1.0f};
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, (int)(screen_width*shadow_size), (int)(screen_width*shadow_size), 0, GL_LUMINANCE, GL_FLOAT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);

			glGenFramebuffers(1, &shadow_fbo); 
			glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);

			// Attach the texture to the FBO
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, LIGHT_LIST.at(i)->RT_Textures[RT_LightDepth], 0 );
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LIGHT_LIST.at(i)->RT_Textures[RT_ExpLightDepth], 0);
		}

		//glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);

		int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

		if( status != GL_FRAMEBUFFER_COMPLETE ) 
		{
			std::cout << "FB error, status: " << status << std::endl;
			return;
		} 


		////
		glGenFramebuffers(1, &exp_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, exp_fbo);

		// CREATE COLOR TEXTURE
		glGenTextures(1, &LIGHT_LIST.at(0)->RT_Textures[RT_ExpLightBlur]);
		glBindTexture(GL_TEXTURE_2D, LIGHT_LIST.at(0)->RT_Textures[RT_ExpLightBlur]);
    
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
		// any objects that end up with tex coords outside of light frustum (0..1) will be white (non-shadowed)
		GLfloat border[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, (int)(screen_width*shadow_size), (int)(screen_width*shadow_size), 0, GL_LUMINANCE, GL_FLOAT, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
    
		// attach textures to FBO color attachment0 point
		glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, LIGHT_LIST.at(0)->RT_Textures[RT_ExpLightBlur], 0 );
		////

		glBindFramebuffer(GL_FRAMEBUFFER,0);

		rt_textures[RT_LightDepth] = LIGHT_LIST.at(0)->RT_Textures[RT_LightDepth];
		rt_textures[RT_ExpLightDepth] = LIGHT_LIST.at(0)->RT_Textures[RT_ExpLightDepth];
		rt_textures[RT_ExpLightBlur] = LIGHT_LIST.at(0)->RT_Textures[RT_ExpLightBlur];
	}

	void Graphics::DrawPP(float dt)
	{
		// Post processing

		//Blur(rt_textures[RT_SceneLighting],1,1);
		

		if( bloom_enabled )
		{
			Bloom();
			Glow();
		}		

		if( volumelight_enabled )
			VolumeLight();

		if( current_fog > 0 )
			Fog(rt_textures[RT_SceneLighting], rt_textures[RT_SceneDepth]);
	}

	void Graphics::Draw2D(float dt)
	{
		// GUI drawing


		
	}

	void Graphics::DrawWireframe(float dt)
	{		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

			modelMat = glm::rotate( modelMat, t->Rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.z, vec3( 0.0f, 0.0f, 1.0f ) );

			if( MODEL_LIST[i]->Owner->GetBehavior() != nullptr )
				modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );

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

	void Graphics::DrawLightShape(float dt)
	{		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glViewport(0,0,screen_width_i,screen_height_i);

		// Draw wireframe geometry
		glDisable( GL_DEPTH_TEST );
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);

		

		for( unsigned i = 0; i < LIGHT_LIST.size(); ++i )
		{
			//glBindVertexArray( MODEL_LIST[i]->MeshID );		// Use the cube mesh		
		
			glUseProgram( SHADERS.at(SH_Wireframe).ShaderProgram );		// Activate phong shader

			SHADERS[SH_Wireframe].SetUniform4fv( "view", &viewMat[0][0] );
			SHADERS[SH_Wireframe].SetUniform4fv( "proj", &projMat[0][0] );

			Transform* t = LIGHT_LIST[i]->GetTransform();

			if( t == nullptr )
				continue;

			//vec3 color = MODEL_LIST[i]->WireColor;

			modelMat = mat4();		

			modelMat = glm::translate( modelMat, t->Position );		

			modelMat = glm::rotate( modelMat, t->Rotation.x, vec3( 1.0f, 0.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.y, vec3( 0.0f, 1.0f, 0.0f ) );
			modelMat = glm::rotate( modelMat, t->Rotation.z, vec3( 0.0f, 0.0f, 1.0f ) );

			modelMat = glm::scale( modelMat, t->Scale*LIGHT_LIST[i]->Radius*10.0f );						

			//glUniform3f( wireColor, color.x, color.y, color.z );
			//glUniformMatrix4fv( wireModel, 1, GL_FALSE, &modelMat[0][0] );		// Pass the locally transformed model matrix to the scene shader	

			SHADERS[SH_Wireframe].SetUniform3f( "wirecolor", vec3(1.0f,1.0f,0.0f) );
			SHADERS[SH_Wireframe].SetUniform4fv( "model", &modelMat[0][0] );
			//glDrawArrays( GL_TRIANGLES, 0, MESH_VERTICES.at(MODEL_LIST[i]->MeshID) );	// Draw first cube

			glEnableVertexAttribArray(0);

			glBindBuffer(GL_ARRAY_BUFFER, LIGHT_LIST.at(i)->GetModel()->VertexBuffer);
				glVertexAttribPointer(
					0,                  // attribute
					3,                  // size
					GL_FLOAT,           // type
					GL_FALSE,           // normalized?
					0,                  // stride
					(void*)0            // array buffer offset
				);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, LIGHT_LIST.at(i)->GetModel()->ElementBuffer);
				glDrawElements(
					GL_TRIANGLES,      // mode
					//indices.size(),    // count
					LIGHT_LIST.at(i)->GetModel()->Indices.size(),
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
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt_textures[RT_SceneLighting], 0);

		glViewport(0,0,screen_width_i,screen_height_i);

		// Draw wireframe geometry
		glDisable( GL_DEPTH_TEST );			// WE DON'T WANT THIS
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);

		for( unsigned i = 0; i < MODEL_LIST.size(); ++i )
		{
			if( MODEL_LIST[i]->MeshName != "Light.dae" && MODEL_LIST[i]->Owner != SelectedEntity )
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

			if( MODEL_LIST[i]->Owner->GetBehavior() != nullptr )
				modelMat = glm::rotate( modelMat, (accum/2000.0f) * 180.0f, vec3( 0.0f, 1.0f, 0.0f ) );


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

		glEnable( GL_DEPTH_TEST );
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);	
	}

	void Graphics::DrawDebugText(std::string text)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

		if( path != "" && image == nullptr )
		{
			std::cout << "Texture '" << path << "' didn't load properly" << std::endl;
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

	GLuint Graphics::LoadCubemap(std::string front, std::string back, std::string left, std::string right,
								std::string top, std::string bottom)
	{
		std::string path = "Assets/Textures/";
		std::string R = path + right;
		std::string L = path + left;
		std::string U = path + top;
		std::string D = path + bottom;
		std::string B = path + back;
		std::string F = path + front;

		GLuint tex_cube = SOIL_load_OGL_cubemap( R.c_str(), L.c_str(), U.c_str(), D.c_str(), B.c_str(), F.c_str(),
							SOIL_LOAD_RGB, SOIL_CREATE_NEW_ID, 0 );

		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri (GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		if( tex_cube == 0 )
		{
			std::cout << "Cubemap texture '" << path << "' didn't load properly" << std::endl;
			return 0;
		}

		return tex_cube;
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

	void Graphics::LoadSkybox( std::string front, std::string back, std::string left, 
								std::string right, std::string top, std::string bottom)
	{
		sky_tex = LoadCubemap(front,back,left,right,top,bottom);
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

		LIGHT_LIST[0]->GetTransform()->Position += cam_rot * move;

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

		switch( current_rt )
		{
			case RT_LightDepth:
				std::cout << " >RT< Shadow depth texture" << std::endl;
				break;
			case RT_ExpLightDepth:
				std::cout << " >RT< Shadow exp depth" << std::endl;
				break;
			case RT_ExpLightBlur:
				std::cout << " >RT< Shadow exp blur" << std::endl;
				break;
			case RT_SceneLighting:
				std::cout << " >RT< Final composite lighting" << std::endl;
				break;
			case RT_SceneShadows:
				std::cout << " >RT< Shadows" << std::endl;
				break;
			case RT_SceneBloom:
				std::cout << " >RT< Bloom" << std::endl;
				break;
			case RT_SceneDiffuse:
				std::cout << " >RT< Diffuse color" << std::endl;
				break;
			case RT_ScenePositions:
				std::cout << " >RT< Positions" << std::endl;
				break;
			case RT_SceneNormals:
				std::cout << " >RT< Normals" << std::endl;
				break;
			case RT_SceneSpecular:
				std::cout << " >RT< Specular" << std::endl;
				break;
			case RT_SceneDepth:
				std::cout << " >RT< Depth" << std::endl;
				break;
			case RT_SceneGlow:
				std::cout << " >RT< Glow" << std::endl;
				break;
			case RT_SceneAttRadiance:
				std::cout << " >RT< Attenuated radiance" << std::endl;
				break;
			case RT_SceneSctRadiance:
				std::cout << " >RT< Scattered radiance" << std::endl;
				break;
			case RT_VolumeLight:
				std::cout << " >RT< Light volumes" << std::endl;
				break;
			case RT_SceneVolumeLight:
				std::cout << " >RT< Sampled light volumes" << std::endl;
				break;
			case RT_CoordTex:
				std::cout << " >RT< Coordinate texture" << std::endl;
				break;
			default:
				break;
		}
	}

	void Graphics::PrevRT()
	{
		--current_rt;

		if( current_rt < 0 )
			current_rt = RT_TOTAL-1;

		screen_tex = rt_textures[current_rt];

		switch( current_rt )
		{
			case RT_LightDepth:
				std::cout << " >RT< Shadow depth texture" << std::endl;
				break;
			case RT_ExpLightDepth:
				std::cout << " >RT< Shadow exp depth" << std::endl;
				break;
			case RT_ExpLightBlur:
				std::cout << " >RT< Shadow exp blur" << std::endl;
				break;
			case RT_SceneLighting:
				std::cout << " >RT< Final composite lighting" << std::endl;
				break;
			case RT_SceneShadows:
				std::cout << " >RT< Shadows" << std::endl;
				break;
			case RT_SceneBloom:
				std::cout << " >RT< Bloom" << std::endl;
				break;
			case RT_SceneDiffuse:
				std::cout << " >RT< Diffuse color" << std::endl;
				break;
			case RT_ScenePositions:
				std::cout << " >RT< Positions" << std::endl;
				break;
			case RT_SceneNormals:
				std::cout << " >RT< Normals" << std::endl;
				break;
			case RT_SceneSpecular:
				std::cout << " >RT< Specular" << std::endl;
				break;
			case RT_SceneDepth:
				std::cout << " >RT< Depth" << std::endl;
				break;
			case RT_SceneGlow:
				std::cout << " >RT< Glow" << std::endl;
				break;
			case RT_SceneAttRadiance:
				std::cout << " >RT< Attenuated radiance" << std::endl;
				break;
			case RT_SceneSctRadiance:
				std::cout << " >RT< Scattered radiance" << std::endl;
				break;
			case RT_VolumeLight:
				std::cout << " >RT< Light volumes" << std::endl;
				break;
			case RT_SceneVolumeLight:
				std::cout << " >RT< Sampled light volumes" << std::endl;
				break;
			case RT_CoordTex:
				std::cout << " >RT< Coordinate texture" << std::endl;
				break;
			default:
				break;
		}
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
			float radius	= 40.0f;//1.0f / dist * 1000.0f;

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

		float dist = glm::distance( SelectedEntity->GetTransform()->Position, cam_pos ) * 0.005f;
		
		if( SelectedEntity->GetLight() && SelectedEntity->GetLight()->Orbital )
		{
			Transform* t = SelectedEntity->GetTransform();
		
			vec3 pos = t->Position;

			float move_speed = 0.0f;

			vec4 axis = vec4(vec3(0,0,0) - pos,1);
			axis.y = 0;

			mat4 rot = mat4();
			rot = glm::rotate(rot, 90.0f, vec3(0,1,0));
			axis = rot * axis;

			pos -= cam_pos;

			rot = mat4();			

			if( Input::GetInstance()->GetKey(SDLK_RSHIFT) == true || Input::GetInstance()->GetKey(SDLK_LSHIFT) == true )
			{
				move_speed = -2.0f*y;
				rot = glm::rotate(rot, move_speed, vec3(axis.x,axis.y,axis.z));
			}
			else
			{
				move_speed = -2.0f*x;
				rot = glm::rotate(rot, move_speed, vec3(0,1,0));
			}


			vec4 new_pos = rot * vec4(pos,1);

			t->Position = vec3(new_pos.x, new_pos.y, new_pos.z) + cam_pos;

			
		}
		else
		{
			vec3 move(0,0,0); 		

			float move_speed = dist*x;
			move += vec3(move_speed,0,0);

			move_speed = dist*y;	

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
		glDeleteRenderbuffers(1, &depth_buffer);
	}
}