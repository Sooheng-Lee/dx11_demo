#include "pch.h"
#include "Client.h"

LRESULT WndProcSetup(HWND handle, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(handle, msg, wParam, lParam);
}

Client::Client() : _instance(nullptr), _handle(nullptr), _className(nullptr), _width(0), _height(0)
{
}

Client::~Client()
{
}

bool Client::CreateClient(HINSTANCE hInstance, UINT width, UINT height, const wchar_t* className)
{
    _instance = hInstance;
    _width = width;
    _height = height;
    _className = LPWSTR(className);
	WNDCLASSEX wndClass = {};
	{
		wndClass.cbClsExtra = 0;
		wndClass.cbSize = sizeof(WNDCLASSEX);
		wndClass.lpszClassName = _className;
		wndClass.cbWndExtra = 0;
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.hCursor = LoadCursor(_instance, IDC_ARROW);
		wndClass.hIcon = nullptr;
		wndClass.hIconSm = nullptr;
		wndClass.lpszMenuName = nullptr;
		wndClass.lpfnWndProc = WndProcSetup;
		wndClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	}
	RegisterClassEx(&wndClass);
    _handle = CreateWindowEx(0, L"dx11_game_class", L"dx11_game", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 
		800, 600, nullptr, nullptr, hInstance, nullptr);
	if (!_handle) return false;
	return true;
}

void Client::Run()
{
	ShowWindow(_handle, SW_SHOW);
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Update();
		Render();
	}

	if (msg.message == WM_QUIT)
	{
		UnregisterClass(_className, _instance);
	}
}
