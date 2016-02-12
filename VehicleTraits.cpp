#ifndef PHYSICS_CPP
#define PHYSICS_CPP
#include "Physics.h"

// Gives default values from snippet
VehicleTraits::VehicleTraits(PxMaterial* material) {
	chassisMass = 1500.0f;
	chassisDims = PxVec3(1.25f, 1.0f, 2.0f);

	chassisMOI = PxVec3((chassisDims.y*chassisDims.y + chassisDims.z*chassisDims.z)*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.z*chassisDims.z)*0.8f*chassisMass / 12.0f,
		(chassisDims.x*chassisDims.x + chassisDims.y*chassisDims.y)*chassisMass / 12.0f);

	chassisCMOffset = PxVec3(0.0f, -chassisDims.y*0.5f + 0.65f, 0.25f);
	
	chassisMaterial = material;
	wheelMass = 20.0f;
	wheelWidth = 0.4f;
	wheelRadius = 0.5f;
	wheelMOI = 0.5f*wheelMass*wheelRadius*wheelRadius;
	wheelMaterial = material;
	numWheels = 4;
}

#endif