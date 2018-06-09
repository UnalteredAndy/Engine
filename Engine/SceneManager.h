#pragma once
#include<vector>
#include"Mesh.h"
#include"Camera.h"
#include"Light.h"

namespace Engine {

	class Mesh;
	class Camera;

	class SceneManager
	{
	
	public:
		static SceneManager * getInstance();
		//replace all functions after this asap!
		void addMesh(Mesh* mesh) { sceneMeshes.push_back(mesh); };
		void addLight(Light* light) { sceneLights.push_back(light); };
		void addCamera(Camera* cam);
		Camera* getMainCamera() { return mainCam; };
		std::vector<Mesh*> sceneMeshes;
		std::vector<Light*> sceneLights;

	private:
		SceneManager();
		~SceneManager();

		Camera * mainCam;
		std::vector<Camera*> sceneCameras;
		static SceneManager * theInstance;
	};

}