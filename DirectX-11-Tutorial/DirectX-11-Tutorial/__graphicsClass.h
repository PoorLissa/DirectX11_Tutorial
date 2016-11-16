#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include <stdio.h>

#include "__d3dClass.h"
#include "__cameraClass.h"
#include "__modelClass.h"
//#include "__colorShaderClass.h"
#include "__textureShaderClass.h"



const bool	FULL_SCREEN   = false;
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
	bool Render();

 private:
	 d3dClass			*m_d3d;
	 CameraClass		*m_Camera;
	 ModelClass			*m_Model;

	 //ColorShaderClass	*m_ColorShader;
	 TextureShaderClass	*m_TextureShader;
};

#endif
