#ifndef CAMERA_CPP
#define CAMERA_CPP

#include "Camera.h"

//Rotates around the x axis
mat4 rotX(float theta)
{
	mat4 newM (	1, 0, 0, 0,
				0, cos(theta), sin(theta), 0,
				0, -sin(theta), cos(theta), 0,
				0, 0, 0, 1);

	return newM;
}

//Rotates around the y axis 
mat4 rotY(float theta)
{
	mat4 newM (	cos(theta), 0, -sin(theta), 0,
				0, 1, 0, 0,
				sin(theta), 0, cos(theta), 0,
				0, 0, 0, 1);

	return newM;
}

//Rotates around the z axis
mat4 rotZ(float theta)
{
	mat4 newM (	cos(theta), sin(theta), 0, 0,
				-sin(theta), cos(theta), 0, 0,
				0, 0, 1, 0,
				0, 0, 0, 1);

	return newM;
}



Camera::Camera():
	dir(vec3 (0.f, 0.f, -1.f)),
	up(vec3 (0.f, 1.f, 0.f)),
	right(vec3 (1.f, 0.f, 0.f)),
	pos(vec3 (0.f, 0.f, 0.f)),
	viewCenter(vec3(0.f, 0.f, 0.f)),
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

	/*if (abs(_dir.y) > maxY)
	{
		if (dir.y < 0)
			dir.y = -maxY;
		else
			dir.y = maxY;
		
		float s = sqrt((1 - maxY*maxY) / (dir.x*dir.x + dir.z*dir.z));
		dir.x *= s;
		dir.z *= s;
		dir = normalize(dir);
	}*/

	up = vec3(0.f, 1.f, 0.f);

	right = normalize(cross(dir, up));
	up = normalize(cross(right, dir));
}

mat4 Camera::getMatrix()
{

	mat4 lookat (	right.x, up.x, -dir.x, 0,
					right.y, up.y, -dir.y, 0,
					right.z, up.z, -dir.z, 0,
					0, 0, 0, 1);
	/*mat4 lookat(right.x, right.y, right.z, 0,
				up.x, up.y, up.z, 0,
				dir.x, dir.y, dir.z, 0,
				0, 0, 0, 1);*/

	mat4 translation(	1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						-pos.x, -pos.y, -pos.z, 1);

	//mat4 lookat{right.x, }
	

	return lookat*translation;
}

void Camera::rotateView(float x, float y)
{	
	float thetaX = -y*right.x + x*up.x;
	float thetaY = -y*right.y + x*up.y;
	float thetaZ = -y*right.z + x*up.z;

	if(cameraMode == FREEROAM_CAMERA)
	{
		vec3 newDir = vec3(rotZ(thetaZ)*rotX(thetaX)*rotY(thetaY) * vec4(dir, 1));
		changeDir(newDir);
	}
	else if(cameraMode == MODELVIEWER_CAMERA)
	{
		pos =  vec3(rotZ(thetaZ)*rotX(thetaX)*rotY(thetaY) * vec4(pos - viewCenter, 1)) + viewCenter;	//Rotate position around center

		float pos_length = length(pos-viewCenter);
		vec3 _dir = normalize(pos-viewCenter);

		if (abs(_dir.y) > maxY)
		{
			if (_dir.y < 0)
				_dir.y = -maxY;
			else
				_dir.y = maxY;

			float s = sqrt((1 - maxY*maxY) / (_dir.x*_dir.x + _dir.z*_dir.z));
			_dir.x *= s;
			_dir.z *= s;
			_dir = normalize(_dir);

			pos = viewCenter+_dir*pos_length;
		}

		changeDir(normalize(viewCenter-pos));
	}

}

float Camera::trackingSpeedFunc(float angleDiff)
{
	float clampedAngle = max(min(angleDiff, PI), 0.f);
	float k = 0.1f;

	return (clampedAngle / PI)*(1.f - k) + k;
}

void Camera::trackDirAroundY(vec3 _dir, float timeStep)
{
	vec3 flatDir = normalize(vec3(dir.x, 0.f, dir.z));
	vec3 flat_Dir = normalize(vec3(_dir.x, 0.f, _dir.z));


	float angleDiff = acos(dot(flatDir, flat_Dir));

	if (angleDiff < 0.01f)
		return;

	float sign = 1.f;

	if (dot(right, _dir) >= 0)
		sign = -1.f;

	float adjustedSpeed = trackingSpeed*timeStep*trackingSpeedFunc(angleDiff);

	pos = vec3(rotY(sign*min(angleDiff, adjustedSpeed))*vec4(pos - viewCenter, 1.f))+viewCenter;
	changeDir(viewCenter - pos);

}

void Camera::rotateViewAround(float x, float y)
{
	float thetaX = -y*right.x + x*up.x;
	float thetaY = -y*right.y + x*up.y;
	float thetaZ = -y*right.z + x*up.z;

	pos =  vec3(rotZ(thetaZ)*rotX(thetaX)*rotY(thetaY)* vec4(pos - viewCenter, 1) )+ viewCenter;	//Rotate position around center

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

vec3 Camera::getUp(){ return up; }

void Camera::setCameraMode(int type)
{
	cameraMode = type;
}

vec3 Camera::getViewCenter(){ return viewCenter; }


void Camera::changeViewCenter(vec3 _viewCenter)
{
	vec3 diff = _viewCenter - viewCenter;
	viewCenter = _viewCenter;
	pos = pos + diff;

	printf("viewCenter - (%f, %f, %f)\n", viewCenter.x, viewCenter.y, viewCenter.z);
	changeDir(viewCenter - pos);
}

void Camera::changeCenterAndPos(vec3 movement)
{
	viewCenter += movement;
	pos += movement;

	//changeDir(viewCenter - pos);
}


#endif //CAMERA_CPP