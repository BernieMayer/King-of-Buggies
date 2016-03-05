#ifndef PHYSICS_CPP
#define PHYSICS_CPP
#include "Physics.h"

#include <iostream>
#include <stdio.h>  
#include <stdlib.h> 

#include "SnippetVehicleFilterShader.h"




#define MAX_NUM_ACTOR_SHAPES 128

struct FilterGroup
{
	enum Enum
	{
		eVEHICLE = (1 << 0),
		eSPHERE  = (1 << 1),
		eGROUND  = (1 << 2)
	};
};

PxVehicleKeySmoothingData gKeySmoothingData =
{
	{
		3.0f,   //rise rate eANALOG_INPUT_ACCEL
		3.0f,   //rise rate eANALOG_INPUT_BRAKE
		10.0f,  //rise rate eANALOG_INPUT_HANDBRAKE
		2.5f,   //rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,   //rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		5.0f,   //fall rate eANALOG_INPUT__ACCEL
		5.0f,   //fall rate eANALOG_INPUT__BRAKE
		10.0f,  //fall rate eANALOG_INPUT__HANDBRAKE
		5.0f,   //fall rate eANALOG_INPUT_STEER_LEFT
		5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxVehiclePadSmoothingData gCarPadSmoothingData =
{
	{
		6.0f,   //rise rate eANALOG_INPUT_ACCEL
		6.0f,   //rise rate eANALOG_INPUT_BRAKE
		12.0f,  //rise rate eANALOG_INPUT_HANDBRAKE
		2.5f,   //rise rate eANALOG_INPUT_STEER_LEFT
		2.5f,   //rise rate eANALOG_INPUT_STEER_RIGHT
	},
	{
		10.0f,  //fall rate eANALOG_INPUT_ACCEL
		10.0f,  //fall rate eANALOG_INPUT_BRAKE
		12.0f,  //fall rate eANALOG_INPUT_HANDBRAKE
		5.0f,   //fall rate eANALOG_INPUT_STEER_LEFT
		5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
	}
};

PxF32 gSteerVsForwardSpeedData[2 * 8] =
{
	0.0f, 0.75f,
	5.0f, 0.75f,
	30.0f, 0.125f,
	120.0f, 0.1f,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32,
	PX_MAX_F32, PX_MAX_F32
};
PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData, 4);

static PxF32 gTireFrictionMultipliers[MAX_NUM_SURFACE_TYPES][MAX_NUM_TIRE_TYPES] =
{
	//NORMAL,	WORN
	{ 2.f, 0.5f }//TARMAC
};


//Need to get rid of all of these globals


//VehicleTraits traits = NULL;

bool forwards = true;

void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask);


vec4 getVec4(const PxVec4& vec)
{
	return vec4(vec.x, vec.y, vec.z, vec.w);
}

