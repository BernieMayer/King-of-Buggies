#ifndef RENDERER_CPP
#define RENDERER_CPP

#include "Renderer.h"

void glErrorCheck(const char* location)
{
	GLenum code = glGetError();

	switch (code)
	{
	case GL_INVALID_ENUM:
		cout << "GL_INVALID_ENUM - " << location << endl;
		break;
	case GL_INVALID_VALUE:
		cout << "GL_INVALID_VALUE - " << location << endl;
		break;
	case GL_INVALID_OPERATION:
		cout << "GL_INVALID_OPERATION - " << location << endl;
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		cout << "GL_INVALID_FRAMEBUFFER_OPERATION - " << location << endl;
		break;
	case GL_OUT_OF_MEMORY:
		cout << "GL_OUT_OF_MEMORY - " << location << endl;
		break;
	}
}


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

Renderer::Renderer(GLFWwindow* _window) :window(_window), debugging(false)
{
	shaderList.initShaders();

	window = _window;

	glGenVertexArrays(VAO::COUNT, vao);
	glGenBuffers(VBO::COUNT, vbo);

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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

void Renderer::assignTransform(unsigned int id, const mat4& transform)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].transform = transform;
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
* Transformations
**/

void Renderer::loadModelviewTransform(const mat4& _modelview)
{
	modelview = _modelview;
}

void Renderer::loadProjectionTransform(const mat4& _projection)
{
	projection = _projection;
}

void Renderer::loadPerspectiveTransform(float n, float f, float fov)
{
	float angle = fov*M_PI / 180.f;

	float width = tan(angle*0.5f)*n;
	float height = tan(angle*0.5f)*n;

	printf("Width %f\nHeight %f\n", width, height);

	mat4 perspective = mat4(1.0);

	perspective[0][0] = n / width;
	perspective[1][1] = n/ height;
	perspective[2][2] = -(f + n) / (f - n);
	perspective[3][2] = -2*f*n / (f - n);
	perspective[2][3] = -1.f;
	perspective[3][3] = 0.f;

	projection = perspective;
}

void Renderer::loadCamera(Camera* _camera)
{
	camera = _camera;
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

	ObjectInfo& object = objects[id];
	if (object.deleted)
		return;

	LightInfo light;
	if (lights.size() < 1)		//If no light, add a default light to scene
		light = LightInfo();
	else
		light = lights[0];		//Else, use first light in array

	object.mat->useShader();

	if (camera != NULL)
		object.mat->loadUniforms(projection*modelview*camera->getMatrix()*object.transform, object.transform, 
						camera->getPos(), light.pos, object.color);	
	else
		object.mat->loadUniforms(projection*modelview*object.transform, object.transform,
						vec3(0.0, 0.0, 0.0), light.pos, object.color);


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

	if (object.indices != NULL)
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
	mesh->push_back(vec3(-halfWidth, halfWidth, 0.f));
	mesh->push_back(vec3(-halfWidth, -halfWidth, 0.f));
	mesh->push_back(vec3(halfWidth, -halfWidth, 0.f));
	mesh->push_back(vec3(halfWidth, halfWidth, 0.f));
	/*//Back face
	mesh.push_back(vec3(-halfWidth, halfWidth, -halfWidth));
	mesh.push_back(vec3(halfWidth, halfWidth, -halfWidth));
	mesh.push_back(vec3(halfWidth, -halfWidth, -halfWidth));
	mesh.push_back(vec3(-halfWidth, -halfWidth, -halfWidth));*/

	normals->push_back(vec3(0.0, 0.0, -1.0));
	normals->push_back(vec3(0.0, 0.0, -1.0));
	normals->push_back(vec3(0.0, 0.0, -1.0));
	normals->push_back(vec3(0.0, 0.0, -1.0));

	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);
	indices->push_back(2);
	indices->push_back(3);
	indices->push_back(0);

	assignMesh(id, mesh);
	assignNormals(id, normals);
	assignIndices(id, indices);
	assignColor(id, vec3(1.0, 0.0, 0.0));

}

void Renderer::assignSphere(unsigned int id, float radius, int divisions,
	vector<vec3>* mesh,
	vector<vec3>* normals,
	vector<unsigned int>* indices)
{

	mesh->clear();
	normals->clear();
	indices->clear();

	int yDivisions = divisions;
	int xDivisions = 2*divisions;

	float uInc = 2.f*M_PI/(float)xDivisions;
	float vInc = M_PI / (float)yDivisions;

	//Front face
	float u = 0.f;
	for (int i = 0; i <= xDivisions; i++)
	{
		float x_base = cos(u);
		float z_base = sin(u);

		float v = -M_PI*0.5;
		for (int j = 0; j <= yDivisions; j++)
		{

			float x = x_base*cos(v);
			float z = z_base*cos(v);
			float y = sin(v);
			mesh->push_back(vec3(x, y, z)*radius);
			normals->push_back(normalize(vec3(x, y, z)));

			v += vInc;
		}
		u += uInc;
	}

	for (unsigned int i = 0; i < xDivisions; i++)
	{
		for (unsigned int j = 0; j < yDivisions; j++)
		{
			indices->push_back(i*(yDivisions + 1) + j);
			indices->push_back(i*(yDivisions + 1) + j+1);
			indices->push_back((i+1)*(yDivisions + 1) + j+1);
			indices->push_back(i*(yDivisions + 1) + j);
			indices->push_back((i+1)*(yDivisions + 1) + j+1);
			indices->push_back((i+1)*(yDivisions + 1) + j);
		}
	}

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