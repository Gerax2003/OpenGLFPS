
#include <iostream>
#include <string>
#include <fstream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Core/Maths.hpp"
#include "Core/Physics.hpp"
#include "Core/Camera.hpp"
#include "Core/Scene.hpp"
#include "Core/Light.hpp"
#include "Core/Debug.hpp"
#include "Game/Explosion.hpp"
#include "Game/Gunsmoke.hpp"

using namespace Resources;

#pragma region CONSTRUCTORS

Scene::Scene::Scene() : camera(SCR_WIDTH, SCR_HEIGHT)
{
	glEnable(GL_DEPTH_TEST);

	models.push_back(LowRenderer::Model("resources/BLACKSMITH_TEX.jpg", "resources/the_noble_craftsman.obj", "resources/shader", { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 0.85f));
	player.model = models[0];

	GLuint tempUBO;
	glGenBuffers(1, &tempUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, tempUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Resources::Scene::ShaderData), NULL, GL_DYNAMIC_DRAW);

	shaderData.lights[0].ambient.xyz = { 0.075f, 0.0f, 0.0f };
	shaderData.lights[0].diffuse.xyz = { 1.f, 0.1f, 0.f };
	shaderData.lights[0].specular.xyz = { 1.0f, 0.0f, 0.0f };
	shaderData.lights[0].attenuation.xyz = { 1.f, 0.f, 0.01f };
	shaderData.lights[0].position.xyz = { 0.f,-1.f, 0.2f };
	shaderData.lights[0].lightEnabled = true;

	UBO = tempUBO;
}

// To replace with scene parser
Scene::Scene::Scene(ResourcesManager* inResourcesManager, GLFWwindow* window) : camera(SCR_WIDTH, SCR_HEIGHT)
{
	resourcesManager = inResourcesManager;
	glEnable(GL_DEPTH_TEST);

	//models.push_back(LowRenderer::Model("resources/blank.png", "resources/ball.obj", "resources/shader", { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 1.f));

	models.push_back(LowRenderer::Model("resources/skybox.jpg", "resources/skyball.obj", "resources/skybox", { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 16069.f));
	
	models.push_back(LowRenderer::Model("resources/BLACKSMITH_TEX.jpg", "resources/skyball.obj", "resources/skybox", { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, 1.0f));
	player = Player(models[models.size() - 1], &camera, window);
	models.pop_back();

	camera.toFollow = &player.model;
	camera.state = LowRenderer::CameraState::FOLLOW;

	platforms.push_back(Platform({0.f, -3.f, 0.f}, {}, {200.f, 1.f, 10.f}));
	platforms.push_back(Platform({ 30.f, -1.f, 0.f }, {}, { 5.f, 1.5f, 2.5f }));
	platforms.push_back(Platform({ 40.f, 1.f, 0.f }, {}, { 5.f, 1.5f, 2.5f }));
	platforms.push_back(Platform({ 50.f, 3.f, 0.f }, {}, { 5.f, 1.5f, 2.5f }));
	platforms.push_back(Platform({ 60.f, 5.f, 0.f }, {0, 0, -1}, { 5.f, 1.5f, 2.5f }));

	platforms[0].model.textureName = "resources/gras.jpg";

	for (int i = 1; i < platforms.size(); i++)
		platforms[i].model.textureName = "resources/crate.png";

	GLuint tempUBO;
	glGenBuffers(1, &tempUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, tempUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Resources::Scene::ShaderData), NULL, GL_DYNAMIC_DRAW);

	shaderData.lights[0].ambient.xyz = { 0.1f, 0.1f, 0.1f };
	shaderData.lights[0].diffuse.xyz = { 1.f, 1.f, 1.f };
	shaderData.lights[0].specular.xyz = { 1.0f, 0.0f, 0.0f };
	shaderData.lights[0].attenuation.xyz = { 1.f, 0.f, 0.00001f };
	shaderData.lights[0].position.xyz = { 10.8f, 3.f, 0.2f };
	shaderData.lights[0].lightEnabled = true;

	shaderData.lights[1].ambient.xyz = { 0.1f, 0.1f, 0.1f };
	shaderData.lights[1].diffuse.xyz = { 1.f, 1.f, 1.f };
	shaderData.lights[1].specular.xyz = { 1.0f, 0.0f, 0.0f };
	shaderData.lights[1].attenuation.xyz = { 1.f, 0.f, 0.00001f };
	shaderData.lights[1].position.xyz = { 60.8f, 10.f, 1.2f };
	shaderData.lights[1].lightEnabled = true;

	UBO = tempUBO;
}

Scene::Scene::Scene(ResourcesManager* inResourcesManager, GLFWwindow* window, const std::string filePath) : camera(SCR_WIDTH, SCR_HEIGHT)
{
	resourcesManager = inResourcesManager;
	GLuint tempUBO;
	glGenBuffers(1, &tempUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, tempUBO);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Resources::Scene::ShaderData), NULL, GL_DYNAMIC_DRAW);

	std::fstream file;
	file.open(filePath);

	if (!file.is_open())
	{
		std::cout << "[ERROR] Couldn't open scene: " << filePath << std::endl;
		return;
	}

	size_t pos = 0;
	std::string line;
	std::string path = ""; // used to load resources
	std::string read; // part of the line currently being read
	while (!file.eof())
	{
		std::getline(file, line);
		pos = line.find(' ');
		read = line.substr(0, pos);
		line.erase(0, pos + 1); // +1 becasuse of the space
		
		if (read == "/" || read == "//" || read == "#")
			continue;
		else if (read == "path")
			path = line;
		else if (read == "m")
			parseLineModel(line, path);
		else if (read == "p")
		{
			parseLineModel(line, path);
			player = Player(models[models.size() - 1], &camera, window);
			models.pop_back();
		}
		else if (read == "e")
		{
			parseLineModel(line, path);
			enemies.push_back(std::make_shared<Enemy>(Enemy(models[models.size() - 1])));
			models.pop_back();
		}
		else if (read == "w")
		{
			parseLineModel(line, path);
			weapon = Weapon(models[models.size() - 1]);
			weapon.model.parent = nullptr;
			models.pop_back();
		}
		else if (read == "pl")
		{
			parseLineModel(line, path);
			platforms.push_back(Platform(models[models.size() - 1]));
			models.pop_back();
		}
		else if (read == "c")
			parseLineCamera(line);
		else if (read == "l")
			parseLineLight(line);
		else if (read == "menu")
			isMenu = true;
		
	}
	file.close();

	// Crosshair
	ui.push_back(LowRenderer::Model({-0.2f, 0.f, -0.02f }, {}, { 0.15f, 0.05f, 0.f }, "resources/blank.png", "resources/quad.obj", "resources/skybox"));
	ui.push_back(LowRenderer::Model({0.2f, 0.f, -0.02f }, {}, { 0.15f, 0.05f, 0.f }, "resources/blank.png", "resources/quad.obj", "resources/skybox"));
	ui.push_back(LowRenderer::Model({0.f, 0.2f, -0.02f }, {0.f, 0.f, PI / 2}, { 0.15f, 0.05f, 0.f }, "resources/blank.png", "resources/quad.obj", "resources/skybox"));
	ui.push_back(LowRenderer::Model({0.f, -0.2f, -0.02f }, { 0.f, 0.f, PI / 2 }, { 0.15f, 0.05f, 0.f }, "resources/blank.png", "resources/quad.obj", "resources/skybox"));

	UBO = tempUBO;
}