mat4 getMat4(const PxTransform& transform)
{
	PxMat44 physxMatrix = PxMat44(transform);
	mat4 finalMatrix(getVec4(physxMatrix[0]),
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

	PxInitVehicleSDK(*mPhysics);
	// Up and forwards
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	initScene();

}

unsigned int Physics::vehicle_create(VehicleTraits traits, vec3 initPos)
{
	vehicleActors.push_back(initVehicle(traits, PxVec3(initPos.x, initPos.y, initPos.z)));

	//This shouldn't be in physics at all
	//Easy way for PHYSX to be notified that a vehicle is the goldenBuggy
	if (vehicleActors.size() == 1){
		goldenBuggy = vehicleActors[0];
		modifySpeed(0, 3);
	}
	vehicleForwards.push_back(-1);
	return vehicleActors.size() - 1;
}
void Physics::modifySpeed(unsigned int vehicleNum , float modSpeed)
{
	PxVehicleDrive4W* veh = vehicleActors[vehicleNum];
	PxVehicleEngineData engine = veh->mDriveSimData.getEngineData();
	engine.mPeakTorque = engine.mPeakTorque + modSpeed * engine.mPeakTorque;
	veh->mDriveSimData.setEngineData(engine);
	

}

void Physics::setSpeed(unsigned int vehicleNum, float speed)
{
	PxVehicleDrive4W* veh = vehicleActors[vehicleNum];
	PxVehicleEngineData engine = veh->mDriveSimData.getEngineData();
	engine.mPeakTorque = speed;
	veh->mDriveSimData.setEngineData(engine);

}

void Physics::vehicle_setVehicleTraits(unsigned int id, VehicleTraits traits)
{ 
	//To be implemented 
}

mat4 Physics::vehicle_getGlobalPose(unsigned int id)
{
	if (id >= vehicleActors.size())
	{
		printf("Error: Vehicle does not exist\n");
		return mat4(1.f);
	}
	return getMat4(vehicleActors[id]->getRigidDynamicActor()->getGlobalPose());
}

mat4 Physics::vehicle_getGlobalPoseWheel(unsigned int id, unsigned int wheelNum)
{
	if (id >= vehicleActors.size())
	{
		printf("Error: Vehicle does not exist\n");
		return mat4(1.f);
	}
	else if (wheelNum > 3)
	{
		printf("Error: Wheel number must be between 0 and 3\n");
		return mat4(1.f);
	}

	PxShape* wheelShape[1];
	vehicleActors[id]->getRigidDynamicActor()->getShapes(wheelShape, 1, wheelNum);

	return vehicle_getGlobalPose(id)*getMat4(wheelShape[0]->getLocalPose());
}

float Physics::vehicle_getFSpeed(unsigned int id) {
	if (id >= vehicleActors.size())
	{
		printf("Error: Vehicle does not exist\n");
		return 0.0f;
	}
	return vehicleActors[id]->computeForwardSpeed();
}

float Physics::vehicle_getSSpeed(unsigned int id) {
	if (id >= vehicleActors.size())
	{
		printf("Error: Vehicle does not exist\n");
		return 0.0f;
	}

	return vehicleActors[id]->computeSidewaysSpeed();
}

bool Physics::vehicle_getForwardsGear(unsigned int id) {
	if (id >= vehicleActors.size())
	{
		printf("Error: Vehicle does not exist\n");
		return 0.0f;
	}

	if (vehicleForwards[id] == 1) {
		return true;
	}
	else {
		return false;
	}
}

float Physics::vehicle_getEngineSpeed(unsigned int id) {
	if (id >= vehicleActors.size())
	{
		printf("Error: Vehicle does not exist\n");
		return 0.0f;
	}

	return vehicleActors[id]->mDriveDynData.getEngineRotationSpeed();
}


unsigned int Physics::ground_createPlane(vec3 normal, float offset)
{
	if (normal == vec3(0.f))
	{
		printf("Error: Normal can't be zero\n");
		return ULLONG_MAX;
	}

	normal = normalize(normal);

	groundActors.push_back(createDrivablePlane(mMaterial, mPhysics, PxVec3(normal.x, normal.y, normal.z), offset));

	if (!groundActors[groundActors.size() - 1])
		printf("Error: Failed to create plane\n");

	gScene->addActor(*groundActors[groundActors.size() - 1]);

	return groundActors.size() - 1;
}

PxRigidStatic* Physics::createDrivableLevel(PxMaterial* material, PxPhysics* physics, MeshObject* levelMesh)
{
	//Add a plane to the scene.
	PxRigidStatic* groundLevel = mPhysics->createRigidStatic(PxTransform(PxIdentity));

	vector<PxVec3> tempVertices = levelMesh->getPhysicsVertices();
	vector<PxU32> tempIndices = levelMesh->getIndices();

	unsigned int verticesSize = tempVertices.size();
	unsigned int indicesSize = tempIndices.size();

	PxVec3* vertices = new PxVec3[verticesSize];
	//int i = 0;
	for (unsigned int i = 0; i < verticesSize; i++) {
		vertices[i] = tempVertices[i];
	}

	PxU32* indices = new PxU32[indicesSize];
	for (unsigned int i = 0; i < indicesSize; i++) {
		indices[i] = (PxU32)tempIndices[i];
	}

	PxTriangleMesh* triangleMesh = createTriangleMesh(vertices, verticesSize, indices, (indicesSize / 3), *mPhysics, *mCooking);
	groundLevel->createShape(PxTriangleMeshGeometry(triangleMesh), *material);

	//Get the plane shape so we can set query and simulation filter data.
	PxShape* shapes[1];
	groundLevel->getShapes(shapes, 1);

	//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
	PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	shapes[0]->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
	PxFilterData simFilterData;
	simFilterData.word0 = COLLISION_FLAG_GROUND;
	simFilterData.word1 = COLLISION_FLAG_GROUND_AGAINST;
	shapes[0]->setSimulationFilterData(simFilterData);



	delete[] vertices;
	delete[] indices;

	return groundLevel;
}

unsigned int Physics::ground_createGeneric(MeshObject* mesh)
{
	groundActors.push_back(createDrivableLevel(mMaterial, mPhysics, mesh));

	if (!groundActors[groundActors.size() - 1])
		printf("Error: Failed to create level\n");

	gScene->addActor(*groundActors[groundActors.size() - 1]);

	return groundActors.size() - 1;
}


unsigned int Physics::dynamic_create(vec3 mesh, vec3 initPos)
{
	//To be implemented
	return 0;
}

unsigned int Physics::dynamic_createSphere(float radius, vec3 initPos)
{
	// Add dynamic thrown ball to scene
	PxRigidDynamic* aSphereActor = mPhysics->createRigidDynamic(PxTransform(PxVec3(initPos.x, initPos.y, initPos.z)));
	// 0.5 = radius
	PxShape* aSphereShape = aSphereActor->createShape(PxSphereGeometry(radius), *mMaterial);
	setupObstacleCollisionHandling(aSphereActor);

	// 1.0f = density
	PxRigidBodyExt::updateMassAndInertia(*aSphereActor, 1.0f);

	// I don't know what effect a 3 vector will have on velocity
	aSphereActor->setLinearVelocity(PxVec3(0.2f, 0.f, 0.f));

	gScene->addActor(*aSphereActor);

	dynamicActors.push_back(aSphereActor);

	return dynamicActors.size() - 1;
}

mat4 Physics::dynamic_getGlobalPose(unsigned int id)
{
	if (id >= dynamicActors.size())
	{
		printf("Error: Vehicle does not exist\n");
		return mat4(1.f);
	}
	return getMat4(dynamicActors[id]->getGlobalPose());
}

unsigned int Physics::createMaterial(float staticFriction, float dynamicFriction, float restitution)
{
	materials.push_back(mPhysics->createMaterial(staticFriction, dynamicFriction, restitution));
	
	//Friction pairs

	return materials.size() - 1;
}

PxMaterial* Physics::getMaterial()
{
	return mMaterial;
}


void Physics::updateGameState(GameState* state, float time)
{
	for (unsigned int i = 0; i < state->numberOfPlayers();  i++)
	{
		PlayerInfo* player = state->getPlayer(i);
		player->setTransform(vehicle_getGlobalPose(player->getPhysicsID()), time);
	
		for (unsigned int j = 0; j < 4; j++)
		{
			player->setWheelTransform(j, vehicle_getGlobalPoseWheel(player->getPhysicsID(), j));
		}

		player->setFSpeed(vehicle_getFSpeed(player->getPhysicsID()));
		player->setSSpeed(vehicle_getSSpeed(player->getPhysicsID()));
		player->setForwardsGear(vehicle_getForwardsGear(player->getPhysicsID()));
		player->setEngineSpeed(vehicle_getEngineSpeed(player->getPhysicsID()));
	}

	for (unsigned int i = 0; i < state->numberOfPowerups(); i++)
	{
		Powerup* powerup = state->getPowerup(i);
		powerup->setTransform(dynamic_getGlobalPose(powerup->getPhysicsID()), time);
	}

	lastState = state;
}

/**
 * Converts the input from input manager
 */
void Physics::giveInput(Input input, int playernum) {

	PxVehicleDrive4WRawInputData* pxInput;

	if (input.isKeyboard) {
		if (input.forward > 0) {
			pxInput->setDigitalAccel(true);
		}
		else {
			pxInput->setDigitalAccel(false);
		}

		if (input.backward > 0) {
			pxInput->setDigitalBrake(true);
		}
		else {
			pxInput->setDigitalBrake(false);
		}
		
		if (input.turnL > 0.f) {
			cout << "Turn left" << endl;
			pxInput->setDigitalSteerLeft(true);
		}
		else {
			pxInput->setDigitalSteerLeft(false);
		}
		
		if (input.turnR > 0.f) {
			cout << "Turn right" << endl;
			pxInput->setDigitalSteerRight(true);
		}
		else {
			pxInput->setDigitalSteerRight(false);
		}
	}
	else {
		pxInput->setAnalogAccel(input.forward);
		pxInput->setAnalogBrake(input.backward);
		pxInput->setAnalogSteer(input.turnR - input.turnL);
	}
	
	inputs[playernum] = pxInput;
}



void Physics::handleInput(Input* input, unsigned int id){

	PxVehicleDrive4W* vehicle = vehicleActors[id];

	//cout << "Engine speed: " << vehicle->mDriveDynData.getEngineRotationSpeed() << "\n";

	float fSpeed = vehicle->computeForwardSpeed();

	// May need to change speed checks to be something like between 0.1 and -0.1
	// And then may need a timer to prevent rapid gear changes during wobbling
	if ((input->forward > input->backward) && (vehicleForwards[id] == 0 || vehicleForwards[id] == -1)) {
		// If not moving and was in reverse gear, but more forwards
		// input than backwards, switch to forwards gear
		vehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
		vehicleForwards[id] = 1;
	}
	else if ((input->forward < input->backward) && (vehicleForwards[id] == 1 || vehicleForwards[id] == -1)) {
		vehicle->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
		vehicleForwards[id] = 0;
	}

	if (vehicleForwards[id] == 1) {
		vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, input->forward);
		vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, input->backward);
	}
	else if (vehicleForwards[id] == 0) {
		vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, input->backward);
		vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, input->forward);
	}

	vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_LEFT, input->turnL);
	vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_STEER_RIGHT, input->turnR);

	if (input->drift) {
		vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_HANDBRAKE, 1);
	}
	else {
		vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_HANDBRAKE, 0);
	}

}

