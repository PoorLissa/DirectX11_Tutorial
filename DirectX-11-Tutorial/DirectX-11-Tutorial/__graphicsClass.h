/*
	LINKS:

	--- Instancing ---
	http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter03.html	// it's rather about DirectX 9 than 11. Not sure if this fits me right.

	--- Sprite Atlas ---
	http://dallinwellington.com/rendering/sprite-atlas-rendering-with-dx-hlsl/

*/

#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include <stdio.h>
#include <vector>
#include <list>
#include <ctime>

#include "__d3dClass.h"
#include "__cameraClass.h"
#include "__modelClass.h"
#include "__colorShaderClass.h"
#include "__textureShaderClass.h"
#include "__lightShaderClass.h"
#include "__lightClass.h"
#include "__bitmapClass.h"
#include "__textOutClass.h"
#include "___Sprite.h"
#include "___SpriteInstanced.h"

#include "__bitmapClassInstancing.h"
#include "__textureShaderClassInstancing.h"

#include "Helpers.h"

// ---------------------------------------------------------------------------------------
#define fullScreen
#undef  fullScreen

#if defined fullScreen
const bool FULL_SCREEN    = true;
const int  windowedWidth  = 0;
const int  windowedHeight = 0;
#else
const bool FULL_SCREEN    = false;
const int  windowedWidth  = 800;
const int  windowedHeight = 600;
#endif

const bool	VSYNC_ENABLED = false;
const float SCREEN_DEPTH  = 1000.0f;
const float SCREEN_NEAR   = 0.1f;
// ---------------------------------------------------------------------------------------

struct keysPressed {
    bool up;
    bool down;
    bool left;
    bool right;
    bool lmbDown;
};



class GraphicsClass {

 public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass &);
   ~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(const int &, const int &, const float &);

	void logMsg(char *);

    bool Render(const float &, const float &, const int &, const int &, const keysPressed *, bool = false);

 private:
    int scrWidth;
    int scrHeight;

    d3dClass				*m_d3d;
    CameraClass			    *m_Camera;
    ModelClass				*m_Model;

    //ColorShaderClass		        *m_ColorShader;

    TextureShaderClass		        *m_TextureShader;
    TextureShaderClass_Instancing   *m_TextureShaderIns;
    TextureShaderClass_Instancing   *m_TextureShaderInsArr;

    LightShaderClass		*m_LightShader;
    LightClass				*m_Light;

    // We create a new private BitmapClass object here.
    BitmapClass			*m_Bitmap;
    BitmapClass			*m_Cursor;

    vector<Sprite*>		 m_spriteVec;
    BitmapClass			*m_BitmapSprite;

    // There is a new private variable for the TextClass object.
	TextOutClass			*m_TextOut;

	BitmapClass_Instancing	*m_BitmapIns;
    BitmapClass_Instancing	*m_BitmapInsArray;

    // Инстанцированный спрайт с анимацией
    InstancedSprite *sprIns1;
    InstancedSprite *sprIns2;
};

#endif