#pragma region LINE PARSING

Maths::Vector3 parseLineVector3(std::string& line)
{
	Maths::Vector3 retVec = {};
	size_t pos = 0;
	std::string read;

	for (int i = 0; i < 3; i++)
	{
		pos = line.find(',');
		read = line.substr(0, pos);
		line.erase(0, pos + 1); // +1 becasuse of the delimiter character
		retVec.e[i] = std::stof(read);
	}
	return retVec;
}

Maths::Vector4 parseLineVector4(std::string& line)
{
	Maths::Vector4 retVec = {};
	size_t pos = 0;
	std::string read;

	for (int i = 0; i < 3; i++)
	{
		pos = line.find(',');
		read = line.substr(0, pos);
		line.erase(0, pos + 1); // +1 becasuse of the delimiter character
		retVec.e[i] = std::stof(read);
	}
	return retVec;
}

void Scene::Scene::parseLineLight(std::string& line)
{
	size_t pos = 0;
	std::string read;
	int lightNum = 0;

	for (int i = 0; i < 7; i++)
	{
		if (i <= 2 || i == 6)
			pos = line.find(' ');
		else
			pos = line.find('|');

		read = line.substr(0, pos);
		line.erase(0, pos + 1); // +1 becasuse of the delimiter character

		if (i == 0)
			lightNum = std::stoi(read);
		if (i == 1)
		{
			if (read == "on")
				shaderData.lights[lightNum].lightEnabled = true;
			else
				shaderData.lights[lightNum].lightEnabled = false;
		}
		if (i == 2)
			shaderData.lights[lightNum].position.xyz = parseLineVector3(read);
		if (i == 3)
			shaderData.lights[lightNum].ambient.xyz = parseLineVector3(read);
		if (i == 4)
			shaderData.lights[lightNum].diffuse.xyz = parseLineVector3(read);
		if (i == 5)
			shaderData.lights[lightNum].specular = parseLineVector4(read);
		if (i == 6)
			shaderData.lights[lightNum].attenuation.xyz = parseLineVector3(read);
	}

}

