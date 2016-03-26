#ifndef PHYSICS_H
#define PHYSICS_H

#include <math.h>
#include "Vehicle.h"
#include "PxSimulationEventCallback.h"
#include "MeshObject.h"
#include "LevelInfo.h"
#include "GameState.h"
#include "PxPhysicsAPI.h"
#include "VehicleSceneQueryData.h"
#include "Timer.h"
#include "Input.h"
#include "VehicleTraits.h"
#include "CollisionEvent.h"
#include "VehicleCollisionEvent.h"
#include "VehicleWallCollisionEvent.h"
#include "VehicleBombCollisionEvent.h"
#include "BombCreationEvent.h"
#include "PowerupCollisionEvent.h"
#include "Bomb.h"




const float gravity = -9.81f;

using namespace physx;

//Tire types. Part of Vehicle
/*
enum
{
	TIRE_TYPE_NORMAL = 0,
	TIRE_TYPE_
	,
	MAX_NUM_TIRE_TYPES
};
*/
enum
{
	SURFACE_TYPE_TARMAC,
	MAX_NUM_SURFACE_TYPES
};

class Physics : public PxSimulationEventCallback {
public:
	Physics();
	void shutdown();
	void startSim(float frameTime);
	//void startSim2(const GameState&);
	GameState getSim();
	Entity* getCollisions();
	void giveInput(Input input, int playerNum);
	void handleInput(Input* input);
	void handleInput(Input* input, unsigned int id);

	//Should be migrated to Vehicle class
	void modifySpeed(unsigned int vehicleNum, float modSpeed);	//revised to Vehicle class BM
	void setSpeed(unsigned int vehicleNum, float speed);
	void applySpeedPadBoost(unsigned int vechicleNum);
	void applyNitroBoost(unsigned int vehicleNum);
	//void applyNitroBoost(unsigned int vehicleNum, float energy);
	void applyMineExplosion(unsigned int vehicleNum);

	/*
	Split between vehicle, ground, and dynamic for the same of collision resolution
	May need to split further at a later point of time


	All of these methods should be migrated to the Vehicle.h & Vehicle.cpp future classes
	*/
	unsigned int vehicle_create(VehicleTraits traits, vec3 initPos);	//Returns ID for vehicle (Ported to Vehicle Bernie Mayer)
	void vehicle_setVehicleTraits(unsigned int id, VehicleTraits traits);		//Modify vehicle traits 
	mat4 vehicle_getGlobalPose(unsigned int id);
	mat4 vehicle_getGlobalPoseWheel(unsigned int id, unsigned int wheelNum);
	float vehicle_getFSpeed(unsigned int id);
	float vehicle_getSSpeed(unsigned int id);
	bool vehicle_getForwardsGear(unsigned int id);
	float vehicle_getEngineSpeed(unsigned int id);

	unsigned int ground_createPlane(vec3 normal, float offset);	//Returns ID for plane
	unsigned int ground_createGeneric(MeshObject* mesh);
	
	unsigned int dynamic_create(vec3 mesh, vec3 initPos);
	unsigned int dynamic_createSphere(float radius, vec3 initPos);
	mat4 dynamic_getGlobalPose(unsigned int id);

	unsigned int createMaterial(float staticFriction, float dynamicFriction, float restitution);
	PxMaterial* getMaterial();
	PxMaterial* getMaterial(unsigned int materialID);

	void updateGameState(GameState* state, float time);

	float getEnvironmentRaycastResult(vec3 origin, vec3 direction, float maxDist);		//Shoots ray into seen and returns distance of closest hit

	// Implements PxSimulationEventCallback
	virtual void  onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	virtual void  onTrigger(PxTriggerPair* pairs, PxU32 count);
	virtual void  onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void  onWake(PxActor**, PxU32) {}
	virtual void  onSleep(PxActor**, PxU32){}

	bool newGoldenBuggy = false;
	int indexOfGoldenBuggy = 0;
	int indexOfOldGoldenBuggy = 0;
	float initVehicleSpeed = 500.f;
	bool goldenBuggyLock = false;
	timeb gbLockStartTime;

