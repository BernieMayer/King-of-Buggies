#ifndef GAMEMANAGER_CPP
#define GAMEMANAGER_CPP

#include "GameManager.h"

#include <GLFW/glfw3.h>  

GameManager::GameManager(GLFWwindow* window) : renderer(window), input(window), state()
{
	
}

void GameManager::createPlayers()
{
	int i = 0;
	while (i < NUM_PLAYERS) {
		unsigned int player = renderer.generateObjectID();
		state.getPlayer(i)->setRenderID(player);
		
		MeshObject playerMesh = meshInfo.getMesh(BUGGY);
		vector<vec3> playerVerts = playerMesh.getVertices();
		vector<vec3> playerNormals = playerMesh.getNormals();
		vector<unsigned int> playerIndices = playerMesh.getIndices();

		renderer.assignMesh(player, &playerVerts);
		renderer.assignNormals(player, &playerNormals);
		renderer.assignIndices(player, &playerIndices);
		renderer.assignMaterial(player, &tsMat);
		i++;
	}
}

void GameManager::createCoins()
{
	int i = 0;
	while (i < NUM_COINS) {
		unsigned int coin = renderer.generateObjectID();
		state.getCoin(i)->setRenderID(coin);

		// leave commented for now, there's no coin mesh
		//MeshObject coinMesh = meshInfo.getMesh(COIN);
		MeshObject coinMesh = meshInfo.getMesh(SPHERE);
		vector<vec3> coinVerts = coinMesh.getVertices();
		vector<vec3> coinNormals = coinMesh.getNormals();
		vector<unsigned int> coinIndices = coinMesh.getIndices();

		renderer.assignMesh(coin, &coinVerts);
		renderer.assignNormals(coin, &coinNormals);
		renderer.assignIndices(coin, &coinIndices);
		renderer.assignMaterial(coin, &tsMat);
		i++;
	}
}

void GameManager::createPowerups()
{
	int i = 0;
	while (i < NUM_POWERUPS) {
		unsigned int powerup = renderer.generateObjectID();
		state.getPowerup(i)->setRenderID(powerup);

		// leave commented for now, there's no powerup meshes
		//MeshObject coinMesh = meshInfo.getMesh(POWERUP);
		MeshObject powerupMesh = meshInfo.getMesh(SPHERE);
		vector<vec3> powerupVerts = powerupMesh.getVertices();
		vector<vec3> powerupNormals = powerupMesh.getNormals();
		vector<unsigned int> powerupIndices = powerupMesh.getIndices();

		renderer.assignMesh(powerup, &powerupVerts);
		renderer.assignNormals(powerup, &powerupNormals);
		renderer.assignIndices(powerup, &powerupIndices);
		renderer.assignMaterial(powerup, &tsMat);
		i++;
	}
}

void GameManager::createPowerupBoxes()
{
	int i = 0;
	while (i < NUM_POWERBOXES) {
		unsigned int box = renderer.generateObjectID();
		state.getPowerupBox(i)->setRenderID(box);

		// leave commented for now, there's no powerup meshes
		//MeshObject coinMesh = meshInfo.getMesh(POWERUP);
		MeshObject boxMesh = meshInfo.getMesh(SPHERE);
		vector<vec3> boxVerts = boxMesh.getVertices();
		vector<vec3> boxNormals = boxMesh.getNormals();
		vector<unsigned int> boxIndices = boxMesh.getIndices();

		renderer.assignMesh(box, &boxVerts);
		renderer.assignNormals(box, &boxNormals);
		renderer.assignIndices(box, &boxIndices);
		renderer.assignMaterial(box, &tsMat);
		i++;
	}
}

void GameManager::gameLoop()
{

}

void GameManager::gameInit()
{

}

void quitGame()
{

}

#endif // GAMEMANAGER_CPP