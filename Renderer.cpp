#ifndef RENDERER_CPP
#define RENDERER_CPP

#include "Renderer.h"

int windowWidth;
int windowHeight;

mat4 winRatio (1.f);		//Only to be modified by resizeFunc

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

unsigned int Renderer::getHeight() { return windowHeight; }
unsigned int Renderer::getWidth() { return windowWidth; }


Renderer::ObjectInfo::ObjectInfo(): 
	mesh(NULL),
	normals(NULL),
	uvs(NULL),
	indices(NULL),
	mat(NULL),
	shadowBehaviour(SHADOW_BEHAVIOUR::NONE),
	color(1.f, 1.f, 1.f),
	deleted(false),
	texID(NO_VALUE),
	transform(1.f),
	scaling(1.f),
	bufferedIndex(NO_VALUE)
{
}

Renderer::LightInfo::LightInfo() : pos(vec3(0.f, 0.f, 0.f)), deleted(false)
{

}

Renderer::ObjectMesh::ObjectMesh(vector<vec3>* verts, vector<vec3>* normals, vector<vec2>* uvs, vector<unsigned int>* indices, bool hasTexture) :
verts(verts), normals(normals), uvs(uvs), indices(indices), hasTexture(hasTexture), offset(NO_VALUE)
{
	
}

bool Renderer::ObjectMesh::operator ==(const ObjectMesh& other) const
{
	return (verts == other.verts) && (normals == other.normals) && (uvs == other.uvs) && (hasTexture == other.hasTexture);
}

Renderer::Renderer(GLFWwindow* _window) : window(_window), debugging(false), 
projection(1.f), modelview(1.f), activeViewport(0), shadowTexUnit(0)
{
	shaderList.initShaders();

	window = _window;
	glfwGetWindowSize(window, &windowWidth, &windowHeight);

	glfwSetWindowSizeCallback(window, resizeFunc);

	glGenVertexArrays(VAO::COUNT, vao);
	glGenBuffers(VBO::COUNT, vbo);

	glEnable(GL_DEPTH_TEST);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClearColor(1.f, 1.f, 1.f, 1.f);

	addViewport(0.f, 0.f, 1.f, 1.f);

	generatePointsOnDisk();		//Generates poisson disk points

	initializeVAOs();

	initializeBufferedVAOs();
	
}



Renderer::~Renderer()
{
	glDeleteVertexArrays(VAO::COUNT, vao);
	glDeleteBuffers(VBO::COUNT, vbo);
}


/**
* Object assignment functions
**/
unsigned int Renderer::generateObjectID()
{
	objects.push_back(ObjectInfo());

	newObjects.push_back(objects.size() - 1);

	return objects.size() - 1;		//Handle for the object
}

void Renderer::assignMeshObject(unsigned int id, MeshObject* mesh)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;

	assignMesh(id, mesh->getVertexPointer());
	assignNormals(id, mesh->getNormalPointer());
	assignUVs(id, mesh->getUvPointer());
	assignIndices(id, mesh->getIndexPointer());
}

void Renderer::assignMesh(unsigned int id, vector<vec3>* mesh)
{
	if ((id >= objects.size()) || (objects[id].deleted) || (mesh == NULL))
		return;
	objects[id].mesh = mesh;
}

void Renderer::assignNormals(unsigned int id, vector<vec3>* normals)
{
	if ((id >= objects.size()) || (objects[id].deleted) || (normals == NULL))
		return;
	objects[id].normals = normals;
}

void Renderer::assignUVs(unsigned int id, vector<vec2>* uvs)
{
	if ((id >= objects.size()) || (objects[id].deleted) || (uvs == NULL))
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
	if ((id >= objects.size()) || (objects[id].deleted) || (indices == NULL))
		return;
	objects[id].indices = indices;
}

void Renderer::assignTexture(unsigned int objectID, unsigned int textureID)
{
	objects[objectID].texID = textureID;
}

void Renderer::assignMaterial(unsigned int id, Material* mat)
{
	if ((id >= objects.size()) || (objects[id].deleted) || (mat == NULL))
		return;
	objects[id].mat = mat;
}

void Renderer::assignTransform(unsigned int id, const mat4& transform)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].transform = transform;
}

void Renderer::assignScale(unsigned int id, const mat4& scaling)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].scaling = scaling;
}

void Renderer::setShadowBehaviour(unsigned int id, unsigned int behaviour)
{
	if ((id >= objects.size()) || (objects[id].deleted))
		return;
	objects[id].shadowBehaviour = behaviour;
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
* Framebuffers
**/

mat4 Framebuffer::ratioMatrix()
{
	mat4 ratio(1.f);

	float minDim = min((float)width,  (float)height);

	ratio[0][0] = (float)width / minDim;
	ratio[1][1] = (float)height / minDim;

	return ratio;
}

unsigned int Renderer::createDepthbuffer(unsigned int width, unsigned int height)
{
	GLuint fb, fb_texture;
	glGenFramebuffers(1, &fb);
	glGenTextures(1, &fb_texture);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	glBindTexture(GL_TEXTURE_2D, fb_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb_texture, 0);

	//glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "Framebuffer initialization failed" << endl;

	//Return to normal frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_FRONT);

	fbo.push_back(Framebuffer(fb, fb_texture, width, height, Framebuffer::DEPTH));

	return fbo.size() - 1;
}

unsigned int Renderer::createFramebuffer(unsigned int width, unsigned int height)
{
	GLuint fb, fb_texture;
	glGenFramebuffers(1, &fb);
	glGenTextures(1, &fb_texture);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	glBindTexture(GL_TEXTURE_2D, fb_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_texture, 0);

	//glDrawBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "Framebuffer initialization failed" << endl;

	//Return to normal frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawBuffer(GL_FRONT);

	fbo.push_back(Framebuffer(fb, fb_texture, width, height, Framebuffer::COLOR));

	return fbo.size() - 1;
}

void Renderer::useFramebuffer(unsigned int id)
{
	if (id == NO_VALUE)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDrawBuffer(GL_FRONT);
		useViewport(activeViewport);
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fbo[id].id);
		//glDrawBuffer(GL_NONE);
		glViewport(0, 0, fbo[id].width, fbo[id].height);
	}

	activeFramebuffer = id;
}


