#pragma once

namespace Roivas
{
	class Shader
	{
		public:
			Shader();
			Shader(GLuint shaderprogram);
			void SetUniform4fv( std::string name, const float* val );
			void SetUniform3f( std::string name, const vec3& val );
			void SetUniform3fArray( std::string name, int size, const float* val );
			void SetUniform1fArray( std::string name, int size, const float* val );
			void SetUniform1i( std::string name, const int val );
			void SetUniform1iArray( std::string name, int size, const GLint* val );
			void SetUniform1uiArray( std::string name, int size, const GLuint* val );

		// Data
			GLuint ShaderProgram;

	};
}