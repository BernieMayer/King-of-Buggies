#ifndef VEHICLETRAITS_H
#define VEHICLETRAITS_H

#include "PxPhysicsAPI.h"

#include <fstream>
#include <string>
#include <iostream>

#pragma warning(disable:4996)

using namespace physx;

class VehicleTraits {
private:
	
public:
	VehicleTraits(PxMaterial* material);
	VehicleTraits(PxF32 cMass, PxVec3 cDims,
		PxVec3 cMOI, PxVec3 cCMOffset, PxMaterial* cMaterial,
		PxF32 wMass, PxF32 wWidth, PxF32 wRadius, PxF32 wMOI,
		PxMaterial* wMaterial, PxU32 nWheels);
	PxF32 chassisMass;
	PxVec3 chassisDims;
	PxVec3 chassisMOI;
	PxVec3 chassisCMOffset;
	PxMaterial* chassisMaterial;
	PxF32 wheelMass;
	PxF32 wheelWidth;
	PxF32 wheelRadius;
	PxF32 wheelMOI;
	PxMaterial* wheelMaterial;
	PxU32 numWheels;
	int steerAngle;

	bool loadConfiguration(std::string configFile);
	void print();
};

#endif // VEHICLETRAITS_H