/**
* Transformations
**/


mat4 perspectiveMatrix(float n, float f, float fov)
{
	float angle = fov*M_PI / 180.f;

	float width = tan(angle*0.5f)*n;
	float height = tan(angle*0.5f)*n;

	//printf("Width %f\nHeight %f\n", width, height);

	mat4 perspective = mat4(1.0);

	perspective[0][0] = n / width;
	perspective[1][1] = n / height;
	perspective[2][2] = -(f + n) / (f - n);
	perspective[3][2] = -2 * f*n / (f - n);
	perspective[2][3] = -1.f;
	perspective[3][3] = 0.f;

	return perspective;
}

mat4 orthographicMatrix(float n, float f, float width, float height)
{
	mat4 orthographic = mat4(1.f);

	orthographic[0][0] = 2.f / width;
	orthographic[1][1] = 2.f / height;
	orthographic[2][2] = 1.f / (f - n);
	orthographic[3][2] = -n / (f - n);

	return orthographic;
}

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
	loadProjectionTransform(perspectiveMatrix(n, f, fov));
}

void Renderer::loadOrthographicTransform(float n, float f, float width, float height)
{
	loadProjectionTransform(orthographicMatrix(n, f, width, height));
}

void Renderer::loadPerspectiveTransformShadow(float n, float f, float fov)
{
	loadProjectionTransform(perspectiveMatrix(n, f, fov));
}

void Renderer::loadOrthographicTransformShadow(float n, float f, float width, float height)
{
	loadProjectionTransform(orthographicMatrix(n, f, width, height));
}


void Renderer::loadCamera(Camera* _camera)
{
	camera = _camera;
}

void Renderer::LightInfo::positionCamera(vec3 sceneCenter, float boundingRadius)
{
	cam = Camera(normalize(pos), vec3(0.f, 1.f, 0.f), sceneCenter + pos);

	projection = orthographicMatrix(length(pos+sceneCenter) - boundingRadius, length(pos+sceneCenter) + boundingRadius, 2.f*boundingRadius, 2.f*boundingRadius);
}

void Renderer::LightInfo::positionCamera(vec3 sceneCenter, float nearP, float farP, float width, float height)
{
	cam = Camera(normalize(pos), vec3(0.f, 1.f, 0.f), sceneCenter + pos);

	projection = orthographicMatrix(length(pos + sceneCenter) + nearP, length(pos + sceneCenter) + farP, width, height);
}

void Renderer::positionLightCamera(unsigned int lightID, vec3 sceneCenter, float boundingRadius)
{
	lights[lightID].positionCamera(sceneCenter, boundingRadius);
}

void Renderer::positionLightCamera(unsigned int lightID, vec3 sceneCenter, float nearP, float farP, float width, float height)
{
	lights[lightID].positionCamera(sceneCenter, nearP, farP, width, height);
}


void Renderer::updateObjectTransforms(GameState* state)
{
	for (unsigned int i = 0; i < state->numberOfPlayers(); i++)
	{
		PlayerInfo* player = state->getPlayer(i);
		assignTransform(player->getRenderID(), player->getTransform());

		for (unsigned int j = 0; j < 4; j++)
		{
			assignTransform(player->getWheelRenderID(j), player->getWheelTransform(j));
		}
	}

	for (unsigned int i = 0; i < state->numberOfPowerups(); i++)
	{
		Powerup* powerup = state->getPowerup(i);
		assignTransform(powerup->getRenderID(), powerup->getTransform());
	}

	for (unsigned int i = 0; i < state->numberOfCoins(); i++)
	{
		Coin* coin = state->getCoin(i);
		assignTransform(coin->getRenderID(), coin->getTransform());
	}

	for (unsigned int i = 0; i < state->numberOfMines(); i++)
	{
		Mine* mine = state->getMine(i);
		assignTransform(mine->getRenderID(), mine->getTransform());
	}

	for (unsigned int i = 0; i < state->numberOfBoostPads(); i++)
	{
		BoostPad* boost = state->getBoostPad(i);
		assignTransform(boost->getRenderID(), boost->getTransform());
	}

	for (unsigned int i = 0; i < state->numberOfPowerupBoxes(); i++)
	{
		PowerupBox* box = state->getPowerupBox(i);
		assignTransform(box->getRenderID(), box->getTransform());
	}
}

/**
* Viewports
**/

Viewport::Viewport(float x, float y, float width, float height) :x(x), y(y), width(width), height(height), viewRatio(1.f) 
{
	float minDim = min(width, height);
	viewRatio[0][0] = minDim / width;
	viewRatio[1][1] = minDim / height;
}

unsigned int Renderer::addViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
	//viewports.push_back(Viewport(x, y, width, height));
	return viewports.size() - 1;
}

unsigned int Renderer::addViewport(float x, float y, float width, float height)
{
	/*unsigned int x_int = (unsigned int)(x*(float)windowWidth);
	unsigned int y_int = (unsigned int)(y*(float)windowHeight);
	unsigned int width_int = (unsigned int)(width*(float)windowWidth);
	unsigned int height_int = (unsigned int)(height*(float)windowHeight);

	return addViewport(x_int, y_int, width_int, height_int);*/
	viewports.push_back(Viewport(x, y, width, height));

	return viewports.size() - 1;
}

void Renderer::splitScreenViewports(unsigned int numViewports)
{
	switch (numViewports)
	{
	case 1:
		addViewport(0.f, 0.f, 1.f, 1.f);
		break;
	case 2:
		addViewport(0.f, 0.5f, 1.f, 0.5f);
		addViewport(0.f, 0.f, 1.f, 0.5f);
		break;
	case 3:
		addViewport(0.f, 0.5f, 0.5f, 0.5f);
		addViewport(0.5f, 0.5f, 0.5f, 0.5f);
		addViewport(0.f, 0.f, 1.f, 0.5f);
		break;
	case 4:
		addViewport(0.f, 0.5f, 0.5f, 0.5f);
		addViewport(0.5f, 0.5f, 0.5f, 0.5f);
		addViewport(0.f, 0.f, 0.5f, 0.5f);
		addViewport(0.5f, 0.f, 0.5f, 0.5f);
		break;
	}


}

