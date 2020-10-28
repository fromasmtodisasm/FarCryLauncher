#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <Windows.h>
#define CRYGAME_EXPORTS
using namespace std;

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

#include "IConsole.h"
#include "IScriptSystem.h"
#include "IXGame.h"

std::string var = "r_Fullscreen";

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
	// Gets current supported CPU features flags. (CPUF_SSE, CPUF_SSE2, CPUF_3DNOW, CPUF_MMX)
	virtual int GetCPUFlags() = 0;

	// Get seconds per processor tick
	virtual double GetSecondsPerCycle() = 0;

	// dumps the memory usage statistics to the log
	virtual void DumpMemoryUsageStatistics() = 0;

	// Quit the appliacation
	virtual void	Quit() = 0;
	// Tells the system if it is relaunching or not
	virtual void	Relaunch(bool bRelaunch) = 0;
	// return true if the application is in the shutdown phase
	virtual bool	IsQuitting() = 0;

	// Display error message.
	// Logs it to console and file and error message box.
	// Then terminates execution.
	virtual void Error(const char* sFormat, ...) = 0;

	//DOC-IGNORE-BEGIN
	//[Timur] DEPRECATED! Use Validator Warning instead.
	// Display warning message.
	// Logs it to console and file and display a warning message box.
	// Not terminates execution.
	//__declspec(deprecated) virtual void Warning( const char *sFormat,... ) = 0;
	//DOC-IGNORE-END

	// Report warning to current Validator object.
	// Not terminates execution.
	virtual void Warning(int module, int severity, int flags, const char* file, const char* format, ...) = 0;
	// Compare specified verbosity level to the one currently set.
	virtual bool CheckLogVerbosity(int verbosity) = 0;

	// returns true if this is dedicated server application
	virtual bool IsDedicated() { return false; }

	// return the related subsystem interface
	virtual IGame* GetIGame() = 0;
	virtual INetwork* GetINetwork() = 0;
	virtual IRenderer* GetIRenderer() = 0;
	virtual IInput* GetIInput() = 0;
	virtual ITimer* GetITimer() = 0;
	virtual IConsole* GetIConsole() = 0;
	virtual IScriptSystem* GetIScriptSystem() = 0;
	virtual I3DEngine* GetI3DEngine() = 0;
	virtual ISoundSystem* GetISoundSystem() = 0;
	virtual IMusicSystem* GetIMusicSystem() = 0;
	virtual IPhysicalWorld* GetIPhysicalWorld() = 0;
	virtual IMovieSystem* GetIMovieSystem() = 0;
	virtual IAISystem* GetAISystem() = 0;
	virtual IMemoryManager* GetIMemoryManager() = 0;
	virtual IEntitySystem* GetIEntitySystem() = 0;
	virtual ICryFont* GetICryFont() = 0;
	virtual ICryPak* GetIPak() = 0;
	virtual ILog* GetILog() = 0;
	virtual IStreamEngine* GetStreamEngine() = 0;
	virtual ICryCharManager* GetIAnimationSystem() = 0;
	virtual IValidator* GetIValidator() = 0;
	virtual IFrameProfileSystem* GetIProfileSystem() = 0;

	virtual void DebugStats(bool checkpoint, bool leaks) = 0;
	virtual void DumpWinHeaps() = 0;
	virtual int DumpMMStats(bool log) = 0;

	//////////////////////////////////////////////////////////////////////////
	// @param bValue set to true when running on a cheat protected server or a client that is connected to it (not used in singlplayer)
	virtual void SetForceNonDevMode(const bool bValue) = 0;
	// @return is true when running on a cheat protected server or a client that is connected to it (not used in singlplayer)
	virtual bool GetForceNonDevMode() const = 0;
	virtual bool WasInDevMode() const = 0;
	virtual bool IsDevMode() const = 0;
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
		cout << "Process switch" << endl;
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

