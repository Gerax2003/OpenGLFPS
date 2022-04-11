#pragma once

#include <vector>
#include <string>
#include "Core/Maths.hpp"

namespace LowRenderer
{
	class Model
	{
	public:
		Model(std::string inTexName = "resources/blank.png", std::string inMeshName = "resources/blank.obj",
		  std::string inShaderName = "resources/shader", Maths::Vector3 inPosition = {}, Maths::Vector3 inRotation = {}, float inScale = 1.f, Model* inParent = nullptr) 
		{
			meshName = inMeshName;
			shaderName = inShaderName;
			textureName = inTexName;
			position = inPosition;
			rotation = inRotation;
			scale = { inScale, inScale, inScale };
			parent = inParent;
		};
		Model(Maths::Vector3 inPosition, Maths::Vector3 inRotation, Maths::Vector3 inScale, std::string inTexName = "resources/blank.png", std::string inMeshName = "resources/blank.obj",
			std::string inShaderName = "resources/shader", Model* inParent = nullptr)
		{
			meshName = inMeshName;
			shaderName = inShaderName;
			textureName = inTexName;
			position = inPosition;
			rotation = inRotation;
			scale = inScale;
			parent = inParent;
		};
		~Model() {};

		std::string textureName;
		std::string meshName;
		std::string shaderName;
		Maths::Vector3 position;
		Maths::Vector3 rotation;
		Maths::Vector3 scale;
		Model* parent = nullptr;
	};
}