void Physics::initScene()
{
	PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, gravity, 0.0f);
	sceneDesc.simulationEventCallback = this;
	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;

	// May need to change this number
	PxU32 numWorkers = 1;
	mDispatcher = PxDefaultCpuDispatcherCreate(numWorkers);
	sceneDesc.cpuDispatcher = mDispatcher;

	static PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
	if (!sceneDesc.filterShader) {
		sceneDesc.filterShader = gDefaultFilterShader;
	}
	// May want something like this later

	sceneDesc.filterShader = VehicleFilterShader;

	gScene = mPhysics->createScene(sceneDesc);
	if (!gScene) {
		// Fatal error
		std::cout << "Scene creation failure\n";
	}

	//Get rid of eventually
	// staticfriction, dynamic friction, restitution
	mMaterial = mPhysics->createMaterial(0.8f, 0.8f, 0.6f);
	if (!mMaterial) {
		// Fatal error
		std::cout << ("Material creation failure\n");
	}

	//Create the batched scene queries for the suspension raycasts.
	gVehicleSceneQueryData = VehicleSceneQueryData::allocate(1, PX_MAX_NB_WHEELS, 1, gAllocator);
	gBatchQuery = VehicleSceneQueryData::setUpBatchedSceneQuery(0, *gVehicleSceneQueryData, gScene);

	gFrictionPairs = createFrictionPairs(mMaterial);
	printf("Friction = %f\n", mMaterial->getDynamicFriction());
}


