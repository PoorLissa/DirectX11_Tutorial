#ifndef _SYSTEMCLASS_H_
#define _SYSTEMCLASS_H_

#define WIN32_LEAN_AND_MEAN
#define appTimerInterval 20

#include <windows.h>

#include "__directInput.h"
#include "__graphicsClass.h"
#include "__fpsClass.h"
#include "__cpuClass.h"
#include "__highPrecTimer.h"



class SystemClass {
 public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

 private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

 private:
	LPCWSTR	  m_applicationName;
	HINSTANCE m_hinstance;
	HWND	  m_hwnd;

	DirectInputClass *m_Input;
	GraphicsClass	 *m_Graphics;

	// Timer, FPS and CPU usage objects
	FpsClass			*m_Fps;
	CpuClass			*m_Cpu;
	HighPrecisionTimer	*m_Timer;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass *ApplicationHandle = 0;

#endif
