#include "__graphicsClass.h"

BitmapClass* Sprite::Bitmap = 0;	// �������������� ����������� ������ ������ � ���������� �������. ������-�� �� �� ����� ������������������ � ����� ������, � ����� ������ �����.
#define NUM 3

std::vector<gameObjectBase*> monstersVector;

GraphicsClass::GraphicsClass()
{
	m_d3d			= 0;
	m_Camera		= 0;
	m_Model			= 0;
	//m_ColorShader	= 0;
	m_TextureShader = 0;
	m_TextureShaderIns = 0;
	m_LightShader	= 0;
	m_Light			= 0;
	m_Bitmap		= 0;
	m_BitmapIns		= 0;
	m_TextOut		= 0;

    sprIns1 = 0;
    sprIns2 = 0;
}

GraphicsClass::GraphicsClass(const GraphicsClass &other)
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

	// �������� ������� �������� ������
	scrWidth  = screenWidth;
	scrHeight = screenHeight;

	// Create the Direct3D object
    SAFE_CREATE(m_d3d, d3dClass);

	// Initialize the Direct3D object
	result = m_d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    CHECK_RESULT(result, L"Could not initialize Direct3D");

	// Create the camera object.
    SAFE_CREATE(m_Camera, CameraClass);

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

#if 1
	// Create the model object.
    SAFE_CREATE(m_Model, ModelClass);

	// Initialize the model object
	//result = m_Model->Initialize(m_d3d->GetDevice(), L"../DirectX-11-Tutorial/data/seafloor.dds");
	//result = m_Model->Initialize(m_d3d->GetDevice(), L"../DirectX-11-Tutorial/data/3da2d4e0.dds");

	// The model initialization now takes in the filename of the model file it is loading.
	result = m_Model->Initialize(m_d3d->GetDevice(), "../DirectX-11-Tutorial/data/_model_cube.txt", L"../DirectX-11-Tutorial/data/3da2d4e0.dds");
	//result = m_Model->Initialize(m_d3d->GetDevice(), "../DirectX-11-Tutorial/data/_model_sphere.txt", L"../DirectX-11-Tutorial/data/3da2d4e0.dds");
    CHECK_RESULT(result, L"Could not initialize the model object.");
#endif

#if 0
	// Create and initialize the color shader object
    SAFE_CREATE(m_ColorShader, ColorShaderClass);
	result = m_ColorShader->Initialize(m_d3d->GetDevice(), hwnd);
    CHECK_RESULT(result, L"Could not initialize the color shader object.");
#endif

#if 0
	// Create and initialize the texture shader (TextureShaderClass) object
    SAFE_CREATE(m_TextureShader, TextureShaderClass);
	result = m_TextureShader->Initialize(m_d3d->GetDevice(), hwnd);
    CHECK_RESULT(result, L"Could not initialize the texture shader object.");
#endif

#if 1
	// --- The new light shader object is created and initialized here ---
	{
		// Create and initialize the light shader object
        SAFE_CREATE(m_LightShader, LightShaderClass);
		result = m_LightShader->Initialize(m_d3d->GetDevice(), hwnd);
        CHECK_RESULT(result, L"Could not initialize the light shader object.");
	}


	// --- The new light object is created here ---
	{
		// Create the light object.
        SAFE_CREATE(m_Light, LightClass);
	
		// Initialize the light object:

		// 1. Set Ambient Color
		m_Light->SetAmbientColor(0.05f, 0.05f, 0.05f, 1.0f);
		//m_Light->SetAmbientColor(0.1f, 0.0f, 0.0f, 1.0f);

		// 2. Set Diffuse Color
		m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_Light->SetDirection(1.0f, 0.0f, 1.0f);

		// 3. Set Specular Color
		m_Light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
		m_Light->SetSpecularPower(32.0f);
	}
#endif

