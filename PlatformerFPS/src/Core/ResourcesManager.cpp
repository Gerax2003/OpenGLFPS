
#include "Core/ResourcesManager.hpp"
#include "Core/Maths.hpp"
#include "Core/Mesh.hpp"
#include "Core/Shader.hpp"
#include "Core/Texture.hpp"
#include "Core/Debug.hpp"
#include "Core/Model.hpp"

#include <STB/stb_image.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

using namespace Resources;

// Searches for a resource with a given name and a given type
int ResourcesManager::ResourcesManager::searchResource(std::string resourceName, ResourceType type)
{
	for (int i = 0; i < resources.size(); i++)
		if (resources[i]->m_name == resourceName && resources[i]->m_type == type)
			return i;

	createResource(resourceName, type);
	return (int)(resources.size()) - 1;
}

// Generates a resource of a given name and a given type
void ResourcesManager::ResourcesManager::createResource(std::string filePath, ResourceType type)
{
	switch (type)
	{
	case Resources::ResourceType::MESH:
		resources.push_back(std::make_unique<Mesh>(filePath, 1.f));
		break;
	case Resources::ResourceType::SHADER:
		resources.push_back(std::make_unique<Shader>(filePath.c_str()));
		break;
	case Resources::ResourceType::TEXTURE:
		resources.push_back(std::make_unique<Texture>(filePath, GL_REPEAT, GL_LINEAR));
		break;
	default:
		break;
	}
}

// Gets the first shader in the list of the resources
int ResourcesManager::ResourcesManager::getFirstShader()
{
	for (int i = 0; i < resources.size(); i++)
		if (resources[i]->m_type == ResourceType::SHADER)
			return i;

	return -1;
}

// Gets the locations of the shader's uniforms
Maths::int3 ResourcesManager::ResourcesManager::getUniforms()
{
	int shader = getFirstShader();

	if (shader == -1)
		return { -1, -1, -1 };

	int shaderProgram = resources[shader]->getData();

	GLint vpLocation = glGetUniformLocation(shaderProgram, "viewProj");
	GLint modelLocation = glGetUniformLocation(shaderProgram, "modelMat");
	GLint shaderDataLocation = glGetUniformBlockIndex(shaderProgram, "shaderData");

	return { vpLocation, modelLocation, shaderDataLocation };
}

int ResourcesManager::ResourcesManager::getShaderProgram(const std::string& name)
{
	int index = searchResource(name, ResourceType::SHADER);
	return resources[index]->getData();
}

int ResourcesManager::ResourcesManager::getBufferSize(const std::string& name)
{
	int index = searchResource(name, ResourceType::MESH);
	return resources[index]->getData();
}