void Renderer::useViewport(unsigned int viewport)
{
	Viewport& vp = viewports[viewport];

	unsigned int x_int = (unsigned int)(vp.x*(float)windowWidth);
	unsigned int y_int = (unsigned int)(vp.y*(float)windowHeight);
	unsigned int width_int = (unsigned int)(vp.width*(float)windowWidth);
	unsigned int height_int = (unsigned int)(vp.height*(float)windowHeight);

	glViewport(x_int, y_int, width_int, height_int);
	activeViewport = viewport;
}

/**
* Rendering functionality
**/
void Renderer::clearDrawBuffers(vec3 color)
{
	glClearColor(color.x, color.y, color.z, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::loadShadowMap(unsigned int texID)
{
	glActiveTexture(GL_TEXTURE0 + shadowTexUnit);		//Move elsewhere! This is just temporary
	glBindTexture(GL_TEXTURE_2D, texID);

}

void Renderer::drawShadowMap(unsigned int id, unsigned int lightID)
{

	ObjectInfo& object = objects[id];
	if (object.deleted || !(object.shadowBehaviour & SHADOW_BEHAVIOUR::CAST) )
		return;

	LightInfo light;
	if (lights.size() < 1)		//If no light, add a default light to scene
		light = LightInfo();
	else
		light = lights[0];		//Else, use first light in array

	shadow.useShader();		//Use shadow map shader

	//Object
	mat4 cameraMatrix = lights[lightID].cam.getMatrix();
	shadowProjection = lights[lightID].projection*modelview*cameraMatrix;
	mat4 projectionMatrix = shadowProjection*object.transform*object.scaling;
	mat4 modelviewMatrix = object.transform*object.scaling;

	vec4 projectedPosition = projectionMatrix*vec4(4.f, 0.f, 0.f, 1.f);
	vec3 normalizedPosition = vec3(projectedPosition) / projectedPosition.w;

	shadow.loadUniforms(projectionMatrix, modelviewMatrix);

	//loadBuffers(object);
	bindBufferedVAO(object);		//Change

	if (object.indices != NULL)
		glDrawElements(GL_TRIANGLES, object.indices->size(), GL_UNSIGNED_INT, (void*)(distinctMeshes[object.bufferedIndex].offset*sizeof(unsigned int)));
	else
		glDrawArrays(GL_TRIANGLES, 0, object.mesh->size());
}

void Renderer::drawShadowMapAll(unsigned int lightID)
{
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		drawShadowMap(i, lightID);
	}
}

void Renderer::drawAllWithShadows(unsigned int shadowTexture, unsigned int lightID)
{
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		if (objects[i].shadowBehaviour & SHADOW_BEHAVIOUR::RECEIVE)
			drawWithShadows(i, shadowTexture, lightID);
		else
			draw(i);
	}
}

void Renderer::drawWithShadows(unsigned int id, unsigned int shadowTexture, unsigned int lightID)
{
	ObjectInfo& object = objects[id];
	if (object.deleted)
		return;

	LightInfo light;
	if (lights.size() < 1)		//If no light, add a default light to scene
		light = LightInfo();
	else
		light = lights[0];		//Else, use first light in array

	//Make into ObjectInfo function?
	if (objects[id].texID == NO_VALUE)
		cout << "No texID" << endl;
	else
		object.mat->useShadowShader();

	//Object
	mat4 cameraMatrix = (camera != NULL) ? camera->getMatrix() : mat4(1.f);
	vec3 viewPos = (camera != NULL) ? camera->getPos() : vec3(0.f);
	mat4 projectionMatrix = winRatio*viewports[activeViewport].viewRatio*projection*modelview*cameraMatrix*object.transform*object.scaling;
	mat4 modelviewMatrix = object.transform*object.scaling;
	mat4 shadowMatrix = biasMatrix()*shadowProjection*modelviewMatrix;

	if ((objects[id].texID == NO_VALUE) || (objects[id].uvs == NULL))
		cout << "No texID or uvs" << endl;
	else
		object.mat->loadUniforms(projectionMatrix, modelviewMatrix, shadowMatrix,
		viewPos, light.pos, objects[id].texID, 1, shadowTexUnit, randomPoints, RANDOM_POINT_NUM);

	loadBuffers(object);

	if (object.indices != NULL)
		glDrawElements(GL_TRIANGLES, object.indices->size(), GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, object.mesh->size());

	glErrorCheck("drawWithShadows - end function");
	//*/

	/*object.mat->useTextureShader();

	//Object
	mat4 cameraMatrix = lights[0].cam.getMatrix();
	shadowProjection = lights[0].projection*modelview*cameraMatrix;
	mat4 projectionMatrix = shadowProjection*object.transform*object.scaling;
	mat4 modelviewMatrix = object.transform*object.scaling;

	vec4 projectedPosition = projectionMatrix*vec4(4.f, 0.f, 0.f, 1.f);
	vec3 normalizedPosition = vec3(projectedPosition) / projectedPosition.w;


	if ((objects[id].texID == NO_VALUE) || (objects[id].uvs == NULL))
		object.mat->loadUniforms(projectionMatrix, modelviewMatrix,
		light.pos, light.pos, object.color);
	else
		object.mat->loadUniforms(projectionMatrix, modelviewMatrix,
		light.pos, light.pos, objects[id].texID, 0);

	loadBuffers(object);

	if (object.indices != NULL)
		glDrawElements(GL_TRIANGLES, object.indices->size(), GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, object.mesh->size());
	//*/
}


