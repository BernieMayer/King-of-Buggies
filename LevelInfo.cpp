#ifndef LEVELINFO_CPP
#define LEVELINFO_CPP

#include "levelInfo.h"

LevelInfo::LevelInfo() :arena(NULL)
{

}

LevelInfo::LevelInfo(MeshObject* mesh) : arena(mesh)
{

}

MeshObject* LevelInfo::getMeshObject()
{
	return arena;
}

#endif // LEVELINFO_H