#if 1
	// --- Create the texture shader objects ---
	{
        SAFE_CREATE(m_TextureShader, TextureShaderClass);
        SAFE_CREATE(m_TextureShaderIns, TextureShaderClass_Instancing);

		// Initialize the texture shader objects:
        if( !m_TextureShader->Initialize(m_d3d->GetDevice(), hwnd)      ||
            !m_TextureShaderIns->Initialize(m_d3d->GetDevice(), hwnd) )
        {
            MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
			return false;
		}
	}


	// --- Bitmaps and Sprites ---
	{
		// Here is where we create and initialize the new BitmapClass object.
		// It uses the seafloor.dds as the texture and I set the size to 256x256.
		// You can change this size to whatever you like as it does not need to reflect the exact size of the texture.

		// Create and initialize the bitmap objects:
        SAFE_CREATE(m_Bitmap, BitmapClass);
        SAFE_CREATE(m_BitmapIns, BitmapClass_Instancing);

		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/seafloor.dds", 256, 256);
		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/bgr.bmp", 1600, 900);
		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/i.jpg", 48, 48);
		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic4.png", 256, 256);
        result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic4.png", 256, 256);
        CHECK_RESULT(result, L"Could not initialize the bitmap object.");

		// �� ������� ����������� ��� ������ � Instancing �������� ������ �� �������. ��� 15k �������� (3x3) � (256x256) FPS - ����������
        // �� ����� ��������, ��� ��� ������� �� �������� ��������� �� ����� ��������. 3x3 ��������� ������� �������, ��� 256x256

        //result = m_BitmapIns->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic5.png", L"../DirectX-11-Tutorial/data/pic5.png", 24, 24);
        WCHAR *frames0[] = { L"../DirectX-11-Tutorial/data/walkingdead.png" };
        result = m_BitmapIns->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic5.png", 24, 24);
        CHECK_RESULT(result, L"Could not initialize the bitmap object.");



        // ���������������� ������ � ���������� ��������
        // ����� ������������ ��� ��� � �������� ��������� ������, ��� � � ���������� �������
        // � ����� ������, � ��������� ������ ���������� ������ �������
        sprIns1 = new InstancedSprite(scrWidth, scrHeight); if (!sprIns1) return false;

#if 0
        // ������ �������, 8 ����
		WCHAR *frames[] = {	L"../DirectX-11-Tutorial/data/monster1/001.png",
							L"../DirectX-11-Tutorial/data/monster1/002.png",
							L"../DirectX-11-Tutorial/data/monster1/003.png",
							L"../DirectX-11-Tutorial/data/monster1/004.png",
							L"../DirectX-11-Tutorial/data/monster1/005.png",
							L"../DirectX-11-Tutorial/data/monster1/006.png",
							L"../DirectX-11-Tutorial/data/monster1/007.png",
							L"../DirectX-11-Tutorial/data/monster1/008.png"
		};

        unsigned int framesNum = sizeof(frames) / sizeof(frames[0]);

        result = sprIns1->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, frames, framesNum, 30, 30);
        CHECK_RESULT(result, L"Could not initialize the instanced sprite object.");

        srand(time(0));
        for (int i = 0; i < NUM; i++) {
            int        x = 50 + (float)rand() / (RAND_MAX + 1) * 700;
            int        y = 50 + (float)rand() / (RAND_MAX + 1) * 500;
            float  speed = (rand() % 250 + 10) * 0.1f;
            int interval = 50 / speed;
            interval = 10;

            // � �������� ��������� anim_Qty �������� ��� ����� ����������� ������ ��� (����� ������ � �������� - 1)
            monstersVector.push_back(new Monster(x, y, 0.0f, speed, interval, 8));
        }