void Scene::Scene::parseLineCamera(std::string& line)
{
	size_t pos = 0;
	std::string read;

	pos = line.find(' ');
	read = line.substr(0, pos);
	line.erase(0, pos + 1); // +1 becasuse of the space

	if (read == "ortho")
	{
		camera.isPerspective = false;

		pos = line.find(' ');
		read = line.substr(0, pos);
		line.erase(0, pos + 1); // +1 becasuse of the space
	}

	if (read == "follow" || read == "fps")
	{
		if (read == "follow")
			camera.state = LowRenderer::CameraState::FOLLOW;
		else if (read == "fps")
			camera.state = LowRenderer::CameraState::FIRST_PERSON;

		pos = line.find(' ');
		read = line.substr(0, pos);
		line.erase(0, pos + 1); // +1 becasuse of the space
		if (read == "p")
		{
			camera.toFollow = &player.model;
			pos = line.find(' ');
			read = line.substr(0, pos);
			line.erase(0, pos + 1); // +1 becasuse of the space
		}
		camera.offsetFromTarget = parseLineVector3(read);
	}
	else if (read == "lock")
	{
		camera.state = LowRenderer::CameraState::LOCK;

		pos = line.find(' ');
		read = line.substr(0, pos);
		line.erase(0, pos + 1); // +1 becasuse of the space
		camera.position = parseLineVector3(read);
	}
	else 
		camera.state = LowRenderer::CameraState::FREE;
}

void Scene::Scene::parseLineModel(std::string& line, const std::string& path)
{
	size_t pos = 0;
	std::string read;
	LowRenderer::Model model = LowRenderer::Model();
	models.push_back(model);

	for (int i = 0; i < 6; i++)
	{
		if (i <= 2)
			pos = line.find(' ');
		else
			pos = line.find('|');

		read = line.substr(0, pos);
		line.erase(0, pos + 1); // +1 becasuse of the delimiter character

		if (i == 0)
			model.textureName = path + read;
		if (i == 1)
			model.meshName = path + read;
		if (i == 2)
			model.shaderName = path + read;
		if (i == 3)
			model.position = parseLineVector3(read);
		if (i == 4)
			model.rotation = parseLineVector3(read);
		if (i == 5)
			model.scale = parseLineVector3(read);
	}
	models[models.size() - 1] = model;
}

#pragma endregion

#pragma endregion

#pragma region UPDATE

void Scene::Scene::checkGameState(GLFWwindow* window, int& currScene)
{
	if (isMenu) return;

	if (enemies.size() <= 0)
	{
		currScene = WIN_SCENE;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else if (player.life <= 0)
	{
		currScene = GAME_OVER_SCENE;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void Scene::Scene::processInput(GLFWwindow* window, Maths::Vector2& mousePos, int& currScene)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		currScene = 0;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	double newMouseX, newMouseY;
	glfwGetCursorPos(window, &newMouseX, &newMouseY);
	camInputs.deltaX = (float)(newMouseX - mousePos.x);
	camInputs.deltaY = (float)(newMouseY - mousePos.y);
	mousePos.x = (float)newMouseX;
	mousePos.y = (float)newMouseY;

	camInputs.moveForward = glfwGetKey(window, GLFW_KEY_UP);
	camInputs.moveLeft = glfwGetKey(window, GLFW_KEY_LEFT);

	camInputs.moveBackward = glfwGetKey(window, GLFW_KEY_DOWN);
	camInputs.moveRight = glfwGetKey(window, GLFW_KEY_RIGHT);
	camInputs.moveUpwards = glfwGetKey(window, GLFW_KEY_SPACE);
	camInputs.moveDownwards = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT);
	
	if (glfwGetKey(window, GLFW_KEY_P)) camera.isPerspective = true;
	else if (glfwGetKey(window, GLFW_KEY_L)) camera.isPerspective = false;

	if (glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT))
		camera.near += 0.0001f;
	else if (glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL))
		camera.near -= 0.0001f;
}

