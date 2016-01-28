#include "Gamepad.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Input.h"
#include <iostream>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>  
#include <glm/glm.hpp>
#include <stdio.h>  
#include <stdlib.h>  
#include <vector>



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
		glfwPollEvents();
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


void renderTest(GLFWwindow* window)
{
	printf("Begin rendering test\n");

	Renderer render = Renderer(window);

	vector<vec3> mesh;
	vector<vec3> normals;
	vector<unsigned int> indices;

	unsigned int square = render.generateObjectID();
	render.assignCube(square, 1.f, &mesh, &normals, &indices);
	Material mat = Material ();
	render.assignMaterial(square, &mat);


	unsigned int light = render.generateLightObject();
	render.setLightPosition(light, vec3(0.0, 10.0, 10.0));


	render.renderLoop();

	printf("End rendering test\n");
}

// A quick example of how to use this
int main() {
	GLFWwindow* window = initializeWindow();
	if (window == NULL)
		return -1;

	//inputTest(window);
	renderTest(window);


	return 0;
}
