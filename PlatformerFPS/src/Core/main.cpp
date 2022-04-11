//#include <Windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cmath>

#include "Core/App.hpp"

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DEBUG_NEW new(_CLIENT_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

int main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	int* p = new int(); // used to see if leak check is enabled

	bool init;
	GLFWwindow* window = App::initOpenGL(init);

	if (!init)
		return -2;

	App::App app(init, window);

	if (!init)
		return -1;

	app.run();

	delete p; // uncomment to see if leak check is activated
	return 0;
}
