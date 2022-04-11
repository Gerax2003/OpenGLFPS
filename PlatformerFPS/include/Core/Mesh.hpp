#pragma once

#include <string>

#include "Core/Maths.hpp"
#include "Core/Resource.hpp"

namespace Resources
{
	union Face3
	{
		Maths::int3 e[3];
		struct { Maths::int3 v1; Maths::int3 v2; Maths::int3 v3; };
	};

	class Mesh : public Resource
	{
	public:
		Mesh(std::string filePath, float scale);
		~Mesh();

		void bindResource() override;
		int getData() override;
		int bufferSize;
	private:
		unsigned int VAO;
		unsigned int VBO;
	};
}