void Renderer::draw(unsigned  int id)
{

	ObjectInfo& object = objects[id];
	if (object.deleted)
		return;

	LightInfo light;
	if (lights.size() < 1)		//If no light, add a default light to scene
		light = LightInfo();
	else
		light = lights[0];		//Else, use first light in array

	//Make into ObjectInfo function?
	if (objects[id].texID == NO_VALUE)
		object.mat->useShader();
	else
		object.mat->useTextureShader();


	//Object
	mat4 cameraMatrix = (camera != NULL) ? camera->getMatrix() : mat4(1.f);
	vec3 viewPos = (camera != NULL) ? camera->getPos() : vec3(0.f);
	mat4 projectionMatrix = winRatio*viewports[activeViewport].viewRatio*projection*modelview*cameraMatrix*object.transform*object.scaling;
	mat4 modelviewMatrix = object.transform*object.scaling;

	if ((objects[id].texID == NO_VALUE) || (objects[id].uvs == NULL))
		object.mat->loadUniforms(projectionMatrix, modelviewMatrix,
						viewPos, light.pos, object.color);
	else
		object.mat->loadUniforms(projectionMatrix, modelviewMatrix,
						viewPos, light.pos, objects[id].texID, 1);


	loadBuffers(object);

	if (object.indices != NULL)
		glDrawElements(GL_TRIANGLES, object.indices->size(), GL_UNSIGNED_INT, 0);
	else
		glDrawArrays(GL_TRIANGLES, 0, object.mesh->size());

	glBindVertexArray(0);
	glUseProgram(0);

	glErrorCheck("Draw - end function");
	
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

void Renderer::bindBufferedVAO(const ObjectInfo& object)
{
	if ((object.mesh != NULL) && (object.normals != NULL) && (object.uvs != NULL))
		glBindVertexArray(vao[VAO::VERT_NORMALS_UVS_BUFFERED]);
	else if ((object.mesh != NULL) && (object.normals != NULL))
		glBindVertexArray(vao[VAO::VERT_NORMALS_BUFFERED]);
	else if ((object.mesh != NULL) && (object.uvs != NULL))
		glBindVertexArray(vao[VAO::VERT_UVS_BUFFERED]);
	else if (object.mesh != NULL)
		glBindVertexArray(vao[VAO::VERT_BUFFERED]);
	else
		debug_message("VAO not bound");
}

void Renderer::drawBuffered(unsigned int id, bool useShadows)
{
	ObjectInfo& object = objects[id];
	if (object.deleted)
		return;

	LightInfo light;
	if (lights.size() < 1)		//If no light, add a default light to scene
		light = LightInfo();
	else
		light = lights[0];		//Else, use first light in array

	//Make into ObjectInfo function?
	if (objects[id].texID == NO_VALUE)
		object.mat->useShader();
	else if (useShadows && (object.shadowBehaviour & SHADOW_BEHAVIOUR::RECEIVE))
		object.mat->useShadowShader();
	else
		object.mat->useTextureShader();

	//Calculate transforms
	mat4 cameraMatrix = (camera != NULL) ? camera->getMatrix() : mat4(1.f);
	vec3 viewPos = (camera != NULL) ? camera->getPos() : vec3(0.f);
	mat4 projectionMatrix = winRatio*viewports[activeViewport].viewRatio*projection*modelview*cameraMatrix*object.transform*object.scaling;
	mat4 modelviewMatrix = object.transform*object.scaling;
	mat4 shadowMatrix = biasMatrix()*shadowProjection*modelviewMatrix;

	//Load uniforms
	if ((objects[id].texID == NO_VALUE) || (objects[id].uvs == NULL))
		object.mat->loadUniforms(projectionMatrix, modelviewMatrix,
		viewPos, light.pos, object.color);
	else if (useShadows && (object.shadowBehaviour & SHADOW_BEHAVIOUR::RECEIVE))
		object.mat->loadUniforms(projectionMatrix, modelviewMatrix, shadowMatrix,
		viewPos, light.pos, objects[id].texID, 1, shadowTexUnit, randomPoints, RANDOM_POINT_NUM);
	else
		object.mat->loadUniforms(projectionMatrix, modelviewMatrix,
		viewPos, light.pos, objects[id].texID, 1);

	bindBufferedVAO(object);

	if (object.indices != NULL)
		glDrawElements(GL_TRIANGLES, object.indices->size(), GL_UNSIGNED_INT, (void*)(distinctMeshes[object.bufferedIndex].offset*sizeof(unsigned int)));

	glBindVertexArray(0);
	glUseProgram(0);

	glErrorCheck("DrawBuffered - end function");
}

void Renderer::drawBufferedAll(bool useShadows)
{
	for (unsigned int i = 0; i < objects.size(); i++)
	{
		drawBuffered(i, useShadows);
	}
}

void Renderer::drawRadar(vector<vec2> radarVecs, vector<vec3> colours)
{
	//glViewport(windowWidth * 0.75, windowHeight * 0.75, windowWidth * 0.25, windowHeight * 0.25);
	
	glUseProgram(0);

	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, 0, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_TRIANGLES);
	glColor3f(colours[0].x, colours[0].y, colours[0].z);
	
	int j = 1;
	for (int i = 0; i < radarVecs.size(); i++){
		vec3 vec = vec3((windowWidth / 2) - radarVecs[i].x * (windowWidth / 12), (windowHeight / 2) - radarVecs[i].y * (windowHeight / 12), 0);
		glVertex3f(vec.x, vec.y, vec.z);
		
		if (i ==2){
			glColor3f(colours[j].x, colours[j].y, colours[j].z);
			j++;
			//glColor3f(1.f, 0.64f, 0.f);
		}
		else if (i == 5)
		{
			glColor3f(colours[j].x, colours[j].y, colours[j].z);
			j++;
			//glColor3f(0, 1, 0);
		}
		else if (i ==8)
		{
			glColor3f(colours[j].x, colours[j].y, colours[j].z);
			j++;
			//glColor3f(1, 1, 0);
		}
	}
	

	glEnd();
	
	//glViewport(0,0, windowWidth, windowHeight);
	useViewport(activeViewport);
}

