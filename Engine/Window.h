#pragma once
#include<Windows.h>
#include<DirectXMath.h>
#include"RenderSystem.h"

namespace Engine {

	class Window
	{
	public:
		static Window* getInstance();
		HWND getMainWindow() { return mainWindow; };
		LRESULT CustomWindProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
		void init();

		int getHeight();
		int getWidth();
		float getAspectRatio();//this needs to propagate to camaras if the window changes. Or just let Cameras query if from window each frame(bit inefficient):

	private:
		Window();
		~Window();

		

		static Window* theInstance;
		HWND mainWindow;

		int height, width;

		bool resizing = false; //used when the window is resizing. Not relevant to the rest of the engine since the engine is paused during that time

	};
}