PxVehicleDrive4W* Physics::initVehicle(VehicleTraits traits, PxVec3 initPos) {
	
	// Create the batched scene queries for the supension raycasts
	//gVehicleSceneQueryData

	PxRigidDynamic* veh4WActor = initVehicleActor(traits.wheelWidth, traits.wheelRadius,
		traits.numWheels, traits.chassisDims, traits.chassisMOI,
		traits.chassisMass, traits.chassisCMOffset, initPos);

	//Set up the sim data for the wheels.
	PxVehicleWheelsSimData* wheelsSimData = initWheelSimData(traits.numWheels,
		traits.chassisDims, traits.wheelWidth, traits.wheelRadius,
		traits.wheelMass, traits.wheelMOI, traits.chassisCMOffset,
		traits.chassisMass);

	//Set up the sim data for the vehicle drive model.
	PxVehicleDriveSimData4W driveSimData = initDriveSimData(wheelsSimData);

	//Create a vehicle from the wheels and drive sim data.
	PxVehicleDrive4W* vehDrive4W = PxVehicleDrive4W::allocate(traits.numWheels);
	vehDrive4W->setup(mPhysics, veh4WActor, *wheelsSimData, driveSimData,
		traits.numWheels - 4);


	//Free the sim data because we don't need that any more.
	wheelsSimData->free();

	gScene->addActor(*veh4WActor);

	vehDrive4W->mDriveDynData.setUseAutoGears(true);

	return vehDrive4W;
}


PxVehicleDriveSimData4W Physics::initDriveSimData(PxVehicleWheelsSimData* wheelsSimData) {
	PxVehicleDriveSimData4W driveSimData;

	//Diff
	PxVehicleDifferential4WData diff;
	diff.mType = PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
	driveSimData.setDiffData(diff);

	//Engine
	PxVehicleEngineData engine;
	engine.mPeakTorque = 500.0f;
	engine.mMaxOmega = 300.0f;//approx 6000 rpm
	driveSimData.setEngineData(engine);


	// TODO: Don't want gears. Will
	// Changing switch time to 0 be good enough for that?
	//Gears
	PxVehicleGearsData gears;
	gears.mSwitchTime = 0.0f;
	driveSimData.setGearsData(gears);

	//Clutch
	PxVehicleClutchData clutch;
	clutch.mStrength = 10.0f;
	driveSimData.setClutchData(clutch);

	//Ackermann steer accuracy
	PxVehicleAckermannGeometryData ackermann;
	ackermann.mAccuracy = 1.0f;
	ackermann.mAxleSeparation =
		wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).z -
		wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).z;
	ackermann.mFrontWidth =
		wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_RIGHT).x -
		wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eFRONT_LEFT).x;
	ackermann.mRearWidth =
		wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_RIGHT).x -
		wheelsSimData->getWheelCentreOffset(PxVehicleDrive4WWheelOrder::eREAR_LEFT).x;
	driveSimData.setAckermannGeometryData(ackermann);

	return driveSimData;
}

PxVehicleWheelsSimData* Physics::initWheelSimData(int nbWheels, const PxVec3 chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxF32 wheelMass,
	const PxF32 wheelMOI, const PxVec3 chassisCMOffset, const PxF32 chassisMass) {
	PxVehicleWheelsSimData* wheelsSimData = PxVehicleWheelsSimData::allocate(nbWheels);

	//Compute the wheel center offsets from the origin.
	PxVec3 wheelCenterActorOffsets[PX_MAX_NB_WHEELS];
	const PxF32 frontZ = chassisDims.z*0.3f;
	const PxF32 rearZ = -chassisDims.z*0.3f;
	computeWheelCenterActorOffsets4W(frontZ, rearZ, chassisDims, wheelWidth, wheelRadius, nbWheels, wheelCenterActorOffsets);

	//Set up the simulation data for all wheels.
	setupWheelsSimulationData
		(wheelMass, wheelMOI, wheelRadius, wheelWidth,
		nbWheels, wheelCenterActorOffsets,
		chassisCMOffset, chassisMass,
		wheelsSimData);

	return wheelsSimData;
}


void setupFiltering(PxRigidActor* actor, PxU32 filterGroup, PxU32 filterMask)
{
	PxFilterData filterData;
	filterData.word0 = filterGroup; // word0 = own ID
	filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback;
	const PxU32 numShapes = actor->getNbShapes();
	PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*)*numShapes);
	actor->getShapes(shapes, numShapes);
	for (PxU32 i = 0; i < numShapes; i++)
	{
		PxShape* shape = shapes[i];
		shape->setSimulationFilterData(filterData);
	}
	free(shapes);
}



PxRigidDynamic* Physics::initVehicleActor(const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 nbWheels, const PxVec3 chassisDims,
	const PxVec3 chassisMOI, const PxF32 chassisMass, const PxVec3 chassisCMOffset, const PxVec3 initPos) {

	PxRigidDynamic* veh4WActor = NULL;

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	PxMaterial* wheelMaterial = mMaterial;

	//Construct a convex mesh for a cylindrical wheel.
	PxConvexMesh* wheelMesh = createWheelMesh(wheelWidth, wheelRadius, *mPhysics, *mCooking);
	//Assume all wheels are identical for simplicity.
	PxConvexMesh* wheelConvexMeshes[PX_MAX_NB_WHEELS];
	PxMaterial* wheelMaterials[PX_MAX_NB_WHEELS];

	//Set the meshes and materials for the driven wheels.
	for (PxU32 i = PxVehicleDrive4WWheelOrder::eFRONT_LEFT; i <= PxVehicleDrive4WWheelOrder::eREAR_RIGHT; i++)
	{
		wheelConvexMeshes[i] = wheelMesh;
		wheelMaterials[i] = wheelMaterial;
	}
	//Set the meshes and materials for the non-driven wheels
	for (PxU32 i = PxVehicleDrive4WWheelOrder::eREAR_RIGHT + 1; i < nbWheels; i++)
	{
		wheelConvexMeshes[i] = wheelMesh;
		wheelMaterials[i] = wheelMaterial;
	}

	PxMaterial* chassisMaterial = wheelMaterial;
	//Chassis just has a single convex shape for simplicity.
	PxConvexMesh* chassisConvexMesh = createChassisMesh(chassisDims, *mPhysics, *mCooking);
	PxConvexMesh* chassisConvexMeshes[1] = { chassisConvexMesh };
	PxMaterial* chassisMaterials[1] = { chassisMaterial };

	//Rigid body data.
	PxVehicleChassisData rigidBodyData;
	rigidBodyData.mMOI = chassisMOI;
	rigidBodyData.mMass = chassisMass;
	rigidBodyData.mCMOffset = chassisCMOffset;

	veh4WActor = createVehicleActor
		(rigidBodyData,
		wheelMaterials, wheelConvexMeshes, nbWheels,
		chassisMaterials, chassisConvexMeshes, 1,
		*mPhysics, initPos);

	return veh4WActor;
}

