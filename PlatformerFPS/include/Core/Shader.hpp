#pragma once

#include <string>
#include "Resource.hpp"

namespace Resources
{
	class Shader : public Resource
	{
	public:
		Shader(const char* filePath);
		~Shader();

		void bindResource() override;
		int getData() override;
	private:
		int shaderProgram = 0;
	};
}