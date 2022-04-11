#pragma once

#include <string>

namespace Resources
{
	enum class ResourceType
	{
		MESH,
		SHADER,
		TEXTURE
	};

	class Resource
	{
	public:
		Resource(const std::string& name, const ResourceType type) { m_name = name; m_type = type; };
		~Resource() {};

		std::string m_name;
		ResourceType m_type;

		virtual void bindResource() {};
		virtual int getData() {return 0;};
	};
}