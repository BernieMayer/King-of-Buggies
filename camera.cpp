#include "camera.h"

//Rotates around the x axis
mat4 rotX(float theta)
{
	/*mat4 newM;
	newM.identity();

	newM.m[1][1] = cos(theta);
	newM.m[1][2] = -sin(theta);
	newM.m[2][1] = sin(theta);
	newM.m[2][2] = cos(theta);*/

	mat4 newM (	1, 0, 0, 0,
				0, cos(theta), sin(theta), 0,
				0, -sin(theta), cos(theta), 0,
				0, 0, 0, 1);

	return newM;
}

//Rotates around the y axis 
mat4 rotY(float theta)
{
	/*mat4 newM;
	newM.identity();

	newM.m[0][0] = cos(theta);
	newM.m[0][2] = sin(theta);
	newM.m[2][0] = -sin(theta);
	newM.m[2][2] = cos(theta);
	*/
	mat4 newM (	cos(theta), 0, -sin(theta), 0,
				0, 1, 0, 0,
				sin(theta), 0, cos(theta), 0,
				0, 0, 0, 1);

	return newM;
}

//Rotates around the z axis
mat4 rotZ(float theta)
{
	/*mat4 newM;
	newM.identity();
	
	newM.m[0][0] = cos(theta);
	newM.m[0][1] = -sin(theta);
	newM.m[1][0] = sin(theta);
	newM.m[1][1] = cos(theta);*/

	mat4 newM (	cos(theta), sin(theta), 0, 0,
				-sin(theta), cos(theta), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);

	return newM;
}



Camera::Camera():
	dir(vec3 (0.0, 0.0, -1.0)),
	up(vec3 (0.0, 1.0, 0.0)),
	right(vec3 (1.0, 0.0, 0.0)),
	pos(vec3 (0.0, 0.0, 0.0)),
	viewCenter(vec3(0.0, 0.0, 0.0)),
	cameraMode(FREEROAM_CAMERA) {}

Camera::Camera(vec3 _dir, vec3 _up, vec3 _pos):
	up(_up),
	pos(_pos),
	cameraMode(FREEROAM_CAMERA)
{
	changeDir(_dir);
}

Camera::Camera(vec3 _dir, vec3 _up, vec3 _pos, int _cameraMode):
	up(_up),
	pos(_pos),
	cameraMode(_cameraMode)
{
	changeDir(_dir);
}

void Camera::changeDir(vec3 _dir)
{
	dir = normalize(_dir);

	up = vec3(0.0, 1.0, 0.0);

	right = normalize(cross(dir, up));
	up = normalize(cross(right, dir));
}

mat4 Camera::getMatrix()
{
	/*mat4 lookat;

	lookat.m[0][0] = right.x;
	lookat.m[0][1] = right.y;
	lookat.m[0][2] = right.z;
 
	lookat.m[1][0] = up.x;
	lookat.m[1][1] = up.y;
	lookat.m[1][2] = up.z;

	lookat.m[2][0] = -dir.x;
	lookat.m[2][1] = -dir.y;
	lookat.m[2][2] = -dir.z;

	mat4 translation;
	translation.m[0][3] = -pos.x;
	translation.m[1][3] = -pos.y;
	translation.m[2][3] = -pos.z;*/

	mat4 lookat (	right.x, up.x, -dir.x, -pos.x,
					right.y, up.y, -dir.y, -pos.y,
					right.z, up.z, -dir.z, -pos.z,
					0, 0, 0, 1);
	

	return lookat*translation;
}

void Camera::rotateView(float x, float y)
{	
	float thetaX = -y*right.x + x*up.x;
	float thetaY = -y*right.y + x*up.y;
	float thetaZ = -y*right.z + x*up.z;

	if(cameraMode == FREEROAM_CAMERA)
	{
		vec3 newDir = MxV(rotZ(thetaZ)*rotX(thetaX)*rotY(thetaY), dir);
		changeDir(newDir);
	}
	else if(cameraMode == MODELVIEWER_CAMERA)
	{
		pos =  MxV(rotZ(thetaZ)*rotX(thetaX)*rotY(thetaY), pos - viewCenter) + viewCenter;	//Rotate position around center

		changeDir(viewCenter-pos);
	}

}

void Camera::rotateViewAround(float x, float y)
{
	float thetaX = -y*right.x + x*up.x;
	float thetaY = -y*right.y + x*up.y;
	float thetaZ = -y*right.z + x*up.z;

	pos =  rotZ(thetaZ)*rotX(thetaX)*rotY(thetaY)* pos - viewCenter + viewCenter;	//Rotate position around center

	changeDir(viewCenter-pos);
}

void Camera::move(vec3 change)
{		
	vec3 movement;
	movement = dir*change.z;
	movement = movement + right*change.x;
	movement = movement + up*change.y;
	
	pos = pos + movement;
	viewCenter = viewCenter + movement;
}

void Camera::moveCenter(vec3 change)
{
	vec3 movement;
	movement = dir*change.z;
	movement = movement + right*change.x;
	movement = movement + up*change.y;
	
	viewCenter = viewCenter + movement;
	pos = pos + movement;
}

void Camera::zoom(float scale)
{
	if(cameraMode == FREEROAM_CAMERA)
		return;
	vec3 toCenter = pos - viewCenter;
	
	if(length(toCenter)*scale < minZoom)
	{
		toCenter = normalize(toCenter)*minZoom;
	}
	else if(length(toCenter)*scale > maxZoom)
	{
		toCenter = normalize(toCenter)*maxZoom;
	}

	pos = viewCenter + toCenter*scale;
}

vec3 Camera::getPos(){ return pos; }

vec3 Camera::getDir(){ return dir; }


void Camera::changeViewCenter(vec3 _viewCenter)
{
	vec3 diff = _viewCenter - viewCenter;
	viewCenter = _viewCenter;
	pos = pos + diff;

	printf("viewCenter - (%f, %f, %f)\n", viewCenter.x, viewCenter.y, viewCenter.z);
	changeDir(viewCenter - pos);
}
