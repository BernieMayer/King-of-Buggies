#include "Gamepad.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Input.h"
#include "camera.h"
#include "Physics.h"
#include "GameManager.h"
#include "NavMesh.h"


#include "MeshInfo.h"

#include <iostream>
#include <GL/glew.h>  
#include <GLFW/glfw3.h>  
#include <glm/glm.hpp>
#include <stdio.h>  
#include <stdlib.h>  
#include <vector>
#include <random>

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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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


int main() {
	srand(time(0));

	GLFWwindow* window = initializeWindow();
	if (window == NULL)
		return -1;
	glfwSetWindowPos(window, 500, 500);    
	GameManager manager(window);

	// Enter main game loop
	//manager.initMenus();
	manager.gameLoop();    return 0;
}