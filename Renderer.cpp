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
	color = vec3(1.0, 1.0, 1.0);
	deleted = false;

	transform = mat4(1.0);
}

Renderer::LightInfo::LightInfo() : pos(vec3(0.0, 0.0, 0.0)), deleted(false)
{

}

Renderer::Renderer(GLFWwindow* _window) :window(_window)
{
	printf("Renderer constructor\n");
	window = _window;

	glGenVertexArrays(VAO::COUNT, vao);
	glGenBuffers(VBO::COUNT, vbo);

	glClearColor(1.0, 1.0, 1.0, 1.0);

	initializeVAOs();

}

Renderer::~Renderer()
{
	glDeleteVertexArrays(VAO::COUNT, vao);
	glDeleteBuffers(VBO::COUNT, vbo);
}

void Renderer::renderLoop()
{
	printf("Enter function\n");
	while (!glfwWindowShouldClose(window))
	{
		//Clear color buffer  
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawAll();

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

void Renderer::assignColor(unsigned int id, vec3 color)
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

void Renderer::assignMaterial(unsigned int id, Material* mat)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].mat = mat;
}

/**
* Lights
**/

unsigned int Renderer::generateLightObject()
{
	lights.push_back(LightInfo());

	return lights.size() - 1;
}

void Renderer::setLightPosition(unsigned int id, vec3 lightPos)
{
	if ((id >= objects.size()) || (lights[id].deleted))
		return;
	lights[id].pos = lightPos;
}




/**
* Rendering functionality
**/
void Renderer::clearDrawBuffers()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::draw(unsigned int id)
{
	ObjectInfo object = objects[id];
	if (object.deleted)
		return;

	LightInfo light;
	if (lights.size() < 1)		//If no light, add a default light to scene
		light = LightInfo();
	else
		light = lights[0];		//Else, use first light in array

	object.mat->useShader();
	object.mat->loadUniforms(projection*modelView*object.transform, light.pos, object.color);
	loadBuffers(object);

	if (object.indices != NULL)
		glDrawElements(GL_TRIANGLES, object.indices->size(), GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, object.mesh->size());
	
}

void Renderer::draw(vector<unsigned int> list)
{
	for (unsigned int i = 0; i < list.size(); i++)
	{
		draw(list[i]);
	}
}

void Renderer::drawAll()
{
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		draw(i);
	}
}


bool Renderer::loadBuffers(const ObjectInfo& object)
{
	bool success = false;

	if ((object.mat->usingVertices()) && (object.mat->usingNormals()) && (object.mat->usingUvs()))
		success = loadVertNormalUVBuffer(object);
	else if ((object.mat->usingVertices()) && (object.mat->usingNormals()))
		success = loadVertNormalBuffer(object);
	else if ((object.mat->usingVertices()) && (object.mat->usingUvs()))
		success = loadVertUVBuffer(object);
	else if (object.mat->usingVertices())
		success = loadVertBuffer(object);
	else
	{
		debug_message("loadBuffers: Invalid material file\n");
	}

	if (object.indices == NULL)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::INDICES]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(unsigned int)*object.indices->size(),
			&object.indices->at(0),
			GL_STATIC_DRAW);
	}

	return success;
}

bool Renderer::loadVertBuffer(const ObjectInfo& object)
{
	if (object.mesh == NULL){
		debug_message("loadVertBuffer: Mesh pointer invalid\n");
		return false;
	}

	glBindVertexArray(vao[VAO::VERT]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERT]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*object.mesh->size(),
		&object.mesh->at(0),
		GL_STATIC_DRAW);

	return true;
}

bool Renderer::loadVertNormalBuffer(const ObjectInfo& object)
{
	if (object.mesh == NULL){
		debug_message("loadVertNormalBuffer: Mesh pointer invalid\n");
		return false;
	}
	if (object.normals == NULL){
		debug_message("loadVertNormalBuffer: Normal pointer invalid\n");
		return false;
	}
		
	glBindVertexArray(vao[VAO::VERT_NORMALS]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERT]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*object.mesh->size(),
		&object.mesh->at(0),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*object.normals->size(),
		&object.normals->at(0),
		GL_STATIC_DRAW);

	return true;
}

