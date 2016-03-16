#ifndef VEHICLE_H
#define VEHICLE_H

#include "VehicleTraits.h"
#include <glm\glm.hpp>

#include <math.h>
//#include "PxSimulationEventCallback.h"
//#include "MeshObject.h"
//#include "LevelInfo.h"
//#include "GameState.h"
#include "PxPhysicsAPI.h"
#include "VehicleSceneQueryData.h"
//#include "Timer.h"
//#include "Input.h"
#include "VehicleTraits.h"
//#include "CollisionEvent.h"
#include "VehicleCollisionEvent.h"
#include "VehicleWallCollisionEvent.h"

using namespace physx;
using namespace glm;

enum
{
	TIRE_TYPE_NORMAL = 0,
	TIRE_TYPE_
	,
	MAX_NUM_TIRE_TYPES
};


class Vehicle {
public:
	static int numOfVehicles = 0;

	PxVehicleDrive4W* vehDrive4W;

	

	int gear;

	bool inAir;

	int forwards; 
	unsigned int vehicle_create(VehicleTraits traits, vec3 initPos);	//Returns ID for vehicle

	Vehicle(PxPhysics& phys, PxCooking& cook);

private:

	PxMaterial* mMaterial = NULL;
	PxPhysics* mPhysics = NULL;
	PxCooking* mCooking = NULL;


	Vehicle* initVehicle(VehicleTraits traits, PxVec3 initPos);
	PxRigidDynamic* initVehicleActor(const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 nbWheels, const PxVec3 chassisDims,
		const PxVec3 chassisMOI, const PxF32 chassisMass, const PxVec3 chassisCMOffset, const PxVec3 initPos);

	PxRigidDynamic* createVehicleActor(const PxVehicleChassisData& chassisData,
		PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels,
		PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, PxPhysics& physics, PxVec3 initPos);
	PxVehicleDriveSimData4W initDriveSimData(PxVehicleWheelsSimData* wheelsSimData);

	PxVehicleWheelsSimData* initWheelSimData(int nbWheels, const PxVec3 chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxF32 wheelMass,
		const PxF32 wheelMOI, const PxVec3 chassisCMOffset, const PxF32 chassisMass);

	void computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius,
		const PxU32 numWheels, PxVec3* wheelCentreOffsets);

	void setupWheelsSimulationData
		(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
		const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
		const PxVec3& chassisCMOffset, const PxF32 chassisMass,
		PxVehicleWheelsSimData* wheelsSimData);

	//Mesh stuff
	PxConvexMesh* createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking);
	PxConvexMesh* createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking);
	PxConvexMesh* createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking);

	void setupNonDrivableSurface(PxFilterData& filterData);

};


#endif //  VEHICLE_H