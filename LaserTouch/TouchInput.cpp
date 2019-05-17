#include "TouchInput.h"
#include "windows.h"



TouchInput::TouchInput(int id, int x, int y, int count)
{
	// initialize the touch info structure
	memset(&contact, 0, sizeof(POINTER_TOUCH_INFO));

	contact.pointerInfo.pointerType = PT_TOUCH;
	contact.pointerInfo.pointerId = id;
	contact.pointerInfo.ptPixelLocation.x = x;
	contact.pointerInfo.ptPixelLocation.y = y;
	contact.pointerInfo.pointerFlags = POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_DOWN;
	contact.touchFlags = TOUCH_FLAG_NONE;
	contact.touchMask = TOUCH_MASK_CONTACTAREA | TOUCH_MASK_ORIENTATION | TOUCH_MASK_PRESSURE;
	contact.orientation = 90;
	contact.pressure = 32000;

	// set the contact area depending on thickness
	contact.rcContact.top = y - 2;
	contact.rcContact.bottom = y + 2;
	contact.rcContact.left = x - 2;
	contact.rcContact.right = x + 2;

	//InjectTouchInput(count, &contact);
}

void TouchInput::UpdateInject(int id, int x, int y, int count)
{
	contact.pointerInfo.ptPixelLocation.x = x;
	contact.pointerInfo.ptPixelLocation.y = y;
	contact.pointerInfo.pointerFlags = POINTER_FLAG_INRANGE | POINTER_FLAG_INCONTACT | POINTER_FLAG_UPDATE;
	
	// set the contact area depending on thickness
	contact.rcContact.top = y - 2;
	contact.rcContact.bottom = y + 2;
	contact.rcContact.left = x - 2;
	contact.rcContact.right = x + 2;

	//InjectTouchInput(count, &contact);
}

void TouchInput::UpInject(int count)
{
	contact.pointerInfo.pointerFlags = POINTER_FLAG_UP;
	// inject a touch up
	//InjectTouchInput(count, &contact);
}


TouchInput::~TouchInput()
{
	
}
