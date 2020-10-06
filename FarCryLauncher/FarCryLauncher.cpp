#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

struct ISystem;
struct ISystem;
struct ILog;
struct IEntitySystem;
struct IEntity;
struct ICryPak;
struct IKeyboard;
struct IMouse;
struct IConsole;
struct IInput;
struct IRenderer;
struct IConsole;
struct IProcess;
struct I3DEngine;
struct ITimer;
struct IGame;
struct IScriptSystem;
struct IAISystem;
struct IFlash;
struct INetwork;
struct ICryFont;
struct IMovieSystem;
class IPhysicalWorld;
struct IMemoryManager;
struct ISoundSystem;
struct IMusicSystem;
struct IFrameProfileSystem;
struct FrameProfiler;
struct IStreamEngine;
struct ICryCharManager;
struct SFileVersion;
struct IDataProbe;
struct ISystemUserCallback;
struct IValidator;

#define CRYSYSTEM_API __declspec(dllimport)

//////////////////////////////////////////////////////////////////////////
// Structure passed to Init method of ISystem interface.
struct SSystemInitParams
{
	void* hInstance;											//
	void* hWnd;														//
	char szSystemCmdLine[512];						// command line, used to execute the early commands e.g. -DEVMODE "g_gametype ASSAULT"
	ISystemUserCallback* pUserCallback;		//
	ILog* pLog;														// You can specify your own ILog to be used by System.
	IValidator* pValidator;								// You can specify different validator object to use by System.
	const char* sLogFileName;							// File name to use for log.
	bool bEditor;													// When runing in Editor mode.
	bool bPreview;												// When runing in Preview mode (Minimal initialization).
	bool bTestMode;												// When runing in Automated testing mode.
	bool bDedicatedServer;								// When runing a dedicated server.
	ISystem* pSystem;											// Pointer to existing ISystem interface, it will be reused if not NULL.

#if defined(LINUX)
	void (*pCheckFunc)(void*);							// authentication function (must be set).
#else
	void* pCheckFunc;											// authentication function (must be set).
#endif

