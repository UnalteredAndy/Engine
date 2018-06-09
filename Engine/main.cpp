#include "Window.h"
#include "RenderSystem.h"
#include "SceneManager.h"
#include "Camera.h"
#include "Light.h"

using namespace Engine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	RenderSystem * rs = RenderSystem::getInstance();
	//rs->init();
	SceneManager * sm = SceneManager::getInstance();
	
	//make a test shader
	Shader mShader = Shader();
	mShader.setDiffuseMap(L"../Engine/Textures/Planet_Terran_01DiffuseMap.png");
	mShader.setNormalMap( L"../Engine/Textures/ColorPlatform_n.png");
	mShader.buildShader();

	//make a light
	Light dirLight = Light(0.8f, 500.0f);
	dirLight.setPosition(XMVectorSet(1.0f, 50.0f, -1.0f, 1.0f));
	dirLight.setColor(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
	dirLight.lootAt(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f)); // look at the cube

	//test making a mesh
	Mesh planet = Mesh();
	planet.loadMesh("../Engine/Meshes/planet.obj");
	planet.setShader(&mShader);
	planet.setPosition(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f));
	planet.setScalingVector(XMVectorSet(0.1f, 0.1f, 0.1f, 1.0f));
	planet.setRotation(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));

	//test mesh platform
	Mesh platform = Mesh();
	platform.loadMesh("../Engine/Meshes/CC_platform.obj");
	platform.setShader(&mShader);
	platform.setPosition(XMVectorSet(10.0f, 10.0f, -10.0f, 1.0f));
	platform.setScalingVector(XMVectorSet(15.1f, 15.1f, 15.1f, 1.0f));

	//test mesh floor
	Mesh floor = Mesh();
	floor.loadMesh("../Engine/Meshes/floor.obj");
	floor.setShader(&mShader);
	floor.setPosition(XMVectorSet(-100.0f, -10.0f, 50.0f, 1.0f));
	floor.setScalingVector(XMVectorSet(150.1f, 150.1f, 150.1f, 1.0f));

	sm->addMesh(&floor);
	sm->addMesh(&planet);
	sm->addMesh(&platform);
	sm->addLight(&dirLight);

	Camera mainCamera = Camera(XM_PI/2, Window::getInstance()->getAspectRatio(), 0.01f, 1000.0f);
	mainCamera.setPosition(XMVectorSet(0.0f, 0.0f, -50.0f, 1.0f));
	//mainCamera.setRotation(XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));
	

	sm->addCamera(&mainCamera);

	bool engineRunning = true;

	MSG msg = { 0 };
	
	//delta time stuff
	__int64 currentTime = 0;
	__int64 previousTime = 0;
	double deltaTime;

	__int64 countPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countPerSecond);
	double secondsPerCount = 1/ (double) countPerSecond; //frequency adjustment


	while (engineRunning) 
	{
		while(msg.message != WM_QUIT){// handles the quit message ( close window and app when the X ist clicked)
			
			
			QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

			deltaTime = (currentTime - previousTime ) * secondsPerCount;
			previousTime = currentTime;

				while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				rs->renderOneFrame(deltaTime);
		}

		return (int)msg.wParam;
			
	}
};