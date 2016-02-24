#ifndef VEHICLETRAITS_CPP
#define VEHICLETRAITS_CPP
#include "VehicleTraits.h"

const std::string configDir = "./conf/";
const std::string extension = ".conf";

// Gives default values from snippet
VehicleTraits::VehicleTraits(PxMaterial* material) {
	chassisMass = 1500.0f;
	// X used to be 1.25
	chassisDims = PxVec3(1.75, 1.0f, 2.0f);

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
	steerAngle = 45;
}

VehicleTraits::VehicleTraits(PxF32 cMass, PxVec3 cDims,
	PxVec3 cMOI, PxVec3 cCMOffset, PxMaterial* cMaterial,
	PxF32 wMass, PxF32 wWidth, PxF32 wRadius, PxF32 wMOI,
	PxMaterial* wMaterial, PxU32 nWheels) {
	
	chassisMass = cMass;
	chassisDims = cDims;
	chassisMOI = cMOI;
	chassisCMOffset = cCMOffset;
	chassisMaterial = cMaterial;
	wheelMass = wMass;
	wheelWidth = wWidth;
	wheelRadius = wRadius;
	wheelMOI = wMOI;
	wheelMaterial = wMaterial;
	numWheels = nWheels;
}


bool VehicleTraits::loadConfiguration(std::string configFile)
{
	printf("Opening %s...\n", (configDir + configFile + extension).c_str());
	std::ifstream f((configDir + configFile + extension).c_str());
	if (!f.is_open())
	{
		printf("Configuration file could not be opened\n");
		return false;
	}

	std::string line;
	float tempf;
	PxVec3 tempv;

	printf("CHASSIS:\n");

	while (std::getline(f, line))
	{
		if (sscanf(line.c_str(), "Mass = %f", &tempf) >= 1){
			chassisMass = tempf;
			printf("\tMass = %.2f\n", tempf);
		}
		else if (sscanf(line.c_str(), "Dimensions = %f %f %f", &tempv.x, &tempv.y, &tempv.z) >= 3){
			chassisDims = tempv;
			printf("\tDimensions = %.2f, %.2f, %.2f\n", tempv.x, tempv.y, tempv.z);
		}
		else if (sscanf(line.c_str(), "MOI = %f %f %f", &tempv.x, &tempv.y, &tempv.z) >= 3){
			chassisMOI = tempv;
			printf("\tMOI = %.2f %.2f %.2f\n", tempv.x, tempv.y, tempv.z);
		}
		else if (sscanf(line.c_str(), "Center of mass = %f %f %f", &tempv.x, &tempv.y, &tempv.z) >= 3){
			chassisCMOffset = tempv;
			printf("\tCenter of mass = %.2f %.2f %.2f\n", tempv.x, tempv.y, tempv.z);
		}
		else if (sscanf(line.c_str(), "Dynamic friction = %f", &tempf) >= 1){
			chassisMaterial->setDynamicFriction(tempf);
			printf("\tDynamic friction = %.2f\n", tempf);
		}
		else if (sscanf(line.c_str(), "Static friction = %f", &tempf) >= 1){
			chassisMaterial->setStaticFriction(tempf);
			printf("\tStatic friction = %.2f\n", tempf);
		}
		else if (sscanf(line.c_str(), "Restitution = %f", &tempf) >= 1){
			chassisMaterial->setRestitution(tempf);
			printf("\tRestitution = %.2f\n", tempf);
		}
		//Break condition
		else if (line.compare("WHEEL:") == 0)
			break;
	}

	printf("WHEEL:\n");

	while (std::getline(f, line))
	{
		if (sscanf(line.c_str(), "Mass = %f", &tempf) >= 1){
			wheelMass = tempf;
			printf("\tMass = %.2f\n", tempf);
		}
		else if (sscanf(line.c_str(), "Width = %f", &tempf) >= 1){
			wheelWidth = tempf;
			printf("\tWidth = %.2f\n", tempf);
		}
		else if (sscanf(line.c_str(), "Radius = %f", &tempf) >= 1){
			wheelRadius = tempf;
			printf("\tRadius = %.2f\n", tempf);
		}
		else if (sscanf(line.c_str(), "MOI = %f", &tempf) >= 1){
			wheelMOI = tempf;
			printf("\tMOI = %.2f\n", tempf);
		}
		else if (sscanf(line.c_str(), "Dynamic friction = %f", &tempf) >= 1){
			wheelMaterial->setDynamicFriction(tempf);
			printf("\tDynamic friction = %.2f\n", tempf);
		}
		else if (sscanf(line.c_str(), "Static friction = %f", &tempf) >= 1){
			wheelMaterial->setStaticFriction(tempf);
			printf("\tStatic friction = %.2f\n", tempf);
		}
		else if (sscanf(line.c_str(), "Restitution = %f", &tempf) >= 1){
			wheelMaterial->setRestitution(tempf);
			printf("\tRestitution = %2f\n", tempf);
		}
	}

	return true;

}

#endif