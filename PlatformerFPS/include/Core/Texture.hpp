#pragma once

#include <string>
#include "Resource.hpp"

//typedef GLint int;

namespace Resources
{
	class Texture : public Resource
	{
	public:
		Texture(std::string filePath, GLint wrapping, GLint filtering);
		~Texture();

		void bindResource() override;
	private:
		unsigned int texture;
	};
}