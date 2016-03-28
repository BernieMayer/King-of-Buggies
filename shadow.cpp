#ifndef SHADOW_CPP
#define SHADOW_CPP

#include "shadow.h"

Shadow::Shadow()
{
	shaderIndex = ShaderList::SHADOW;
	programID = shaderList.shaderIDs[ShaderList::DIFFUSE];
}

void Shadow::loadUniforms(const mat4& transform, const mat4& objectTransform, vec3 viewer, vec3 light, vec3 color)
{

}

void Shadow::loadUniforms(const mat4& transform, const mat4& objectTransform)
{

}









#endif //SHADOW_CPP