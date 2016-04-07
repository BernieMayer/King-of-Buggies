#include "MeshInfo.h"

MeshInfo::MeshInfo() {
	meshLoader = MeshInfoLoader();

	meshLoader.loadModel("models\\brick.obj");
	//unsigned int texID = meshLoader.LoadTexture("textures\\bricktexture.bmp");
	MeshObject brickObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);
	
	meshes.push_back(brickObject);

	meshLoader.clearData();
	
	meshLoader.loadModel("models\\cube.obj");
	unsigned int texID = LoadTexture("textures\\cubetexture.png");
	MeshObject cubeObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices, texID);
	meshes.push_back(cubeObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\wheel.obj");
	texID = LoadTexture("textures\\wheeltexture.png");
	MeshObject wheelObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices, texID);
	meshes.push_back(wheelObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\coin.obj");
	MeshObject coinObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);
	meshes.push_back(coinObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\mine.obj");
	texID = LoadTexture("textures\\minetexture.png");
	MeshObject mineObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices, texID);
	meshes.push_back(mineObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\bomb.obj");
	texID = LoadTexture("textures\\bombtexture.png");
	MeshObject bombObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices, texID);
	meshes.push_back(bombObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\boost.obj");
	//texID = LoadTexture("textures\\bombtexture.png");
	MeshObject boostObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);//, texID);
	meshes.push_back(boostObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\donutlevel.obj");
	texID = LoadTexture("textures\\donutleveltexture.png");
	MeshObject levelObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices, texID);
	meshes.push_back(levelObject);

	meshLoader.clearData();

	meshLoader.loadModel("models\\islandlevel.obj");
	//texID = LoadTexture("textures\\donutleveltexture.png");
	levelObject = MeshObject(meshLoader.vertices, meshLoader.normals, meshLoader.uvs, meshLoader.indices);//, texID);
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
	redBuggyTexID = LoadTexture("textures\\redbuggytexture.png");
	greenBuggyTexID = LoadTexture("textures\\greenbuggytexture.png");
	blueBuggyTexID = LoadTexture("textures\\bluebuggytexture.png");
	orangeBuggyTexID = LoadTexture("textures\\yellowbuggytexture.png");
	purpleBuggyTexID = LoadTexture("textures\\purplebuggytexture.png");

	buggyTexIDs.push_back(redBuggyTexID);
	buggyTexIDs.push_back(blueBuggyTexID);
	buggyTexIDs.push_back(greenBuggyTexID);
	buggyTexIDs.push_back(orangeBuggyTexID);
	buggyTexIDs.push_back(purpleBuggyTexID);

	goldenBuggyTexID = LoadTexture("textures\\goldenbuggytexture.png");

	// load UI textures for displaying powerup information

	UInitroID = LoadTexture("textures\\hud\\nitro.png");
	UIbombID = LoadTexture("textures\\hud\\bomb.png");
	UImineID = LoadTexture("textures\\hud\\mine.png");

	UIcomponentIDs.push_back(UInitroID);
	UIcomponentIDs.push_back(UIbombID);
	UIcomponentIDs.push_back(UImineID);

	coinComponentIDs.push_back(LoadTexture("textures\\hud\\0.png"));
	coinComponentIDs.push_back(LoadTexture("textures\\hud\\1.png"));
	coinComponentIDs.push_back(LoadTexture("textures\\hud\\2.png"));
	coinComponentIDs.push_back(LoadTexture("textures\\hud\\3.png"));
	coinComponentIDs.push_back(LoadTexture("textures\\hud\\4.png"));
	coinComponentIDs.push_back(LoadTexture("textures\\hud\\5.png"));
	coinComponentIDs.push_back(LoadTexture("textures\\hud\\6.png"));
	coinComponentIDs.push_back(LoadTexture("textures\\hud\\7.png"));
	coinComponentIDs.push_back(LoadTexture("textures\\hud\\8.png"));
	coinComponentIDs.push_back(LoadTexture("textures\\hud\\9.png"));

	// Load UI for displaying current buggy / score information
	buggyIndicator = LoadTexture("textures\\hud\\buggyindicator.png");

	redGoldenBuggy = LoadTexture("textures\\hud\\red.png");
	greenGoldenBuggy = LoadTexture("textures\\hud\\green.png");
	blueGoldenBuggy = LoadTexture("textures\\hud\\blue.png");
	purpleGoldenBuggy = LoadTexture("textures\\hud\\purple.png");
	youGoldenBuggy = LoadTexture("textures\\hud\\you.png");

	scoreBar = LoadTexture("textures\\hud\\scorebar.png");
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

unsigned int MeshInfo::getUIcomponentID(unsigned int itemType) {
	return UIcomponentIDs[itemType];
}

unsigned int MeshInfo::getCoinComponentID(unsigned int number) {
	return coinComponentIDs[number];
}