void Renderer::drawRadarForSplitScreen(vector<vec2> radarVecs, vector<vec3> colours, int playerID)
{
	int viewportWidth = 0;
	int viewportHeight = 0;

	glViewport(viewportWidth * 0.75, viewportHeight * 0.75, viewportWidth * 0.25, viewportHeight * 0.25);

	glUseProgram(0);


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, viewportWidth, 0, viewportHeight);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glBegin(GL_TRIANGLES);
	glColor3f(colours[0].x , colours[0].y, colours[0].z);

	int j = 0;
	for (int i = 0; i < radarVecs.size(); i++){
		vec3 vec = vec3((windowWidth / 2) - radarVecs[i].x * (windowWidth / 15), (windowHeight / 2) - radarVecs[i].y * (windowHeight / 15), 0);
		glVertex3f(vec.x, vec.y, vec.z);

		if (i == 2){
			glColor3f(colours[j].x, colours[j].y, colours[j].z);
			j++;
		}
		else if (i ==5 )
		{
			glColor3f(colours[j].x, colours[j].y, colours[j].z);
			j++;
		}
		else if (i== 8)
		{
			glColor3f(colours[j].x, colours[j].y, colours[j].z);
			j++;
		}
	}


	glEnd();

	//I need a function here to deal with the viewport stuff...
	useViewport(activeViewport);
	//glViewport(0, 0, windowWidth, windowHeight);

}



void Renderer::drawUI(const vector<vector<vec3>>& segments, vector<vec3> colors)
{
	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, windowWidth, 0, windowHeight);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);
	
	// each vector in segments stores the scorebar information for an individual player
	for (unsigned int i = 0; i < min(segments.size(), colors.size()); i++) {
		vec3 color = colors[i];
		glColor3f(color.x, color.y, color.z);

		for (unsigned int j = 3; j < segments[i].size(); j += 4)
		{
			vec3 a = segments[i][j - 3];
			vec3 b = segments[i][j - 2];
			vec3 c = segments[i][j - 1];
			vec3 d = segments[i][j];
			
			glVertex3f(a.x, a.y, a.z);
			glVertex3f(b.x, b.y, b.z);
			glVertex3f(c.x, c.y, c.z);
			glVertex3f(d.x, d.y, d.z);

		}
	}

	glEnd();
}

void Renderer::drawLines(const vector<vec3>& segments, vec3 color, const mat4& objectTransform)
{
	GLint dimensions [4];
	glGetIntegerv(GL_VIEWPORT, dimensions);

	glUseProgram(0);

	mat4 modelTransform = modelview*camera->getMatrix()*objectTransform;
	mat4 projTransform = winRatio*viewports[activeViewport].viewRatio*projection;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&projTransform[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&modelTransform[0][0]);

	glBegin(GL_LINES);
	glColor3f(color.x, color.y, color.z);
	for (unsigned int i = 1; i < segments.size(); i+=2)
	{
		vec3 a = segments[i-1];
		vec3 b = segments[i];
		glVertex3f(a.x, a.y, a.z);
		glVertex3f(b.x, b.y, b.z);
	}
	glEnd();
		
}


void Renderer::drawPoints(const vector<vec3>& segments, vec3 color, const mat4& objectTransform)
{
	glUseProgram(0);
	glPointSize(10.f);

	mat4 modelTransform = modelview*camera->getMatrix()*objectTransform;
	mat4 projTransform = winRatio*projection;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&projTransform[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(&modelTransform[0][0]);

	glBegin(GL_POINTS);
	glColor3f(color.x, color.y, color.z);
	for (unsigned int i = 1; i < segments.size(); i ++)
	{
		vec3 a = segments[i - 1];
		glVertex3f(a.x, a.y, a.z);
	}
	glEnd();

}

bool Renderer::loadBuffers(const ObjectInfo& object)
{
	bool success = false;

	if ((object.mesh != NULL) && (object.normals != NULL) && (object.uvs != NULL) && (object.mat->buffersUsed() & (Material::VERTICES | Material::NORMALS | Material::UVS)))
		success = loadVertNormalUVBuffer(object);
	else if ((object.mesh != NULL) && (object.normals != NULL) && (object.mat->buffersUsed() & (Material::VERTICES | Material::NORMALS)))
		success = loadVertNormalBuffer(object);
	else if ((object.mesh != NULL) && (object.uvs != NULL) && (object.mat->buffersUsed() & (Material::VERTICES | Material::UVS)))
		success = loadVertUVBuffer(object);
	else if ((object.mesh != NULL) && (object.mat->buffersUsed() & (Material::VERTICES)))
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
			GL_DYNAMIC_DRAW);
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
		GL_DYNAMIC_DRAW);

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
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*object.normals->size(),
		&object.normals->at(0),
		GL_DYNAMIC_DRAW);

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

	return loadVertUVBuffer(object.mesh, object.uvs);

}

bool Renderer::loadVertUVBuffer(vector<vec3>* verts, vector<vec2>* uvs)
{
	glBindVertexArray(vao[VAO::VERT_UVS]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VERT]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*verts->size(),
		&verts->at(0),
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec2)*uvs->size(),
		&uvs->at(0),
		GL_DYNAMIC_DRAW);

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
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::NORMALS]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*object.normals->size(),
		&object.normals->at(0),
		GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::UVS]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec2)*object.uvs->size(),
		&object.uvs->at(0),
		GL_DYNAMIC_DRAW);

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

