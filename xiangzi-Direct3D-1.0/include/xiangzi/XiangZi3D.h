
#include <thread>
#include <windows.h>
#include <string>
#include <ctime>
#include <chrono>   
#include <vector>
#include <fstream>
#include <sstream>

#include <imm.h>  
#pragma comment (lib ,"imm32.lib")

#include<powrprof.h>
#pragma comment(lib,"powrprof.lib")

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace std;
using namespace std::chrono;
using namespace DirectX;

#define Pi 3.1415926535897932384626433832795028841971693993751059320781640628620899

#define PlaySound(name)	{	PlaySoundA(name, NULL, SND_ASYNC);	}
#define MessageBox(ch)	{	MessageBoxA(NULL, ch, "demo", MB_ICONEXCLAMATION | MB_OK);	}
#define pRelease(p) { if(p != NULL) p->Release(); p = NULL;}

#include "XiangZi3D/Timer.h"
#include "XiangZi3D/Key.h"
#include "XiangZi3D/Window.h"

#include "XiangZi3D/Color.h"
#include "XiangZi3D/Scene.h"
#include "XiangZi3D/Draw.h"

#include "XiangZi3D/Load.h"
#include "XiangZi3D/Texture.h"

#include "XiangZi3D/Base.h"

//#include "XiangZi3D/New.h"
//#include "XiangZi3D/List.h"