	// Initialization defaults.
	SSystemInitParams()
	{
		hInstance = 0;
		hWnd = 0;
		memset(szSystemCmdLine, 0, sizeof(szSystemCmdLine));
		pLog = 0;
		pValidator = 0;
		pUserCallback = 0;
		sLogFileName = 0;
		bEditor = false;
		bPreview = false;
		bTestMode = false;
		bDedicatedServer = false;
		pSystem = 0;
		pCheckFunc = 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// Structure passed to CreateGame method of ISystem interface.
struct SGameInitParams
{
	const char* sGameDLL;							// Name of Game DLL. (Win32 Only)
	IGame* pGame;								// Pointer to already created game interface.
	bool					bDedicatedServer;			// When runing a dedicated server.
	char					szGameCmdLine[256];		// command line, used to execute the console commands after game creation e.g. -DEVMODE "g_gametype ASSAULT"

	SGameInitParams()
	{
		sGameDLL = NULL;
		pGame = NULL;
		bDedicatedServer = false;
		memset(szGameCmdLine, 0, 256);
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
// Main Engine Interface
// initialize and dispatch all engine's subsystems 
struct ISystem
{
	// Loads GameDLL and creates game instance.
	virtual bool CreateGame(const SGameInitParams& params) = 0;

	// Release ISystem.
	virtual void Release() = 0;

	// Update all subsystems (including the ScriptSink() )
	// @param flags one or more flags from ESystemUpdateFlags sructure.
	// @param boolean to set when in pause or cutscene mode in order to avoid
	// certain subsystem updates 0=menu/pause, 1=cutscene mode
	virtual bool Update(int updateFlags = 0, int nPauseMode = 0) = 0;

	// update _time, _frametime (useful after loading level to apply the time value)
	virtual void UpdateScriptSink() = 0;

	// Begin rendering frame.
	virtual void	RenderBegin() = 0;
	// Render subsystems.
	virtual void	Render() = 0;
	// End rendering frame and swap back buffer.
	virtual void	RenderEnd() = 0;

	// Renders the statistics; this is called from RenderEnd, but if the 
	// Host application (Editor) doesn't employ the Render cycle in ISystem,
	// it may call this method to render the essencial statistics
	virtual void RenderStatistics() = 0;

	// Retrieve the name of the user currently logged in to the computer
	virtual const char* GetUserName() = 0;
};


//////////////////////////////////////////////////////////////////////////
// CrySystem DLL Exports.
//////////////////////////////////////////////////////////////////////////
typedef ISystem* (*PFNCREATESYSTEMINTERFACE)(SSystemInitParams& initParams);

// Get the system interface (must be defined locally in each module)
extern ISystem* GetISystem();

// interface of the DLL
extern "C"
{
	CRYSYSTEM_API ISystem* CreateSystemInterface(SSystemInitParams& initParams);
}

//////////////////////////////////////////////////////////////////////////
// User defined callback, which can be passed to ISystem.
struct ISystemUserCallback
{
	/** Signals to User that engine error occured.
			@return true to Halt execution or false to ignore this error.
	*/
	virtual bool OnError(const char* szErrorString) = 0;
	/** If working in Editor environment notify user that engine want to Save current document.
			This happens if critical error have occured and engine gives a user way to save data and not lose it
			due to crash.
	*/
	virtual void OnSaveDocument() = 0;

	/** Notify user that system wants to switch out of current process.
			(For ex. Called when pressing ESC in game mode to go to Menu).
	*/
	virtual void OnProcessSwitch() = 0;
};

struct CSystemUserCallback : public ISystemUserCallback
{
	// Inherited via ISystemUserCallback
	virtual bool OnError(const char* szErrorString) override
	{
		cout << "ERROR: " << szErrorString << endl;
		return false;

	}
	virtual void OnSaveDocument() override
	{
	}
	virtual void OnProcessSwitch() override
	{
	}
};

// Timur.
// This is FarCry.exe authentication function, this code is not for public release!!
static void GameSystemAuthCheckFunction(void* data)
{
	// src and trg can be the same pointer (in place encryption)
	// len must be in bytes and must be multiple of 8 byts (64bits).
	// key is 128bit:  int key[4] = {n1,n2,n3,n4};
	// void encipher(unsigned int *const v,unsigned int *const w,const unsigned int *const k )
#define TEA_ENCODE(src, trg, len, key) {                                                                                      \
  unsigned int* v = (src), * w = (trg), * k = (key), nlen = (len) >> 3;                                                       \
  unsigned int delta = 0x9E3779B9, a = k[0], b = k[1], c = k[2], d = k[3];                                                    \
  while (nlen--) {                                                                                                            \
    unsigned int y = v[0], z = v[1], n = 32, sum = 0;                                                                         \
    while (n-- > 0) { sum += delta; y += (z << 4) + a ^ z + sum ^ (z >> 5) + b; z += (y << 4) + c ^ y + sum ^ (y >> 5) + d; } \
    w[0] = y; w[1] = z; v += 2, w += 2; }                                                                                     \
}

	// src and trg can be the same pointer (in place decryption)
	// len must be in bytes and must be multiple of 8 bytes (64bits).
	// key is 128bit: int key[4] = {n1,n2,n3,n4};
	// void decipher(unsigned int *const v,unsigned int *const w,const unsigned int *const k)
#define TEA_DECODE(src, trg, len, key) {                                                                                      \
  unsigned int* v = (src), * w = (trg), * k = (key), nlen = (len) >> 3;                                                       \
  unsigned int delta = 0x9E3779B9, a = k[0], b = k[1], c = k[2], d = k[3];                                                    \
  while (nlen--) {                                                                                                            \
    unsigned int y = v[0], z = v[1], sum = 0xC6EF3720, n = 32;                                                                \
    while (n-- > 0) { z -= (y << 4) + c ^ y + sum ^ (y >> 5) + d; y -= (z << 4) + a ^ z + sum ^ (z >> 5) + b; sum -= delta; } \
    w[0] = y; w[1] = z; v += 2, w += 2; }                                                                                     \
}

	// Data assumed to be 32 bytes.
	int key1[4] = { 1178362782, 223786232, 371615531, 90884141 };
	TEA_DECODE((unsigned int*)data, (unsigned int*)data, 32, (unsigned int*)key1);
	int key2[4] = { 89158165, 1389745433, 971685123, 785741042 };
	TEA_ENCODE((unsigned int*)data, (unsigned int*)data, 32, (unsigned int*)key2);
}

LRESULT CALLBACK MainWndProc(
	HWND hwnd,        // handle to window
	UINT uMsg,        // message identifier
	WPARAM wParam,    // first message parameter
	LPARAM lParam)    // second message parameter
{

	switch (uMsg)
	{
	case WM_CREATE:
		// Initialize the window. 
		return 0;

	case WM_PAINT:
		// Paint the window's client area. 
		return 0;

	case WM_SIZE:
		// Set the size and position of the window. 
		return 0;

	case WM_DESTROY:
		// Clean up window-specific data objects. 
		return 0;

		// 
		// Process other messages. 
		// 

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	CSystemUserCallback userCallback;
	SSystemInitParams startupParams;
	startupParams.sLogFileName = "Game.log";
	startupParams.hInstance = hInstance;
	startupParams.pUserCallback = &userCallback;
	startupParams.pCheckFunc = &GameSystemAuthCheckFunction;


	// Note: lpCmdLine does not contain the filename.
	string cmdLine = GetCommandLineA();
	strcpy(startupParams.szSystemCmdLine, cmdLine.c_str());

	WNDCLASS wc;

	// Register the main window class. 
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = "";
	wc.lpszClassName = "CryENGINE";

	if (!RegisterClass(&wc))
		return FALSE;

	auto L = LoadLibraryA("CrySystem.dll");
	if (L)
	{
		PFNCREATESYSTEMINTERFACE CreateSystemInterface = (PFNCREATESYSTEMINTERFACE)GetProcAddress(L, "CreateSystemInterface");
		if (auto system = CreateSystemInterface(startupParams); system)
		{
			SGameInitParams startupParams;
			startupParams.sGameDLL = "CryGame.dll";
			if (!system->CreateGame(startupParams))
				return EXIT_FAILURE;

		}
	}

	return EXIT_FAILURE;
}

