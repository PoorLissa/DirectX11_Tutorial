#include "__graphicsClass.h"

GraphicsClass::GraphicsClass()
{
	m_d3d		  = 0;
	m_Camera	  = 0;
	m_Model		  = 0;
	//m_ColorShader = 0;
	m_TextureShader = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

void GraphicsClass::logMsg(char *str) {

	FILE *f = NULL;

	fopen_s(&f, "___msgLog.log", "a");
	if (f != NULL) {
		fputs(str, f);
		fputs("\n", f);
		fclose(f);
	}
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

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
		return false;

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -20.0f);

	// Create the model object.
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	// Initialize the model object.
	//result = m_Model->Initialize(m_d3d->GetDevice(), L"../DirectX-11-Tutorial/data/seafloor.dds");
	result = m_Model->Initialize(m_d3d->GetDevice(), L"../DirectX-11-Tutorial/data/3da2d4e0.dds");

	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	// Initialize the model object.
/*
	result = m_Model->Initialize(m_d3d->GetDevice());
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
*/

	// The new TextureShaderClass object is created and initialized.
	// Create the texture shader object.
	m_TextureShader = new TextureShaderClass;

	if (!m_TextureShader)
		return false;

	// Initialize the texture shader object.
	result = m_TextureShader->Initialize(m_d3d->GetDevice(), hwnd);

	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the color shader object.
/*
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
		return false;

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_d3d->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}
*/

	// --- log videocard info ---
	char cardName[128];
	int  cardMemory = -1;
	m_d3d->GetVideoCardInfo(cardName, cardMemory);
	logMsg(cardName);

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the color shader object.
/*
	if (m_ColorShader) {
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}
*/
	// Release the texture shader object.
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the model object.
	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera) {
		delete m_Camera;
		m_Camera = 0;
	}

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
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrixX, worldMatrixY, worldMatrixZ;
	bool result;

	static float zoom = 0.0f;
	if (true) {
		zoom += 0.002;
		m_Camera->SetPosition(0.0f, 0.0f, -30.0f + 20 * sin(50 * zoom));
	}

	// Clear the buffers to begin the scene.
	//m_d3d->BeginScene(0.25f, 0.5f, 0.5f, 1.0f);		// blue/green
	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);			// black

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_d3d->GetWorldMatrix(worldMatrixX);
	m_d3d->GetProjectionMatrix(projectionMatrix);

D3DXMatrixRotationX(&worldMatrixX, 10*zoom);
D3DXMatrixRotationY(&worldMatrixY, 11*zoom);
D3DXMatrixRotationZ(&worldMatrixZ, 500*zoom);


	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_Model->Render(m_d3d->GetDeviceContext());

	// Render the model using the color shader.
/*
	result = m_ColorShader->Render(m_d3d->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix);
	if (!result)
		return false;
*/

	// Render the model using the texture shader.
	result = m_TextureShader->Render(m_d3d->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrixZ, viewMatrix, projectionMatrix, m_Model->GetTexture());

	if (!result)
		return false;

	// Present the rendered scene to the screen.
	m_d3d->EndScene();


	return true;
}