void Renderer::initializeBufferedVAOs()
{

	//Vertex only VAO
	glBindVertexArray(vao[VAO::VERT_BUFFERED]);		//Bind vertex array
	//Vertex vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::V_VERT_BUFFERED]);
	glVertexAttribPointer(
		0,				//Attribute
		3,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);


	//Vertex and Normal VAO
	glBindVertexArray(vao[VAO::VERT_NORMALS_BUFFERED]);		//Bind vertex array
	//Vertex vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VN_VERT_BUFFERED]);
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
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VN_NORMALS_BUFFERED]);
	glVertexAttribPointer(
		1,				//Attribute
		3,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec3),	//Stride
		(void*)0			//Offset
		);

	//Vertex and UV VAO
	glBindVertexArray(vao[VAO::VERT_UVS_BUFFERED]);		//Bind vertex array
	//Vertex vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VT_VERT_BUFFERED]);
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
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VT_UVS_BUFFERED]);
	glVertexAttribPointer(
		1,				//Attribute
		2,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec2),	//Stride
		(void*)0			//Offset
		);

	//Vertex, Normal and UV VAO
	glBindVertexArray(vao[VAO::VERT_NORMALS_UVS_BUFFERED]);	//Bind vertex array
	//Vertex vbo
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VNT_VERT_BUFFERED]);
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
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VNT_NORMALS_BUFFERED]);
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
	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VNT_UVS_BUFFERED]);
	glVertexAttribPointer(
		2,				//Attribute
		2,				//Size
		GL_FLOAT,			//Type
		GL_FALSE,			//Normalized?
		sizeof(vec2),	//Stride
		(void*)0			//Offset
		);

	glBindVertexArray(0);

}

void Renderer::getNewMeshes(vector<ObjectMesh>* newMeshes)
{
	if (newObjects.size() == 0)
		return;

	for (unsigned int i = 0; i < newObjects.size(); i++)
	{
		unsigned int index = newObjects[i];
		ObjectMesh newMesh(objects[index].mesh, objects[index].normals, objects[index].uvs, objects[index].indices, objects[index].texID != NO_VALUE);

		bool inBuffer = false;

		for (unsigned int j = 0; j < distinctMeshes.size(); j++)
		{
			if (newMesh == distinctMeshes[j])
			{
				inBuffer = true;
				objects[index].bufferedIndex = j;
				break;
			}
		}

		if (!inBuffer)
		{
			newMeshes->push_back(newMesh);
			objects[index].bufferedIndex = newMeshes->size() - 1;
		}
	}

	newObjects.clear();
}


void Renderer::loadOptimizedBuffers()
{
	unsigned int prevSize = distinctMeshes.size();
	getNewMeshes(&distinctMeshes);

	if (distinctMeshes.size() == prevSize)
		return;

	bool vChanged = false;
	bool vnChanged = false;
	bool vtChanged = false;
	bool vntChanged = false;

	//Check which buffers have changed
	for (unsigned int i = prevSize; i < distinctMeshes.size(); i++)
	{
		if (distinctMeshes[i].hasTexture && (distinctMeshes[i].verts != NULL) && (distinctMeshes[i].normals != NULL) && (distinctMeshes[i].uvs != NULL))
			vntChanged = true;
		else if ((distinctMeshes[i].verts != NULL) && (distinctMeshes[i].normals != NULL))
			vnChanged = true;
		else if (distinctMeshes[i].hasTexture && (distinctMeshes[i].verts != NULL) && (distinctMeshes[i].uvs != NULL))
			vtChanged = true;
		else if (distinctMeshes[i].verts != NULL)
			vChanged = true;
		else
			cout << "Can't recognize mesh format" << endl;
	}

	if (vChanged)
		loadOptimizedVBuffers();
	if (vnChanged)
		loadOptimizedVNBuffers();
	if (vntChanged)
		loadOptimizedVNTBuffers();
	if (vtChanged)
		loadOptimizedVTBuffers();


	
}

void Renderer::loadOptimizedVBuffers()
{
	v_verts.clear();

	vector<unsigned int> ids;
	unsigned int totalSize = 0;
	unsigned int totalIndexSize = 0;

	for (unsigned int i = 0; i < distinctMeshes.size(); i++)
	{
		if ((distinctMeshes[i].verts != NULL) && (distinctMeshes[i].normals == NULL) && (distinctMeshes[i].uvs == NULL))
		{
			//distinctMeshes[i].offset = totalIndexSize;		//Get offset for mesh

			ids.push_back(i);
			totalSize += distinctMeshes[i].verts->size();
			totalIndexSize += distinctMeshes[i].indices->size();
		}
	}

	//Allocate memory for buffer
	v_verts.reserve(totalSize);
	v_ind.reserve(totalIndexSize);

	//Fill buffers
	for (unsigned int i = 0; i < ids.size(); i++)
	{
		ObjectMesh& object = distinctMeshes[ids[i]];
		object.offset = v_ind.size();

		//Copy and adjust indices
		for (unsigned int j = 0; j < object.indices->size(); j++)
		{
			v_ind.push_back(object.indices->at(j) + v_verts.size());
		}

		//Add mesh to optimized buffer
		v_verts.insert(v_verts.end(), object.verts->begin(), object.verts->end());
	}

	//Load buffer
	glBindVertexArray(vao[VAO::VERT_BUFFERED]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::V_VERT_BUFFERED]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*v_verts.size(),
		&v_verts[0],
		GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::V_INDICES_BUFFERED]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*v_ind.size(),
		&v_ind[0],
		GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void Renderer::loadOptimizedVNBuffers()
{
	vn_verts.clear();
	vn_normals.clear();

	vector<unsigned int> ids;
	unsigned int totalSize = 0;
	unsigned int totalIndexSize = 0;

	for (unsigned int i = 0; i < distinctMeshes.size(); i++)
	{
		if ((distinctMeshes[i].verts != NULL) && (distinctMeshes[i].normals != NULL) && (distinctMeshes[i].uvs == NULL))
		{
			distinctMeshes[i].offset = totalIndexSize;		//Get offset for mesh

			ids.push_back(i);
			totalSize += distinctMeshes[i].verts->size();
			totalIndexSize += distinctMeshes[i].indices->size();
		}
	}

	//Allocate memory for buffer
	vn_verts.reserve(totalSize);
	vn_normals.reserve(totalSize);
	vn_ind.reserve(totalIndexSize);

	//Fill buffers
	for (unsigned int i = 0; i < ids.size(); i++)
	{
		ObjectMesh& object = distinctMeshes[ids[i]];
		object.offset = vn_ind.size();

		//Copy and adjust indices
		for (unsigned int j = 0; j < object.indices->size(); j++)
		{
			vn_ind.push_back(object.indices->at(j) + vn_verts.size());
		}

		//Add mesh to optimized buffer
		vn_verts.insert(vn_verts.end(), object.verts->begin(), object.verts->end());
		vn_normals.insert(vn_normals.end(), object.normals->begin(), object.normals->end());
	}

	glBindVertexArray(vao[VAO::VERT_NORMALS_BUFFERED]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VN_VERT_BUFFERED]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*vn_verts.size(),
		&vn_verts[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VN_NORMALS_BUFFERED]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*vn_normals.size(),
		&vn_normals[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::VN_INDICES_BUFFERED]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*vn_ind.size(),
		&vn_ind[0],
		GL_STATIC_DRAW);

	glBindVertexArray(0);

}


