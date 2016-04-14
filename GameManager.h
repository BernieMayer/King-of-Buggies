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

const float CAM_SENSITIVITY = 0.1f;

class GameManager {
private:
	// Object materials
	Diffuse mat;
	Specular shinyMat;
	TorranceSparrow tsMat;
	TorranceSparrow matteMat;
	Shadow shadowMat;

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
	timeb pauseStartTime;
	float totalPausedTime;

	Camera cam[MAX_PLAYERS];
	Camera freeCam;

	GLFWwindow* window;

	MeshObject* levelMesh;

	unsigned int fbo;

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

	vector<vec3> buggyExplosionVerts;
	vector<vec2> buggyExplosionUVs;
	vector<unsigned int> buggyExplosionIndices;
	unsigned int buggyExplosionTexture;
	unsigned int explosionID;

	vec3 explosionLocation;

	unsigned int blTex;
	

	vector<bool> hasPowerup;

	unsigned int carSelectScreen;

	timeb pauseTime;
	timeb lastScoreUpdateTime;

	vector<int> playerColours;

	unsigned int numScreens = 0;

	vector<unsigned int> powerupComponentIDs;
	vector<unsigned int> playerCoinIDs;
	vector<unsigned int> buggyIndicatorUIs;
	vector<unsigned int> scoreBarIDs;

	vector<timeb> lastTimeUsedBomb;

	//Game loop functions
	void changeGoldenBuggy();
	void handleBombCollisionEvent(Event* e);
	void handlePowerupBoxCollisionEvent(Event* e);
	void handleBuggySwitchEvent(Event* e);
	void handleRespawnEvent(Event* e);
	void processEvents();
	void checkCoinCollisions();
	void updateCamera(unsigned int i, Input input, float frameTime);

	int selectedLevel = 2;
	vec3 backgroundColor = vec3(1.f, 1.f, 1.f);

	// temporary?
	timeb blinkTime;
	bool blinking = false;

	//temporary
	vector<float> scoreBarWidths;

	unsigned int p1PauseTexture;
	unsigned int p1PauseIcon;

	unsigned int pauseScreenTexture;
	unsigned int pauseScreen;

	unsigned int pauseMenuBackground;
	unsigned int pauseMenu;

	bool gbInputLock = false;
	timeb buggyInputLockTime = clock.getCurrentTime();
	float buggyInputLockEnd = 7.0f;

public:
	GameManager(GLFWwindow* newWindow);

	void initMenus();
	void changePlayerIcon(int playerNum, bool selected);

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
	void createMine(unsigned int playerID);
	void createBomb(unsigned int playerID);

	void removePlayer(unsigned int playerID);

	void createDecoyGoldenBuggie(vec3 position, VehicleTraits traits);
	void startDecoy(float time);
	float decrementDecoyTimer(float time);
	bool isDecoyTimerUp();
	void applyPowerupEffect(int playerNun);
	void switchBuggyUI();

	void cameraEnvironmentCollision(Camera* cam);

	//Buggy explosion
	void startBuggyExplosion(vec3 loc);
	void updateBuggyExplosion();
	void endBuggyExplosion();
	
	void gameLoop();
	void gameInit();
	void initUI();
	void displayEndScreen(unsigned int winnerID);

	int randomPowerup();

	bool paused = false;
	void displayPauseMenu();

	void partialTeardown();

	void incScoreBar(unsigned int playerID);
};

#endif // GAMEMANAGER_H