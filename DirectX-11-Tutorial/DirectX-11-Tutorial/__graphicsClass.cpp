#include "stdafx.h"
#include "__graphicsClass.h"

extern std::string strMsg;

Game theGame;
// ------------------------------------------------------------------------------------------------------------------------



// Начальная инициализация полей класса
GraphicsClass::GraphicsClass()
{
	m_d3d			   = 0;
	m_Camera		   = 0;
	m_Model			   = 0;
	m_TextureShader    = 0;
	m_TextureShaderIns = 0;
	m_LightShader	   = 0;
	m_Light			   = 0;
	m_Bitmap_Tree	   = 0;
	m_BitmapIns		   = 0;
	m_TextOut		   = 0;

    msg = "...";
}
// ------------------------------------------------------------------------------------------------------------------------



GraphicsClass::GraphicsClass(const GraphicsClass &other)
{}
// ------------------------------------------------------------------------------------------------------------------------



GraphicsClass::~GraphicsClass()
{}
// ------------------------------------------------------------------------------------------------------------------------



// Free the resources
void GraphicsClass::Shutdown()
{
    // Clean up the screen
    m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

        m_d3d->TurnZBufferOff();

        msg = "Bye-bye... Shutting down.";
        m_TextOut->SetText(msg, m_d3d->GetDeviceContext());
        m_TextOut->Render(m_d3d->GetDeviceContext(), matrixWorldX, matrixOrthographic);

        m_d3d->TurnZBufferOn();

    m_d3d->EndScene();

	// Release the Text object:
    SAFE_SHUTDOWN(m_TextOut);

	// Release the Bitmap objects:
	SAFE_SHUTDOWN(m_Bitmap_Tree);
    SAFE_SHUTDOWN(m_BitmapIns);

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

    theGame.Shutdown();

    // Release d3d object:
    SAFE_SHUTDOWN(m_d3d);

	return;
}
// ------------------------------------------------------------------------------------------------------------------------



// Вывод строки в файл
void GraphicsClass::logMsg(const std::string &str, bool doCleanFile, char *fileName) {

	FILE *f = NULL;

	fopen_s(&f, fileName, doCleanFile ? "w" : "a");
	if( f != NULL ) {
		fputs(str.c_str(), f);
		fputs("\n", f);
		fclose(f);
	}
}
// ------------------------------------------------------------------------------------------------------------------------



bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HighPrecisionTimer *appTimer, HWND hwnd)
{
	bool result;

	// Запомним размеры текущего экрана
	scrWidth      = screenWidth;
	scrHeight     = screenHeight;
    scrHalfWidth  = screenWidth  * 0.5;
    scrHalfHeight = screenHeight * 0.5;

	// Create the Direct3D object
    SAFE_CREATE(m_d3d, d3dClass);

	// Initialize the Direct3D object
	result = m_d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    CHECK_RESULT(hwnd, result, L"Could not initialize Direct3d!");

	// Create the camera object
    SAFE_CREATE(m_Camera, CameraClass);

	// Set the initial position of the camera
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

#if 1
	// Create the model object
    SAFE_CREATE(m_Model, ModelClass);

	// Initialize the model object
	//result = m_Model->Initialize(m_d3d->GetDevice(), L"../DirectX-11-Tutorial/data/seafloor.dds");
	//result = m_Model->Initialize(m_d3d->GetDevice(), L"../DirectX-11-Tutorial/data/3da2d4e0.dds");

	// The model initialization now takes in the filename of the model file it is loading.
	result = m_Model->Initialize(m_d3d->GetDevice(), "../DirectX-11-Tutorial/data/_model_cube.txt", L"../DirectX-11-Tutorial/data/3da2d4e0.dds");
	//result = m_Model->Initialize(m_d3d->GetDevice(), "../DirectX-11-Tutorial/data/_model_sphere.txt", L"../DirectX-11-Tutorial/data/3da2d4e0.dds");
    CHECK_RESULT(hwnd, result, L"Could not initialize the model object.");
#endif

#if 0
	// Create and initialize the color shader object
    SAFE_CREATE(m_ColorShader, ColorShaderClass);
	result = m_ColorShader->Initialize(m_d3d->GetDevice(), hwnd);
    CHECK_RESULT(hwnd, result, L"Could not initialize the color shader object.");
#endif

#if 1
	// --- The new light shader object is created and initialized here ---
	{
		// Create and initialize the light shader object
        SAFE_CREATE(m_LightShader, LightShaderClass);
		result = m_LightShader->Initialize(m_d3d->GetDevice(), hwnd);
        CHECK_RESULT(hwnd, result, L"Could not initialize the light shader object.");
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
        SAFE_CREATE(m_TextureShader,    TextureShaderClass);
        SAFE_CREATE(m_TextureShaderIns, TextureShaderClass_Instancing);

		// Initialize the texture shader objects:
        if( !m_TextureShader    -> Initialize(m_d3d->GetDevice(), hwnd) ||
            !m_TextureShaderIns -> Initialize(m_d3d->GetDevice(), hwnd)   ) 
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
		SAFE_CREATE(m_Bitmap_Tree, BitmapClass);
        SAFE_CREATE(m_BitmapIns,   BitmapClass_Instancing);

		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/seafloor.dds", 256, 256);
		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/i.jpg", 48, 48);
		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic4.png", 256, 256);
		//result = m_Bitmap_Bgr->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/bgr.bmp", 1600, 900);
		//result = m_Bitmap_Bgr->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/bgr_1600.jpg", 1600, 900);

		result = m_Bitmap_Tree->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic4.png", 256, 256);
        CHECK_RESULT(hwnd, result, L"Could not initialize the bitmap object.");

		// От размера изображения при работе с Instancing скорость работы не зависит. Для 15k битмапов (3x3) и (256x256) FPS - одинаковый
        // Но нужно понимать, что фпс зависит от размеров выводимых на экран спрайтов. 3x3 выводится гораздо быстрее, чем 256x256

        //result = m_BitmapIns->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic5.png", L"../DirectX-11-Tutorial/data/pic5.png", 24, 24);
        result = m_BitmapIns->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic5.png", 24, 24);
        CHECK_RESULT(hwnd, result, L"Could not initialize the bitmap object.");

	}



    // --- Game Object ---
    {
        result = theGame.Init(screenWidth, screenHeight, appTimer, this, hwnd);
        CHECK_RESULT(hwnd, result, L"Could not initialize the game object.");
    }

#endif


	// --- text Object ---
	{
		// We create a new view matrix from the camera object for the TextClass to use
		// It will always use this view matrix so that the text is always drawn in the same location on the screen
		D3DXMATRIX baseViewMatrix;

		// Initialize a base view matrix with the camera for 2D user interface rendering
		m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
		m_Camera->Render();
		m_Camera->GetViewMatrix(baseViewMatrix);

		// Here we create and initialize the new TextOutClass object
        SAFE_CREATE(m_TextOut, TextOutClass);
		result = m_TextOut->Initialize(m_d3d->GetDevice(), m_d3d->GetDeviceContext(), hwnd, screenWidth, screenHeight, baseViewMatrix);
        CHECK_RESULT(hwnd, result, L"Could not initialize the text object.");
	}


	// --- log videocard info ---
	{
		char cardName[128];
		int  cardMemory = -1;
		m_d3d->GetVideoCardInfo(cardName, cardMemory);

        std::string str = "Video Card info: ";
        str += cardName;

		if (cardMemory >= 0)
            str += " with " + std::to_string(cardMemory) + " MBytes of Memory";

        str += "\n\n";

		logMsg(str, true);  // пересоздаем файл в новой сессии
	}

	return true;
}
// ------------------------------------------------------------------------------------------------------------------------