// Updates the scene (calls the update)
void Scene::Scene::update(float deltaTime, GLFWwindow* window, Maths::Vector2& mousePos, int& currScene)
{
	checkGameState(window, currScene);

	player.p_camera = &camera;
	m_interface.update(window, currScene, models);

	//std::cout << models[0].textureName << models[0].position << std::endl;
	updateCollisions();
	player.update(deltaTime, platforms, enemies, particles, weapon);

	for (int i = 0; i < enemies.size(); i++)
		enemies[i]->update(deltaTime,platforms, player);
	

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->update(deltaTime);
		if (particles[i]->toDestroy) particles.erase(particles.begin() + i);
	}

	weapon.update(deltaTime);
	weapon.model.parent = &camera.model;

	updateShaderData();

	camera.toFollow = &player.model;
	// input
	processInput(window, mousePos, currScene);
	camera.update(deltaTime, camInputs);
}

void Scene::Scene::updateCollisions()
{
	Vector3 intersectPoint = {};
	Vector3 intersectNormal = {0.f, 0.f, 0.f};
	Vector3 point2 = player.model.position;
	point2.y -= player.collider.radius - 0.007f;

	if (player.velocity.y <= 0) point2.y += player.velocity.y;

	// grounding test
	for (int i = 0; i < platforms.size(); i++)
	{
		if (Maths::intersectSegmentBox(player.model.position, point2, platforms[i].collider, intersectPoint, intersectNormal))
		{
			player.isGrounded = true;
			player.velocity.y = 0;
		}
	}
	if (intersectNormal == Maths::nullVector3()) player.isGrounded = false;

	for (int i = 0; i < enemies.size(); i++)
	{
		intersectNormal = { 0.f, 0.f, 0.f };
		point2 = enemies[i]->model.position;
		point2.y -= enemies[i]->collider.radius - 0.007f;
		for (int j = 0; j < platforms.size(); j++)
		{
			if (Maths::intersectSegmentBox(enemies[i]->model.position, point2, platforms[j].collider, intersectPoint, intersectNormal))
			{
				enemies[i]->isGrounded = true;
				enemies[i]->velocity.y = 0;
			}
		}
		if (intersectNormal == Maths::nullVector3()) enemies[i]->isGrounded = false;
	}


	//bool testCollide = false;

	//for (int i = 0; i < platforms.size(); i++)
	//{
	//	if (Maths::collisionSphereBox(player.collider, platforms[i].collider))
	//	{
	//		/*player.model.position = intersectPoint + intersectNormal * 0.01f;
	//		player.collider.center = intersectPoint + intersectNormal * 0.01f;*/
	//		//player.velocity = {};
	//		testCollide = true;
	//	}

	//	testCollide = false;
	//}
}

void Scene::Scene::updateShaderData()
{
	shaderData.camPos[0] = camera.position.x;
	shaderData.camPos[1] = camera.position.y; // shaderData's values MUST be set one by one due to GLSL's uniform buffer objects padding
	shaderData.camPos[2] = camera.position.z; // requiring the usage of C standard arrays instead of structures/unions
}
#pragma endregion

#pragma region RENDERING

// Binds resources needed for a model
void Scene::Scene::bindModelResources(const LowRenderer::Model& model)
{
	int textureIndex, shaderIndex, meshIndex;
	textureIndex = resourcesManager->searchResource(model.textureName, ResourceType::TEXTURE);
	shaderIndex = resourcesManager->searchResource(model.shaderName, ResourceType::SHADER);
	meshIndex = resourcesManager->searchResource(model.meshName, ResourceType::MESH);

	resourcesManager->bindResource(textureIndex);
	resourcesManager->bindResource(shaderIndex);
	resourcesManager->bindResource(meshIndex);
}

// Moves and rotate a model to its parent
Maths::mat4x4 moveToParent(const LowRenderer::Model* model)
{
	Maths::mat4x4 modelMat = Maths::mat4::identity();

	if (model->parent != nullptr)
		modelMat = moveToParent(model->parent);

	modelMat = modelMat * Physics::translate(model->position);
	modelMat = modelMat * Physics::rotate3f(model->rotation) * Physics::scale(model->scale);

	return modelMat;
}