void Physics::setupWheelsSimulationData
(const PxF32 wheelMass, const PxF32 wheelMOI, const PxF32 wheelRadius, const PxF32 wheelWidth,
const PxU32 numWheels, const PxVec3* wheelCenterActorOffsets,
const PxVec3& chassisCMOffset, const PxF32 chassisMass,
PxVehicleWheelsSimData* wheelsSimData)
{
	//Set up the wheels.
	PxVehicleWheelData wheels[PX_MAX_NB_WHEELS];
	{
		//Set up the wheel data structures with mass, moi, radius, width.
		for (PxU32 i = 0; i < numWheels; i++)
		{
			wheels[i].mMass = wheelMass;
			wheels[i].mMOI = wheelMOI;
			wheels[i].mRadius = wheelRadius;
			wheels[i].mWidth = wheelWidth;
		}

		//Enable the handbrake for the rear wheels only.
		wheels[PxVehicleDrive4WWheelOrder::eREAR_LEFT].mMaxHandBrakeTorque = 4000.0f;
		wheels[PxVehicleDrive4WWheelOrder::eREAR_RIGHT].mMaxHandBrakeTorque = 4000.0f;
		//Enable steering for the front wheels only.
		wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi*0.25f;
		wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi*0.25f;
	}

	//Set up the tires.
	PxVehicleTireData tires[PX_MAX_NB_WHEELS];
	{
		//Set up the tires.
		for (PxU32 i = 0; i < numWheels; i++)
		{
			tires[i].mType = TIRE_TYPE_NORMAL;
		}
	}

	//Set up the suspensions
	PxVehicleSuspensionData suspensions[PX_MAX_NB_WHEELS];
	{
		//Compute the mass supported by each suspension spring.
		PxF32 suspSprungMasses[PX_MAX_NB_WHEELS];
		PxVehicleComputeSprungMasses
			(numWheels, wheelCenterActorOffsets,
			chassisCMOffset, chassisMass, 1, suspSprungMasses);

		//Set the suspension data.
		for (PxU32 i = 0; i < numWheels; i++)
		{
			suspensions[i].mMaxCompression = 0.3f;
			suspensions[i].mMaxDroop = 0.1f;
			suspensions[i].mSpringStrength = 35000.0f;
			suspensions[i].mSpringDamperRate = 4500.0f;
			suspensions[i].mSprungMass = suspSprungMasses[i];
		}

		//Set the camber angles.
		const PxF32 camberAngleAtRest = 0.0;
		const PxF32 camberAngleAtMaxDroop = 0.01f;
		const PxF32 camberAngleAtMaxCompression = -0.01f;
		for (PxU32 i = 0; i < numWheels; i += 2)
		{
			suspensions[i + 0].mCamberAtRest = camberAngleAtRest;
			suspensions[i + 1].mCamberAtRest = -camberAngleAtRest;
			suspensions[i + 0].mCamberAtMaxDroop = camberAngleAtMaxDroop;
			suspensions[i + 1].mCamberAtMaxDroop = -camberAngleAtMaxDroop;
			suspensions[i + 0].mCamberAtMaxCompression = camberAngleAtMaxCompression;
			suspensions[i + 1].mCamberAtMaxCompression = -camberAngleAtMaxCompression;
		}
	}

	//Set up the wheel geometry.
	PxVec3 suspTravelDirections[PX_MAX_NB_WHEELS];
	PxVec3 wheelCentreCMOffsets[PX_MAX_NB_WHEELS];
	PxVec3 suspForceAppCMOffsets[PX_MAX_NB_WHEELS];
	PxVec3 tireForceAppCMOffsets[PX_MAX_NB_WHEELS];
	{
		//Set the geometry data.
		for (PxU32 i = 0; i < numWheels; i++)
		{
			//Vertical suspension travel.
			suspTravelDirections[i] = PxVec3(0, -1, 0);

			//Wheel center offset is offset from rigid body center of mass.
			wheelCentreCMOffsets[i] =
				wheelCenterActorOffsets[i] - chassisCMOffset;

			//Suspension force application point 0.3 metres below 
			//rigid body center of mass.
			suspForceAppCMOffsets[i] =
				PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);

			//Tire force application point 0.3 metres below 
			//rigid body center of mass.
			tireForceAppCMOffsets[i] =
				PxVec3(wheelCentreCMOffsets[i].x, -0.3f, wheelCentreCMOffsets[i].z);
		}
	}

	//Set up the filter data of the raycast that will be issued by each suspension.
	PxFilterData qryFilterData;
	setupNonDrivableSurface(qryFilterData);

	//Set the wheel, tire and suspension data.
	//Set the geometry data.
	//Set the query filter data
	for (PxU32 i = 0; i < numWheels; i++)
	{
		wheelsSimData->setWheelData(i, wheels[i]);
		wheelsSimData->setTireData(i, tires[i]);
		wheelsSimData->setSuspensionData(i, suspensions[i]);
		wheelsSimData->setSuspTravelDirection(i, suspTravelDirections[i]);
		wheelsSimData->setWheelCentreOffset(i, wheelCentreCMOffsets[i]);
		wheelsSimData->setSuspForceAppPointOffset(i, suspForceAppCMOffsets[i]);
		wheelsSimData->setTireForceAppPointOffset(i, tireForceAppCMOffsets[i]);
		wheelsSimData->setSceneQueryFilterData(i, qryFilterData);
		wheelsSimData->setWheelShapeMapping(i, i);
	}
}

