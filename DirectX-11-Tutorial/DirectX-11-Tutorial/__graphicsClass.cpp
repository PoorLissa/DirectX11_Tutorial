#include "stdafx.h"
#include "__graphicsClass.h"

ThreadPool *thPool; // указатель на пул потоков

// Вспомогательная структура для хранения списка монстров и всей сопутствующей инфы
// Хотел объявить ее в файле gameClasses.h, но не удалось из-за перекрестных инклюдов :(
struct MonsterList {
    std::list<gameObjectBase*> objList;
    unsigned int         listSize;
    float                rotationAngle;
    InstancedSprite     *spriteInst;
} monsterList1, monsterList2;           // Списки с монстрами

std::vector< MonsterList* > VEC;        // Вектор, в котором содержатся все наши списки монстров. Передаем его в обработчик перемещения каждой пули для просчета стрельбы

// Вспомогательная структура для хранения списка бонусов и всей сопутствующей инфы
struct BonusList {
    std::list<gameObjectBase*> objList;
    unsigned int         listSize;
    float                rotationAngle;
    InstancedSprite     *spriteInst;
} bonusList1;

// Вспомогательная структура для хранения списка оружия и всей сопутствующей инфы
struct WeaponList {
    std::list<gameObjectBase*> objList;
    unsigned int         listSize;
    InstancedSprite     *spriteInst;
} weaponList1;

std::list<gameObjectBase*> bulletList;
unsigned int bulletListSize = 0;

HighPrecisionTimer gameTimer;
char buf[100];
// ------------------------------------------------------------------------------------------------------------------------



// Начальная инициализация полей класса
GraphicsClass::GraphicsClass()
{
	m_d3d			   = 0;
	m_Camera		   = 0;
	m_Model			   = 0;
	m_TextureShader    = 0;
	m_TextureShaderIns = 0;
    m_BulletShader     = 0;
	m_LightShader	   = 0;
	m_Light			   = 0;
	m_Bitmap_Tree	   = 0;
	m_Bitmap_Bgr	   = 0;
	m_BitmapIns		   = 0;
	m_TextOut		   = 0;
    sprIns1            = 0;
    sprIns2            = 0;
    sprIns3            = 0;
    sprIns4            = 0;
    m_PlayerBitmapIns1 = 0;
    m_PlayerBitmapIns2 = 0;
    m_BulletBitmapIns  = 0;

    thPool             = 0;

    msg = "...";
}
// ------------------------------------------------------------------------------------------------------------------------



GraphicsClass::GraphicsClass(const GraphicsClass &other)
{
}
// ------------------------------------------------------------------------------------------------------------------------



GraphicsClass::~GraphicsClass()
{
}
// ------------------------------------------------------------------------------------------------------------------------



void GraphicsClass::logMsg(char *str) {

	FILE *f = NULL;

	fopen_s(&f, "___msgLog.log", "a");
	if (f != NULL) {
		fputs(str, f);
		fputs("\n", f);
		fclose(f);
	}
}
// ------------------------------------------------------------------------------------------------------------------------



bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HighPrecisionTimer *appTimer, HWND hwnd)
{
	bool result;

    gameTimer.Initialize(100);		// пока что используется просто для замеров интервалов времени при расчетах, onTimer не используется

    thPool = new ThreadPool(50);
    gameObjectBase::setThreadPool(thPool);

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
        SAFE_CREATE(m_BulletShader,     bulletShader_Instancing);

		// Initialize the texture shader objects:
        if( !m_TextureShader    -> Initialize(m_d3d->GetDevice(), hwnd) ||
            !m_TextureShaderIns -> Initialize(m_d3d->GetDevice(), hwnd) ||
            !m_BulletShader     -> Initialize(m_d3d->GetDevice(), hwnd)   ) 
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
        SAFE_CREATE(m_Bitmap_Bgr,   BitmapClass);
		SAFE_CREATE(m_Bitmap_Tree,  BitmapClass);
        SAFE_CREATE(m_BitmapIns,    BitmapClass_Instancing);

		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/seafloor.dds", 256, 256);
		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/i.jpg", 48, 48);
		//result = m_Bitmap->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic4.png", 256, 256);
		//result = m_Bitmap_Bgr->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/bgr.bmp", 1600, 900);
		//result = m_Bitmap_Bgr->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/bgr_1600.jpg", 1600, 900);
		result = m_Bitmap_Bgr->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/bgr_1600.jpg", screenWidth, screenHeight);
        CHECK_RESULT(hwnd, result, L"Could not initialize the bitmap object.");

		result = m_Bitmap_Tree->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic4.png", 256, 256);
        CHECK_RESULT(hwnd, result, L"Could not initialize the bitmap object.");

		// От размера изображения при работе с Instancing скорость работы не зависит. Для 15k битмапов (3x3) и (256x256) FPS - одинаковый
        // Но нужно понимать, что фпс зависит от размеров выводимых на экран спрайтов. 3x3 выводится гораздо быстрее, чем 256x256

        //result = m_BitmapIns->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic5.png", L"../DirectX-11-Tutorial/data/pic5.png", 24, 24);
        result = m_BitmapIns->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/pic5.png", 24, 24);
        CHECK_RESULT(hwnd, result, L"Could not initialize the bitmap object.");



        // Инстанцированный спрайт с поддержкой анимации
        // Можем использовать его как с массивом отдельных файлов, так и с текстурным атласом
        // В любом случае, в связанный шейдер передается массив текстур
        sprIns1 = new InstancedSprite(scrWidth, scrHeight);            if (!sprIns1)            return false;
        sprIns2 = new InstancedSprite(scrWidth, scrHeight);            if (!sprIns2)            return false;
        sprIns3 = new InstancedSprite(scrWidth, scrHeight);            if (!sprIns3)            return false;
        sprIns4 = new InstancedSprite(scrWidth, scrHeight);            if (!sprIns4)            return false;
        m_PlayerBitmapIns1 = new InstancedSprite(scrWidth, scrHeight); if (!m_PlayerBitmapIns1) return false;
        m_PlayerBitmapIns2 = new InstancedSprite(scrWidth, scrHeight); if (!m_PlayerBitmapIns2) return false;
        m_BulletBitmapIns  = new InstancedSprite(scrWidth, scrHeight); if (!m_BulletBitmapIns)  return false;

        srand(unsigned int(time(0)));



        // Монстры, список 1
        {
            // Текстурный атлас, 10 кадров 200x310
            WCHAR *frames[] = { L"../DirectX-11-Tutorial/data/walkingdead.png" };

            result = sprIns1->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, frames, 1, 45, 45, 200, 310);
            CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object.");

            monsterList1.spriteInst    = sprIns1;
            monsterList1.rotationAngle = -90.0f;

            for (int i = 0; i < MONSTERS_QTY; i++) {
                int        x = 50 + rand() % (scrWidth  - 100);
                int        y = 50 + rand() % (scrHeight - 100);
                float  speed = (rand() % 250 + 10) * 0.1f;
				float  scale = 0.5f + (rand() % 16) * 0.1f;
                int interval = int(50 / speed);

                speed = 0.01f;

                // в качестве параметра anim_Qty передаем или число загружаемых файлов или [число кадров в текстуре - 1]
                monsterList1.objList.push_back(new Monster(x, y, scale, monsterList1.rotationAngle, speed, interval, 9));
                monsterList1.listSize++;
            }
        }


        // Монстры, список 2
        {
            // Массив текстур, 8 штук
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

            result = sprIns2->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, frames, framesNum, 30, 30);
            CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object.");

            monsterList2.spriteInst    = sprIns2;
            monsterList2.rotationAngle = 0.0f;

            for (int i = 0; i < MONSTERS_QTY; i++) {
                int        x = 50 + rand() % (scrWidth  - 100);
                int        y = 50 + rand() % (scrHeight - 100);
                float  speed = (rand() % 250 + 10) * 0.1f;
				float  scale = 0.5f + (rand() % 16) * 0.1f;
                int interval = int(50 / speed);

                speed = 0.01f;

                // в качестве параметра anim_Qty передаем или число загружаемых файлов или (число кадров в текстуре - 1)
                monsterList2.objList.push_back(new Monster(x, y, scale, monsterList2.rotationAngle, speed, interval, 8));
                monsterList2.listSize++;
            }
        }


        // Общий вектор списков с монстрами
        VEC.push_back(&monsterList1);
        VEC.push_back(&monsterList2);


        // Бонусы
        {
            // Массив текстур, 1 шт
            // Порядок следования файлов должен совпадать с порядком бонусов в BonusEffects::Effects!
		    WCHAR *frames[] = {	L"../DirectX-11-Tutorial/data/bonus_Heal.png",
                                L"../DirectX-11-Tutorial/data/bonus_Freeze.png",
                                L"../DirectX-11-Tutorial/data/bonus_Shield.png",
                                L"../DirectX-11-Tutorial/data/bonus_Piercing.png",
                                L"../DirectX-11-Tutorial/data/bonus_Slow.png"
            };

            unsigned int framesNum = sizeof(frames) / sizeof(frames[0]);

            result = sprIns3->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, frames, framesNum, 45, 45);
            CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object for Bonuses.");

            bonusList1.spriteInst    = sprIns3;
            bonusList1.rotationAngle = 0.0f;

            for (int i = 0; i < framesNum; i++) {
                int       x = 50 + rand() % (scrWidth  - 100);
                int       y = 50 + rand() % (scrHeight - 100);

                bonusList1.objList.push_back(new Bonus(x, y, Bonus::Effects(i)));
                bonusList1.listSize++;
            }
        }


        // Оружие
        {
            // Массив текстур, 3 штук
		    WCHAR *frames[] = {	L"../DirectX-11-Tutorial/data/bonus_weapon_Pistol.png",
							    L"../DirectX-11-Tutorial/data/bonus_weapon_Rifle.png",
							    L"../DirectX-11-Tutorial/data/bonus_weapon_Shotgun.png"
		    };

            unsigned int framesNum = sizeof(frames) / sizeof(frames[0]);

            result = sprIns4->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, frames, framesNum, 30, 30);
            CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object.");

            weaponList1.spriteInst = sprIns4;
            weaponList1.listSize   = 0;


            for (int i = BonusWeapons::Weapons::PISTOL; i < BonusWeapons::Weapons::_lastWeapon; i++) {
                int x = 50 + rand() % (scrWidth  - 100);
                int y = 50 + rand() % (scrHeight - 100);

                weaponList1.objList.push_back(new Weapon(x, y, BonusWeapons::Weapons(i)));
                weaponList1.listSize++;
            }
        }


        // Игрок
        {
		    WCHAR *frames1[] = { L"../DirectX-11-Tutorial/data/tank_body_1.png" };
            result = m_PlayerBitmapIns1->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, frames1, 1, 40, 40);
            CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object for the Player.");

		    WCHAR *frames2[] = { L"../DirectX-11-Tutorial/data/tank_tower_1.png" };
            result = m_PlayerBitmapIns2->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, frames2, 1, 40, 40);
            CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object for the Player.");

		    m_Player = new Player(scrHalfWidth, scrHalfHeight, screenWidth/800, 90.0f, 5.0f, 1000, 1, appTimer);
            ((Player*)m_Player)->resetBulletsType();
            ((Player*)m_Player)->setEffect(Weapon::BonusWeapons::PISTOL);
