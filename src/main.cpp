#include "pch.h"

int WINAPI wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow)
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr)) return 0;
	Engine engine;
	if (!engine.CreateClient(hInstance, 800, 600, L"dx11_game_class")) return -1;
	if (!engine.Init()) return -1;
	engine.Run();
	CoUninitialize();
	return 0;
}
