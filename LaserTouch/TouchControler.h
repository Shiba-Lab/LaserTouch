#include "TouchInput.h"
using namespace std;
#pragma once
class TouchControler
{
public:
	vector<TouchInput> touchs;
	TouchControler();
	void InjectAll(vector<TouchInput> touchs);
	~TouchControler();
};

