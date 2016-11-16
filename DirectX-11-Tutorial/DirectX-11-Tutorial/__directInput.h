#ifndef _DIRECTINPUTCLASS_H_
#define _DIRECTINPUTCLASS_H_

// Direct Input is the high speed method of interfacing with input devices that the DirectX API provides.
// In DirectX 11 the Direct Input portion of the API has not changed from previous versions, it is still version 8.
// However Direct Input was implemented very well in the first place(similar to direct sound) so there hasn't been any need to update it.
// Direct Input provides incredible speed over the regular windows input system.
// Any high performance application that requires highly responsive input devices should be using Direct Input.

// This tutorial will focus on how to implement Direct Input for keyboard and mouse devices.
// We will also use the TextClass to display the current location of the mouse pointer.
// As the previous tutorials already had an InputClass we will just re-write it using Direct Input instead of the Windows methods that were previously used.



// You need to define the version of Direct Input you are using in the header or the compiler will generate annoying messages that it is defaulting to version 8.
#define DIRECTINPUT_VERSION 0x0800

// The following two libraries need to be linked for Direct Input to work.
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class DirectInputClass {
 public:
	DirectInputClass();
	DirectInputClass(const DirectInputClass&);
   ~DirectInputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();
	void GetMouseLocation(int&, int&, int&);

 private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

 private:
	// The first three private member variables are the interfaces to Direct Input, the keyboard device, and the mouse device.
	IDirectInput8		*m_directInput;
	IDirectInputDevice8	*m_keyboard;
	IDirectInputDevice8	*m_mouse;

	// The next two private member variables are used for recording the current state of the keyboard and mouse devices.
	unsigned char m_keyboardState[256];
	DIMOUSESTATE  m_mouseState;

	int m_screenWidth;
	int m_screenHeight;
	int m_mouseX;
	int m_mouseY;
	int m_mouseZ;
};

#endif
