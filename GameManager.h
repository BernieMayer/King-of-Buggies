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
#include "Timer.h"
#include "Definitions.h"


#include <GLFW/glfw3.h>  

class GameManager {
private:
	// Object materials
	Diffuse mat;
	Specular shinyMat;
	TorranceSparrow tsMat;
	TorranceSparrow matteMat;

	GameState state;
	AIManager ai = AIManager(&state);
	DrivingController dc;
	InputManager input;
	InterfaceManager _interface;
	MeshInfo meshInfo;
	Physics physics;
	Renderer renderer;
	SoundManager sound;
	vector<InputSmoother> smoothers;

	Timer clock;

	Camera cam[MAX_PLAYERS];

	GLFWwindow* window;

	unsigned int winner;
	bool gameOver;
	bool displayDebugging = false;
	

	float timePassedDecoy;

	vector<vec3> vehicleColours;

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
	vector<vec2> ballUVs;				//TEMPORARY

	
	//Skybox - Temporary
	vector<vec3> skyboxVerts;
	vector<vec2> skyboxUVs;
	vector<unsigned int> skyboxIndices;
	unsigned int skyboxTextureID;
	unsigned int skyboxID;
	Unshaded skyMaterial;

	vector<bool> hasPowerup;

	unsigned int carSelectScreen;

	timeb pauseTime;

	vector<int> playerColours;

public:
	GameManager(GLFWwindow* newWindow);

	void initMenus();

	// create physics and render objects through GameState
	void createPlayer(vec3 position, VehicleTraits traits, unsigned int textureID);		//Add forward parameter later, maybe also VehicleTraits
	void createAI(vec3 position);
	void createGroundPlane(vec3 normal, float offset);
	void createLevel(unsigned int objectID);
	void createBall(float radius);
	// These entities get created based on LevelInfo:
	void createCoin(unsigned int coinIndex);
	void createBoostPad(unsigned int objectID);
	void createPowerupBox(unsigned int objectID);
	// These entities get created dynamically:
	void createMine();
	void createBomb(unsigned int playerID);

	void createDecoyGoldenBuggie(vec3 position, VehicleTraits traits);
	void startDecoy(float time);
	bool isDecoyTimerUp();

	void cameraEnvironmentCollision(Camera* cam);
	
	void gameLoop();
	void gameInit();
	void initTestScene();
	void quitGame(unsigned int winnerID);

	int randomPowerup();


};

#endif // GAMEMANAGER_H