#include "MeshInfo.h"

MeshInfo::MeshInfo() {
	meshLoader = MeshInfoLoader();

	meshLoader.loadModel("models\\brick.obj");
	//unsigned int texID = meshLoader.LoadTexture("textures\\bricktexture.bmp");
	MeshObject brickObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);
	
	meshes.push_back(brickObject);

	meshLoader.clearData();
	
	meshLoader.loadModel("models\\cube.obj");
	MeshObject cubeObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);
	meshes.push_back(cubeObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\wheel.obj");
	MeshObject wheelObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);
	meshes.push_back(wheelObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\coin.obj");
	MeshObject coinObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);
	meshes.push_back(coinObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\mine.obj");
	unsigned int texID = LoadTexture("textures\\minetexture.png");
	MeshObject mineObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices, texID);
	meshes.push_back(mineObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\bomb.obj");
	texID = LoadTexture("textures\\bombtexture.png");
	MeshObject bombObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices, texID);
	meshes.push_back(bombObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\donutlevel.obj");
	texID = LoadTexture("textures\\donutleveltexture.png");
	MeshObject levelObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices, texID);
	meshes.push_back(levelObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\oldlevel.obj");
	MeshObject oldObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);
	meshes.push_back(oldObject);

	meshLoader.clearData();

	// Buggy textures are loaded separately, since there are multiple textures for the same mesh
	meshLoader.loadModel("models\\buggy.obj");
	MeshObject buggyObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);
	meshes.push_back(buggyObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\unitbuggy.obj");
	MeshObject unitBuggyObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);
	meshes.push_back(unitBuggyObject);

	meshLoader.clearData();

	// load buggy textures
	buggyTexIDs.push_back(LoadTexture("textures\\redbuggytexture.png"));
	buggyTexIDs.push_back(LoadTexture("textures\\greenbuggytexture.png"));
	buggyTexIDs.push_back(LoadTexture("textures\\bluebuggytexture.png"));
	buggyTexIDs.push_back(LoadTexture("textures\\yellowbuggytexture.png"));
	buggyTexIDs.push_back(LoadTexture("textures\\purplebuggytexture.png"));

	goldenBuggyTexID = LoadTexture("textures\\goldenbuggytexture.png");
}

MeshObject MeshInfo::getMesh(int objectType) {
	return meshes[objectType];
}

MeshObject* MeshInfo::getMeshPointer(int objectType){
	return &meshes[objectType];
}

vector<vec3> MeshInfo::getMeshVertices(int objectType) {
	return meshes[objectType].getVertices();
}

vector<vec3> MeshInfo::getMeshNormals(int objectType) {
	return meshes[objectType].getNormals();
}

vector<vec2> MeshInfo::getMeshUvs(int objectType) {
	return meshes[objectType].getUvs();
}

vector<unsigned int> MeshInfo::getMeshIndices(int objectType) {
	return meshes[objectType].getIndices();
}

unsigned int MeshInfo::getBuggyTexID(int index) {
	return buggyTexIDs[index];
}

unsigned int MeshInfo::getGoldenBuggyTexID() {
	return goldenBuggyTexID;
}