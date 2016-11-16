#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include <stdio.h>
#include <vector>

#include "__d3dClass.h"
#include "__cameraClass.h"
#include "__modelClass.h"
#include "__colorShaderClass.h"
#include "__textureShaderClass.h"
#include "__lightShaderClass.h"
#include "__lightClass.h"
#include "__bitmapClass.h"
#include "__textOutClass.h"

#define fullScreen
#undef  fullScreen

#if defined fullScreen
	const bool	FULL_SCREEN   = true;
#else
	const bool	FULL_SCREEN   = false;
#endif

const bool	VSYNC_ENABLED = true;
const float SCREEN_DEPTH  = 1000.0f;
const float SCREEN_NEAR   = 0.1f;



class GraphicsClass {
 public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass &);
   ~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

	void logMsg(char *);

 private:
	//bool Render();
	 bool Render(float);

 private:
	 d3dClass				*m_d3d;
	 CameraClass			*m_Camera;
	 ModelClass				*m_Model;

	 //ColorShaderClass		*m_ColorShader;
	 TextureShaderClass		*m_TextureShader;

	 LightShaderClass		*m_LightShader;
	 LightClass				*m_Light;

	 // We create a new private BitmapClass object here.
	 BitmapClass			*m_Bitmap;
	 vector<BitmapClass*>	 m_BitmapVector;

	 struct PT { float X; float Y; };

	 vector<PT> m_coordsVec;

	// There is a new private variable for the TextClass object.
	TextOutClass			*m_TextOut;
};

#endif