void Physics::setupObstacleCollisionHandling(PxRigidActor* actor)
{
	PxShape* shapes[1];
	actor->getShapes(shapes, 1);

	//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
	PxFilterData simFilterData;
	simFilterData.word0 = COLLISION_FLAG_OBSTACLE;
	simFilterData.word1 = COLLISION_FLAG_OBSTACLE_AGAINST;
	shapes[0]->setSimulationFilterData(simFilterData);
}

PxRigidStatic* Physics::createDrivablePlane(physx::PxMaterial* material, PxPhysics* physics)
{
	//Add a plane to the scene.
	PxRigidStatic* groundPlane = PxCreatePlane(*physics, PxPlane(0, 1, 0, 0), *material);

	//Get the plane shape so we can set query and simulation filter data.
	PxShape* shapes[1];
	groundPlane->getShapes(shapes, 1);

	//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
	physx::PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	shapes[0]->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
	PxFilterData simFilterData;
	simFilterData.word0 = COLLISION_FLAG_GROUND;
	simFilterData.word1 = COLLISION_FLAG_GROUND_AGAINST;
	shapes[0]->setSimulationFilterData(simFilterData);

	return groundPlane;
}

PxRigidStatic* Physics::createDrivablePlane(PxMaterial* material, PxPhysics* physics, PxVec3 normal, PxReal offset)
{
	//Add a plane to the scene.
	PxRigidStatic* groundPlane = PxCreatePlane(*physics, 
		PxPlane(normal.x, normal.y, normal.z, offset), *material);

	//Get the plane shape so we can set query and simulation filter data.
	PxShape* shapes[1];
	groundPlane->getShapes(shapes, 1);

	//Set the query filter data of the ground plane so that the vehicle raycasts can hit the ground.
	physx::PxFilterData qryFilterData;
	setupDrivableSurface(qryFilterData);
	shapes[0]->setQueryFilterData(qryFilterData);

	//Set the simulation filter data of the ground plane so that it collides with the chassis of a vehicle but not the wheels.
	PxFilterData simFilterData;
	simFilterData.word0 = COLLISION_FLAG_GROUND;
	simFilterData.word1 = COLLISION_FLAG_GROUND_AGAINST;
	shapes[0]->setSimulationFilterData(simFilterData);

	return groundPlane;
}



void Physics::computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 numWheels, PxVec3* wheelCentreOffsets)
{
	//chassisDims.z is the distance from the rear of the chassis to the front of the chassis.
	//The front has z = 0.5*chassisDims.z and the rear has z = -0.5*chassisDims.z.
	//Compute a position for the front wheel and the rear wheel along the z-axis.
	//Compute the separation between each wheel along the z-axis.
	const PxF32 numLeftWheels = numWheels / 2.0f;
	const PxF32 deltaZ = (wheelFrontZ - wheelRearZ) / (numLeftWheels - 1.0f);
	//Set the outside of the left and right wheels to be flush with the chassis.
	//Set the top of the wheel to be just touching the underside of the chassis.
	//Begin by setting the rear-left/rear-right/front-left,front-right wheels.
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_LEFT] = PxVec3((-chassisDims.x + wheelWidth)*0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + 0 * deltaZ*0.5f);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eREAR_RIGHT] = PxVec3((+chassisDims.x - wheelWidth)*0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + 0 * deltaZ*0.5f);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_LEFT] = PxVec3((-chassisDims.x + wheelWidth)*0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1)*deltaZ);
	wheelCentreOffsets[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] = PxVec3((+chassisDims.x - wheelWidth)*0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + (numLeftWheels - 1)*deltaZ);
	//Set the remaining wheels.
	for (PxU32 i = 2, wheelCount = 4; i < numWheels - 2; i += 2, wheelCount += 2)
	{
		wheelCentreOffsets[wheelCount + 0] = PxVec3((-chassisDims.x + wheelWidth)*0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i*deltaZ*0.5f);
		wheelCentreOffsets[wheelCount + 1] = PxVec3((+chassisDims.x - wheelWidth)*0.5f, -(chassisDims.y / 2 + wheelRadius), wheelRearZ + i*deltaZ*0.5f);
	}
}

