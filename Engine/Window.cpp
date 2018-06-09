#include "Window.h"

using namespace Engine;

Window * Window::theInstance = NULL;

Window *Window::getInstance()
{
	if (Window::theInstance == NULL) {
		RenderSystem::getInstance();
		Window::theInstance = new Window();
	}
	return Window::theInstance;

}

int Window::getHeight()
{
	return height;
}

int Window::getWidth()
{
	return width;
}

float Window::getAspectRatio()
{
	return float(width)/float(height);
}

LRESULT Window::CustomWindProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	RenderSystem * rs = RenderSystem::getInstance();
	
	//MessageBox(Window::getInstance()->getMainWindow(), L"Doing Message Stuff ", L"Window", 0); // do failed stuff;

	//Window is destroyed when the engine quits
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 1;
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		if (rs->getImmediateContext())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				//pause the engine
			}else if (wParam == SIZE_MAXIMIZED)
			{
				//unpause and reallocate buffers
				rs->onResize();
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (resizing) {

				}
				else {
					resizing = false;
					rs->onResize();
				}
			}
		}
		return 0;
	case WM_ENTERSIZEMOVE:
		//pause app
		resizing = true;
		return 0;
	case WM_EXITSIZEMOVE:
		//unpause app
		resizing = false;
		rs->onResize();
		return 0;
	case WM_GETMINMAXINFO:
		MINMAXINFO* mmi = (MINMAXINFO*)lParam;
		mmi->ptMinTrackSize.x = (100);
		mmi->ptMinTrackSize.y = (100);

		return 0;
	
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	
	return Window::getInstance()->CustomWindProc(hWnd, uMsg, wParam, lParam);
	//return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

Window::Window() :mainWindow(0)
{
	return;
}

Window::~Window()
{
}

void Window::init()
{
	//LPCWSTR windowClassName = L"Engine";

	WNDCLASSEX wnd;

	wnd.cbSize = 
	wnd.cbClsExtra = 0;
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wnd.hInstance = GetModuleHandle(NULL);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wnd.lpszClassName = L"Engine";
	wnd.lpszMenuName = NULL;
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	wnd.lpfnWndProc = MainWindowProc;

	if (!RegisterClassEx(&wnd)) {				//register the Windows class
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return;
	}

	width = GetSystemMetrics(SM_CXSCREEN) / 2;
	height = GetSystemMetrics(SM_CYSCREEN) / 2;

												//create window and store handle
	mainWindow = CreateWindow(L"Engine", L"Engine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, GetModuleHandle(NULL), 0);


	ShowWindow(mainWindow, SW_SHOW);
	UpdateWindow(mainWindow);


}
