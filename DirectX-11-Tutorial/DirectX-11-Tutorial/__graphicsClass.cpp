#include "__graphicsClass.h"

GraphicsClass::GraphicsClass()
{
	m_d3d			= 0;
	m_Camera		= 0;
	m_Model			= 0;
	//m_ColorShader	= 0;
	m_TextureShader = 0;
	m_LightShader	= 0;
	m_Light			= 0;
	m_Bitmap		= 0;
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
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

#if 1
	// Create the model object.
	m_Model = new ModelClass;
	if (!m_Model)
		return false;

	// Initialize the model object
	//result = m_Model->Initialize(m_d3d->GetDevice(), L"../DirectX-11-Tutorial/data/seafloor.dds");
	//result = m_Model->Initialize(m_d3d->GetDevice(), L"../DirectX-11-Tutorial/data/3da2d4e0.dds");

	// The model initialization now takes in the filename of the model file it is loading.
	// In this tutorial we will use the cube.txt file so this model loads in a 3D cube object for rendering.
	result = m_Model->Initialize(m_d3d->GetDevice(), "../DirectX-11-Tutorial/data/_model_cube.txt", L"../DirectX-11-Tutorial/data/3da2d4e0.dds");
	//result = m_Model->Initialize(m_d3d->GetDevice(), "../DirectX-11-Tutorial/data/_model_sphere.txt", L"../DirectX-11-Tutorial/data/3da2d4e0.dds");
	
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
#endif

#if 0
	// Initialize the model object.
	result = m_Model->Initialize(m_d3d->GetDevice());
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}
#endif

#if 0
	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
		return false;

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_d3d->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}
#endif

#if 0
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
#endif

#if 1
	// The new light shader object is created and initialized here.

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
		return false;

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_d3d->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}
	
	// The new light object is created here.

	// Create the light object.
	m_Light = new LightClass;
	if (!m_Light)
		return false;
	
	// Initialize the light object.

	// Set Ambient Color
	m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
	//m_Light->SetAmbientColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Set Diffuse Color
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(1.0f, 0.0f, 1.0f);

	// Set Specular Color
	m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetSpecularPower(32.0f);
#endif

#if 1
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

	// Here is where we create and initialize the new BitmapClass object.
	// It uses the seafloor.dds as the texture and I set the size to 256x256.
	// You can change this size to whatever you like as it does not need to reflect the exact size of the texture.

	// Create the bitmap object.
	m_Bitmap = new BitmapClass;
	if (!m_Bitmap)
		return false;

	// Initialize the bitmap object.

	//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/seafloor.dds", 256, 256);
	//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/bgr.bmp", 1600, 900);
	//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/i.jpg", 48, 48);
	result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic4.png", 256, 256);
	
	int NUM = 333;
	PT  pt;

	for (int i = 0; i < NUM; i++) {
		BitmapClass *bm1 = new BitmapClass();
		bm1->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic5.png", 24, 24);
		m_BitmapVector.push_back(bm1);

		pt.X = (float)rand() / (RAND_MAX+1) * 800;
		pt.Y = (float)rand() / (RAND_MAX+1) * 600;
		m_coordsVec.push_back(pt);
/*
		BitmapClass *bm2 = new BitmapClass();
		bm2->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic2.bmp", 48, 48);
		m_BitmapVector.push_back(bm2);

		pt.X = (float)rand() / (RAND_MAX + 1) * 800;
		pt.Y = (float)rand() / (RAND_MAX + 1) * 600;
		m_coordsVec.push_back(pt);


		BitmapClass *bm3 = new BitmapClass();
		bm3->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic3.bmp", 48, 48);
		m_BitmapVector.push_back(bm3);

		pt.X = (float)rand() / (RAND_MAX + 1) * 800;
		pt.Y = (float)rand() / (RAND_MAX + 1) * 600;
		m_coordsVec.push_back(pt);
*/
	}



	if (!result) {
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

#endif



	// --- log videocard info ---

	char cardInfo[256] = "Video Card info: ";
	char cardName[128];
	char intBuff[32];
	int  cardMemory = -1;
	m_d3d->GetVideoCardInfo(cardName, cardMemory);

	strcat_s(cardInfo, 256, cardName);

	if (cardMemory >= 0) {
		_itoa_s(cardMemory, intBuff, 10);
		strcat_s(cardInfo, 256, " with ");
		strcat_s(cardInfo, 128, intBuff);
		strcat_s(cardInfo, 256, " MBytes of Memory");
	}

	logMsg(cardInfo);

	return true;
}