PxConvexMesh* Physics::createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking)
{
	const PxF32 x = dims.x*0.5f;
	const PxF32 y = dims.y*0.5f;
	const PxF32 z = dims.z*0.5f;
	PxVec3 verts[8] =
	{
		PxVec3(x, y, -z),
		PxVec3(x, y, z),
		PxVec3(x, -y, z),
		PxVec3(x, -y, -z),
		PxVec3(-x, y, -z),
		PxVec3(-x, y, z),
		PxVec3(-x, -y, z),
		PxVec3(-x, -y, -z)
	};

	return createConvexMesh(verts, 8, physics, cooking);
}

PxConvexMesh* Physics::createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking)
{
	PxVec3 points[2 * 16];
	for (PxU32 i = 0; i < 16; i++)
	{
		const PxF32 cosTheta = PxCos(i*PxPi*2.0f / 16.0f);
		const PxF32 sinTheta = PxSin(i*PxPi*2.0f / 16.0f);
		const PxF32 y = radius*cosTheta;
		const PxF32 z = radius*sinTheta;
		points[2 * i + 0] = PxVec3(-width / 2.0f, y, z);
		points[2 * i + 1] = PxVec3(+width / 2.0f, y, z);
	}

	return createConvexMesh(points, 32, physics, cooking);
}

PxConvexMesh* Physics::createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking)
{
	// Create descriptor for convex mesh
	PxConvexMeshDesc convexDesc;
	convexDesc.points.count = numVerts;
	convexDesc.points.stride = sizeof(PxVec3);
	convexDesc.points.data = verts;
	convexDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX | PxConvexFlag::eINFLATE_CONVEX;

	PxConvexMesh* convexMesh = NULL;
	PxDefaultMemoryOutputStream buf;
	if (cooking.cookConvexMesh(convexDesc, buf))
	{
		PxDefaultMemoryInputData id(buf.getData(), buf.getSize());
		convexMesh = physics.createConvexMesh(id);
	}

	return convexMesh;
}

PxTriangleMesh* Physics::createTriangleMesh(const PxVec3* verts, const PxU32 numVerts, const PxU32* indices, const PxU32 triCount, PxPhysics& physics, PxCooking& cooking)
{
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = numVerts;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = verts;

	meshDesc.triangles.count = triCount;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices;

	PxTriangleMesh* triangleMesh = NULL;
	PxDefaultMemoryOutputStream buffer;
	if (cooking.cookTriangleMesh(meshDesc, buffer))
	{
		PxDefaultMemoryInputData id(buffer.getData(), buffer.getSize());
		triangleMesh = mPhysics->createTriangleMesh(id);
	}

	return triangleMesh;
}

PxRigidDynamic* Physics::createVehicleActor(const PxVehicleChassisData& chassisData,
	PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels,
	PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes, PxPhysics& physics, PxVec3 initPos)
{
	//We need a rigid body actor for the vehicle.
	//Don't forget to add the actor to the scene after setting up the associated vehicle.
	PxRigidDynamic* vehActor = physics.createRigidDynamic(PxTransform(initPos));
	vehActor->setRigidBodyFlag(PxRigidBodyFlag::eENABLE_CCD, true);

	//Wheel and chassis simulation filter data.
	PxFilterData wheelSimFilterData;
	wheelSimFilterData.word0 = COLLISION_FLAG_WHEEL;
	wheelSimFilterData.word1 = COLLISION_FLAG_WHEEL_AGAINST;
	PxFilterData chassisSimFilterData;
	chassisSimFilterData.word0 = COLLISION_FLAG_CHASSIS;
	chassisSimFilterData.word1 = COLLISION_FLAG_CHASSIS_AGAINST;

	//Wheel and chassis query filter data.
	//Optional: cars don't drive on other cars.
	PxFilterData wheelQryFilterData;
	setupNonDrivableSurface(wheelQryFilterData);
	PxFilterData chassisQryFilterData;
	setupNonDrivableSurface(chassisQryFilterData);

	//Add all the wheel shapes to the actor.
	for (PxU32 i = 0; i < numWheels; i++)
	{
		PxConvexMeshGeometry geom(wheelConvexMeshes[i]);
		PxShape* wheelShape = vehActor->createShape(geom, *wheelMaterials[i]);
		wheelShape->setQueryFilterData(wheelQryFilterData);
		wheelShape->setSimulationFilterData(wheelSimFilterData);
		wheelShape->setLocalPose(PxTransform(PxIdentity));
	}

	//Add the chassis shapes to the actor.
	for (PxU32 i = 0; i < numChassisMeshes; i++)
	{
		PxShape* chassisShape = vehActor->createShape
			(PxConvexMeshGeometry(chassisConvexMeshes[i]), *chassisMaterials[i]);
		chassisShape->setQueryFilterData(chassisQryFilterData);
		chassisShape->setSimulationFilterData(chassisSimFilterData);
		chassisShape->setLocalPose(PxTransform(PxIdentity));
	}

	vehActor->setMass(chassisData.mMass);
	vehActor->setMassSpaceInertiaTensor(chassisData.mMOI);
	vehActor->setCMassLocalPose(PxTransform(chassisData.mCMOffset, PxQuat(PxIdentity)));

	return vehActor;
}


void Physics::setupNonDrivableSurface(PxFilterData& filterData)
{
	filterData.word3 = UNDRIVABLE_SURFACE;
}

void Physics::shutdown() {
	mPhysics->release();
	mFoundation->release();
}

