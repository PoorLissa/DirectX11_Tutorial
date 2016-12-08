// Precompiled Headers File, reduces compilation time
// https://habrahabr.ru/company/pvs-studio/blog/227521/
// -----------------------------------------------------------------------------------

#pragma once

// ��� ������� D3DCompileFromFile
#pragma comment(lib, "d3dcompiler.lib")

// Standard
#include <vector>
#include <list>
#include <iterator>
#include <fstream>
#include <ctime>
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>

// DirectX
#include <dinput.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dx11async.h>
#include <d3dx11tex.h>
#include <d3dcommon.h>
#include "d3dcompiler.h"
#include <d3dx10math.h>

// Project-related
#include "Helpers.h"

// -----------------------------------------------------------------------------------

// Let us put our global defines here for a time being...
#define appTimerInterval        20
#define EFFECT_DEFAULT_LENGTH   100
#define SLOW_EFFECT_FACTOR      5

#define useThread
//#undef  useThread         // don't use Threading
#define piercingBullets
//#undef  piercingBullets   // don't use Piercing Bullets
#define singleShot
#undef  singleShot          // don't use single shot bullets
