#include "pch.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Engine engine;
	if (!engine.CreateClient(hInstance, 800, 600, L"dx11_game_class")) return -1;
	if (!engine.Init()) return -1;
	engine.Run();
	return 0;
}