#else
        // ���������� �����, 10 ������ 200x310
        WCHAR *frames[] = { L"../DirectX-11-Tutorial/data/walkingdead.png" };

        result = sprIns1->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, frames, 1, 45, 45, 200, 310);
        CHECK_RESULT(result, L"Could not initialize the instanced sprite object.");

        srand(time(0));
        for (int i = 0; i < NUM; i++) {
            int        x = 50 + (float)rand() / (RAND_MAX + 1) * 700;
            int        y = 50 + (float)rand() / (RAND_MAX + 1) * 500;
            float  speed = (rand() % 250 + 10) * 0.1f;
            int interval = 50 / speed;

            // � �������� ��������� anim_Qty �������� ��� ����� ����������� ������ ��� (����� ������ � �������� - 1)
            monstersVector.push_back(new Monster(x, y, -90.0f, speed, interval, 9));
        }

#endif

        

        SAFE_CREATE(m_BitmapSprite, BitmapClass);

		result = m_BitmapSprite->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic5.png", 24, 24);
        CHECK_RESULT(result, L"Could not initialize the bitmap object.");

		for (int i = 0; i < NUM; i++) {

			int X = (float)rand() / (RAND_MAX + 1) * scrWidth;
			int Y = (float)rand() / (RAND_MAX + 1) * scrHeight;

			Sprite *spr = new Sprite(X, Y);
			spr->setBitmap(m_BitmapSprite);
			m_spriteVec.push_back(spr);
		}
	}


	// --- Cursor ---
	{
        SAFE_CREATE(m_Cursor, BitmapClass);
		result = m_Cursor->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/cursor.png", 24, 24);
        CHECK_RESULT(result, L"Could not initialize the cursor object.");
	}

#endif


	// --- text Object ---
	{
		// We create a new view matrix from the camera object for the TextClass to use.
		// It will always use this view matrix so that the text is always drawn in the same location on the screen.
		D3DXMATRIX baseViewMatrix;

		// Initialize a base view matrix with the camera for 2D user interface rendering.
		m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
		m_Camera->Render();
		m_Camera->GetViewMatrix(baseViewMatrix);

		// Here we create and initialize the new TextOutClass object.

		// Create and init the text object.
        SAFE_CREATE(m_TextOut, TextOutClass);
		result = m_TextOut->Initialize(m_d3d->GetDevice(), m_d3d->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
        CHECK_RESULT(result, L"Could not initialize the text object.");
	}


	// --- log videocard info ---
	{
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
	}

	return true;
}

void GraphicsClass::Shutdown()
{
    if (monstersVector.size() > 0)
        for (int i = 0; i < monstersVector.size(); i++)
            SAFE_DELETE(monstersVector[i]);

    if (m_spriteVec.size() > 0)
        for (int i = 0; i < m_spriteVec.size(); i++)
            SAFE_DELETE(m_spriteVec[i]);

    SAFE_SHUTDOWN(sprIns1);
    SAFE_SHUTDOWN(sprIns2);

	// Release the Text object:
    SAFE_SHUTDOWN(m_TextOut);

	// Release the Bitmap objects:
    SAFE_SHUTDOWN(m_Bitmap);
    SAFE_SHUTDOWN(m_BitmapIns);
    SAFE_SHUTDOWN(m_BitmapSprite);
    SAFE_SHUTDOWN(m_Cursor);

    // Release the light object:
    SAFE_DELETE(m_Light);

    // Release the Shader objects:
    //SAFE_SHUTDOWN(m_ColorShader);
    SAFE_SHUTDOWN(m_TextureShader);
    SAFE_SHUTDOWN(m_TextureShaderIns);
    SAFE_SHUTDOWN(m_LightShader);

	// Release the model object:
    SAFE_SHUTDOWN(m_Model);

	// Release the camera object:
    SAFE_DELETE(m_Camera);

    // Release d3d object:
    SAFE_SHUTDOWN(m_d3d);

	return;
}

bool GraphicsClass::Frame(const int &fps, const int &cpu, const float &frameTime)
{
	bool result;

	// Set the frames per second
	result = m_TextOut->SetFps(fps, m_d3d->GetDeviceContext());
	if (!result)
		return false;

	// Set the cpu usage
	result = m_TextOut->SetCpu(cpu, m_d3d->GetDeviceContext());
	if (!result)
		return false;

	return true;
}

