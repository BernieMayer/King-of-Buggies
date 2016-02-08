#include "Physics.h"

#include <iostream>
#include <stdio.h>  
#include <stdlib.h>  

#include "PxPhysicsAPI.h"

using namespace physx;

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;

PxFoundation* mFoundation = NULL;
PxPhysics* mPhysics = NULL;
PxCooking* mCooking = NULL;

PxDefaultCpuDispatcher* mDispatcher = NULL;
PxScene* gScene = NULL;

PxMaterial* mMaterial = NULL;

//VehicleSceneQueryData* gVehicleSceneQueryData = NULL;

Physics::Physics() {
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,
		gAllocator, gErrorCallback);
	if (!mFoundation) {
		std::cout << "Foundation creation failure\n";
	}

	bool recordMemoryAllocations = true;

	// May need to replace null with a ProfileZoneManager
	// Is supposed to be optional
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, 
		PxTolerancesScale(), recordMemoryAllocations, NULL);
	if (!mPhysics) {
		// Fatal error
		std::cout << "Physics creation failure\n";
	}

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation,	
		PxCookingParams(PxTolerancesScale()));
	if (!mCooking) {
		// Fatal error
		std::cout << "Cooking creation failure\n";
	}

	if (!PxInitExtensions(*mPhysics)) {
		// Fatal error
		std::cout << "Extensions failure\n";
	}

	initDefaultScene();

}

void Physics::initDefaultScene() {
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	// May need to change this number
	PxU32 numWorkers = 1;
	mDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = mDispatcher;

	static PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	if (!sceneDesc.filterShader) {
		sceneDesc.filterShader = gDefaultFilterShader;
	}
	// May want something like this later
	//sceneDesc.filterShader = VehicleFilterShader;

	gScene = mPhysics->createScene(sceneDesc);
	if (!gScene) {
		// Fatal error
		std::cout << "Scene creation failure\n";
	}

	// staticfriction, dynamic friction, restitution
	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.1f);
	if (!mMaterial) {
		// Fatal error
		std::cout << ("Material creation failure\n");
	}

	// Add static ground plane to the scene
	PxRigidStatic* plane = PxCreatePlane(*mPhysics, PxPlane(PxVec3(0, 1, 0), 0),
		*mMaterial);
	if (!plane) {
		// Fatal error
		std::cout << "Plane creation failure\n";
	}
	gScene->addActor(*plane);

	// Add dynamic thrown ball to scene
	PxRigidDynamic* aSphereActor = mPhysics->createRigidDynamic(PxTransform(PxVec3(0, 1, 0)));
	// 0.5 = radius
	PxShape* aSphereShape = aSphereActor->createShape(PxSphereGeometry(0.5), *mMaterial);
	// 1.0f = density
	PxRigidBodyExt::updateMassAndInertia(*aSphereActor, 1.0f);

	// I don't know what effect a 0 vector will have on velocity
	aSphereActor->setLinearVelocity(PxVec3(0));

	gScene->addActor(*aSphereActor);

	initVehicle();
}

void Physics::initVehicle() {
	PxInitVehicleSDK(*mPhysics);
	// Up and forwards
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	// Create the batched scene queries for the supension raycasts
	//gVehicleSceneQueryData

	PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(4);
	PxVehicleDriveSimData4W driveSimData;

	PxVehicleChassisData chassisData;
	//createVehicle4WSimulationData(chassisMass, chasisConvexMesh, 20.0f, wheelConvexMeshs4,
		//wheelCentreOffsets4, *wheelsSimData, driveSimData, chassisData);
}


void Physics::shutdown() {
	mPhysics->release();
	mFoundation->release();
}

void Physics::startSim(const GameState& state) {
	// Simulate at 60 fps... probably what it means
	gScene->simulate(1.0f / 60.0f);
}

GameState Physics::getSim() {
	// Will get the simulation results
	// True means that it will wait until the simulation is done if needed
	gScene->fetchResults(true);
	return GameState();
}

Entity* Physics::getCollisions() {
	return NULL;
}