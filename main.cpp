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

		if (input.turnL != turn) {
			turn = input.turnL;
			std::cout << "Turn: " << turn << "\n";
		}
	}
}


void physicsTest() {
	Physics physics = Physics();

	for (int i = 0; i < 100; i++) {
		physics.startSim(GameState());
		physics.getSim();
	}

	physics.shutdown();
}

void physicsAndRenderTest(GLFWwindow* window)
{
	printf("Begin physics and rendering test\n");

	Physics physics = Physics();

	Renderer render = Renderer(window);
	MeshInfo meshLoader = MeshInfo();

	// Object creation
	Diffuse mat = Diffuse();
	Specular shinyMat = Specular(20.f);
	TorranceSparrow tsMat = TorranceSparrow(3.f);

	//Create plane
	unsigned int plane = render.generateObjectID();
	vector<vec3> meshPlane;
	vector<vec3> normalsPlane;
	vector<unsigned int> indicesPlane;
	render.assignPlane(plane, 8.f, &meshPlane, &normalsPlane, &indicesPlane);
	render.assignMaterial(plane, &tsMat);
	render.assignColor(plane, vec3(0.5f, 0.5f, 0.5f));

	//Create sphere
	vector<vec3> mesh;
	vector<vec3> normals;
	vector<unsigned int> indices;
	
	unsigned int sphere = render.generateObjectID();
	render.assignSphere(sphere, 0.5f, 20, &mesh, &normals, &indices);
	render.assignMaterial(sphere, &tsMat);

	MeshObject carMesh = meshLoader.getMesh(CUBE);
	vector<vec3> carVerts = carMesh.getVertices();
	vector<vec3> carNormals = carMesh.getNormals();
	vector<unsigned int> carIndices = carMesh.getIndices();

	unsigned int car = render.generateObjectID();
	render.assignMesh(car, &carVerts);
	render.assignNormals(car, &carNormals);
	render.assignIndices(car, &carIndices);
	render.assignMaterial(car, &tsMat);


	printf("Vertices = %d, Normals %d, Indices %d\n", (carMesh.getVertices()).size()
		, (carMesh.getNormals()).size(), (carMesh.getIndices()).size());


	//Light creation
	vec3 lightPos(5.0, 5.0, 5.0);
	unsigned int light = render.generateLightObject();
	render.setLightPosition(light, lightPos);


	InputManager im(window);

	//Starting direction, up vector, starting position
	Camera cam(vec3(0.0, 0.0, -1.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 5.0),
		MODELVIEWER_CAMERA);

	render.loadPerspectiveTransform(0.1f, 20.f, 90.f);		//Near, far, fov

	render.loadCamera(&cam);

	mat4 translation;
	float theta = 0.0;

	mat4 carScaling(1.f);
	carScaling[0][0] = 1.25f;
	carScaling[1][1] = 1.f;
	carScaling[2][2] = 2.f;

	while (!glfwWindowShouldClose(window))
	{
		//Physics sim
		physics.startSim(GameState());
		physics.getSim();

		Input in = im.getInput(1);		//Get input

		float scale = 1.f;
		//cam.rotateView(in.turn*scale, in.forward*scale);
		if (in.powerup)
			cam.rotateView(-in.camH*scale, in.camV*scale);
		if (in.drift)
			cam.zoom(in.camV*0.95f + 1.f);

		physx::PxRigidActor* sphereActor = getSphere();
		physx::PxRigidDynamic* carActor = getCar();

		render.assignTransform(sphere, getMat4(sphereActor->getGlobalPose()));
		render.assignTransform(car, getMat4(carActor->getGlobalPose())*carScaling);
		//displayMat4(getMat4(sphereActor->getGlobalPose()));

		render.clearDrawBuffers();

		render.drawAll();

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	printf("End rendering test\n");
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
	TorranceSparrow tsMat = TorranceSparrow(8.f);
	
	unsigned int model = render.generateObjectID();
	
	mesh = meshLoader.getMeshVertices(BRICK);
	normals = meshLoader.getMeshNormals(BRICK);
	indices = meshLoader.getMeshIndices(BRICK);

	render.assignMesh(model, &mesh);
	render.assignNormals(model, &normals);
	render.assignIndices(model, &indices);
	render.assignMaterial(model, &tsMat);
	render.assignColor(model, vec3(1.0, 0.0, 0.0));
	
	vector<vec3> mesh2;
	vector<vec3> normals2;
	vector<unsigned int> indices2;
	mat4 translation2;
	translation2[3][2] = -0.7f;

	unsigned int disco = render.generateObjectID();
	mesh2 = meshLoader.getMeshVertices(SPHERE);
	normals2 = meshLoader.getMeshNormals(SPHERE);
	indices2 = meshLoader.getMeshIndices(SPHERE);

	render.assignMesh(disco, &mesh2);
	render.assignNormals(disco, &normals2);
	render.assignIndices(disco, &indices2);
	render.assignMaterial(disco, &tsMat);
	render.assignColor(disco, vec3(1.0, 0.0, 1.0));
	render.assignTransform(disco, translation2);

	vector<vec3> mesh3;
	vector<vec3> normals3;
	vector<unsigned int> indices3;

	unsigned int plane = render.generateObjectID();
	mesh3 = meshLoader.getMeshVertices(PLANE);
	normals3 = meshLoader.getMeshNormals(PLANE);
	indices3 = meshLoader.getMeshIndices(PLANE);

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

		float scale = 0.001f;
		float forward = in.forward - in.backward;
		float turn = in.turnR - in.turnL;
		cam.rotateView(turn*scale, forward*scale);
		
		theta += 0.0001f;
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
	//physicsTest();
	//renderTest(window);

	physicsAndRenderTest(window);


	return 0;
}
