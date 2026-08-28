#include "..\pch.h"
#include "GameTimer.h"

std::shared_ptr<GameTimer> GameTimer::_instance = nullptr;

GameTimer::GameTimer()
{
	_deltaTime = 0.0f;
	_bUpdating = false;
	QueryPerformanceFrequency(&_frequency);
	_currentCounter = {};
	_prevCounter = {};
}

GameTimer::~GameTimer()
{
}

void GameTimer::Start()
{
	_bUpdating = true;
	QueryPerformanceCounter(&_prevCounter);
	_deltaTime = 0.0f;
}

void GameTimer::Stop()
{
	_bUpdating = false;
	_prevCounter = {};
	_currentCounter = {};
	_deltaTime = 0.0f;
}

void GameTimer::Update()
{
	if (!_bUpdating) return;
	QueryPerformanceCounter(&_currentCounter);
	_deltaTime = static_cast<FLOAT>(_currentCounter.QuadPart - _prevCounter.QuadPart) / _frequency.QuadPart;
	_prevCounter = _currentCounter;
}

void GameTimer::ToggleState()
{
	if (_bUpdating) {
		Stop();
	}
	else{
		Start();
	}
}
