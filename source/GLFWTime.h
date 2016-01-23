#pragma once

class GLFWTime
{
public:
	static void init()
	{
		_currentTime = _lastTime = _dt = 0.0f;
	}

	static void setTime(float seconds)
	{
		_lastTime = _currentTime;
		_currentTime = seconds;
		_dt = _currentTime - _lastTime;
	}

	static float getTime() { return _currentTime; }
	static float getCurrentTime() { return _currentTime; }
	static float getLastTime() { return _lastTime; }
	static float getDT() { return _dt; }

public:
	static float     _currentTime;
	static float     _dt;
	static float     _lastTime;
};