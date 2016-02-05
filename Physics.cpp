#include "Physics.h"

#include "PxPhysicsAPI.h"

using namespace physx;

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;

PxFoundation* mFoundation = NULL;
PxPhysics* mPhysics = NULL;
PxCooking* mCooking = NULL;

PxDefaultCpuDispatcher* mDispatcher = NULL;
PxScene* gScene = NULL;

Physics::Physics() {
	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,
		gAllocator, gErrorCallback);
	if (!mFoundation) {
		// Fatal error
	}

	bool recordMemoryAllocations = true;

	// May need to replace null with a ProfileZoneManager
	// Is supposed to be optional
	mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, 
		PxTolerancesScale(), recordMemoryAllocations, NULL);
	if (!mPhysics) {
		// Fatal error
	}

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation,	
		PxCookingParams(PxTolerancesScale()));
	if (!mCooking) {
		// Fatal error
	}

	if (!PxInitExtensions(*mPhysics)) {
		// Fatal error
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
	}
}

void Physics::shutdown() {
	mPhysics->release();
	mFoundation->release();
}

void Physics::startSim(GameState state) {

}

GameState Physics::getSim() {
	return GameState();
}

Entity* Physics::getCollisions() {
	return NULL;
}