// Renders every model of the scene
void Scene::Scene::render()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Maths::mat4x4 viewProj = camera.getProjectionMatrix() * camera.getViewMatrix();

	glNamedBufferSubData(UBO, 0, sizeof(Resources::Scene::ShaderData), &shaderData);

	Maths::mat4x4 modelMat = Maths::mat4::identity();
	modelMat = modelMat * Physics::translate(player.model.position);
	modelMat = modelMat * Physics::rotate3f(player.model.rotation) * Physics::scale(player.model.scale);
	renderModel(player.model, modelMat, viewProj);

	for (int i = 0; i < platforms.size(); i++)
	{
		modelMat = Maths::mat4::identity();

		if (platforms[i].model.parent != nullptr)
			modelMat = moveToParent(platforms[i].model.parent);

		modelMat = modelMat * Physics::translate(platforms[i].model.position);
		modelMat = modelMat * Physics::rotate3f(platforms[i].model.rotation) * Physics::scale(platforms[i].model.scale);
		renderModel(platforms[i].model, modelMat, viewProj);
	}

	for (int i = 0; i < enemies.size(); i++)
	{
		modelMat = Maths::mat4::identity();

		if (enemies[i]->model.parent != nullptr)
			modelMat = moveToParent(enemies[i]->model.parent);

		modelMat = modelMat * Physics::translate(enemies[i]->model.position);
		modelMat = modelMat * Physics::rotate3f(enemies[i]->model.rotation) * Physics::scale(enemies[i]->model.scale);
		renderModel(enemies[i]->model, modelMat, viewProj);
	}

	for (int i = 0; i < models.size(); i++)
	{
		modelMat = Maths::mat4::identity();

		if (models[i].parent != nullptr)
			modelMat = moveToParent(models[i].parent);

		modelMat = modelMat * Physics::translate(models[i].position);
		modelMat = modelMat * Physics::rotate3f(models[i].rotation) * Physics::scale(models[i].scale);
		renderModel(models[i], modelMat, viewProj);
	}

	for (int i = 0; i < particles.size(); i++)
	{
		for (int j = 0; j < PARTICLE_MAX_MODELS; j++)
		{
			modelMat = Maths::mat4::identity();

			if (particles[i]->models[j].parent != nullptr)
				modelMat = moveToParent(particles[i]->models[j].parent);

			modelMat = modelMat * Physics::translate(particles[i]->models[j].position);
			modelMat = modelMat * Physics::rotate3f(particles[i]->models[j].rotation) * Physics::scale(particles[i]->models[j].scale);
			renderModel(particles[i]->models[j], modelMat, viewProj);
		}
	}

	glClear(GL_DEPTH_BUFFER_BIT);

	modelMat = Maths::mat4::identity();

	if (weapon.model.parent != nullptr)
		modelMat = moveToParent(weapon.model.parent);

	modelMat = modelMat * Physics::translate(weapon.model.position);
	modelMat = modelMat * Physics::rotate3f(weapon.model.rotation) * Physics::scale(weapon.model.scale);
	renderModel(weapon.model, modelMat, viewProj);
}

// renders a single model
void Scene::Scene::renderModel(int model, const Maths::mat4x4& modelMat, const Maths::mat4x4& viewProj)
{
	bindModelResources(models[model]);
	Maths::int3 uniforms = resourcesManager->getUniforms();

	int shader = resourcesManager->getShaderProgram(models[model].shaderName);

	if (uniforms.e[0] == -1)
		std::cout << "VP matrix location not found" << std::endl;
	else
		glUniformMatrix4fv(uniforms.e[0], 1, GL_FALSE, viewProj.e);

	if (uniforms.e[1] == -1)
		std::cout << "Model matrix location not found" << std::endl;
	else
		glUniformMatrix4fv(uniforms.e[1], 1, GL_FALSE, modelMat.e);

	if (uniforms.e[2] == -1)
		std::cout << "Shader data location not found" << std::endl;
	else
	{
		glUniformBlockBinding(shader, uniforms.e[2], 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	}

	glDrawArrays(GL_TRIANGLES, 0,  resourcesManager->getBufferSize(models[model].meshName) / 8);
}

void Scene::Scene::renderModel(LowRenderer::Model& model, const Maths::mat4x4& modelMat, const Maths::mat4x4& viewProj)
{
	bindModelResources(model);
	Maths::int3 uniforms = resourcesManager->getUniforms();

	int shader = resourcesManager->getShaderProgram(model.shaderName);

	if (uniforms.e[0] == -1)
		std::cout << "VP matrix location not found" << std::endl;
	else
		glUniformMatrix4fv(uniforms.e[0], 1, GL_FALSE, viewProj.e);

	if (uniforms.e[1] == -1)
		std::cout << "Model matrix location not found" << std::endl;
	else
		glUniformMatrix4fv(uniforms.e[1], 1, GL_FALSE, modelMat.e);

	if (uniforms.e[2] == -1)
		std::cout << "Shader data location not found" << std::endl;
	else
	{
		glUniformBlockBinding(shader, uniforms.e[2], 0);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBO);
	}

	glDrawArrays(GL_TRIANGLES, 0, resourcesManager->getBufferSize(model.meshName) / 8);
}

#pragma endregion
