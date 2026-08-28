#include "..\pch.h"
#include "Keyboard.h"

Keyboard::Keyboard()
{
	_lastEvent;
}

Keyboard::~Keyboard()
{
}

void Keyboard::AddKeyboardEvent(const UCHAR keyCode, eKeyboardState state)
{
	KeyboardEvent event(keyCode, state);
	std::wstring codeStr = std::to_wstring(keyCode) + std::wstring(L"\n");
	std::wstring keyLog;
	switch (state)
	{
	case None:
		keyLog = std::wstring(L"State : None, KeyCode : ") + codeStr;
		break;
	case Down:
		keyLog = std::wstring(L"State : Down, KeyCode : ") + codeStr;
		break;
	case Pressed:
		keyLog = std::wstring(L"State : Pressed, KeyCode : ") + codeStr;
		break;
	case Up:
		keyLog = std::wstring(L"State : Up, KeyCode : ") + codeStr;
		break;

	}
	OutputDebugStringW(keyLog.c_str());
	_events.push(event);
}

KeyboardEvent Keyboard::GetKeyboardEvent()
{
	if (_events.empty()) {
		_lastEvent = KeyboardEvent();
		return KeyboardEvent();
	};
	KeyboardEvent event = _events.front();
	_lastEvent = event;
	_events.pop();
	return event;
}

bool Keyboard::GetKeyDown(UINT keyCode)
{
	return _lastEvent.state == eKeyboardState::Down && _lastEvent.keyCode == keyCode;
}

bool Keyboard::GetKey(UINT keyCode)
{
	return _lastEvent.state == eKeyboardState::Pressed && _lastEvent.keyCode == keyCode;
}

bool Keyboard::GetKeyUp(UINT keyCode)
{
	return _lastEvent.state == eKeyboardState::Up && _lastEvent.keyCode == keyCode;
}
