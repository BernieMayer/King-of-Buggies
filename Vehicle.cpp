#ifndef VEHICLE_CPP
#define VEHICLE_CPP
#include "Vehicle.h"
#include "SnippetVehicleFilterShader.h"


struct FilterGroup
{
	enum Enum
	{
		eVEHICLE = (1 << 0),
		eWHEEL = (1 << 1),
		eSPHERE = (1 << 2),
		eGROUND = (1 << 3)
	};
};



Vehicle::Vehicle(PxPhysics& phys, PxCooking& cook, PxScene& aScene, VehicleTraits traits, vec3 initPos){
	mPhysics =  &phys;
	mMaterial = mPhysics->createMaterial(0.8f, 0.8f, 0.6f);
	mCooking = &cook;
	gScene = &aScene;
	gear = 0;
	vehicle_create(traits, initPos);
}

Vehicle::Vehicle(PxVehicleDrive4W& veh, VehicleTraits Vehtraits)
{
	vehDrive4W = &veh;
	traits = Vehtraits;
}

void updateGameState(GameState* state)
{

}

void Vehicle::modifySpeed(float modSpeed)
{

	PxVehicleDrive4W* veh = vehDrive4W;
	PxVehicleEngineData engine = veh->mDriveSimData.getEngineData();
	engine.mPeakTorque = engine.mPeakTorque + modSpeed * engine.mPeakTorque;
	veh->mDriveSimData.setEngineData(engine);
}

void Vehicle::setSpeed( float speed)
{

	PxVehicleEngineData engine = vehDrive4W->mDriveSimData.getEngineData();
	engine.mPeakTorque = speed;
	vehDrive4W->mDriveSimData.setEngineData(engine);
}

void Vehicle::applyNitroBoost()
{
	
	PxRigidDynamic* vehBody = vehDrive4W->getRigidDynamicActor();

	vec3 forceVec = lastState->getPlayer(vehicleNum)->getForward();
	forceVec = 39050.f * forceVec * vec3(1, 0, 1);
	vehBody->addForce(getPxVec3(forceVec));
}

PxVehicleDrive4W* Vehicle::getVehDrive4W()
{
	return vehDrive4W;
}

void Vehicle::setVehDrive4W(PxVehicleDrive4W* veh)
{
	vehDrive4W = veh;
}

unsigned int Vehicle::vehicle_create(VehicleTraits traits, vec3 initPos)
{
	vehicleNum = numOfVehicles;
	numOfVehicles++;
	vehDrive4W =initVehicle(traits, PxVec3(initPos.x, initPos.y, initPos.z));

	gear = -1;
	inAir = false;
	return numOfVehicles - 1;
}

PxVehicleDrive4W* Vehicle::initVehicle(VehicleTraits traits, PxVec3 initPos) {

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

PxRigidDynamic* Vehicle::createVehicleActor(const PxVehicleChassisData& chassisData,
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
	wheelQryFilterData.word0 = FilterGroup::eWHEEL;
	setupNonDrivableSurface(wheelQryFilterData);
	PxFilterData chassisQryFilterData;
	chassisQryFilterData.word0 = FilterGroup::eVEHICLE;

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

PxVehicleDriveSimData4W Vehicle::initDriveSimData(PxVehicleWheelsSimData* wheelsSimData) {
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

PxVehicleWheelsSimData* Vehicle::initWheelSimData(int nbWheels, const PxVec3 chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius, const PxF32 wheelMass,
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

void Vehicle::computeWheelCenterActorOffsets4W(const PxF32 wheelFrontZ, const PxF32 wheelRearZ, const PxVec3& chassisDims, const PxF32 wheelWidth, const PxF32 wheelRadius,
	const PxU32 numWheels, PxVec3* wheelCentreOffsets)
{

}



void Vehicle::setupWheelsSimulationData
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

PxRigidDynamic* Vehicle::initVehicleActor(const PxF32 wheelWidth, const PxF32 wheelRadius, const PxU32 nbWheels, const PxVec3 chassisDims,
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

PxConvexMesh* Vehicle::createWheelMesh(const PxF32 width, const PxF32 radius, PxPhysics& physics, PxCooking& cooking)
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

PxConvexMesh* Vehicle::createChassisMesh(const PxVec3 dims, PxPhysics& physics, PxCooking& cooking)
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

PxConvexMesh* Vehicle::createConvexMesh(const PxVec3* verts, const PxU32 numVerts, PxPhysics& physics, PxCooking& cooking)
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

void Vehicle::setupNonDrivableSurface(PxFilterData& filterData)
{
	filterData.word3 = UNDRIVABLE_SURFACE;
}

PxVec3 Vehicle::getPxVec3(const vec3& vec)
{
	return PxVec3(vec.x, vec.y, vec.z);
}

#endif 
