#include "Interfaces.h"
#include "Utilities.h"
#include "GlowManager.h"
#include "gameUI.hpp"
#include "IMDLCache.h"
#include "IGameUIFuncs.h"

typedef void* (__cdecl* CreateInterface_t)(const char*, int*);
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);

CreateInterface_t EngineFactory = NULL; 
CreateInterface_t ClientFactory = NULL;
CreateInterface_t VGUISurfaceFactory = NULL;
CreateInterface_t VGUI2Factory = NULL; 
CreateInterface_t MatFactory = NULL;
CreateInterface_t PhysFactory = NULL;
CreateInterface_t StdFactory = NULL;
CreateInterface_t InputSystemPointer = NULL;
CreateInterface_t CacheFactory = NULL;



void Interfaces::Initialise()
{
	EngineFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Engine, "CreateInterface");
	ClientFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Client, "CreateInterface");
	VGUI2Factory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::VGUI2, "CreateInterface");
	VGUISurfaceFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::VGUISurface, "CreateInterface");
	MatFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Material, "CreateInterface");
	PhysFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::VPhysics, "CreateInterface");
	StdFactory = (CreateInterface_t)GetProcAddress((HMODULE)Offsets::Modules::Stdlib, "CreateInterface");
	char* CHLClientInterfaceName = (char*)Utilities::Memory::FindTextPattern("client_panorama.dll", "VClient0");
	char* VGUI2PanelsInterfaceName = (char*)Utilities::Memory::FindTextPattern("vgui2.dll", "VGUI_Panel0");
	char* VGUISurfaceInterfaceName = (char*)Utilities::Memory::FindTextPattern("vguimatsurface.dll", "VGUI_Surface0");
	char* EntityListInterfaceName = (char*)Utilities::Memory::FindTextPattern("client_panorama.dll", "VClientEntityList0");
	char* EngineDebugThingInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VDebugOverlay0");
	char* EngineClientInterfaceName = (char*)Utilities::Memory::FindTextPattern("engine.dll","VEngineClient0");
	char* ClientPredictionInterface = (char*)Utilities::Memory::FindTextPattern("client_panorama.dll", "VClientPrediction0");
	char* MatSystemInterfaceName = (char*)Utilities::Memory::FindTextPattern("materialsystem.dll", "VMaterialSystem0");
	char* EngineRenderViewInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineRenderView0");
	char* EngineModelRenderInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineModel0");
	char* EngineModelInfoInterface = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VModelInfoClient0");
	char* EngineTraceInterfaceName = (char*)Utilities::Memory::FindTextPattern("engine.dll", "EngineTraceClient0");
	char* PhysPropsInterfaces = (char*)Utilities::Memory::FindTextPattern("client_panorama.dll", "VPhysicsSurfaceProps0");
	char* VEngineCvarName = (char*)Utilities::Memory::FindTextPattern("engine.dll", "VEngineCvar00");
	char* ModelCacheStuff = (char*)Utilities::Memory::FindTextPattern("datacache.dll", "MDLCache004");
	InputSystemPointer = (CreateInterface_t)GetProcAddress((HMODULE)Utilities::Memory::WaitOnModuleHandle("inputsystem.dll"), "CreateInterface");
	InputSystem = (IInputSystem*)InputSystemPointer("InputSystemVersion001", NULL);
	Client = (IBaseClientDLL*)ClientFactory(CHLClientInterfaceName, NULL);
	Engine = (IVEngineClient*)EngineFactory(EngineClientInterfaceName, NULL);
	Panels = (IPanel*)VGUI2Factory(VGUI2PanelsInterfaceName, NULL);
	Surface = (ISurface*)VGUISurfaceFactory(VGUISurfaceInterfaceName, NULL);
//	gameUI = (CGameUI*)g_InterfaceUtils->grabInterface("client_panorama.dll", "GameUI011");
	EntList = (IClientEntityList*)ClientFactory(EntityListInterfaceName, NULL);
	DebugOverlay = (IVDebugOverlay*)EngineFactory(EngineDebugThingInterface, NULL);
	Prediction = (DWORD*)ClientFactory(ClientPredictionInterface, NULL);
	MaterialSystem = (CMaterialSystem*)MatFactory(MatSystemInterfaceName, NULL);
	RenderView = (CVRenderView*)EngineFactory(EngineRenderViewInterface, NULL);
	ModelRender = (IVModelRender*)EngineFactory(EngineModelRenderInterface, NULL);
	ModelInfo = (CModelInfo*)EngineFactory(EngineModelInfoInterface, NULL);
	Trace = (IEngineTrace*)EngineFactory(EngineTraceInterfaceName, NULL);
	PhysProps = (IPhysicsSurfaceProps*)PhysFactory(PhysPropsInterfaces, NULL);
	CVar = (ICVar*)StdFactory(VEngineCvarName, NULL);
	EventManager = (IGameEventManager2*)EngineFactory("GAMEEVENTSMANAGER002", NULL);
//	MdlCache = (IMDLCache*)CacheFactory(ModelCacheStuff, NULL);
//	glow_manager = *(CGlowObjectManager**)(Utilities::Memory::FindPatternV2("client_panorama.dll", "0F 11 05 ? ? ? ? 83 C8 01") + 3);
	ClientMode = **(IClientModeShared***)((*(uintptr_t**)Client)[10] + 0x5);
	Globals = **reinterpret_cast< CGlobalVarsBase*** >((*reinterpret_cast< DWORD** >(Client))[0] + 0x1B);
	PDWORD pdwClientVMT = *(PDWORD*)Client;
	client_state = **(CClientState***)(Utilities::Memory::FindPatternV2("engine.dll", "A1 ? ? ? ? 8B 80 ? ? ? ? C3") + 1);
	pInput = *(CInput**)((*(DWORD**)Client)[16] + 0x1);
}

namespace Interfaces
{

	IInputSystem* InputSystem;
	IBaseClientDLL* Client;
	IVEngineClient* Engine;
	IPanel* Panels;
	CClientState* client_state;
	IClientEntityList* EntList;
	ISurface* Surface;
	IVDebugOverlay* DebugOverlay;
	IGameUIFuncs* gameuifuncs;
	IClientModeShared *ClientMode;
	CGlobalVarsBase *Globals;
//	CGameUI* gameUI;
	DWORD *Prediction;
	CMaterialSystem* MaterialSystem;
	CVRenderView* RenderView;
	IVModelRender* ModelRender;
	CModelInfo* ModelInfo;
	IEngineTrace* Trace;
	IPhysicsSurfaceProps* PhysProps;
	ICVar *CVar;
	CInput* pInput;
	IGameEventManager2 *EventManager;
	HANDLE __FNTHANDLE;
//	IMDLCache* MdlCache;
	IMoveHelper* MoveHelper;
	IPrediction *Prediction1;
	IGameMovement* GameMovement;
//	CGlowObjectManager* glow_manager;
};