HWND g_HWND = 0;
LRESULT CALLBACK MainWndProc(
	HWND hwnd,        // handle to window
	UINT uMsg,        // message identifier
	WPARAM wParam,    // first message parameter
	LPARAM lParam)    // second message parameter
{

	switch (uMsg)
	{

	case WM_CREATE:
		char class_name[64];
		if (auto cn = GetClassName(hwnd, class_name, sizeof(class_name)); cn)
		{
			if (!strcmp(class_name, "CryENGINE"))
			{
				g_HWND = hwnd;
			}
			else
			{
				return 1;
			}

		}
		return 0;
	case WM_DESTROY:
		// Clean up window-specific data objects. 
		PostQuitMessage(0);
		return 0;

		// 
		// Process other messages. 
		// 

	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

//forward declarations.
//////////////////////////////////////////////////////////////////////
typedef void* WIN_HWND;
typedef void* WIN_HINSTANCE;
typedef void* WIN_HDC;
typedef void* WIN_HGLRC;

#include "IGame.h"

struct CGame : public IGame
{
	// Inherited via IGame
	virtual bool Init(ISystem* pSystem, bool bDedicatedSrv, bool bInEditor, const char* szGameMod) override
	{
		return true;
	}
	virtual bool Update() override
	{
		return true;
	}
	virtual bool Run(bool& bRelaunch) override
	{
		return false;
	}
	virtual const char* IsMODLoaded() override
	{
		return nullptr;
	}
	virtual IGameMods* GetModsInterface() override
	{
		return nullptr;
	}
	virtual void Release() override
	{
		delete this;
	}
	virtual void ExecuteScheduledEvents() override
	{
	}
	virtual bool UseFixedStep() override
	{
		return false;
	}
	virtual int SnapTime(float fTime, float fOffset = 0.5f) override
	{
		return 0;
	}
	virtual int SnapTime(int iTime, float fOffset = 0.5f) override
	{
		return 0;
	}
	virtual int GetiFixedStep() override
	{
		return 0;
	}
	virtual float GetFixedStep() override
	{
		return 0.0f;
	}
	virtual bool LoadLevelForEditor(const char* pszLevelDirectory, const char* pszMissionName = 0) override
	{
		return false;
	}
	virtual IEntityClassRegistry* GetClassRegistry() override
	{
		return nullptr;
	}
	virtual void OnSetVar(ICVar* pVar) override
	{
	}
	virtual void ResetState() override
	{
	}
	virtual void GetMemoryStatistics(ICrySizer* pSizer) override
	{
	}
	virtual void SaveConfiguration(const char* sSystemCfg, const char* sGameCfg, const char* sProfile) override
	{
	}
	virtual void ReloadScripts() override
	{
	}
	virtual bool GetModuleState(EGameCapability eCap) override
	{
		return false;
	}
	virtual IPhysicsStreamer* GetPhysicsStreamer() override
	{
		return nullptr;
	}
	virtual IPhysicsEventClient* GetPhysicsEventClient() override
	{
		return nullptr;
	}
	virtual void UpdateDuringLoading() override
	{
	}
	virtual IXAreaMgr* GetAreaManager() override
	{
		return nullptr;
	}
	virtual ITagPointManager* GetTagPointManager() override
	{
		return nullptr;
	}
	virtual void SendMessageA(const char* s)
	{
	}
};

CRYGAME_API IGame* CreateGameInstance()
{
	return new CGame();
}
// 02FC9F38 - scriptsystem interface
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
	//wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = "";
	wc.lpszClassName = "CryENGINE";

	if (!RegisterClass(&wc))
		return FALSE;
#if 0
	g_HWND = CreateWindow(wc.lpszClassName, "Инициализация Direct3D",
							 WS_OVERLAPPEDWINDOW, 200, 100,
							 500, 500, NULL, NULL, hInstance, NULL);
	startupParams.hWnd = (void*)g_HWND;
#endif

	auto L = LoadLibraryA("CrySystem.dll");
	if (L)
	{
		PFNCREATESYSTEMINTERFACE CreateSystemInterface = (PFNCREATESYSTEMINTERFACE)GetProcAddress(L, "CreateSystemInterface");
		if (auto system = CreateSystemInterface(startupParams); system)
		{
			puts("cxconsole");
			auto con = system->GetIConsole();
			auto rf = con->GetCVar(var.c_str());
			if (rf)
			{
				auto i = rf->GetIVal();
				auto f = rf->GetFVal();
				auto s = rf->GetString();
				rf->Refresh();
				rf->Set((int)1);
			}
			auto str = R"(setglobal( "r_Fullscreen", 1))";
			auto size = strlen(str);
			char sz[24] = { '1' };
			sz[sprintf(sz, "%d", size)] = 0;
			OutputDebugString(sz);
			system->GetIScriptSystem()->ExecuteBuffer(str, size);
			//system->GetIScriptSystem()->SetGlobalValue("r_Fullscreen", 0);

			puts("end_cxconsole");
			auto hwnd = system->GetIRenderer();
			SGameInitParams startupParams;
			startupParams.sGameDLL = "CryGame.dll";
			if (!system->CreateGame(startupParams))
			{
				cout << "lskdfj" << endl;
				return EXIT_FAILURE;

			}
			else
			{
				if (auto game = system->GetIGame(); game)
				{

					IScriptSystem* ss = system->GetIScriptSystem();
					auto fh = ss->GetScriptHandle();
					std::cout << "fuckyou" << std::endl;

					auto r = false;
					game->Run(r);
					system->Relaunch(true);
					system->Release();
				}
			}

		}
	}

	if (g_HWND)
	{
		DestroyWindow(g_HWND);
		UnregisterClass(wc.lpszClassName, NULL);
	}	return EXIT_FAILURE;
}