bool GraphicsClass::Frame(const int &fps, const int &cpu, const float &frameTime)
{
    ID3D11DeviceContext *devContext = m_d3d->GetDeviceContext();

    // Set FPS, CPU usage and a text message:
	if( !m_TextOut->SetFps( fps, devContext) ||
        !m_TextOut->SetCpu( cpu, devContext) ||
        !m_TextOut->SetText(msg, devContext)   )
        return false;

	return true;
}
// ------------------------------------------------------------------------------------------------------------------------



bool GraphicsClass::Render(const float &rotation, const float &zoom, const int &mouseX, const int &mouseY, const keysPressed *Keys, const bool &onTimer)
{
	bool result;
#if defined doLogMessages
    strMsg = "";
#endif

#if 1
		//m_Camera->SetPosition(0.0f, 0.0f, -20.0f + 15 * sin(10 * zoom));
		// zoom with the mouse wheel
		m_Camera->SetPosition(0.0f, 0.0f, -10.0f + 0.005f*zoom);
#endif

	// Clear the buffers to begin the scene
	m_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
    //m_d3d->BeginScene(0.9f, 0.9f, 0.9f, 1.0f);

	// Generate the view matrix based on the camera's position
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects
	m_Camera->GetViewMatrix    ( matrixView        );
	m_d3d->GetWorldMatrix      ( matrixWorldX      );
	m_d3d->GetWorldMatrix      ( matrixWorldY      );
	m_d3d->GetWorldMatrix      ( matrixWorldZ      );
	m_d3d->GetProjectionMatrix ( matrixProjection  );
    m_d3d->GetWorldMatrix      ( matrixTranslation );
	m_d3d->GetWorldMatrix      ( matrixScaling     );



    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// --- 2d Rendering ---
    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if( !Render2d(rotation, zoom, mouseX, mouseY, Keys, onTimer) )
        return false;



    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// --- 3d Rendering ---
    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    if( !Render3d(rotation, zoom, mouseX, mouseY, Keys, onTimer) )
        return false;



    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	// --- Present the rendered scene to the screen ---
    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
	m_d3d->EndScene();

#if defined doLogMessages
    if( strMsg != "" )
        logMsg(strMsg);
#endif

	return true;
}
// ------------------------------------------------------------------------------------------------------------------------



