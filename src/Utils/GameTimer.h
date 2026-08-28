#pragma once
class GameTimer
{
public:
	GameTimer();
	~GameTimer();
	static GameTimer* GetInstance()
	{
		if (_instance == nullptr)
		{
			_instance = std::make_shared<GameTimer>();
		}
		return _instance.get();
	}
	void Update();
	void ToggleState();
	FLOAT GetDeltaTime() { return _deltaTime; }

private:
	void Start();
	void Stop();

private:
	static std::shared_ptr<GameTimer> _instance;
	FLOAT _deltaTime;
	bool _bUpdating;
	LARGE_INTEGER _frequency;
	LARGE_INTEGER _currentCounter;
	LARGE_INTEGER _prevCounter;
};

