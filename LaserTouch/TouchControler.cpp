#include "TouchControler.h"



TouchControler::TouchControler()
{

}


TouchControler::~TouchControler()
{
}


void TouchControler::InjectAll(vector<TouchInput> touchs)
{
	vector<POINTER_TOUCH_INFO> contacts;
	for (const auto touch : touchs)
	{
		contacts.push_back(touch.contact);
	}
	InjectTouchInput((int)contacts.size(), &contacts[0]);
}