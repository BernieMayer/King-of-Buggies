#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include "AIManager.h"
#include "DrivingController.h"
#include "GameState.h"
#include "InputManager.h"
#include "InterfaceManager.h"
#include "MeshInfo.h"
#include "Physics.h"
#include "Renderer.h"
#include "SoundManager.h"
#include "NavMesh.h"


#include <GLFW/glfw3.h>  

class GameManager {
private:
	// Object materials
	Diffuse mat;
	Specular shinyMat;
	TorranceSparrow tsMat;
	TorranceSparrow matteMat;

	AIManager ai;
	DrivingController dc;
	GameState state;
	InputManager input;
	InterfaceManager _interface;
	MeshInfo meshInfo;
	Physics physics;
	Renderer renderer;
	SoundManager sound;

	Camera cam;

	GLFWwindow* window;

	//void updateTransform(const Entity& )

	unsigned int surfaceRenderID;		//TEMPORARY - Replace as soon as possible
	unsigned int surfacePhysicsID;		//TEMPORARY

	unsigned int sphereRenderID;		//TEMPORARY
	unsigned int spherePhysicsID;		//TEMPORARY

	vector<vec3> surfaceVertices;		//TEMPORARY
	vector<vec3> surfaceNormals;		//TEMPORARY
	vector<unsigned int> surfaceIndices;	//TEMPORARY

	vector<vec3> ballVertices;			//TEMPORARY
	vector<vec3> ballNormals;			//TEMPORARY
	vector<unsigned int> ballIndices;	//TEMPORARY

public:
	GameManager(GLFWwindow* newWindow);

	// create physics and render objects through GameState
	void createPlayer(vec3 position, VehicleTraits traits);		//Add forward parameter later, maybe also VehicleTraits
	void createAI(vec3 position);
	void createGroundPlane(vec3 normal, float offset);
	void createTestLevel();
	void createCoins();
	void createPowerups();
	void createPowerupBoxes();
	void createBall(float radius);
	
	void gameLoop();
	void gameInit();
	void initTestScene();
	void quitGame(); // should this take arguments? 

	void physicsAndRenderTest();

};

#endif // GAMEMANAGER_H