bool GraphicsClass::Render(const float &rotation, const float &zoom, const int &mouseX, const int &mouseY, bool onTimer)
{
	bool		 result;
	D3DXMATRIX	 viewMatrix, projectionMatrix, worldMatrixX, worldMatrixY, worldMatrixZ, orthoMatrix, translationMatrix;

	if (true) {
		//m_Camera->SetPosition(0.0f, 0.0f, -20.0f + 15 * sin(10 * zoom));

		// zoom with the mouse wheel
		m_Camera->SetPosition(0.0f, 0.0f, -10.0f + 0.005*zoom);
	}

	// Clear the buffers to begin the scene.
	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
    //m_d3d->BeginScene(0.9f, 0.9f, 0.9f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_d3d->GetWorldMatrix(worldMatrixX);
	m_d3d->GetWorldMatrix(worldMatrixY);
	m_d3d->GetWorldMatrix(worldMatrixZ);
	m_d3d->GetProjectionMatrix(projectionMatrix);

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// --- 2d Rendering ---
	// ���� ������������ ������ ��������� ��������������, � ��� ������ ���������� ��������� ������ � ���� � �� �� ������� (� ����� ������),
	// ����� ���������� �� ������������� ������������� ��������� � ���������� ����� � ������ BitmapClass::Render() -> UpdateBuffers()
	// ����� �������, ��� �������, ����� ��������� �������� ��������� 2d-�����
	// TODO: ������������ ������������� BitmapClass ���, ����� ���������� � �� D3D11_USAGE_DYNAMIC � D3D11_CPU_ACCESS_WRITE
	// TODO: ����������� ������������� ����� ��������
	// TODO: ����������� Instancing:
	// http://www.rastertek.com/dx11tut37.html
	// http://stackoverflow.com/questions/3884885/what-is-the-best-pratice-to-render-sprites-in-directx-11
	// http://www.gamedev.net/topic/588291-sprites-in-directx11/

    // If you're using orthographic projection, then you can specify any coordinates you want for the left/right/top/bottom value.  This means that if you're using 800x600 resolution, you can set the orthographic projections to left : 0, top; 0, right:800, bottom : 600 which will give you a 1 : 1 mapping of vertex coordinated to pixels, which makes doing UI really simple.For the textures, I assume you're rendering some type of  quad  using the texture?  If you want a centered  quad , you can draw it at ((screenWidth/2)-(textureWidth/2),((screenHeight/2)-(textureHeight-2)) of size (textureWidth x textureHeight).  This will give you a centered  quad  with 1:1 mapping of texels to pixels.  You can render this anywhere on the screen too, if you setup your orthographic projection matrix to the screen size, then you can render a  texture  sized  quad  anywhere you want, and it will map 1:1 on the screen.
    // For scrolling / zooming, you can then either update the quad coordinates by hand before drawing them, or use a view matrix to offset them in the vertex program.Zooming depends on how you're trying to zoom though, orthographic projection uses parallel lines, so things farther away will be the same size after projection, so you'd only be affecting the depth, but not actually "zooming" in on something(making it larger as you zoom in).To accomplish that, you'd need to  scale  things manually based on depth/zoom, or use perspective projection (which already automagically scales based on depth).
    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	if( true )
	{
		// ���� ����� ����� ������� � �������������, �������� ����� ������������
		// ��������, ����� ��� �������� ���� ��� � �� ����� ������, ����� �� ��������� ������ ������
        // ??? ���������, ��� ��� ������ �� ���
		m_d3d->TurnOnAlphaBlending();

		// We now also get the ortho matrix from the D3DClass for 2D rendering. We will pass this in instead of the projection matrix.
		m_d3d->GetOrthoMatrix(orthoMatrix);

		// ������� ��� �������� � �������� � ������ �������
		D3DXMATRIX matScale;
        m_d3d->GetWorldMatrix(translationMatrix);
		m_d3d->GetWorldMatrix(matScale);

		// The Z buffer is turned off before we do any 2D rendering.
		m_d3d->TurnZBufferOff();


		// ���������� ������ ������
		int xCenter = scrWidth /2;
		int yCenter = scrHeight/2;

		// �������� ����� � �����
		if (!m_Bitmap->Render(m_d3d->GetDeviceContext(), xCenter - 128, yCenter - 128))
			return false;

		// ������������ ����������� �������������� ������
		D3DXMatrixRotationZ(&worldMatrixZ, rotation / 5);
        D3DXMatrixTranslation(&translationMatrix, 100.0f, 100.0f, 0.0f);
		D3DXMatrixScaling(&matScale, 0.85f + 0.03*sin(rotation) + 0.0001*zoom, 0.85f + 0.03*sin(rotation) + 0.0001*zoom, 1.0f);


		// Once the vertex / index buffers are prepared we draw them using the texture shader.
		// Notice we send in the orthoMatrix instead of the projectionMatrix for rendering 2D.
		// Due note also that if your view matrix is changing you will need to create a default one for 2D rendering and use it instead of the regular view matrix.
		// In this tutorial using the regular view matrix is fine as the camera in this tutorial is stationary.

		// �������� ������ ��� ������ ����������� �������
		if ( !m_TextureShader->Render(m_d3d->GetDeviceContext(), m_Bitmap->GetIndexCount(),
										worldMatrixZ
                                        * translationMatrix
										* matScale
										,
										viewMatrix, orthoMatrix, m_Bitmap->GetTexture()) )
			return false;

		// �������� ������
		if (!m_Cursor->Render(m_d3d->GetDeviceContext(), mouseX, mouseY))
			return false;

		m_d3d->GetWorldMatrix(worldMatrixX);
		result = m_TextureShader->Render(m_d3d->GetDeviceContext(), m_Cursor->GetIndexCount(), worldMatrixX, viewMatrix, orthoMatrix, m_Cursor->GetTexture());
		if (!result)
			return false;



		// render bitmaps using Instancing
#if 0
		{
			// reset world matrices
			m_d3d->GetWorldMatrix(worldMatrixZ);
            m_d3d->GetWorldMatrix(translationMatrix);
			m_d3d->GetWorldMatrix(matScale);

			// �� ����� ������������� � ���������� �� GPU ������� ������ � ������ ������, ����� ��� �������������� � ������������� � ��������, ��� ������� ��� FPS
			if( onTimer )
				if ( !m_BitmapIns->initializeInstances(m_d3d->GetDevice()) )
					return false;

			// �������� ������ ����� � ����� !!!
			int xCenter = scrWidth  / 2;
			int yCenter = scrHeight / 2;
			int bmpSize = 24;
			if ( !m_BitmapIns->Render(m_d3d->GetDeviceContext()) )
				return false;
#if 0
			D3DXMatrixRotationZ(&worldMatrixZ, rotation / 5);
			D3DXMatrixTranslation(&matTrans, 100.0f, 100.0f, 0.0f);
			D3DXMatrixScaling(&matScale, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 1.0f);
#endif

			// The Render function for the shader now requires the vertex and instance count from the model object.
			// Render the model using the texture shader.
			result = m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
						m_BitmapIns->GetVertexCount(), m_BitmapIns->GetInstanceCount(),
							worldMatrixZ * translationMatrix * matScale,
								viewMatrix, orthoMatrix, m_BitmapIns->GetTexture(), mouseX - xCenter, yCenter - mouseY);

			if (!result)
				return false;
		}
#endif




        // render sprites using Instancing from vector
#if 1
        {
            // reset world matrices
            m_d3d->GetWorldMatrix(worldMatrixZ);
            m_d3d->GetWorldMatrix(translationMatrix);
            m_d3d->GetWorldMatrix(matScale);

            // �� ����� ������������� � ���������� �� GPU ������� ������ � ������ ������, ����� ��� �������������� � ������������� � ��������, ��� ������� ��� FPS

#if 1
            if (onTimer) {

                for (int i = 0; i < monstersVector.size(); i++)
                    monstersVector[i]->Move(mouseX, mouseY);

                if (!sprIns1->initializeInstances(m_d3d->GetDevice(), &monstersVector))
                    return false;
            }

            // ���������� ������ ������� � GPU
            if (!sprIns1->Render(m_d3d->GetDeviceContext()))
                return false;

            //D3DXMatrixRotationZ(&worldMatrixZ, rotation / 5);
            //D3DXMatrixTranslation(&matTrans, 100.0f, 100.0f, 0.0f);
            //D3DXMatrixScaling(&matScale, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 1.0f);
            //D3DXMatrixScaling(&matScale, 1.0f + 0.1*sin(rotation) + 0.1*zoom, 1.0f + 0.1*sin(rotation) + 0.1*zoom, 1.0f);

            // The Render function for the shader now requires the vertex and instance count from the model object.
            // Render the model using the texture shader.
            result = m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
                sprIns1->GetVertexCount(), sprIns1->GetInstanceCount(),
                    worldMatrixZ * translationMatrix * matScale,
                        viewMatrix, orthoMatrix, sprIns1->GetTextureArray(), mouseX - xCenter, yCenter - mouseY);

            if (!result)
                return false;
#endif
        }

