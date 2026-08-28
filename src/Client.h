#pragma once
class Client
{
public:
	Client();
	~Client();
	bool CreateClient(HINSTANCE hInstance, UINT width, UINT height, const wchar_t* className);
	void Run();
public:
	Keyboard* GetKeyboardPtr() {
		return _keyboard.get();
	}

protected:
	virtual void Update();
	virtual void Render() = 0;

protected:
	HINSTANCE _instance;
	HWND _handle;
	LPWSTR _className;
	UINT _width;
	UINT _height;

	std::shared_ptr<Keyboard> _keyboard;
};