	int createBomb(vec3 location, int playerID);
	//createConvexMesh
	void createPowerupBox(vector<vec3> mesh, vec3 location);


private:
	Timer clock;
	vector<Vehicle> vehicleActors;	//Eventually should be a vector of Vehicles
	vector<Vehicle> vehiclesToDelete;
	vector<PxRigidStatic*> groundActors;
	vector<PxRigidDynamic*> dynamicActors;
	vector<PxRigidDynamic*> bombActors;
	vector<PxRigidDynamic*> powerupActors;
	vector<timeb> bombStartTimes;

	Vehicle* goldenBuggy;	       //Should be a vehicle

	vector<PxMaterial*> materials;

	PxDefaultAllocator gAllocator;
	PxDefaultErrorCallback gErrorCallback;

	PxFoundation* mFoundation = NULL;
	PxPhysics* mPhysics = NULL;			//Now is a part of Vehicle as well
	PxCooking* mCooking = NULL;			//Now is a part of Vehicle as well

	PxDefaultCpuDispatcher* mDispatcher = NULL;
	PxScene* gScene = NULL;

	PxMaterial* mMaterial = NULL;		//A version of this has been added to Vehicle (Bernie Mayer)

	PxVehicleDrive4WRawInputData* inputs[5];	//Move to Vehicle.h & Vehicle.cpp

	PxBatchQuery*			gBatchQuery = NULL;
	PxVehicleDrivableSurfaceToTireFrictionPairs* gFrictionPairs = NULL;

	VehicleSceneQueryData*	gVehicleSceneQueryData = NULL;

	//
	vector<int> vehicleForwards;	//Vehicle class
	vector<bool> vehicleInAir;

	GameState* lastState = NULL;
	float lastFrameTime = 1;


	void initScene();

	//Initialize vehicle move to vehicle class
	PxVehicleDrive4W* initVehicle(VehicleTraits traits, PxVec3 initPos);
	PxRigidDynamic* initVehicleActor(const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 nbWheels, const PxVec3 chassisDims,
		const PxVec3 chassisMOI, const PxF32 chassisMass, const PxVec3 chassisCMOffset, const PxVec3 initPos);
	PxRigidDynamic* createVehicleActor(const PxVehicleChassisData& chassisData,
		PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels,
		PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, PxPhysics& physics, PxVec3 initPos);

	//Wheel positions move to vehicle class
	void computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius,
		const PxU32 numWheels, PxVec3* wheelCentreOffsets);		//Now is a part of the Vehicle class

	//Vehicle and wheel sim data, move to vehicle class
	PxVehicleDriveSimData4W initDriveSimData(PxVehicleWheelsSimData* wheelsSimData);		//Now is a part of the Vehicle class B.M
	PxVehicleWheelsSimData* initWheelSimData(int nbWheels, const PxVec3 chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxF32 wheelMass,
		const PxF32 wheelMOI, const PxVec3 chassisCMOffset, const PxF32 chassisMass);			//Now is a part of the Vehicle class
	void setupWheelsSimulationData
		(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
		const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
		const PxVec3& chassisCMOffset, const PxF32 chassisMass,
		PxVehicleWheelsSimData* wheelsSimData);
	void setupNonDrivableSurface(PxFilterData& filterData);			//Now is a part of the Vehicle class
	PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs(const PxMaterial* defaultMaterial);

	//Initialize ground
	PxRigidStatic* createDrivablePlane(physx::PxMaterial* material, PxPhysics* physics);
	PxRigidStatic* createDrivablePlane(physx::PxMaterial* material, PxPhysics* physics, PxVec3 normal, PxReal offset);
	PxRigidStatic* createDrivableLevel(PxMaterial* material, PxPhysics* physics, MeshObject* levelMesh);

	//Create meshes
	PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking);		//Now is a part of the Vehicle class
	PxConvexMesh* createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking);							//Now is a part of the Vehicle class
	PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking);  //Now is a part of the Vehicle class
	PxTriangleMesh* createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* indices, const PxU32 triCount, PxPhysics& physics, PxCooking& cooking);
	

	void setupObstacleCollisionHandling(PxRigidActor* actor);

	void buggyExplosion(int gBuggyIndex);
	PxVec3 getPxVec3(vec3 vec) { return PxVec3(vec.x, vec.y, vec.z); }
	vec3 getVec3(PxVec3 vec) { return vec3(vec.x, vec.y, vec.z); }

	void bombExplosion(int bombID);
};



mat4 getMat4(const physx::PxTransform& transform);
vec4 getVec4(const physx::PxVec4& vec);


#endif // PHYSICS_H