void GraphicsClass::Shutdown()
{
	// Release the bitmap object.
	if(m_Bitmap) {
		m_Bitmap->Shutdown();
		delete m_Bitmap;
		m_Bitmap = 0;
	}

	if( m_BitmapVector.size() > 0 ) {
		for(int i = 0; i < m_BitmapVector.size(); i++) {
			m_BitmapVector[i]->Shutdown();
			delete m_BitmapVector[i];
			m_BitmapVector[i] = 0;
		}
	}

#if 0
	// Release the color shader object.
	if (m_ColorShader) {
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}
#endif

#if 1
	// Release the texture shader object.
	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
#endif

#if 1
	// Release the light object.
	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	// Release the light shader object.
	if (m_LightShader) {
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}
#endif

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

	// We add a new static variable to hold an updated rotation value each frame that will be passed into the Render function.
	static float rotation = 0.0f;

	// Update the rotation variable each frame.
	rotation += (float)D3DX_PI * 0.01f;
	if (rotation > 360.0f)
		rotation -= 360.0f;

	// Render the graphics scene.
	result = Render(rotation);

	if (!result)
		return false;

	return true;
}

bool GraphicsClass::Render(float rotation)
{
	static float zoom = 0.0f;
	bool		 result;
	D3DXMATRIX	 viewMatrix, projectionMatrix, worldMatrixX, worldMatrixY, worldMatrixZ, orthoMatrix;

	if (true) {
		zoom += 0.002;
		//m_Camera->SetPosition(0.0f, 0.0f, -20.0f + 15 * sin(10 * zoom));
	}

	// Clear the buffers to begin the scene.
	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_d3d->GetWorldMatrix(worldMatrixX);
	m_d3d->GetWorldMatrix(worldMatrixY);
	m_d3d->GetWorldMatrix(worldMatrixZ);
	m_d3d->GetProjectionMatrix(projectionMatrix);

	// --- 2d Rendering ---
	// Если использовать только матричные преобразования, а сам битмап изначально рендерить всегда в одну и ту же позицию (в центр экрана),
	// можно избавиться от необходимости пересоздавать вершинный и текстурный буфер в методе BitmapClass::Render() -> UpdateBuffers()
	// Таким образом, мне кажется, можно несколько ускорить отрисовку 2d-сцены
	// TODO: организовать инициализацию BitmapClass так, чтобы избавиться и от D3D11_USAGE_DYNAMIC и D3D11_CPU_ACCESS_WRITE
	// TODO: реализовать использование одной текстуры
	// TODO: реализовать Instancing:
	// http://www.rastertek.com/dx11tut37.html
	// http://stackoverflow.com/questions/3884885/what-is-the-best-pratice-to-render-sprites-in-directx-11
	// http://www.gamedev.net/topic/588291-sprites-in-directx11/

	if(true)
	{
		// Если нужен вывод текстур с прозрачностью, включаем режим прозрачности
		// Наверное, можно его включить один раз и до конца работы, чтобы не выполнять лишнюю работу
		m_d3d->TurnOnAlphaBlending();

		// We now also get the ortho matrix from the D3DClass for 2D rendering. We will pass this in instead of the projection matrix.
		m_d3d->GetOrthoMatrix(orthoMatrix);

		// Матрицы для поворота и переноса в нужную позицию
		D3DXMATRIX matScale;
		D3DXMATRIX matTrans;
		m_d3d->GetWorldMatrix(matTrans);
		m_d3d->GetWorldMatrix(matScale);

		// The Z buffer is turned off before we do any 2D rendering.
		m_d3d->TurnZBufferOff();

		// Координаты центра экрана
		int xCenter = 800/2;
		int yCenter = 600/2;
		int bitMapSize = 256;

		// Рендерим точно в центр
		//if (!m_Bitmap->Render(m_d3d->GetDeviceContext(), xCenter - bitMapSize / 2, yCenter - bitMapSize / 2))
		if (!m_Bitmap->Render(m_d3d->GetDeviceContext(), xCenter - 128, yCenter - 128))
			return false;

		// Осуществляем необходимые преобразования матриц
		D3DXMatrixRotationZ(&worldMatrixZ, rotation / 5);
		D3DXMatrixTranslation(&matTrans, 100.0f, 100.0f, 0.0f);
		D3DXMatrixScaling(&matScale, 0.5f + 0.1*sin(10 * zoom), 0.5f + 0.1*sin(10 * zoom), 1.0f);


		// Once the vertex / index buffers are prepared we draw them using the texture shader.
		// Notice we send in the orthoMatrix instead of the projectionMatrix for rendering 2D.
		// Due note also that if your view matrix is changing you will need to create a default one for 2D rendering and use it instead of the regular view matrix.
		// In this tutorial using the regular view matrix is fine as the camera in this tutorial is stationary.

		// Рендерим битмап при помощи текстурного шейдера
		if ( !m_TextureShader->Render(m_d3d->GetDeviceContext(), m_Bitmap->GetIndexCount(),
										worldMatrixZ
										* matTrans
										* matScale
										,
										viewMatrix, orthoMatrix, m_Bitmap->GetTexture()) )
			return false;

		// render bitmaps from vector
#if 1
		for (int i = 0; i < m_BitmapVector.size(); i++) {

			if ( !m_BitmapVector[i]->Render(m_d3d->GetDeviceContext(), xCenter - 24, yCenter - 24) )
				return false;

			int x = m_coordsVec[i].X;
			int y = m_coordsVec[i].Y;

			D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i)/ 5);
			D3DXMatrixTranslation(&matTrans, x + 10*cos(rotation + 2*i) - 400.0f, y - 300.0f, 0.0f);
			D3DXMatrixScaling(&matScale, 1.0f + 0.5*sin(10*zoom), 1.0f + 0.5*sin(10*zoom), 1.0f);

			if (!m_TextureShader->Render(m_d3d->GetDeviceContext(), m_BitmapVector[i]->GetIndexCount(),
											worldMatrixZ
										  //* matScale
										  * matTrans
											,
											viewMatrix, orthoMatrix, m_BitmapVector[i]->GetTexture()) )
				return false;
		}