bool Renderer::loadVertUVBuffer(const ObjectInfo& object)
{
	if (object.mesh == NULL){
		debug_message("loadVertUVBuffer: Mesh pointer invalid\n");
		return false;
	}
	if (object.uvs == NULL){
		debug_message("loadVertUVBuffer: UV pointer invalid\n");
		return false;
	}

	glBindVertexArray(vao[VAO::VERT_UVS]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERT]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*object.mesh->size(),
		&object.mesh->at(0),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec2)*object.uvs->size(),
		&object.uvs->at(0),
		GL_STATIC_DRAW);

	return true;
}

bool Renderer::loadVertNormalUVBuffer(const ObjectInfo& object)
{
	if (object.mesh == NULL){
		debug_message("loadVertNormalUVBuffer: Mesh pointer invalid\n");
		return false;
	}
	if (object.normals == NULL){
		debug_message("loadVertNormalUVBuffer: Normal pointer invalid\n");
		return false;
	}
	if (object.uvs == NULL){
		debug_message("loadVertNormalUVBuffer: UV pointer invalid\n");
		return false;
	}
		

	glBindVertexArray(vao[VAO::VERT_NORMALS_UVS]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERT]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*object.mesh->size(),
		&object.mesh->at(0),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*object.normals->size(),
		&object.normals->at(0),
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec2)*object.uvs->size(),
		&object.uvs->at(0),
		GL_STATIC_DRAW);

	return true;
}

void Renderer::initializeVAOs()
{
	//Vertex only VAO
	glBindVertexArray(vao[VAO::VERT]);		//Bind vertex array
	//Vertex vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERT]);
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	//Vertex and Normal VAO
	glBindVertexArray(vao[VAO::VERT_NORMALS]);		//Bind vertex array
	//Vertex vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERT]);
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	//Normal vbo
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glVertexAttribPointer(
		1,				//Attribute
		3,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	//Vertex and UV VAO
	glBindVertexArray(vao[VAO::VERT_UVS]);		//Bind vertex array
	//Vertex vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERT]);
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	//UV vbo
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glVertexAttribPointer(
		1,				//Attribute
		2,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec2),	//Stride
		(void*)0			//Offset
		);

	
	//Vertex, Normal and UV VAO
	glBindVertexArray(vao[VAO::VERT_NORMALS_UVS]);	//Bind vertex array
	//Vertex vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERT]);
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	//Normal vbo
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glVertexAttribPointer(
		1,				//Attribute
		3,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	//UV vbo
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glVertexAttribPointer(
		2,				//Attribute
		2,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec2),	//Stride
		(void*)0			//Offset
		);
}


/**
* Preset mesh generation functions
**/

void Renderer::assignCube(	unsigned int id, float width,
							vector<vec3>* mesh,
							vector<vec3>* normals,
							vector<unsigned int>* indices)
{

	float halfWidth = 0.5*width;
	//Front face
	mesh->push_back(vec3(-halfWidth, halfWidth, halfWidth));
	mesh->push_back(vec3(-halfWidth, -halfWidth, halfWidth));
	mesh->push_back(vec3(halfWidth, -halfWidth, halfWidth));
	mesh->push_back(vec3(halfWidth, halfWidth, halfWidth));
	/*//Back face
	mesh.push_back(vec3(-halfWidth, halfWidth, -halfWidth));
	mesh.push_back(vec3(halfWidth, halfWidth, -halfWidth));
	mesh.push_back(vec3(halfWidth, -halfWidth, -halfWidth));
	mesh.push_back(vec3(-halfWidth, -halfWidth, -halfWidth));*/

	normals->push_back(vec3(0.0, 0.0, 1.0));
	normals->push_back(vec3(0.0, 0.0, 1.0));
	normals->push_back(vec3(0.0, 0.0, 1.0));
	normals->push_back(vec3(0.0, 0.0, 1.0));

	*indices = { 0, 1, 2, 2, 3, 0 };

	assignMesh(id, mesh);
	assignNormals(id, normals);
	assignIndices(id, indices);
	assignColor(id, vec3(1.0, 0.0, 0.0));

}



/**
* Debugging
**/

void Renderer::debug_message(string message)
{
	if (true)
		printf("Renderer>> %s\n", message.c_str());
}

#endif // RENDERER_CPP