void Renderer::loadOptimizedVNTBuffers()
{
	vnt_verts.clear();
	vnt_normals.clear();
	vnt_uvs.clear();

	vector<unsigned int> ids;
	unsigned int totalSize = 0;
	unsigned int totalIndexSize = 0;

	for (unsigned int i = 0; i < distinctMeshes.size(); i++)
	{
		if ((distinctMeshes[i].verts != NULL) && (distinctMeshes[i].normals != NULL) && (distinctMeshes[i].uvs != NULL))
		{
			distinctMeshes[i].offset = totalIndexSize;		//Get offset for mesh

			ids.push_back(i);
			totalSize += distinctMeshes[i].verts->size();
			totalIndexSize += distinctMeshes[i].indices->size();
		}
	}

	//Allocate memory for buffer
	vnt_verts.reserve(totalSize);
	vnt_normals.reserve(totalSize);
	vnt_ind.reserve(totalIndexSize);

	//Fill buffers
	for (unsigned int i = 0; i < ids.size(); i++)
	{
		ObjectMesh& object = distinctMeshes[ids[i]];
		object.offset = vnt_ind.size();

		//Copy and adjust indices
		for (unsigned int j = 0; j < object.indices->size(); j++)
		{
			vnt_ind.push_back(object.indices->at(j) + vnt_verts.size());
		}

		//Add mesh to optimized buffer
		vnt_verts.insert(vnt_verts.end(), object.verts->begin(), object.verts->end());
		vnt_normals.insert(vnt_normals.end(), object.normals->begin(), object.normals->end());
		vnt_uvs.insert(vnt_uvs.end(), object.uvs->begin(), object.uvs->end());
	}

	glBindVertexArray(vao[VAO::VERT_NORMALS_UVS_BUFFERED]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VNT_VERT_BUFFERED]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*vnt_verts.size(),
		&vnt_verts[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VNT_NORMALS_BUFFERED]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*vnt_normals.size(),
		&vnt_normals[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VNT_UVS_BUFFERED]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec2)*vnt_uvs.size(),
		&vnt_uvs[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::VNT_INDICES_BUFFERED]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*vnt_ind.size(),
		&vnt_ind[0],
		GL_STATIC_DRAW);

	glBindVertexArray(0);

}


void Renderer::loadOptimizedVTBuffers()
{
	vt_verts.clear();
	vt_uvs.clear();

	vector<unsigned int> ids;
	unsigned int totalSize = 0;
	unsigned int totalIndexSize = 0;

	for (unsigned int i = 0; i < distinctMeshes.size(); i++)
	{
		if ((distinctMeshes[i].verts != NULL) && (distinctMeshes[i].normals == NULL) && (distinctMeshes[i].uvs != NULL))
		{
			distinctMeshes[i].offset = totalIndexSize;		//Get offset for mesh

			ids.push_back(i);
			totalSize += distinctMeshes[i].verts->size();
			totalIndexSize += distinctMeshes[i].indices->size();
		}
	}

	//Allocate memory for buffer
	vt_verts.reserve(totalSize);
	vt_ind.reserve(totalIndexSize);

	//Fill buffers
	for (unsigned int i = 0; i < ids.size(); i++)
	{
		ObjectMesh& object = distinctMeshes[ids[i]];
		object.offset = vt_ind.size();

		//Copy and adjust indices
		for (unsigned int j = 0; j < object.indices->size(); j++)
		{
			vt_ind.push_back(object.indices->at(j) + vt_verts.size());
		}

		//Add mesh to optimized buffer
		vt_verts.insert(vt_verts.end(), object.verts->begin(), object.verts->end());
		vt_uvs.insert(vt_uvs.end(), object.uvs->begin(), object.uvs->end());
	}

	glBindVertexArray(vao[VAO::VERT_UVS_BUFFERED]);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VT_VERT_BUFFERED]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec3)*vt_verts.size(),
		&vt_verts[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[VBO::VT_UVS_BUFFERED]);
	glBufferData(GL_ARRAY_BUFFER,
		sizeof(vec2)*vt_uvs.size(),
		&vt_uvs[0],
		GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[VBO::VT_INDICES_BUFFERED]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(unsigned int)*vt_ind.size(),
		&vt_ind[0],
		GL_STATIC_DRAW);

	glBindVertexArray(0);

}


/**
* Preset mesh generation functions
**/

void Renderer::assignCube(	unsigned int id, float width,
							vector<vec3>* mesh,
							vector<vec3>* normals,
							vector<unsigned int>* indices)
{

	float halfWidth = 0.5f*width;
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

	normals->push_back(vec3(0.f, 0.f, -1.f));
	normals->push_back(vec3(0.f, 0.f, -1.f));
	normals->push_back(vec3(0.f, 0.f, -1.f));
	normals->push_back(vec3(0.f, 0.f, -1.f));

	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);
	indices->push_back(2);
	indices->push_back(3);
	indices->push_back(0);

	assignMesh(id, mesh);
	assignNormals(id, normals);
	assignIndices(id, indices);
	assignColor(id, vec3(1.f, 0.f, 0.f));

}