//            ((Player*)m_Player)->setBulletsType_On(Player::BulletsType::PIERCING);
            ((Player*)m_Player)->setBulletsType_On(Player::BulletsType::ION);
        }


        // Пули
        {
            Bullet::setScrSize(screenWidth, screenHeight);
            //WCHAR *frames2[] = { L"../DirectX-11-Tutorial/data/bullet-red-icon-128.png" };
            WCHAR *frames2[] = { L"../DirectX-11-Tutorial/data/bullet-red-icon-128_blurred.png" };
            result = m_BulletBitmapIns->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, frames2, 1, 10, 10);
            CHECK_RESULT(hwnd, result, L"Could not initialize the instanced sprite object for the bullet.");
            // ??? если за время игры не была выпущена ни одна пуля, все крашится при выходе
            bulletList.push_back(new Bullet(-100, -100, 1.0f, -105, -105, 1.0, 0));
            bulletListSize++;
        }
	}


	// --- Cursor ---
	{
        SAFE_CREATE(m_Cursor, BitmapClass);
		result = m_Cursor->Initialize(m_d3d->GetDevice(), screenWidth, screenHeight, L"../DirectX-11-Tutorial/data/cursor.png", 24, 24);
        CHECK_RESULT(hwnd, result, L"Could not initialize the cursor object.");
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
// ------------------------------------------------------------------------------------------------------------------------



void GraphicsClass::Shutdown()
{
    // Игрок
    if (m_Player)
        SAFE_DELETE(m_Player);

    // Список пуль
    if( bulletList.size() > 0 ) {
        std::list<gameObjectBase*>::iterator iter = bulletList.begin(), end = bulletList.end();
        while (iter != end) {
            SAFE_DELETE(*iter);
            ++iter;
        }
    }

    // Список списков монстров
    if( VEC.size() ) {
    
        for (unsigned int i = 0; i < VEC.size(); i++) {

            std::list<gameObjectBase*> *list = &(VEC.at(i)->objList);

            if( list && list->size() ) {
            
                std::list<gameObjectBase*>::iterator iter = list->begin(), end = list->end();
                while (iter != end) {
                    SAFE_DELETE(*iter);
                    ++iter;
                }
            }
        }
    }

    SAFE_SHUTDOWN(m_PlayerBitmapIns1);
    SAFE_SHUTDOWN(m_PlayerBitmapIns2);
    SAFE_SHUTDOWN(m_BulletBitmapIns);
    SAFE_SHUTDOWN(sprIns1);
    SAFE_SHUTDOWN(sprIns2);
    SAFE_SHUTDOWN(sprIns3);
    SAFE_SHUTDOWN(sprIns4);

	// Release the Text object:
    SAFE_SHUTDOWN(m_TextOut);

	// Release the Bitmap objects:
    SAFE_SHUTDOWN(m_Bitmap_Bgr);
	SAFE_SHUTDOWN(m_Bitmap_Tree);
    SAFE_SHUTDOWN(m_BitmapIns);
    SAFE_SHUTDOWN(m_Cursor);

    // Release the light object:
    SAFE_DELETE(m_Light);

    // Release the Shader objects:
    //SAFE_SHUTDOWN(m_ColorShader);
    SAFE_SHUTDOWN(m_TextureShader);
    SAFE_SHUTDOWN(m_TextureShaderIns);
    SAFE_SHUTDOWN(m_LightShader);
    SAFE_SHUTDOWN(m_BulletShader);

	// Release the model object:
    SAFE_SHUTDOWN(m_Model);

	// Release the camera object:
    SAFE_DELETE(m_Camera);

    SAFE_DELETE(thPool);

    // Release d3d object:
    SAFE_SHUTDOWN(m_d3d);

	return;
}
// ------------------------------------------------------------------------------------------------------------------------



bool GraphicsClass::Frame(const int &fps, const int &cpu, const float &frameTime)
{
    // Set FPS, CPU usage and a text message:
	if( !m_TextOut->SetFps( fps, m_d3d->GetDeviceContext()) ||
        !m_TextOut->SetCpu( cpu, m_d3d->GetDeviceContext()) ||
        !m_TextOut->SetText(msg, m_d3d->GetDeviceContext())   )
        return false;

	return true;
}
// ------------------------------------------------------------------------------------------------------------------------



bool GraphicsClass::Render(const float &rotation, const float &zoom, const int &mouseX, const int &mouseY, const keysPressed *Keys, const bool &onTimer)
{
	bool result;

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

	return true;
}
// ------------------------------------------------------------------------------------------------------------------------



// --- 2d Rendering ---
bool GraphicsClass::Render2d(const float &rotation, const float &zoom, const int &mouseX, const int &mouseY, const keysPressed *Keys, const bool & onTimer)
{
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

	// задник
	{
		// Рендерим точно в центр
		if (!m_Bitmap_Bgr->Render(m_d3d->GetDeviceContext(), 0, 0) )
			return false;

		// Осуществляем необходимые преобразования матриц
		//D3DXMatrixTranslation(&matrixTranslation, 100.0f, 0.0f, 0.0f);
		//D3DXMatrixScaling(&matrixScaling, 1.0f + 0.01f*sin(rotation/10) + 0.0001f*zoom, 1.0f + 0.01f*sin(rotation/10) + 0.0001f*zoom, 1.0f);

		// Once the vertex / index buffers are prepared we draw them using the texture shader.
		// Notice we send in the matrixOrthographic instead of the projectionMatrix for rendering 2D.
		// Due note also that if your view matrix is changing you will need to create a default one for 2D rendering and use it instead of the regular view matrix.
		// In this tutorial using the regular view matrix is fine as the camera in this tutorial is stationary.

		// Рендерим битмап при помощи текстурного шейдера
		if ( !m_TextureShader->Render(m_d3d->GetDeviceContext(), m_Bitmap_Bgr->GetIndexCount(),
				matrixWorldZ * matrixTranslation * matrixScaling ,
					matrixView, matrixOrthographic, m_Bitmap_Bgr->GetTexture()) )
			return false;
	}

	// Вращающееся дерево - просто для красоты!
	{
		if (!m_Bitmap_Tree->Render(m_d3d->GetDeviceContext(), scrHalfWidth - 128, scrHalfHeight - 128))
			return false;

		D3DXMatrixRotationZ(&matrixWorldZ, rotation / 5);
		D3DXMatrixTranslation(&matrixTranslation, 100.0f, 100.0f, 0.0f);
		D3DXMatrixScaling(&matrixScaling, 0.85f + 0.03f*sin(rotation) + 0.0001f*zoom, 0.85f + 0.03f*sin(rotation) + 0.0001f*zoom, 1.0f);

		if ( !m_TextureShader->Render(m_d3d->GetDeviceContext(), m_Bitmap_Tree->GetIndexCount(),
				matrixWorldZ * matrixTranslation * matrixScaling,
					matrixView, matrixOrthographic, m_Bitmap_Tree->GetTexture()) )
			return false;
	}

	// Рендерим курсор
	{
		if (!m_Cursor->Render(m_d3d->GetDeviceContext(), mouseX, mouseY))
			return false;

		m_d3d->GetWorldMatrix(matrixWorldX);
		if( !m_TextureShader->Render(m_d3d->GetDeviceContext(), m_Cursor->GetIndexCount(),
				matrixWorldX, matrixView, matrixOrthographic, m_Cursor->GetTexture()) )
			return false;
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
		if ( !m_BitmapIns->Render(m_d3d->GetDeviceContext()) )
			return false;
#if 0
		D3DXMatrixRotationZ(&matrixWorldZ, rotation / 5);
		D3DXMatrixTranslation(&matTrans, 100.0f, 100.0f, 0.0f);
		D3DXMatrixScaling(&matScale, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 1.0f);
#endif

		// The Render function for the shader now requires the vertex and instance count from the model object.
		// Render the model using the texture shader.
		result = m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
					m_BitmapIns->GetVertexCount(), m_BitmapIns->GetInstanceCount(),
						matrixWorldZ * matrixTranslation * matScale,
							matrixView, matrixOrthographic, m_BitmapIns->GetTexture(), mouseX - xCenter, yCenter - mouseY);

		if (!result)
			return false;
	}
#endif




    // render sprites from vector using Instancing
    {
        // reset world matrices
        m_d3d->GetWorldMatrix(matrixWorldZ);
        m_d3d->GetWorldMatrix(matrixTranslation);
        m_d3d->GetWorldMatrix(matrixScaling);

        static float playerPosX = 0, playerPosY = 0;

        // не нужно пересчитывать и передавать на GPU большие буфера с каждым кадром, пусть они просчитываются в синхронизации с таймером, это добавит нам FPS
        if( onTimer ) {

            {
                gameTimer.Frame();

                char str[100] = "time passed:  ";
                char num_string[32];

                sprintf_s(num_string, "%f", gameTimer.GetTime());
                strcat_s(str, 100, num_string);

                logMsg(str);
            }

#if defined useSorting
            {
                monsterList1.objList.sort(sortPredicate<gameObjectBase>);
                monsterList2.objList.sort(sortPredicate<gameObjectBase>);

                gameTimer.Frame();

                char str[100] = "sorting took:  ";
                char num_string[32];

                sprintf_s(num_string, "%f", gameTimer.GetTime());
                strcat_s(str, 100, num_string);

                logMsg(str);
            }
#endif

            std::list<gameObjectBase*>::iterator iter, end;

            Player *player = (Player*)m_Player;

            // --- Player ---
            {
                player->setDirectionL(Keys->left );
                player->setDirectionR(Keys->right);
                player->setDirectionU(Keys->up   );
                player->setDirectionD(Keys->down );

                player->Move();

                // Получим текущие координаты игрока
                playerPosX = m_Player->getPosX();
                playerPosY = m_Player->getPosY();

                if (!m_PlayerBitmapIns1->initializeInstances(m_d3d->GetDevice(), m_Player))
                    return false;
                if (!m_PlayerBitmapIns2->initializeInstances(m_d3d->GetDevice(), m_Player))
                    return false;
            }

            // --- Bullets ---
            {
                #define BulletObj (*iter)

                // Если нажата левая кнопка мыши, добавляем в очередь новые пули в соответствии с типом оружия и прочими эффектами
                if ( Keys->lmbDown ) {

                    if( player->isWeaponReady() )
                        player->spawnBullet_Normal(mouseX, mouseY, &bulletList, bulletListSize);
                }

                // Просчитываем движение всех пуль
                iter = bulletList.begin();
                end  = bulletList.end();

                while (iter != end) {

                    if( BulletObj->isAlive() ) {
                        //BulletObj->Move(0, 0, &monsterList1);     // 1 список 
                        // ??? - поскольку начинаем просчет всегда с одного и того же списка, то все последующие списки имеют меньший шанс, чтобы быть застреленными
                        BulletObj->Move(0, 0, &VEC);                // вектор списков
                    }
                    else {
                        delete BulletObj;
                        iter = bulletList.erase(iter);
                        bulletListSize--;
                        continue;
                    }

                    ++iter;
                }
                thPool->waitForAll();   // Ждем, пока все потоки отработают до конца

                if (!m_BulletBitmapIns->initializeInstances(m_d3d->GetDevice(), &bulletList, &bulletListSize, true))
                    return false;

                {
                    gameTimer.Frame();

                    char str[100] = "bullets time: ";
                    char num_string[32];

                    sprintf_s(num_string, "%f", gameTimer.GetTime());
                    strcat_s(str, 100, num_string);
                    strcat_s(str, 100, "; bullets num: ");
                    sprintf_s(num_string, "%d", bulletListSize);
                    strcat_s(str, 100, num_string);
                    strcat_s(str, 100, "\n");

                    logMsg(str);
                }
            }

            // --- Monsters ---
            {
                #define MonsterObj (*iter)
                MonsterList *monsterList;
                for (unsigned int listNo = 0; listNo < VEC.size(); listNo++) {
                
                    monsterList = VEC.at(listNo);
                    iter = monsterList->objList.begin();
                    end  = monsterList->objList.end();

                    while (iter != end) {

                        if( MonsterObj->isAlive() ) {

                            MonsterObj->Move(playerPosX, playerPosY);

                        }
                        else {

                            // Spawn new Bonus in the place of killed Monster
                            if( !(rand() % BONUS_DROP_CHANCE) )
                            {
                                bonusList1.objList.push_back(new Bonus(MonsterObj->getPosX(), MonsterObj->getPosY(),
                                    Bonus::Effects(rand() % Bonus::Effects::_totalEffectsQty)));
                                bonusList1.listSize++;
                            }

                            delete MonsterObj;
                            iter = monsterList->objList.erase(iter);
                            monsterList->listSize--;

                            // add new monsters
#if 1
                            {
                                int x = rand() % scrWidth;
                                int y = rand() % scrHeight;

                                x += x % 2 ? scrWidth  : -scrWidth;
                                y += y % 2 ? scrHeight : -scrHeight;

                                float  speed = (rand() % 250 + 10) * 0.1f;
								float  scale = 0.5f + (rand() % 16) * 0.1f;
                                int interval = 50 / speed;

                                monsterList->objList.push_back(new Monster(x, y, scale, monsterList->rotationAngle, speed, interval, 9));
                                monsterList->listSize++;
                            }
#endif
                            continue;
                        }

                        ++iter;
                    }

                    if( !monsterList->spriteInst->initializeInstances(m_d3d->GetDevice(), &monsterList->objList, &monsterList->listSize) )
                        return false;
                }
            }

            // Bonuses
            {
                #define BonusObj (*iter) 
                if( bonusList1.listSize ) {

                    iter = bonusList1.objList.begin();
                    end  = bonusList1.objList.end();

                    while (iter != end) {

                        if( BonusObj->isAlive() ) {

                            BonusObj->Move(0, 0, m_Player);

                        }
                        else {

                            delete BonusObj;
                            iter = bonusList1.objList.erase(iter);
                            bonusList1.listSize--;
                            continue;
                        }

                        ++iter;
                    }

                    if( !bonusList1.spriteInst->initializeInstances(m_d3d->GetDevice(), &bonusList1.objList, &bonusList1.listSize) )
                        return false;
                }
            }

            // Weapons
            {
                #define WeaponObj (*iter) 
                if( weaponList1.listSize ) {

                    iter = weaponList1.objList.begin();
                    end  = weaponList1.objList.end();

                    while (iter != end) {

                        if( WeaponObj->isAlive() ) {

                            WeaponObj->Move(0, 0, m_Player);

                        }
                        else {

                            delete WeaponObj;
                            iter = weaponList1.objList.erase(iter);
                            weaponList1.listSize--;
                            continue;
                        }

                        ++iter;
                    }

                    if( !weaponList1.spriteInst->initializeInstances(m_d3d->GetDevice(), &weaponList1.objList, &weaponList1.listSize) )
                        return false;
                }
            }

        } // if-onTimer



        // --- Активируем модели спрайтов в GPU и затем отрисовываем все инстанции ---
        // ---------------------------------------------------------------------------

        // Monsters from all the lists
        {
            InstancedSprite *Sprite;
            for (unsigned int listNo = 0; listNo < VEC.size(); listNo++) {

                if( VEC.at(listNo)->listSize > 0 ) {

                    Sprite = VEC.at(listNo)->spriteInst;

                    if( !Sprite->Render(m_d3d->GetDeviceContext()) )
                        return false;

                    // Apply Matrices if needed
#if 0
                    D3DXMatrixRotationZ(&matrixWorldZ, rotation / 5);
                    D3DXMatrixTranslation(&matrixTranslation, 100.0f, 100.0f, 0.0f);
                    D3DXMatrixScaling(&matrixScaling, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 0.5f + 0.3*sin(rotation/5) + 0.0001*zoom, 1.0f);
                    D3DXMatrixScaling(&matrixScaling, 1.0f + 0.1*sin(rotation) + 0.1*zoom, 1.0f + 0.1*sin(rotation) + 0.1*zoom, 1.0f);
#endif

                    // Render the sprites using the texture shader
                    if( !m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
                            Sprite->GetVertexCount(), Sprite->GetInstanceCount(),
                                matrixWorldZ * matrixTranslation * matrixScaling,
                                    matrixView, matrixOrthographic, Sprite->GetTextureArray(), 1, playerPosX - scrHalfWidth, scrHalfHeight - playerPosY) )
                    return false;
                }
            }
        }

        // Player
        {
            // Корпус
            if( !m_PlayerBitmapIns1->Render(m_d3d->GetDeviceContext()) )
                return false;

            if( !m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
                    m_PlayerBitmapIns1->GetVertexCount(), m_PlayerBitmapIns1->GetInstanceCount(),
                        matrixWorldZ * matrixTranslation * matrixScaling,
                            matrixView, matrixOrthographic, m_PlayerBitmapIns1->GetTextureArray(), 0, mouseX - scrHalfWidth, scrHalfHeight - mouseY) )
            return false;

            // Башня
            if( !m_PlayerBitmapIns2->Render(m_d3d->GetDeviceContext()) )
                return false;

            if( !m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
                    m_PlayerBitmapIns2->GetVertexCount(), m_PlayerBitmapIns2->GetInstanceCount(),
                        matrixWorldZ * matrixTranslation * matrixScaling,
                            matrixView, matrixOrthographic, m_PlayerBitmapIns2->GetTextureArray(), 1, mouseX - scrHalfWidth, scrHalfHeight - mouseY) )
            return false;
        }

        // Bullets
        if( bulletListSize > 0 )
        {
            if( !m_BulletBitmapIns->Render(m_d3d->GetDeviceContext()) )
                return false;

            //if( !m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
            if( !m_BulletShader->Render(m_d3d->GetDeviceContext(),
                    m_BulletBitmapIns->GetVertexCount(), m_BulletBitmapIns->GetInstanceCount(),
                        matrixWorldZ * matrixTranslation * matrixScaling,
                            matrixView, matrixOrthographic, m_BulletBitmapIns->GetTextureArray(), 0, 0, 0) )
            return false;
        }
        _itoa_s(bulletListSize, buf, 100, 10);
        msg = buf;

        // Bonuses
        if( bonusList1.listSize > 0 )
        {
            if( !bonusList1.spriteInst->Render(m_d3d->GetDeviceContext()) )
                return false;

            if( !m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
                    bonusList1.spriteInst->GetVertexCount(), bonusList1.spriteInst->GetInstanceCount(),
                        matrixWorldZ * matrixTranslation * matrixScaling,
                            matrixView, matrixOrthographic, bonusList1.spriteInst->GetTextureArray(), 0, 0, 0) )
            return false;        
        }

        // Weapons
        if( weaponList1.listSize > 0 )
        {
            if( !sprIns4->Render(m_d3d->GetDeviceContext()) )
                return false;

            if( !m_TextureShaderIns->Render(m_d3d->GetDeviceContext(),
                    weaponList1.spriteInst->GetVertexCount(), weaponList1.spriteInst->GetInstanceCount(),
                        matrixWorldZ * matrixTranslation * matrixScaling,
                            matrixView, matrixOrthographic, weaponList1.spriteInst->GetTextureArray(), 0, 0, 0) )
            return false;        
        }

    }



	// text Out
	// We call the text object to render all its sentences to the screen here.
	// And just like with 2D images we disable the Z buffer before drawing and then enable it again after all the 2D has been drawn.

	// Render the text strings
	if( !m_TextOut->Render(m_d3d->GetDeviceContext(), matrixWorldX, matrixOrthographic) )
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
    // Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing
    m_Model->Render(m_d3d->GetDeviceContext());

	// Here we rotate the world matrix by the rotation value so that when we render the model using this updated world matrix
	D3DXMatrixRotationX  (&matrixWorldX, tan(0.1*rotation));
	D3DXMatrixRotationY  (&matrixWorldY, tan(0.1*rotation));
    D3DXMatrixTranslation(&matrixTranslation, 10.0f, 7.0f, 10.0f);

	// Если мы сначала умножаем на поворачивающие матрицы, а потом уже на сдвигающую, то повернутый объект правильно сдвигается в нужную точку
	// Если сначала поставить сдвигающую, то объект начинает бегать по экрану
	if( !m_LightShader->Render(m_d3d->GetDeviceContext(), m_Model->GetIndexCount(),
            matrixWorldX * matrixWorldY * matrixWorldZ * matrixTranslation,
		        matrixView, matrixProjection,
			        m_Model->GetTexture(),
				        m_Light->GetDirection(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(),
					        m_Camera->GetPosition(), m_Light->GetSpecularColor(), m_Light->GetSpecularPower() ) )
    return false;

    return true;
}
// ------------------------------------------------------------------------------------------------------------------------



//
void GraphicsClass::Sort(std::list<gameObjectBase*> *list)
{

}
// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------