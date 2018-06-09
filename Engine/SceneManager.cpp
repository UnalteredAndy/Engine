#include "SceneManager.h"

using namespace Engine;

SceneManager * SceneManager::theInstance = nullptr;

SceneManager* Engine::SceneManager::getInstance()
{
	if (SceneManager::theInstance == nullptr)
	{
		SceneManager::theInstance = new SceneManager();
	}
	return SceneManager::theInstance;
}

void Engine::SceneManager::addCamera(Camera * cam)
{
	sceneCameras.push_back(cam);
	mainCam = cam;
}

SceneManager::SceneManager()
{
}


SceneManager::~SceneManager()
{
}
