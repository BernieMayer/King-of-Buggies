#ifndef RENDERER_H
#define RENDERER_H

#include <Windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>

#include "MeshObject.h"
#include "camera.h"
#include "Material.h"
#include "Specular.h"
#include "diffuse.h"
#include "torranceSparrow.h"
#include "unshaded.h"
#include "shadow.h"
#include "GameState.h"
#include "Definitions.h"

using namespace std;
using namespace glm;



#define M_PI  3.14159265358979323846f

struct SHADOW_BEHAVIOUR{
	enum {
			NONE	= 0,
			CAST	= 1 << 0, 
			RECEIVE = 1 << 1
			 };
};
struct VAO{
	enum { VERT = 0, VERT_NORMALS, VERT_UVS, VERT_NORMALS_UVS, 
			VERT_BUFFERED, VERT_NORMALS_BUFFERED, VERT_UVS_BUFFERED, VERT_NORMALS_UVS_BUFFERED, COUNT };
};

struct VBO{
	enum { VERT = 0, NORMALS, UVS, INDICES, 
			V_VERT_BUFFERED, V_INDICES_BUFFERED,
			VN_VERT_BUFFERED, VN_NORMALS_BUFFERED, VN_INDICES_BUFFERED,
			VT_VERT_BUFFERED, VT_UVS_BUFFERED, VT_INDICES_BUFFERED,
			VNT_VERT_BUFFERED, VNT_NORMALS_BUFFERED, VNT_UVS_BUFFERED, VNT_INDICES_BUFFERED,
			COUNT };
};

struct Viewport{
	float x, y, width, height;		//Change to floats
	mat4 viewRatio;							//Get rid of?

	const unsigned int DEFAULT = 0;

	Viewport(float x, float y, float width, float height);
};

struct Framebuffer{
	GLuint id, texture;
	unsigned int width, height;
	unsigned int type;

	enum { DEPTH, COLOR };

	Framebuffer(GLuint id, GLuint texture, unsigned int width, unsigned int height, unsigned int type) :id(id), texture(texture), width(width), height(height), type(type) {}

	mat4 ratioMatrix();
};

const unsigned int DEFAULT_WIDTH = 800;
const unsigned int DEFAULT_HEIGHT = 800;


class Renderer {
private:
	class ObjectInfo{
	public:
		ObjectInfo();

		vector<vec3>* mesh;
		vector<vec3>* normals;
		vector<vec2>* uvs;
		vector<unsigned int>* indices;
		GLuint texID;
		Material* mat;
		vec3 color;
		unsigned int shadowBehaviour;
		bool deleted;

		unsigned int bufferedIndex;

		//Information about object orientation
		mat4 transform;
		mat4 scaling;
	};

	class LightInfo
	{
	public:
		vec3 pos;
		bool deleted;
		Camera cam;
		mat4 projection;

		LightInfo();

		void positionCamera(vec3 sceneCenter, float boundingRadius);
		void positionCamera(vec3 sceneCenter, float nearP, float farP, float width, float height);
	};

	class ObjectMesh
	{
	public:
		vector<vec3>* verts;
		vector<vec3>* normals;
		vector<vec2>* uvs;
		vector<unsigned int>* indices;
		bool hasTexture;
		unsigned int offset;

		ObjectMesh(vector<vec3>* verts, vector<vec3>* normals, vector<vec2>* uvs, vector<unsigned int>* indices, bool hasTexture);

		bool operator ==(const ObjectMesh& other) const;
	};

	GLFWwindow * window;
	


	vector<ObjectInfo> objects;		//Objects to be drawn
	vector<LightInfo> lights;
	vector<Viewport> viewports;

	//Deleted object indices
	vector<unsigned int> deletedObjects;

	vector<unsigned int> newObjects;

	//Optimized buffers
	vector<unsigned int> v_ind;
	vector<vec3> v_verts;
	vector<unsigned int> vn_ind;
	vector<vec3> vn_verts;
	vector<vec3> vn_normals;
	vector<unsigned int> vt_ind;
	vector<vec3> vt_verts;
	vector<vec2> vt_uvs;
	vector<unsigned int> vnt_ind;
	vector<vec3> vnt_verts;
	vector<vec3> vnt_normals;
	vector<vec2> vnt_uvs;

	vector<ObjectMesh> distinctMeshes;

	unsigned int activeViewport;
	unsigned int activeFramebuffer;
	unsigned int shadowTexUnit;

	//Transform matrices
	mat4 projection;
	mat4 modelview;
	mat4 shadowProjection;

	Camera* camera;
	Camera lightCamera;

	Shadow shadow;

	GLuint vao[VAO::COUNT];
	GLuint vbo[VBO::COUNT];
	vector<Framebuffer> fbo;

	float randomPoints[RANDOM_POINT_NUM];

	bool debugging;
	

	/**
	* Functions
	**/

	void initializeVAOs();
	void initializeBufferedVAOs();
	bool loadBuffers(const ObjectInfo& object);		
	bool loadVertBuffer(const ObjectInfo& object);	
	bool loadVertNormalBuffer(const ObjectInfo& object);
	bool loadVertUVBuffer(const ObjectInfo& object);	
	bool loadVertNormalUVBuffer(const ObjectInfo& object);

	//Optimized buffers
	void getNewMeshes(vector<ObjectMesh>* newMeshes);
	void loadOptimizedVNTBuffers();
	void loadOptimizedVBuffers();
	void loadOptimizedVNBuffers();
	void loadOptimizedVTBuffers();

	void bindBufferedVAO(const ObjectInfo& object);