#endif



		m_d3d->TurnOffAlphaBlending();

		// After all the 2D rendering is done we turn the Z buffer back on for the next round of 3D rendering.
		// Turn the Z buffer back on now that all 2D rendering has completed.
		m_d3d->TurnZBufferOn();
	}

	// --- 3d Rendering ---
	{
#if 1
		// Here we rotate the world matrix by the rotation value so that when we render the triangle using this updated world matrix
		// it will spin the triangle by the rotation amount.

		D3DXMatrixRotationX(&worldMatrixX, tan(zoom));
		//D3DXMatrixRotationY(&worldMatrixY, atan(rotation));

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Model->Render(m_d3d->GetDeviceContext());

		D3DXMATRIX	 mat;
		m_d3d->GetWorldMatrix(mat);
		D3DXMatrixTranslation(&mat, 9.0f, 6.5f, 10.0f);

		result = m_LightShader->Render(m_d3d->GetDeviceContext(), m_Model->GetIndexCount(),
								// Если мы сначала умножаем на поворачивающие матрицы, а потом уже на сдвигающую, то повернутый объект правильно сдвигается в нужную точку
								// Если сначала поставить сдвигающую, то объект начинает бегать по экрану
								worldMatrixX
								* worldMatrixY
								* worldMatrixZ
								* mat
								,
								viewMatrix, projectionMatrix,
								m_Model->GetTexture(),
								m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
								m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()
		);
#endif
	}



	// Present the rendered scene to the screen.
	m_d3d->EndScene();
	return true;
}
