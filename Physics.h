#ifndef PHYSICS_H
#define PHYSICS_H

#include <math.h>
#include "PxSimulationEventCallback.h"
#include "MeshObject.h"
#include "LevelInfo.h"
#include "GameState.h"
#include "PxPhysicsAPI.h"
#include "VehicleSceneQueryData.h"
#include "Timer.h"
#include "Input.h"
#include "VehicleTraits.h"

//Hello World (Test commit)

const float gravity = -9.81f;

using namespace physx;

//Tire types.
enum
{
	TIRE_TYPE_NORMAL = 0,
	TIRE_TYPE_
	,
	MAX_NUM_TIRE_TYPES
};

enum
{
	SURFACE_TYPE_TARMAC,
	MAX_NUM_SURFACE_TYPES
};

class Physics : public PxSimulationEventCallback {
public:
	Physics();
	void shutdown();
	void startSim(const GameState&, float frameTime);
	//void startSim2(const GameState&);
	GameState getSim();
	Entity* getCollisions();
	void giveInput(Input input, int playerNum);
	void handleInput(Input* input);
	void handleInput(Input* input, unsigned int id);
	VehicleTraits getVehicleTraits();

	/*
	Split between vehicle, ground, and dynamic for the same of collision resolution
	May need to split further at a later point of time
	*/
	unsigned int vehicle_create(VehicleTraits traits, vec3 initPos);	//Returns ID for vehicle
	void vehicle_setVehicleTraits(unsigned int id, VehicleTraits traits);		//Modify vehicle traits
	mat4 vehicle_getGlobalPose(unsigned int id);
	mat4 vehicle_getGlobalPoseWheel(unsigned int id, unsigned int wheelNum);

	unsigned int ground_createPlane(vec3 normal, float offset);	//Returns ID for plane
	unsigned int ground_createGeneric(MeshObject* mesh);
	
	unsigned int dynamic_create(vec3 mesh, vec3 initPos);
	unsigned int dynamic_createSphere(float radius, vec3 initPos);
	mat4 dynamic_getGlobalPose(unsigned int id);

	unsigned int createMaterial(float staticFriction, float dynamicFriction, float restitution);
	PxMaterial* getMaterial();
	PxMaterial* getMaterial(unsigned int materialID);

	void updateGameState(GameState* state);

	// Implements PxSimulationEventCallback
	virtual void  onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs);
	virtual void  onTrigger(PxTriggerPair* pairs, PxU32 count){}
	virtual void  onConstraintBreak(PxConstraintInfo*, PxU32) {}
	virtual void  onWake(PxActor**, PxU32) {}
	virtual void  onSleep(PxActor**, PxU32){}


private:
	Timer clock;
	vector<PxVehicleDrive4W*> vehicleActors;
	vector<PxRigidStatic*> groundActors;
	vector<PxRigidDynamic*> dynamicActors;

	vector<PxMaterial*> materials;

	//
	vector<bool> vehicleForwards;


	void initDefaultScene();
	void initScene();
	PxVehicleDrive4W* initVehicle();
	PxVehicleDrive4W* initVehicle(VehicleTraits traits, PxVec3 initPos);
	PxRigidStatic* createDrivablePlane(physx::PxMaterial* material, PxPhysics* physics);
	PxRigidStatic* createDrivablePlane(physx::PxMaterial* material, PxPhysics* physics, PxVec3 normal, PxReal offset);
	PxRigidStatic* createDrivableLevel(PxMaterial* material, PxPhysics* physics, MeshObject* levelMesh);

	PxVehicleDriveSimData4W initDriveSimData(PxVehicleWheelsSimData* wheelsSimData);
	PxVehicleWheelsSimData* initWheelSimData(int nbWheels, const PxVec3 chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxF32 wheelMass,
		const PxF32 wheelMOI, const PxVec3 chassisCMOffset, const PxF32 chassisMass);
	PxRigidDynamic* initVehicleActor(const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 nbWheels, const PxVec3 chassisDims,
		const PxVec3 chassisMOI, const PxF32 chassisMass, const PxVec3 chassisCMOffset);
	PxRigidDynamic* initVehicleActor(const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 nbWheels, const PxVec3 chassisDims,
		const PxVec3 chassisMOI, const PxF32 chassisMass, const PxVec3 chassisCMOffset, const PxVec3 initPos);
	PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking);
	PxConvexMesh* createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking);
	PxRigidDynamic* createVehicleActor
		(const PxVehicleChassisData& chassisData,
		PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels,
		PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes,
		PxPhysics& physics);
	PxRigidDynamic* createVehicleActor(const PxVehicleChassisData& chassisData,
		PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels,
		PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, PxPhysics& physics, PxVec3 initPos);
	void computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius,
		const PxU32 numWheels, PxVec3* wheelCentreOffsets);
	void setupWheelsSimulationData
		(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
		const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
		const PxVec3& chassisCMOffset, const PxF32 chassisMass,
		PxVehicleWheelsSimData* wheelsSimData);
	void setupNonDrivableSurface(PxFilterData& filterData);

	PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking);
	PxTriangleMesh* createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* indices, const PxU32 triCount, PxPhysics& physics, PxCooking& cooking);

	PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs(const PxMaterial* defaultMaterial);

	void setupObstacleCollisionHandling(PxRigidActor* actor);

};

physx::PxRigidActor* getSphere();	//TEMPORARY FUNCTION
physx::PxRigidDynamic* getCar();	//TEMPORARY FUNCTION
void getWheels(physx::PxShape** wheels);

mat4 getMat4(const physx::PxTransform& transform);
vec4 getVec4(const physx::PxVec4& vec);

#endif // PHYSICS_H
