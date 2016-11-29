#include "__systemClass.h"

SystemClass::SystemClass()
{
	m_Input	   = 0;
	m_Graphics = 0;

	// �������������� ������ � �������� FPS � CPU
	m_Fps   = 0;
	m_Cpu   = 0;
	m_Timer = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	bool result;
	int  screenWidth, screenHeight;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth  = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the direct input object. This object will be used to handle reading the keyboard input from the user.
	m_Input = new DirectInputClass;
	if (!m_Input)
		return false;

	// Initialize the direct input object
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result) {
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
		return false;

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
		return false;

	// Create and initialize the FpsClass object
	m_Fps = new FpsClass;
	if (!m_Fps)
		return false;

	// Initialize the fps object
	m_Fps->Initialize();
	
	
	// Create and initialize the CpuClass object
	m_Cpu = new CpuClass;
	if (!m_Cpu)
		return false;

	// Initialize the cpu object.
	m_Cpu->Initialize();

	// Create and initialize the TimerClass object
	m_Timer = new HighPrecisionTimer;
	if (!m_Timer)
		return false;

	// Initialize the timer object ()
	result = m_Timer->Initialize(appTimerInterval);
	if (!result) {
		MessageBox(m_hwnd, L"Could not initialize the Timer object.", L"Error", MB_OK);
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// Release the timer object.
	if (m_Timer) {
		delete m_Timer;
		m_Timer = 0;
	}

	// Release the cpu object.
	if (m_Cpu) {
		m_Cpu->Shutdown();
		delete m_Cpu;
		m_Cpu = 0;
	}

	// Release the fps object.
	if (m_Fps) {
		delete m_Fps;
		m_Fps = 0;
	}

	// Release the graphics object.
	if (m_Graphics) {
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Releasing the DirectInput object now requires a Shutdown call previous to deleting the object.
	// Release the input object.
	if (m_Input) {
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user
	done = false;

	while (!done) {

		// Handle the windows messages
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}

		// If windows signals to end the application then exit out
		if (msg.message == WM_QUIT) {

			done = true;

		}
		else {

			// Otherwise do the frame processing
			result = Frame();
			if (!result)
				done = true;

		}

		// The check for the escape key in the Run function is now done slightly different by checking the return value of the helper function in the InputClass.
		// Check if the user pressed escape and wants to quit.
		if( m_Input->IsEscapePressed() )
			done = true;
	}

	return;
}

bool SystemClass::Frame()
{
	bool result;
	int  mouseX, mouseY, mouseZ;

	// Update the system stats
	bool onTimer = m_Timer->Frame();
	m_Fps->Frame();
	m_Cpu->Frame();

	// During the Frame function we call the DirectInput object's own Frame function to update the states of the keyboard and mouse.
	// This call can fail so we need to check the return value.

	// Do the input frame processing
	result = m_Input->Frame();
	if (!result)
		return false;

	// After the input device updates have been read we update the GraphicsClass with the location of the mouse so it can render that in text on the screen.
	// Get the location of the mouse from the input object,
	m_Input->GetMouseLocation(mouseX, mouseY, mouseZ);

	// Do the frame processing for the graphics object
	result = m_Graphics->Frame(m_Fps->GetFps(), m_Cpu->GetCpuPercentage(), m_Timer->GetTime());
	if (!result)
		return false;


	// --- Render Graphics to the screen ---
	{
		// We add a new static variable to hold an updated rotation value each frame that will be passed into the Render function.
		static float rotation = 0.0f;
		static float zoom     = 0.0f;

        keysPressed Keys;

		// Update the rotation variable each time onTimer event happens
		if (onTimer) {

			rotation += (float)D3DX_PI * 0.01f;
			zoom += 0.002;

			if (rotation > 360.0f)
				rotation -= 360.0f;

            // ��������� ��������� ���������� � ��������������� ������� �������� ������ ������
            Keys.down    = m_Input->IsKeyPressed(DIK_S);
            Keys.up      = m_Input->IsKeyPressed(DIK_W);
            Keys.left    = m_Input->IsKeyPressed(DIK_A);
            Keys.right   = m_Input->IsKeyPressed(DIK_D);
            Keys.lmbDown = m_Input->IsLeftMouseButtonDown();
		}

		// Finally render the graphics to the screen anyway
        result = m_Graphics->Render(rotation, mouseZ, mouseX, mouseY, &Keys, onTimer);
		if (!result)
			return false;
	}

	return true;
}

// We have removed the Windows keyboard reads from the MessageHandler function.Direct Input handles all of this for us now.
LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
/*
	switch( umsg )
	{
		// Check if a key has been pressed on the keyboard.
		case WM_KEYDOWN: {
			// If a key is pressed send it to the input object so it can record that state.
			m_Input->KeyDown((unsigned int)wparam);
			return 0;
		}

		// Check if a key has been released on the keyboard.
		case WM_KEYUP: {
			// If a key is released then send it to the input object so it can unset the state for that key.
			m_Input->KeyUp((unsigned int)wparam);
			return 0;
		}

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
			return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
*/
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX	wc;
	DEVMODE		dmScreenSettings;
	int			posX, posY;

	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style		 = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc	 = WndProc;
	wc.cbClsExtra	 = 0;
	wc.cbWndExtra	 = 0;
	wc.hInstance	 = m_hinstance;
	wc.hIcon		 = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm		 = wc.hIcon;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName	 = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize		 = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth  = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if( FULL_SCREEN ) {

		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize		  = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth  = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields	  = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else {

		// If windowed then set it to manual resolution
		screenWidth  = windowedWidth;
		screenHeight = windowedHeight;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth ) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
							WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
							posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL
	);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
		ChangeDisplaySettings(NULL, 0);

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY: {
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE: {
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}