// --- 2d Rendering ---
bool GraphicsClass::Render2d(const float &rotation, const float &zoom, const int &mouseX, const int &mouseY, const keysPressed *Keys, const bool & onTimer)
{
    ID3D11DeviceContext *devContext = m_d3d->GetDeviceContext();

    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // If you're using orthographic projection, then you can specify any coordinates you want for the left/right/top/bottom value.
    // This means that if you're using 800x600 resolution, you can set the orthographic projections to left : 0, top; 0, right:800, bottom : 600
    // which will give you a 1 : 1 mapping of vertex coordinated to pixels, which makes doing UI really simple.
    // For the textures, I assume you're rendering some type of  quad  using the texture?
    // If you want a centered  quad , you can draw it at ((screenWidth/2)-(textureWidth/2),((screenHeight/2)-(textureHeight-2)) of size (textureWidth x textureHeight).
    // This will give you a centered  quad  with 1:1 mapping of texels to pixels.  
    // You can render this anywhere on the screen too, if you setup your orthographic projection matrix to the screen size, then you can render a  texture  sized  quad
    // anywhere you want, and it will map 1:1 on the screen.
    // For scrolling / zooming, you can then either update the quad coordinates by hand before drawing them, or use a view matrix to offset them in the vertex program.
    // Zooming depends on how you're trying to zoom though, orthographic projection uses parallel lines, so things farther away will be the same size after projection,
    // so you'd only be affecting the depth, but not actually "zooming" in on something(making it larger as you zoom in).To accomplish that, you'd need to  scale  things
    // manually based on depth/zoom, or use perspective projection (which already automagically scales based on depth).
    // -------------------------------------------------------------------------------------------------------------------------------------------------------------------

	// Если нужен вывод текстур с прозрачностью, включаем режим прозрачности
	// Наверное, можно его включить один раз и до конца работы, чтобы не выполнять лишнюю работу
    // ??? проверить, как это влияет на фпс
	m_d3d->TurnOnAlphaBlending();

	// We now also get the ortho matrix from the D3DClass for 2D rendering. We will pass this in instead of the projection matrix
	m_d3d->GetOrthoMatrix(matrixOrthographic);

	// The Z buffer is turned off before we do any 2D rendering
	m_d3d->TurnZBufferOff();



    // Рендерим все игровые объекты
    {
        if( !theGame.Render2d(rotation, zoom, mouseX, mouseY, Keys, onTimer) )
            return false;

        msg = theGame.getMsg();
    }



	// render bitmaps using Instancing
#if 0
	{
		// reset world matrices
		m_d3d->GetWorldMatrix(matrixWorldZ);
        m_d3d->GetWorldMatrix(matrixTranslation);
		m_d3d->GetWorldMatrix(matScale);

		// не нужно пересчитывать и передавать на GPU большие буфера с каждым кадром, пусть они просчитываются в синхронизации с таймером, это добавит нам FPS
		if( onTimer )
			if ( !m_BitmapIns->initializeInstances(m_d3d->GetDevice()) )
				return false;

		// Рендерим модель точно в центр !!!
		int xCenter = scrWidth  / 2;
		int yCenter = scrHeight / 2;
		int bmpSize = 24;
		if ( !m_BitmapIns->Render(devContext) )
			return false;
#if 0
		D3DXMatrixRotationZ(&matrixWorldZ, rotation / 5);
		D3DXMatrixTranslation(&matTrans, 100.0f, 100.0f, 0.0f);
		D3DXMatrixScaling(&matScale, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 1.0f);
#endif

		// The Render function for the shader now requires the vertex and instance count from the model object.
		// Render the model using the texture shader.
		result = m_TextureShaderIns->Render(devContext,
					m_BitmapIns->GetVertexCount(), m_BitmapIns->GetInstanceCount(),
						matrixWorldZ * matrixTranslation * matScale,
							matrixView, matrixOrthographic, m_BitmapIns->GetTexture(), mouseX - xCenter, yCenter - mouseY);

		if (!result)
			return false;
	}
#endif



	// text Out
	// We call the text object to render all its sentences to the screen here.
	// And just like with 2D images we disable the Z buffer before drawing and then enable it again after all the 2D has been drawn.

	// Render the text strings
	if( !m_TextOut->Render(devContext, matrixWorldX, matrixOrthographic) )
        return false;

    // Заканчиваем работу с 2d-режимом:
	m_d3d->TurnOffAlphaBlending();

	// After all the 2D rendering is done we turn the Z buffer back on for the next round of 3D rendering.
	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_d3d->TurnZBufferOn();

    return true;
}
// ------------------------------------------------------------------------------------------------------------------------



bool GraphicsClass::Render3d(const float &rotation, const float &zoom, const int &mouseX, const int &mouseY, const keysPressed *Keys, const bool & onTimer)
{
    ID3D11DeviceContext *devContext = m_d3d->GetDeviceContext();

    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
    m_Model->Render(devContext);

	// Here we rotate the world matrix by the rotation value so that when we render the model using this updated world matrix
	D3DXMatrixRotationX  (&matrixWorldX, tan(0.1*rotation));
	D3DXMatrixRotationY  (&matrixWorldY, tan(0.1*rotation));
    D3DXMatrixTranslation(&matrixTranslation, 10.0f, 7.0f, 10.0f);

	// Если мы сначала умножаем на поворачивающие матрицы, а потом уже на сдвигающую, то повернутый объект правильно сдвигается в нужную точку
	// Если сначала поставить сдвигающую, то объект начинает бегать по экрану
	if( !m_LightShader->Render(devContext, m_Model->GetIndexCount(),
            matrixWorldX * matrixWorldY * matrixWorldZ * matrixTranslation,
		        matrixView, matrixProjection,
			        m_Model->GetTexture(),
				        m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
					        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower() ) )
    return false;

    return true;
}
// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------