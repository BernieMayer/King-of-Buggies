#include "Gamepad.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Input.h"
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

void inputTest(GLFWwindow* window) {
	InputManager manager = InputManager(window);

	float LX = 0;
	float LY = 0;

	float forward = 0;
	float turn = 0;

	while (true) {
		Input input = manager.getInput(1);

		if (input.drift) {
			std::cout << "Drift\n";
		}

		if (input.powerup) {
			std::cout << "Powerup\n";
		}

		if (input.menu) {
			std::cout << "Menu\n";
		}

		if (input.camH != LX ||
			input.camV != LY)
		{
			LX = input.camH;
			LY = input.camV;
			std::cout << "X: " << LX << '\n';
			std::cout << "Y: " << LY << '\n';
			std::cout << '\n';
		}

		if (input.forward != forward) {
			forward = input.forward;
			std::cout << "Forward: " << forward << "\n";
		}

		if (input.turn != turn) {
			turn = input.turn;
			std::cout << "Turn: " << turn << "\n";
		}
	}
}

// A quick example of how to use this
int main() {
	GLFWwindow* window = initializeWindow();
	if (window == NULL)
		return -1;

	inputTest(window);

	//Set a background color  
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	printf("Debugging\n");

	Renderer render = Renderer(window);
	render.renderLoop();
	printf("Done\n");

	return 0;
}