#endif



		// render bitmaps from vector
#if 0
		// test-fast-render

		// ��� ����� ���������� ��� ��� ������ �� �������
		xCenter = 600;
		yCenter = 450;

		if (!m_spriteVec.size() || !m_spriteVec[0]->Render(m_d3d->GetDeviceContext(), xCenter - 24, yCenter - 24))
			return false;

		ID3D11DeviceContext		 *device   = m_d3d->GetDeviceContext();
		ID3D11ShaderResourceView *texture  = m_spriteVec[0]->getTexture();
		int						  indexCnt = m_spriteVec[0]->getIndexCount();
		int x, y;

		static int selector;
		static float frameCount = 1001.0f;
		frameCount++;

		if( frameCount > 1000 ) {
			frameCount = 0.0f;
			selector = (float)rand() / (RAND_MAX + 1) * 20;
		}

selector = -1;

		for (int i = 0; i < m_spriteVec.size(); i++) {

			m_spriteVec[i]->getCoords(x, y);

			switch( selector ) {

				case -1:

					D3DXMatrixRotationZ(&worldMatrixZ, i/100.0);
					D3DXMatrixScaling(&matScale, 1.0f, 1.0f, 1.0f);
					break;
			
				case 0:
					// ���� �����
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.5*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 0.0005*sin(rotation*i/5000) + 0.05*zoom, 1.0f);
					break;

				case 1:
					// �������� ������
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.25*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 0.0005*sin(rotation*i/5000) + 0.05*zoom, 1.0f);
					break;

				case 2:
					// ������ ����������, ��� ��������������� ����� ���� ������
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 1.0f + 0.05*sin(rotation*i/5000) + 0.0005*zoom, 1.0f + 0.05*sin(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 3:
					// Opera
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.1*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 0.0005*sin(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 4:
					// Big Opera
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.1*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 0.05*sin(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 5:
					// Big ROUND Opera
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.1*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 0.1*sin(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 6:
					// Big SQUARE Opera
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.1*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 0.1*cos(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 7:
					// Moebeus DNA 1
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + sin(i) * 0.03*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + sin(i) * 0.03*cos(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 8:
					// square MOBEUS DNA 2
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + sin(i) * 0.05*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + cos(i) * 0.05*cos(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 9:
					// round pulsing jaws of atan
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + sin(i) * 0.05*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 2*sin(rotation*0.5)*atan(i) * 0.05*cos(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 10:
					// majic ninja mask
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + sin(i) * 0.05*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 5*sin(rotation*0.5)* 0.05*cos(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 11:
					// rotating circles 1
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.75*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 0.751*sin(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 12:
					// rotating wheel of crawling bugs
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.1*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 0.101*sin(rotation*i/5000) + 0.0005*zoom, 1.0f);
					break;

				case 13:
					// circle of changing phases 1
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.1*sin(rotation*i/5000) + 0.0005*zoom, 0.5f + 0.1*sin(rotation*i/3000) + 0.0005*zoom, 1.0f);
					break;

				case 14:
					// circle of SLOW changing phases 2
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.1*sin(rotation*i/50000) + 0.0005*zoom, 0.5f + 0.1*sin(rotation*i/25000) + 0.0005*zoom, 1.0f);
					break;

				case 15:
					// circle of SLOW changing phases 3 - eye of the Dragon
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					D3DXMatrixScaling(&matScale, 0.5f + 0.35*sin(rotation*i/50000) + 0.0005*zoom, 0.5f + 0.1*sin(rotation*i/25000) + 0.0005*zoom, 1.0f);
					break;

				case 16:

					break;

				default:
					D3DXMatrixRotationZ(&worldMatrixZ, (rotation+i) / 10);
					//D3DXMatrixTranslation(&matTrans, x * cos(rotation/100 + .002*i) - 400.0f, y - 300.0f, 0.0f);
					//D3DXMatrixScaling(&matScale, 1.0f + 0.05*sin(rotation*i/5000) + 0.0005*zoom, 1.0f + 0.05*sin(rotation*i/5000) + 0.0005*zoom, 1.0f);
					D3DXMatrixScaling(&matScale, 0.5f + 0.35*sin(rotation*i/10000) + 0.0005*zoom, 0.5f + 0.1*sin(rotation*i/15000) + 0.0005*zoom, 1.0f);
			}

			// **********************************************************************************************************************************

			if( i == 0 ) {

				if (!m_TextureShader->Render(device, indexCnt,
					worldMatrixZ * matScale * matTrans,
					viewMatrix, orthoMatrix, texture, true))
					return false;
			}
			else {

				if (!m_TextureShader->Render(device, indexCnt,
					worldMatrixZ * matScale * matTrans,
					viewMatrix, orthoMatrix, texture, false))
					return false;
			}
		}

#endif

		// text Out
		// We call the text object to render all its sentences to the screen here.
		// And just like with 2D images we disable the Z buffer before drawing and then enable it again after all the 2D has been drawn.

		// Render the text strings.
		result = m_TextOut->Render(m_d3d->GetDeviceContext(), worldMatrixX, orthoMatrix);
		if(!result)
			return false;


        // ����������� ������ � 2d-�������:
		m_d3d->TurnOffAlphaBlending();
		// After all the 2D rendering is done we turn the Z buffer back on for the next round of 3D rendering.
		// Turn the Z buffer back on now that all 2D rendering has completed.
		m_d3d->TurnZBufferOn();
	}


    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// --- 3d Rendering ---
    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
#if 1
	{
        // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
        m_Model->Render(m_d3d->GetDeviceContext());

		// Here we rotate the world matrix by the rotation value so that when we render the model using this updated world matrix
		D3DXMatrixRotationX(&worldMatrixX, tan(0.1*rotation));
		D3DXMatrixRotationY(&worldMatrixY, tan(0.1*rotation));
        D3DXMatrixTranslation(&translationMatrix, 10.0f, 7.0f, 10.0f);

		result = m_LightShader->Render(m_d3d->GetDeviceContext(), m_Model->GetIndexCount(),
								// ���� �� ������� �������� �� �������������� �������, � ����� ��� �� ����������, �� ���������� ������ ��������� ���������� � ������ �����
								// ���� ������� ��������� ����������, �� ������ �������� ������ �� ������
								worldMatrixX * worldMatrixY * worldMatrixZ * translationMatrix,
								viewMatrix, projectionMatrix,
								m_Model->GetTexture(),
								m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
								m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower()
		);
	}
#endif



    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// Present the rendered scene to the screen.
    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	m_d3d->EndScene();

	return true;
}
