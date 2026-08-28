#pragma once

enum eKeyboardState
{
	None,
	Down,
	Pressed,
	Up,
};

struct KeyboardEvent
{
	KeyboardEvent() : keyCode(0), state(eKeyboardState::None)
	{}
	KeyboardEvent(UCHAR keyCode, eKeyboardState state = eKeyboardState::None) : keyCode(keyCode), state(state)
	{}
	UCHAR keyCode;
	eKeyboardState state;
};

class Keyboard
{
public:
	Keyboard();
	~Keyboard();
	void AddKeyboardEvent(const UCHAR keyCode, eKeyboardState state);
	KeyboardEvent GetKeyboardEvent();
	bool GetKeyDown(UINT keyCode);
	bool GetKey(UINT keyCode);
	bool GetKeyUp(UINT keyCode);
private:
	std::queue<KeyboardEvent> _events;
	KeyboardEvent _lastEvent;
};

