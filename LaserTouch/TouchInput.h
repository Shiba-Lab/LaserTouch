#pragma once

#include "opencv2/core/core.hpp"
#include "windows.h"

#define MAX_COUNT 10

class TouchInput
{
public:
	POINTER_TOUCH_INFO contact;
	float x;
	float y;
	int id;
	bool flag;

	TouchInput(int id, int x, int y, int count);
	void UpdateInject(int id, int x, int y, int count);
	void UpInject(int count);
	~TouchInput();
};

