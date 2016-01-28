#include "Gamepad.h"
#include "Renderer.h"
#include <iostream>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>  
#include <stdio.h>  
#include <stdlib.h>  



GLFWwindow* initializeWindow()
{

	//Initialize GLFW  
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	//Set the GLFW window creation hints - these are optional  
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Request a specific OpenGL version  
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //Request a specific OpenGL version  
	//glfwWindowHint(GLFW_SAMPLES, 4); //Request 4x antialiasing  
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  

	//Declare a window object  
	GLFWwindow* window;

	//Create a window and create its OpenGL context  
	window = glfwCreateWindow(640, 480, "Test Window", NULL, NULL);

	//If the window couldn't be created  
	if (!window)
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//This function makes the context of the specified window current on the calling thread.   
	glfwMakeContextCurrent(window);

	//Initialize GLEW  
	GLenum err = glewInit();

	//If GLEW hasn't initialized  
	if (err != GLEW_OK)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		return NULL;
	}

	return window;

}


// A quick example of how to use this
int main() {

	/*Gamepad* controller;
	controller = new Gamepad(1);

	float LX = 0;
	float LY = 0;

	while (true) {
		controller->Update();

		if (controller->GetButtonPressed(XButtons.A)) {
			std::cout << "A\n";
		}

		if (!(controller->LStick_InDeadzone()) &&
			(controller->LeftStick_X() != LX ||
				controller->LeftStick_Y() != LY))
		{
			LX = controller->LeftStick_X();
			LY = controller->LeftStick_Y();
			std::cout << "X: " << LX << '\n';
			std::cout << "Y: " << LY << '\n';
			std::cout << '\n';
		}

		controller->RefreshState();
	}

	return 0;*/

	GLFWwindow* window = initializeWindow();
	if (window == NULL)
		return -1;

	//Set a background color  
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	printf("Debugging\n");

	Renderer render = Renderer(window);
	render.renderLoop();
	printf("Done\n");

	return 0;
}
