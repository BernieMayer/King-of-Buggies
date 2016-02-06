#include "Gamepad.h"
#include "Renderer.h"
#include "InputManager.h"
#include "Input.h"
#include "camera.h"
#include "Physics.h"

#include "MeshInfo.h"

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


void physicsTest() {
	Physics physics = Physics();

	physics.shutdown();
}

void renderTest(GLFWwindow* window)
{
	printf("Begin rendering test\n");

	Renderer render = Renderer(window);
	MeshInfo meshLoader = MeshInfo();

	vector<vec3> mesh;
	vector<vec3> normals;
	vector<unsigned int> indices;

	// Object creation
	Diffuse mat = Diffuse();
	Specular shinyMat = Specular(20.f);
	TorranceSparrow tsMat = TorranceSparrow(0.5f);
	
	unsigned int model = render.generateObjectID();
	
	mesh = meshLoader.getMeshVertices(model);
	normals = meshLoader.getMeshNormals(model);
	indices = meshLoader.getMeshIndices(model);

	render.assignMesh(model, &mesh);
	render.assignNormals(model, &normals);
	render.assignIndices(model, &indices);
	render.assignMaterial(model, &tsMat);
	render.assignColor(model, vec3(1.0, 0.0, 0.0));
	
	vector<vec3> mesh2;
	vector<vec3> normals2;
	vector<unsigned int> indices2;

	unsigned int disco = render.generateObjectID();
	mesh2 = meshLoader.getMeshVertices(disco);
	normals2 = meshLoader.getMeshNormals(disco);
	indices2 = meshLoader.getMeshIndices(disco);

	render.assignMesh(disco, &mesh2);
	render.assignNormals(disco, &normals2);
	render.assignIndices(disco, &indices2);
	render.assignMaterial(disco, &tsMat);
	render.assignColor(disco, vec3(1.0, 0.0, 1.0));

	vector<vec3> mesh3;
	vector<vec3> normals3;
	vector<unsigned int> indices3;

	unsigned int plane = render.generateObjectID();
	mesh3 = meshLoader.getMeshVertices(plane);
	normals3 = meshLoader.getMeshNormals(plane);
	indices3 = meshLoader.getMeshIndices(plane);

	render.assignMesh(plane, &mesh3);
	render.assignNormals(plane, &normals3);
	render.assignIndices(plane, &indices3);
	render.assignMaterial(plane, &tsMat);
	render.assignColor(plane, vec3(0.65, 0.65, 0.65));

	/*
	vector<vec3> mesh2;
	vector<vec3> normals2;
	vector<unsigned int> indices2;

	//Create square
	unsigned int square = render.generateObjectID();
	render.assignCube(square, 1.f, &mesh, &normals, &indices);
	render.assignMaterial(square, &mat);

	//Create sphere
	unsigned int sphere = render.generateObjectID();
	render.assignSphere(sphere, 0.5f, 20, &mesh2, &normals2, &indices2);
	render.assignMaterial(sphere, &tsMat);

	printf("Mesh = %d\nNormals = %d\nIndices = %d\n\n", mesh2.size(), normals2.size(), indices2.size());
	*/
	//Light creation
	//unsigned int light = render.generateLightObject();
	//render.setLightPosition(light, vec3(0.0, 0.0, -5.0));
	vec3 lightPos(5.0, 5.0, 5.0);

	unsigned int light = render.generateLightObject();
	render.setLightPosition(light, lightPos);


	InputManager im(window);

	Camera cam (vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0),
							MODELVIEWER_CAMERA);

	render.loadPerspectiveTransform(0.1f, 20.f, 90.f);		//Near, far, fov

	render.loadCamera(&cam);
	
	mat4 translation;
	float theta = 0.0;

	while (!glfwWindowShouldClose(window))
	{
		Input in = im.getInput(1);		//Get input

		float scale = 0.001;
		cam.rotateView(in.turn*scale, in.forward*scale);
		
		theta += 0.0001;
		translation[3][1] = 0.5f*sin(theta);
		translation[3][2] = 0.5f*cos(theta);
		render.assignTransform(model, translation);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render.drawAll();
		//render.draw(square);

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	printf("End rendering test\n");
}

// A quick example of how to use this
int main() {
	GLFWwindow* window = initializeWindow();
	if (window == NULL)
		return -1;

	//inputTest(window);
	physicsTest();
	renderTest(window);


	return 0;
}
