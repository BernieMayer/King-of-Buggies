#include "Physics.h"

#include <iostream>
#include <stdio.h>  
#include <stdlib.h> 


#define MAX_NUM_ACTOR_SHAPES 128

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

PxVehicleDrive4W* vehicle;

PxVehicleDrive4WRawInputData* inputs[4];

//VehicleSceneQueryData* gVehicleSceneQueryData = NULL;

//TEMPORARY FUNCTION
PxRigidActor* getSphere() { return aSphereActor; }
PxRigidDynamic* getCar() { return vehicle->getRigidDynamicActor(); }
void getWheels(physx::PxShape** wheels)
{
	getCar()->getShapes(wheels, 4, 0);
}


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

	initDefaultScene();

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
		
		if (input.turnL > 1) {
			pxInput->setDigitalSteerLeft(true);
		}
		else {
			pxInput->setDigitalSteerLeft(false);
		}
		
		if (input.turnR > 1) {
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

void Physics::handleInput(Input* input){
	vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_ACCEL, input->forward);

	//The code below is used to handle the braking, leftSteer, rightSteer

	//vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANALOG_INPUT_BRAKE, input->brake);
	//vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANOLOG_INPUT_STEER_LEFT, input->leftSteer);
	//vehicle->mDriveDynData.setAnalogInput(PxVehicleDrive4WControl::eANOLOG_INPUT_STEER_RIGHT, input->rightSteer);
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
	mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
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
	aSphereActor->setLinearVelocity(PxVec3(0.2f, 0.f, 0.f));

	gScene->addActor(*aSphereActor);

	vehicle = initVehicle();
}

PxVehicleDrive4W* Physics::initVehicle() {
	PxInitVehicleSDK(*mPhysics);
	// Up and forwards
	PxVehicleSetBasisVectors(PxVec3(0, 1, 0), PxVec3(0, 0, 1));
	PxVehicleSetUpdateMode(PxVehicleUpdateMode::eVELOCITY_CHANGE);

	// Create the batched scene queries for the supension raycasts
	//gVehicleSceneQueryData

	const PxVec3 chassisDims(1.25f, 1.0f, 2.0f);
	const PxF32 wheelWidth = 0.4f;
	const PxF32 wheelRadius = 0.5f;
	const PxU32 nbWheels = 4;

	//Set up the wheel mass, radius, width, moment of inertia, and number of wheels.
	//Moment of inertia is just the moment of inertia of a cylinder.
	const PxF32 wheelMass = 20.0f;
	const PxF32 wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;

	//Set up the chassis mass, dimensions, moment of inertia, and center of mass offset.
	//The moment of inertia is just the moment of inertia of a cuboid but modified for easier steering.
	//Center of mass offset is 0.65m above the base of the chassis and 0.25m towards the front.
	const PxF32 chassisMass = 1500.0f;
	const PxVec3 chassisMOI
		((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);
	const PxVec3 chassisCMOffset(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);

	PxRigidDynamic* veh4WActor = initVehicleActor(wheelWidth, wheelRadius, nbWheels, chassisDims, chassisMOI, chassisMass, chassisCMOffset);

	//Set up the sim data for the wheels.
	PxVehicleWheelsSimData* wheelsSimData = initWheelSimData(nbWheels, chassisDims, wheelWidth, wheelRadius, wheelMass,
		wheelMOI, chassisCMOffset, chassisMass);

	//Set up the sim data for the vehicle drive model.
	PxVehicleDriveSimData4W driveSimData = initDriveSimData(wheelsSimData);

	//Create a vehicle from the wheels and drive sim data.
	PxVehicleDrive4W* vehDrive4W = PxVehicleDrive4W::allocate(nbWheels);
	vehDrive4W->setup(mPhysics, veh4WActor, *wheelsSimData, driveSimData, nbWheels - 4);

	//Free the sim data because we don't need that any more.
	wheelsSimData->free();

	gScene->addActor(*veh4WActor);

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
	engine.mMaxOmega = 600.0f;//approx 6000 rpm
	driveSimData.setEngineData(engine);


	// TODO: Don't want gears. Will
	// Changing switch time to 0 be good enough for that?
	//Gears
	PxVehicleGearsData gears;
	gears.mSwitchTime = 0.5f;
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

PxRigidDynamic* Physics::initVehicleActor(const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 nbWheels, const PxVec3 chassisDims,
	const PxVec3 chassisMOI, const PxF32 chassisMass, const PxVec3 chassisCMOffset) {

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
		*mPhysics);

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
		wheels[PxVehicleDrive4WWheelOrder::eFRONT_LEFT].mMaxSteer = PxPi*0.3333f;
		wheels[PxVehicleDrive4WWheelOrder::eFRONT_RIGHT].mMaxSteer = PxPi*0.3333f;
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

PxRigidDynamic* Physics::createVehicleActor
(const PxVehicleChassisData& chassisData,
PxMaterial** wheelMaterials, PxConvexMesh** wheelConvexMeshes, const PxU32 numWheels,
PxMaterial** chassisMaterials, PxConvexMesh** chassisConvexMeshes, const PxU32 numChassisMeshes,
PxPhysics& physics)
{
	//We need a rigid body actor for the vehicle.
	//Don't forget to add the actor to the scene after setting up the associated vehicle.
	PxRigidDynamic* vehActor = physics.createRigidDynamic(PxTransform(PxVec3(0.f, 1.5f, 0.f)));

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

void Physics::startSim(const GameState& state) {
	// Simulate at 60 fps... probably what it means
	/*float timeElapsed = clock.getElapsedTime();

	gScene->simulate(max(timeElapsed, 0.001f));*/
	float frameTime = 1 / 60.f;
	clock.waitUntil(frameTime);
	gScene->simulate(frameTime);
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