	//Debugging
	void debug_message(string message);		

public:
	Renderer(GLFWwindow* _window);
	~Renderer();
	void renderLoop();

	//Transformations
	void loadModelviewTransform(const mat4& _modelview);
	void loadProjectionTransform(const mat4& _projection);
	void loadPerspectiveTransform(float nearP, float farP, float fov);
	void loadOrthographicTransform(float nearP, float farP, float width, float height);
	void loadPerspectiveTransformShadow(float nearP, float farP, float fov);
	void loadOrthographicTransformShadow(float nearP, float farP, float width, float height);
	void loadCamera(Camera* _camera);
	void positionLightCamera(unsigned int lightID, vec3 sceneCenter, float boundingRadius);	//Sets up light camera and projection
	void positionLightCamera(unsigned int lightID, vec3 sceneCenter, float nearP, float farP, float width, float height);

	//Drawable objects
	unsigned int generateObjectID();
	void assignMeshObject(unsigned int id, MeshObject* mesh);
	void assignMesh(unsigned int object, vector<vec3>* mesh);
	void assignNormals(unsigned int object, vector<vec3>* normals);
	void assignUVs(unsigned int object, vector<vec2>* uvs);
	void assignIndices(unsigned int object, vector<unsigned int>* indices);		//If no indices are assigned, will traverse mesh in order
	void assignColor(unsigned int object, vec3 color);
	void assignTexture(unsigned int objectID, unsigned int textureID);
	void setShadowBehaviour(unsigned int object, unsigned int behaviour);
	void assignMaterial(unsigned int object, Material* mat);
	void assignTransform(unsigned int object, const mat4& transform);
	void assignScale(unsigned int object, const mat4& scaling);
	
	void assignCube(unsigned int object, float width, 
					vector<vec3>* mesh,
					vector<vec3>* normals,
					vector<unsigned int>* indices);
	void assignPlane(unsigned int object, float width,
					vector<vec3>* mesh,
					vector<vec3>* normals,
					vector<unsigned int>* indices);
	void assignSphere(unsigned int object, float radius, unsigned int divisions,
					vector<vec3>* mesh,
					vector<vec3>* normals,
					vector<vec2>* uvs,
					vector<unsigned int>* indices);

	void assignSkyDome(unsigned int object, float radius, unsigned int divisions,
					vector<vec3>* mesh,
					vector<vec2>* uvs,
					vector<unsigned int>* indices,
					unsigned int textureID);

	void loadShadowMap(unsigned int texID);

	//Lights
	unsigned int generateLightObject();
	void setLightPosition(unsigned int object, vec3 lightPos);
	void assignLightMesh(unsigned int object, vector<vec3>* mesh);
	void assignLightColor(unsigned int object, const vec4& color);
	void assignLightIntensity(unsigned int object, float intensity);
	void enableShadowCasting(unsigned int object);
	void disableShadowCasting(unsigned int object);
	Camera* getLightCamera(unsigned int object);
	mat4 getLightProjection(unsigned int object);

	//Framebuffer objects
	unsigned int createFramebuffer(unsigned int width, unsigned int height);
	unsigned int createDepthbuffer(unsigned int width, unsigned int height);
	void useFramebuffer(unsigned int id);		//NO_VALUE uses default framebuffer
	GLuint getFramebufferTexture(unsigned int id) { return fbo[id].texture; }
	void deleteFramebuffer(unsigned int id);

	//Drawing calls
	void clearDrawBuffers(vec3 color);
	void draw(unsigned int object);
	void draw(vector<unsigned int> list);	//Preferred over individual calls, easier to optimize
	void drawAll();		//Easiest to optimize

	//Preloaded drawing calls
	void drawBuffered(unsigned int object, bool useShadows);
	void drawBufferedAll(bool useShadows);

	//Draw shadows
	void drawAllWithShadows(unsigned int shadowTexture, unsigned int lightID);
	void drawWithShadows(unsigned int object, unsigned int shadowTexture, unsigned int lightID);

	void drawRadar(vector<vec2> radarVecs, vector<vec3> colours);

	void drawUI(const vector<vector<vec3>>& segments, vector<vec3> colors);
	
	//Debugging draw calls
	void drawLines(const vector<vec3>& segments, vec3 color, const mat4& objectTransform);
	void drawPoints(const vector<vec3>& points, vec3 color, const mat4& objectTransform);

	//Draw shadow map
	void drawShadowMap(unsigned int id, unsigned int lightID);
	void drawShadowMapAll(unsigned int lightID);

	//Viewports
	unsigned int addViewport(float x, float y, float width, float height);	//Set viewport in 0 to 1 floats
	unsigned int addViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);	  //Set viewport in pixels
	void useViewport(unsigned int index);
	void splitScreenViewports(unsigned int numViewports);
	mat4 getViewportRatio(unsigned int viewport);
	unsigned int numberOfViewports() { return viewports.size(); }
	Viewport getActiveViewport() { return viewports[activeViewport]; }

	//Delete objects
	void deleteDrawableObject(int object);
	void deleteAllDrawableObjects();
	void deleteLightObject(int object);
	void deleteIDs();

	//Update
	void updateObjectTransforms(GameState* state);

	//External buffer loading
	bool loadVertUVBuffer(vector<vec3>* verts, vector<vec2>* uvs);

	//Optimize buffer
	void loadOptimizedBuffers();		//Make call from outside

	//Window dimensions
	unsigned int getHeight();
	unsigned int getWidth();

	void generatePointsOnDisk();

};

void displayVec4(const vec4& vec);
void displayMat4(const mat4& mat);
mat4 biasMatrix();

void resizeFunc(GLFWwindow* window, int width, int height);

#endif // RENDERER_H