#ifndef PHYSICS_H
#define PHYSICS_H

#include <math.h>
#include "MeshObject.h"
#include "LevelInfo.h"
#include "GameState.h"
#include "PxPhysicsAPI.h"
#include "VehicleSceneQueryData.h"
#include "Timer.h"
#include "Input.h"


const float gravity = -9.81f;

using namespace physx;

enum
{
	COLLISION_FLAG_GROUND = 1 << 0,
	COLLISION_FLAG_WHEEL = 1 << 1,
	COLLISION_FLAG_CHASSIS = 1 << 2,
	COLLISION_FLAG_OBSTACLE = 1 << 3,
	COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,

	COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_WHEEL_AGAINST = COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
	COLLISION_FLAG_CHASSIS_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
	COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST = COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
};
/*
enum
{
	DRIVABLE_SURFACE = 0xffff0000,
	UNDRIVABLE_SURFACE = 0x0000ffff
};
*/
//Tire types.
enum
{
	TIRE_TYPE_NORMAL = 0,
	TIRE_TYPE_WORN,
	MAX_NUM_TIRE_TYPES
};

enum
{
	SURFACE_TYPE_TARMAC,
	MAX_NUM_SURFACE_TYPES
};

class Physics {
public:
	Physics();
	void shutdown();
	void startSim(const GameState&);
	GameState getSim();
	Entity* getCollisions();
	void giveInput(Input input, int playerNum);
	void handleInput(Input* input);
private:
	Timer clock;
	void initDefaultScene();
	PxVehicleDrive4W* initVehicle();
	PxVehicleDriveSimData4W initDriveSimData(PxVehicleWheelsSimData* wheelsSimData);
	PxVehicleWheelsSimData* initWheelSimData(int nbWheels, const PxVec3 chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxF32 wheelMass,
		const PxF32 wheelMOI, const PxVec3 chassisCMOffset, const PxF32 chassisMass);
	PxRigidDynamic* initVehicleActor(const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 nbWheels, const PxVec3 chassisDims,
		const PxVec3 chassisMOI, const PxF32 chassisMass, const PxVec3 chassisCMOffset);
	PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking);
	PxConvexMesh* createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking);
	PxRigidDynamic* createVehicleActor
		(const PxVehicleChassisData& chassisData,
		PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels,
		PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes,
		PxPhysics& physics);
	void computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius,
		const PxU32 numWheels, PxVec3* wheelCentreOffsets);
	void setupWheelsSimulationData
		(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
		const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
		const PxVec3& chassisCMOffset, const PxF32 chassisMass,
		PxVehicleWheelsSimData* wheelsSimData);
	void setupNonDrivableSurface(PxFilterData& filterData);
	PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking);
	PxVehicleDrivableSurfaceToTireFrictionPairs* createFrictionPairs(const PxMaterial* defaultMaterial);
};

physx::PxRigidActor* getSphere();	//TEMPORARY FUNCTION
physx::PxRigidDynamic* getCar();	//TEMPORARY FUNCTION
void getWheels(physx::PxShape** wheels);

mat4 getMat4(const physx::PxTransform& transform);
vec4 getVec4(const physx::PxVec4& vec);

#endif // PHYSICS_H
