#include "__graphicsClass.h"

GraphicsClass::GraphicsClass()
{
	m_d3d = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	// Create the Direct3D object.
	m_d3d = new d3dClass;
	if( !m_d3d )
		return false;

	// Initialize the Direct3D object.
	result = m_d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if( !result ) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	if( m_d3d ) {
		m_d3d->Shutdown();
		delete m_d3d;
		m_d3d = 0;
	}

	return;
}

bool GraphicsClass::Frame()
{
	bool result;

	// Render the graphics scene.
	result = Render();

	if (!result)
		return false;

	return true;
}

bool GraphicsClass::Render()
{
	// Clear the buffers to begin the scene.
	m_d3d->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	// Present the rendered scene to the screen.
	m_d3d->EndScene();

	return true;
}