void Renderer::assignPlane(unsigned int id, float width,
	vector<vec3>* mesh,
	vector<vec3>* normals,
	vector<unsigned int>* indices)
{

	float halfWidth = 0.5f*width;
	//Front face
	mesh->push_back(vec3(-halfWidth, 0.f, halfWidth));
	mesh->push_back(vec3(-halfWidth, 0.f, - halfWidth));
	mesh->push_back(vec3(halfWidth, 0.f, -halfWidth));
	mesh->push_back(vec3(halfWidth, 0.f, halfWidth));
	/*//Back face
	mesh.push_back(vec3(-halfWidth, halfWidth, -halfWidth));
	mesh.push_back(vec3(halfWidth, halfWidth, -halfWidth));
	mesh.push_back(vec3(halfWidth, -halfWidth, -halfWidth));
	mesh.push_back(vec3(-halfWidth, -halfWidth, -halfWidth));*/

	normals->push_back(vec3(0.f, 1.f, 0.f));
	normals->push_back(vec3(0.f, 1.f, 0.f));
	normals->push_back(vec3(0.f, 1.f, 0.f));
	normals->push_back(vec3(0.f, 1.f, 0.f));

	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);
	indices->push_back(2);
	indices->push_back(3);
	indices->push_back(0);

	assignMesh(id, mesh);
	assignNormals(id, normals);
	assignIndices(id, indices);
	assignColor(id, vec3(1.f, 0.f, 0.f));

}

void Renderer::assignSphere(unsigned int id, float radius, unsigned int divisions,
	vector<vec3>* mesh,
	vector<vec3>* normals,
	vector<vec2>* uvs,
	vector<unsigned int>* indices)
{

	mesh->clear();
	normals->clear();
	indices->clear();
	uvs->clear();

	unsigned int yDivisions = divisions;
	unsigned int xDivisions = 2*divisions;

	float uInc = 2.f*M_PI/(float)xDivisions;
	float vInc = M_PI / (float)yDivisions;

	//Front face
	float u = 0.f;
	for (unsigned int i = 0; i <= xDivisions; i++)
	{
		float x_base = cos(u);
		float z_base = sin(u);

		float v = -M_PI*0.5;
		for (unsigned int j = 0; j <= yDivisions; j++)
		{

			float x = x_base*cos(v);
			float z = z_base*cos(v);
			float y = sin(v);
			mesh->push_back(vec3(x, y, z)*radius);
			normals->push_back(normalize(vec3(x, y, z)));
			uvs->push_back(vec2(u / (2.f*M_PI), (v+M_PI*0.5) / (M_PI)));

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
	assignUVs(id, uvs);
	assignIndices(id, indices);
	assignColor(id, vec3(1.f, 0.f, 0.f));

}


void Renderer::assignSkyDome(unsigned int id, float radius, unsigned int divisions,
	vector<vec3>* mesh,
	vector<vec2>* uvs,
	vector<unsigned int>* indices,
	unsigned int textureID)
{

	mesh->clear();
	indices->clear();
	uvs->clear();

	unsigned int yDivisions = divisions;
	unsigned int xDivisions = 2 * divisions;

	float uInc = 2.f*M_PI / (float)xDivisions;
	float vInc = M_PI*0.5f / (float)yDivisions;

	//Front face
	float u = 0.f;
	for (unsigned int i = 0; i <= xDivisions; i++)
	{
		float x_base = cos(u);
		float z_base = sin(u);

		float v = 0.f;
		for (unsigned int j = 0; j <= yDivisions; j++)
		{

			float x = x_base*cos(v);
			float z = z_base*cos(v);
			float y = sin(v);
			mesh->push_back(vec3(x, y, z)*radius);
			uvs->push_back(vec2(u / (2.f*M_PI), v / (M_PI*0.5f)));

			v += vInc;
		}
		u += uInc;
	}

	for (unsigned int i = 0; i < xDivisions; i++)
	{
		for (unsigned int j = 0; j < yDivisions; j++)
		{
			indices->push_back(i*(yDivisions + 1) + j);
			indices->push_back(i*(yDivisions + 1) + j + 1);
			indices->push_back((i + 1)*(yDivisions + 1) + j + 1);
			indices->push_back(i*(yDivisions + 1) + j);
			indices->push_back((i + 1)*(yDivisions + 1) + j + 1);
			indices->push_back((i + 1)*(yDivisions + 1) + j);
		}
	}

	assignMesh(id, mesh);
	assignUVs(id, uvs);
	assignIndices(id, indices);
	assignTexture(id, textureID);

}

void Renderer::deleteDrawableObject(int objectID)
{
	objects[objectID].deleted = true;
}


mat4 biasMatrix()
{
	return mat4 (	0.5f, 0, 0, 0,
					0, 0.5f, 0, 0,
					0, 0, 0.5f, 0,
					0.5f, 0.5f, 0.5f, 1);
}

void Renderer::generatePointsOnDisk()
{
	for (int i = 0; i + 1 < RANDOM_POINT_NUM * 2; i += 2)
	{
		float r = sqrt(((float)rand()) / RAND_MAX);
		float phi = 2 * M_PI*((float)rand()) / RAND_MAX;

		float y = sin(phi)*r;
		float x = cos(phi)*r;

		randomPoints[i] = x;
		randomPoints[i + 1] = y;
	}
}


/**
* Debugging
**/

void Renderer::debug_message(string message)
{
	if (true)
		printf("Renderer>> %s\n", message.c_str());
}

void displayVec4(const vec4& vec)
{
	printf("[%f, %f, %f, %f]\n", vec.x, vec.y, vec.z, vec.w);
}
void displayMat4(const mat4& mat)
{
	for (unsigned int i = 0; i < 4; i++)
	{
		displayVec4(mat[i]);
	}
}


/////////////
//Callbacks
////////////

void resizeFunc(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);

	float minDim = min((float)width, (float)height);
	winRatio[0][0] = minDim / (float)width;
	winRatio[1][1] = minDim / (float)height;

	windowWidth = width;
	windowHeight = height;
}

#endif // RENDERER_CPP