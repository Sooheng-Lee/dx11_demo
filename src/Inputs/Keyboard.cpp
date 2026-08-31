#include "..\pch.h"
#include "Keyboard.h"

Keyboard::Keyboard()
{
	_lastEvent = KeyboardEvent();
	_keyStates.fill(eKeyboardState::None);
	_keyPressed.fill(false);
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

void Keyboard::Update()
{
	for (UINT idx = 0; idx < KEY_COUNT; ++idx)
	{
		if (_keyStates[idx] == eKeyboardState::Down)
		{
			_keyStates[idx] = eKeyboardState::Pressed;
		}
		else if (_keyStates[idx] == eKeyboardState::Up)
		{
			_keyStates[idx] = eKeyboardState::None;
		}
	}

	while (!_events.empty())
	{
		KeyboardEvent event = _events.front();
		_events.pop();
		_lastEvent = event;

		const UINT keyCode = event.keyCode;
		if (keyCode >= KEY_COUNT)
		{
			continue;
		}

		if (event.state == eKeyboardState::Down || event.state == eKeyboardState::Pressed)
		{
			_keyStates[keyCode] = _keyPressed[keyCode] ? eKeyboardState::Pressed : eKeyboardState::Down;
			_keyPressed[keyCode] = true;
		}
		else if (event.state == eKeyboardState::Up || event.state == eKeyboardState::None)
		{
			_keyStates[keyCode] = _keyPressed[keyCode] ? eKeyboardState::Up : eKeyboardState::None;
			_keyPressed[keyCode] = false;
		}
	}
}

KeyboardEvent Keyboard::GetKeyboardEvent()
{
	if (_events.empty()) {
		if (_lastEvent.state == Pressed)
			return _lastEvent;
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
	return keyCode < KEY_COUNT && _keyStates[keyCode] == eKeyboardState::Down;
}

bool Keyboard::GetKey(UINT keyCode)
{
	return keyCode < KEY_COUNT
		&& (_keyStates[keyCode] == eKeyboardState::Down || _keyStates[keyCode] == eKeyboardState::Pressed);
}

bool Keyboard::GetKeyUp(UINT keyCode)
{
	return keyCode < KEY_COUNT && _keyStates[keyCode] == eKeyboardState::Up;
}
