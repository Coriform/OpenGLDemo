#pragma once

namespace Roivas
{
	class Shader
	{
		public:
			Shader();
			void SetUniform4fv( std::string name, const float* mat );

		private:
			GLuint shader_program;

	};
}