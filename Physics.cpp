#include "Physics.h"

#include <iostream>
#include <stdio.h>  
#include <stdlib.h>  


#define MAX_NUM_ACTOR_SHAPES 128

using namespace physx;

PxDefaultAllocator gAllocator;
PxDefaultErrorCallback gErrorCallback;

PxFoundation* mFoundation = NULL;
PxPhysics* mPhysics = NULL;
PxCooking* mCooking = NULL;

PxDefaultCpuDispatcher* mDispatcher = NULL;
PxScene* gScene = NULL;

PxMaterial* mMaterial = NULL;

PxRigidStatic* plane;
PxRigidDynamic* aSphereActor;

//VehicleSceneQueryData* gVehicleSceneQueryData = NULL;

//TEMPORARY FUNCTION
PxRigidActor* getSphere() { return aSphereActor; }

vec4 getVec4(const PxVec4& vec)
{
	return vec4(vec.x, vec.y, vec.z, vec.w);
}

mat4 getMat4(const PxTransform& transform)
{
	PxMat44 physxMatrix = PxMat44(transform);
	mat4 finalMatrix(	getVec4(physxMatrix[0]), 
						getVec4(physxMatrix[1]), 
						getVec4(physxMatrix[2]), 
						getVec4(physxMatrix[3]));

	return finalMatrix;
}

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
	sceneDesc.gravity = PxVec3(0.0f, gravity, 0.0f);

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
	plane = PxCreatePlane(*mPhysics, PxPlane(PxVec3(0, 1, 0), 0),
		*mMaterial);
	if (!plane) {
		// Fatal error
		std::cout << "Plane creation failure\n";
	}
	gScene->addActor(*plane);

	// Add dynamic thrown ball to scene
	aSphereActor = mPhysics->createRigidDynamic(PxTransform(PxVec3(0, 5.f, 0)));
	// 0.5 = radius
	PxShape* aSphereShape = aSphereActor->createShape(PxSphereGeometry(0.5), *mMaterial);
	// 1.0f = density
	PxRigidBodyExt::updateMassAndInertia(*aSphereActor, 1.0f);

	// I don't know what effect a 3 vector will have on velocity
	//aSphereActor->setLinearVelocity(PxVec3(3));

	gScene->addActor(*aSphereActor);

	//initVehicle();
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
	float timeElapsed = clock.getElapsedTime();

	gScene->simulate(max(timeElapsed, 0.0001f));
}

GameState Physics::getSim() {
	// Will get the simulation results
	// True means that it will wait until the simulation is done if needed
	gScene->fetchResults(true);

	// An example of how to get position
	PxVec3 pos = aSphereActor->getGlobalPose().p;
	//std::cout << "Sphere Y: " << pos.y << "\n";

	// How to get rotation
	PxQuat rotation = aSphereActor->getGlobalPose().q;

	// Get number of shapes
	const PxU32 nbShapes = aSphereActor->getNbShapes();
	PxShape* shapes[MAX_NUM_ACTOR_SHAPES];
	aSphereActor->getShapes(shapes, nbShapes);
	// Get geometry
	PxGeometryHolder geo = shapes[0]->getGeometry();
	// Find type (Box, Capsule, convex mesh, heightfield, plane, sphere or triangle mesh)
	geo.getType();
	// Get actual sphere
	PxSphereGeometry sphere = geo.sphere();
	// This will contain stuff about the shape of the object
	// In the case of a sphere, only the radius
	

	return GameState();
}

Entity* Physics::getCollisions() {
	return NULL;
}