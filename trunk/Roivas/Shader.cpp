#include "CommonLibs.h"
#include "Shader.h"

namespace Roivas
{
	Shader::Shader() {}

	Shader::Shader(GLuint shaderprogram) : ShaderProgram(shaderprogram) {}

	void Shader::SetUniform4fv( std::string name, const float* val )
	{
		glUniformMatrix4fv( glGetUniformLocation(ShaderProgram, name.c_str()), 1, GL_FALSE, val );
	}

	void Shader::SetUniform3f( std::string name, const vec3& val )
	{
		glUniform3f( glGetUniformLocation(ShaderProgram, name.c_str()), val.x, val.y, val.z );
	}

	void Shader::SetUniform3fArray( std::string name, int size, const float* val )
	{
		glUniform3fv( glGetUniformLocation(ShaderProgram, name.c_str()), size, val );
	}

	void Shader::SetUniform1fArray( std::string name, int size, const float* val )
	{
		glUniform1fv( glGetUniformLocation(ShaderProgram, name.c_str()), size, val );
	}

	void Shader::SetUniform1i( std::string name, const int val )
	{
		glUniform1i( glGetUniformLocation(ShaderProgram, name.c_str()), val );
	}

	void Shader::SetUniform1iArray( std::string name, int size, const GLint* val )
	{
		glUniform1iv( glGetUniformLocation(ShaderProgram, name.c_str()), size, val );
	}

	void Shader::SetUniform1uiArray( std::string name, int size, const GLuint* val )
	{
		glUniform1uiv( glGetUniformLocation(ShaderProgram, name.c_str()), size, val );
	}

}