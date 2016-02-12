#ifndef VEHICLETRAITS_H
#define VEHICLETRAITS_H

#include "PxPhysicsAPI.h"

using namespace physx;

class VehicleTraits {
private:
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
};

#endif // VEHICLETRAITS_H