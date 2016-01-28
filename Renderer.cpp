#ifndef RENDERER_CPP
#define RENDERER_CPP

#include "Renderer.h"

Renderer::ObjectInfo::ObjectInfo()
{ 
	mesh = NULL;
	normals = NULL;
	uvs = NULL;
	indices = NULL;
	mat = NULL;
	shadowBehaviour = SHADOW_BEHAVIOUR::NONE;
	color = vec4(1.0, 1.0, 1.0, 1.0);
	deleted = false;

	vec3 position = vec3(0.0, 0.0, 0.0);
	vec3 forward = vec3(0.0, 0.0, 1.0);
	vec3 up = vec3(0.0, 1.0, 0.0);
}

Renderer::Renderer(GLFWwindow* _window) :window(_window)
{
	printf("Renderer constructor\n");
	window = _window;
}

void Renderer::renderLoop()
{
	printf("Enter function\n");
	while (!glfwWindowShouldClose(window))
	{
		//Clear color buffer  
		glClear(GL_COLOR_BUFFER_BIT);

		//Swap buffers  
		glfwSwapBuffers(window);
		//Get and organize events, like keyboard and mouse input, window resizing, etc...  
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}


/**
* Object assignment functions
**/
unsigned int Renderer::generateObjectID()
{
	objects.push_back(ObjectInfo());

	return objects.size() - 1;		//Handle for the object
}

void Renderer::assignMesh(unsigned int id, vector<vec3>* mesh)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].mesh = mesh;
}

void Renderer::assignNormals(unsigned int id, vector<vec3>* normals)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].normals = normals;
}

void Renderer::assignUVs(unsigned int id, vector<vec2>* uvs)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].uvs = uvs;
}

void Renderer::assignColor(unsigned int id, const vec4& color)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].color = color;
}

void Renderer::assignIndices(unsigned int id, vector<unsigned int>* indices)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].indices = indices;
}





/**
* Rendering functionality
**/




#endif // RENDERER_CPP