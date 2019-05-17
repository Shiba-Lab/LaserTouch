#pragma once

#include "opencv2/core/core.hpp"
#include "windows.h"


class TouchInput
{
public:
	POINTER_TOUCH_INFO contact;
	float x;
	float y;
	int id;

	TouchInput(int id, int x, int y);
	void UpdateInput(int id, int x, int y);
	~TouchInput();
};

