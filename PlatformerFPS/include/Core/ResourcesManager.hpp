#pragma once

#include "Maths.hpp"
#include "Mesh.hpp"
#include "Model.hpp"
#include "Resource.hpp"

#include <vector>
#include <memory>
#include <string>
#include <glad/glad.h>

namespace Resources
{
	class ResourcesManager
	{
	private:
		std::vector<std::unique_ptr<Resource>> resources;
	public:
		ResourcesManager() {};
		~ResourcesManager() {};
	
		void bindResource(int index) 
		{ 
			resources[index]->bindResource(); 
		};
		int searchResource(std::string resourceName, ResourceType type);
		void createResource(std::string filePath, ResourceType type);
		Maths::int3 getUniforms();
		int getFirstShader();
		int getShaderProgram(const std::string& name);
		int getBufferSize(const std::string& name);
	};
}