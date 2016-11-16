#include "__directInput.h"

DirectInputClass::DirectInputClass()
{
	m_directInput = 0;
	m_keyboard	  = 0;
	m_mouse		  = 0;
}

DirectInputClass::DirectInputClass(const DirectInputClass& other)
{
}

DirectInputClass::~DirectInputClass()
{
}

bool DirectInputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	// Store the screen size which will be used for positioning the mouse cursor
	m_screenWidth  = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen
	m_mouseX = 0;
	m_mouseY = 0;
	m_mouseZ = 0;

	// This function call will initialize the interface to Direct Input. Once you have a Direct Input object you can initialize other input devices.
	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**) &m_directInput, NULL);
	if (FAILED(result))
		return false;

	// --- The first input device we will initialize will be the keyboard ---
	{
		// Initialize the direct input interface for the keyboard.
		result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
		if (FAILED(result))
			return false;

		// Set the data format. In this case since it is a keyboard we can use the predefined data format.
		result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
		if (FAILED(result))
			return false;

		// Setting the cooperative level of the keyboard is important in both what it does and how you use the device from that point forward.
		// In this case we will set it to not share with other programs (DISCL_EXCLUSIVE).
		// This way if you press a key only your application can see that input and no other application will have access to it.
		// However if you want other applications to have access to keyboard input while your program is running you can set it to non-exclusive (DISCL_NONEXCLUSIVE).
		// Now the print screen key works again and other running applications can be controlled by the keyboard and so forth.
		// Just remember that if it is non-exclusive and you are running in a windowed mode then you will need to check for when the device loses focus
		// and when it re-gains that focus so it can re-acquire the device for use again.
		// This focus loss generally happens when other windows become the main focus over your window or your window is minimized.

		// Set the cooperative level of the keyboard to not share with other programs.
		result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		if (FAILED(result))
			return false;

		// Once they keyboard is setup we then call Acquire to finally get access to the keyboard for use from this point forward.
		// Now acquire the keyboard.
		result = m_keyboard->Acquire();
		if (FAILED(result))
			return false;
	}


	// --- The next input device we setup is the mouse ---
	{
		// Initialize the direct input interface for the mouse.
		result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
		if (FAILED(result))
			return false;

		// Set the data format for the mouse using the pre-defined mouse data format.
		result = m_mouse->SetDataFormat(&c_dfDIMouse);
		if (FAILED(result))
			return false;

		// We use non-exclusive cooperative settings for the mouse.
		// We will have to check for when it goes in and out of focus and re-acquire it each time.

		// Set the cooperative level of the mouse to share with other programs.
		//result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		// При использовании DISCL_NONEXCLUSIVE имеем следующее: у нас два курсора, один внутри экрана, другой снаружи, и они все время рассинхронизируются
		// Поэтому перейдем в эксклюзивный режим и не будем морочиться
		result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE );
		
		if (FAILED(result))
			return false;

		// Once the mouse is setup we acquire it so that we can begin using it
		// Acquire the mouse.
		result = m_mouse->Acquire();
		if (FAILED(result))
			return false;
	}

	return true;
}

// The Shutdown function releases the two devices and the interface to Direct Input.
// Notice that the devices are always un-acquired first and then released.
void DirectInputClass::Shutdown()
{
	// Release the mouse.
	if (m_mouse) {
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if (m_keyboard) {
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if (m_directInput) {
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}

// The Frame function for the InputClass will read the current state of the devices into state buffers we setup.
// After the state of each device is read it then processes the changes.
bool DirectInputClass::Frame()
{
	bool result;

	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if (!result)
		return false;

	// Read the current state of the mouse.
	result = ReadMouse();
	if (!result)
		return false;

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}

// ReadKeyboard will read the state of the keyboard into the m_keyboardState variable.
// The state will show any keys that are currently pressed or not pressed.
// If it fails reading the keyboard then it can be for one of five different reasons.
// The only two that we want to recover from are if the focus is lost or if it becomes un-acquired.
// If this is the case we call acquire each frame until we do get control back.
// The window may be minimized in which case Acquire will fail, but once the window comes to the foreground again then Acquire will succeed
// and we will be able to read the keyboard state.
// The other three error types I don't want to recover from in this tutorial.
bool DirectInputClass::ReadKeyboard()
{
	HRESULT result;

	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID) &m_keyboardState);
	if (FAILED(result)) {

		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_keyboard->Acquire();
		else
			return false;
	}

	return true;
}

// ReadMouse will read the state of the mouse similar to how ReadKeyboard read in the state of the keyboard.
// However the state of the mouse is just changes in the position of the mouse from the last frame.
// So for example updates will look like the mouse has moved 5 units to the right, but it will not give you the actual position of the mouse on the screen.
// This delta information is very useful for different purposes and we can maintain the position of the mouse on the screen ourselves.
bool DirectInputClass::ReadMouse()
{
	HRESULT result;

	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID) &m_mouseState);
	if (FAILED(result)) {

		// If the mouse lost focus or was not acquired then try to get control back.
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			m_mouse->Acquire();
		else
			return false;
	}

	return true;
}

// The ProcessInput function is where we deal with the changes that have happened in the input devices since the last frame.
// For this tutorial we will just do a simple mouse location update similar to how Windows keeps track of where the mouse cursor is.
// To do so we use the m_mouseX and m_mouseY variables that were initialized to zero and simply add the changes in the mouse position to these two variables.
// This will maintain the position of the mouse based on the user moving the mouse around.

// Note that we do check to make sure the mouse location never goes off the screen.
// Even if the user keeps moving the mouse to the left we will just keep the cursor at the zero position until they start moving it to the right again.
void DirectInputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;
	m_mouseZ += m_mouseState.lZ;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if (m_mouseX < 0)
		m_mouseX = 0;

	if (m_mouseY < 0)
		m_mouseY = 0;

	if (m_mouseX > m_screenWidth)
		m_mouseX = m_screenWidth;

	if (m_mouseY > m_screenHeight)
		m_mouseY = m_screenHeight;

	return;
}

// I have added a function to the InputClass called IsEscapePressed.
// This shows how to utilize the keyboard to check if specific keys are currently being pressed.
// You can write other functions to check for any other keys that are of interest to your application.
bool DirectInputClass::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
		return true;

	return false;
}

// GetMouseLocation is a helper function I wrote which returns the location of the mouse.
// GraphicsClass can get this info and then use TextClass to render the mouse X and Y position to the screen.
void DirectInputClass::GetMouseLocation(int &mouseX, int &mouseY, int &mouseZ)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	mouseZ = m_mouseZ;

	return;
}