void Physics::startSim(float frameTime) {

	vector<PxVehicleWheels*> vehicles;
	for (unsigned int i = 0; i < vehicleActors.size(); i++)
		vehicles.push_back(vehicleActors[i]);

	PxRaycastQueryResult* raycastResults = gVehicleSceneQueryData->getRaycastQueryResultBuffer(0);
	const PxU32 raycastResultsSize = gVehicleSceneQueryData->getRaycastQueryResultBufferSize();
	PxVehicleSuspensionRaycasts(gBatchQuery, vehicles.size(), &vehicles[0], raycastResultsSize, raycastResults);

	const PxVec3 grav = gScene->getGravity();
	vector<PxWheelQueryResult> wheelQueryResults;
	wheelQueryResults.resize(vehicles.size()*PX_MAX_NB_WHEELS);

	vector<PxVehicleWheelQueryResult> vehicleQueryResults;
	for (unsigned int i = 0; i < vehicles.size(); i++)
		vehicleQueryResults.push_back({ &wheelQueryResults[i*PX_MAX_NB_WHEELS], vehicles[i]->mWheelsSimData.getNbWheels() });

	PxVehicleUpdates(frameTime, grav, *gFrictionPairs, vehicles.size(), &vehicles[0], &vehicleQueryResults[0]); 

	lastFrameTime = frameTime;
	gScene->simulate(frameTime);
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

PxVehicleDrivableSurfaceToTireFrictionPairs* Physics::createFrictionPairs(const PxMaterial* defaultMaterial)
{
	PxVehicleDrivableSurfaceType surfaceTypes[1];
	surfaceTypes[0].mType = SURFACE_TYPE_TARMAC;

	const PxMaterial* surfaceMaterials[1];
	surfaceMaterials[0] = defaultMaterial;

	PxVehicleDrivableSurfaceToTireFrictionPairs* surfaceTirePairs =
		PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES);

	surfaceTirePairs->setup(MAX_NUM_TIRE_TYPES, MAX_NUM_SURFACE_TYPES, surfaceMaterials, surfaceTypes);

	for (PxU32 i = 0; i < MAX_NUM_SURFACE_TYPES; i++)
	{
		for (PxU32 j = 0; j < MAX_NUM_TIRE_TYPES; j++)
		{
			surfaceTirePairs->setTypePairFriction(i, j, gTireFrictionMultipliers[i][j]);
		}
	}
	return surfaceTirePairs;
}

void Physics::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	std::cout  << "Callback has been called \n";
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];


		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			PxRigidActor* actor1 = pairHeader.actors[0];
			int index1 = 0;
			PxRigidActor* actor2 = pairHeader.actors[1];
			int index2 = 0;

			bool isVehicle1 = false;
			bool isVehicle2 = false;

			for (int i = 0; i < vehicleActors.size(); i++) {
				if (actor1 == vehicleActors[i]->getRigidDynamicActor()) {
					isVehicle1 = true;
					index1 = i;
				}
				if (actor2 == vehicleActors[i]->getRigidDynamicActor()) {
					isVehicle2 = true;
					index2 = i;
				}
			}

			PxContactPairPoint* contactPoint = NULL;
			int numContacts = cp.contactCount;
			if (numContacts > 0) {
				cp.extractContacts(contactPoint, 1);
				float posX = contactPoint->position.x;
				float posY = contactPoint->position.y;
				float posZ = contactPoint->position.z;
				vec3 pos = vec3(posX, posY, posZ);
				vec3 normal = vec3(contactPoint->normal.x, contactPoint->normal.y, contactPoint->normal.z);
				vec3 force = vec3(contactPoint->impulse.x / lastFrameTime, contactPoint->impulse.y / lastFrameTime, contactPoint->impulse.z / lastFrameTime);

				if (isVehicle1 && isVehicle2) {
					lastState->pushEvent(new VehicleCollisionEvent(index1, index2, pos, normal, force));
				}
				else {
					// TODO add collision against wall
				}
			}

			if ((pairHeader.actors[0] == goldenBuggy->getRigidDynamicActor()) || (pairHeader.actors[1] == goldenBuggy->getRigidDynamicActor()))
			{
				int pairIndexOfGoldenBuggy;
				if (pairHeader.actors[0] == goldenBuggy->getRigidDynamicActor())
					pairIndexOfGoldenBuggy = 0;
				else
					pairIndexOfGoldenBuggy = 1;

				for (unsigned int i = 0; i < vehicleActors.size(); i++)
				{
					if (pairIndexOfGoldenBuggy == 0){
						if (pairHeader.actors[1] == vehicleActors[i]->getRigidDynamicActor()) 
						{
							cout << "A Golden buggie switch has happened and vehicle " << i << " is the golden buggie \n";
							indexOfOldGoldenBuggy = indexOfGoldenBuggy;
							setSpeed(indexOfOldGoldenBuggy, initVehicleSpeed);
							indexOfGoldenBuggy = i;
							goldenBuggy = vehicleActors[i];
							newGoldenBuggy = true;
							modifySpeed(i, 3); 
							break;
						}
					}
					else {
						if (pairHeader.actors[0] == vehicleActors[i]->getRigidDynamicActor())
						{
							cout << "A Golden buggie switch has happened and vehicle " << i << " is the golden buggie \n";
							indexOfOldGoldenBuggy = indexOfGoldenBuggy;
							setSpeed(indexOfOldGoldenBuggy, initVehicleSpeed);
							indexOfGoldenBuggy = i;
							goldenBuggy = vehicleActors[i];
							newGoldenBuggy = true;
							modifySpeed(i, 3);
							break;
						}

					}
						
						

				}
					

				cout << "A collision with the goldenBuggy has been detected \n";
				break;
			}
		}
	}
}


#endif 
