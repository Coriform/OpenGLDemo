#include "CommonLibs.h"
#include "Shader.h"

namespace Roivas
{
	Shader::Shader() {}

	void Shader::SetUniform4fv( std::string name, const float* mat )
	{
		glUniformMatrix4fv( glGetUniformLocation(shader_program, name.c_str()), 1, GL_FALSE, mat );
	}

}