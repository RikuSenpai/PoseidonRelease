#include "EdgyLagComp.h"
#include "backtracking.h"
#include "Backtracking Rage.h"
//#include "Resolver.h"
#include "Hooks.h"
#include "Hacks.h"
#include "Chams.h"
#include "Menu.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "MiscHacks.h"
#include "CRC32.h"
#include "hitmarker.h"
#include <intrin.h>
#include "MiscDefinitions.h"
#include "EnginePrediction.h"
#include "laggycompensation.h"
#include "lin_extp.h"
#include "GlowManager.h"
#include "SkinChanger.h"
#include "Singleton.hpp"
#include "IMDLCache.h"


CEsp c_esp;
LinearExtrapolations linear_extraps;
static CPredictionSystem* IPrediction = new CPredictionSystem();
beam * c_beam = new beam();
extern float lineLBY;
extern float lineRealAngle;
extern float lineFakeAngle;
Vector LastAngleAAReal;
Vector LBYThirdpersonAngle;

namespace GlobalBREAK
{
	bool bVisualAimbotting = false;
	QAngle vecVisualAimbotAngs = QAngle(0.f, 0.f, 0.f);
	int ChokeAmount = 0;
	float flFakewalked = 0.f;
	bool NewRound = false;
	bool WeaponFire = false;
	QAngle fakeangleslocal;
	bool bRainbowCross = true;
	bool dohitmarker;
	float LastTimeWeFired = 0;
	int ShotsFiredLocally = 0;
	int ShotsHitPerEntity[65];
	bool HeadShottedEntity[65] = { false };
	float curFov = 0;
	bool bUsingFakeAngles[65];
	float HitMarkerAlpha = 0.f;
	int TicksOnGround = 0;
	int ticks_while_unducked = 0;
	char* breakmode;
	int AnimationPitchFix = 0;
	float hitchance;
	int NextPredictedLBYUpdate = 0;
	int breakangle;
	int prevChoked = 0;
	bool AAFlip = false;
	bool LEFT;
	bool RIGHT;
	bool BACK;
	char my_documents_folder[MAX_PATH];
	float smt = 0.f;
	QAngle visualAngles = QAngle(0.f, 0.f, 0.f);
	bool bSendPacket = false;
	bool bAimbotting = false;
	CUserCmd* userCMD = nullptr;
	char* szLastFunction = "<No function was called>";
	HMODULE hmDll = nullptr;
	bool bFakewalking = false;
	Vector vecUnpredictedVel = Vector(0, 0, 0);
	float flFakeLatencyAmount = 0.f;
	float flEstFakeLatencyOnServer = 0.f;
	matrix3x4 traceHitboxbones[128];
	std::array<std::string, 64> resolverModes;
}


//DWORD GlowManager = *(DWORD*)(Utilities::Memory::FindPatternV2("client_panorama.dll", "0F 11 05 ?? ?? ?? ?? 83 C8 01 C7 05 ?? ?? ?? ?? 00 00 00 00") + 3);

#define MakePtr(cast, ptr, addValue) (cast)( (DWORD)(ptr) + (DWORD)(addValue))

#define TEXTURE_GROUP_LIGHTMAP                      "Lightmaps"
#define TEXTURE_GROUP_WORLD                         "World textures"
#define TEXTURE_GROUP_MODEL                         "Model textures"
#define TEXTURE_GROUP_VGUI                          "VGUI textures"
#define TEXTURE_GROUP_PARTICLE                      "Particle textures"
#define TEXTURE_GROUP_DECAL                         "Decal textures"
#define TEXTURE_GROUP_SKYBOX                        "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS                "ClientEffect textures"
#define TEXTURE_GROUP_OTHER                         "Other textures"
#define TEXTURE_GROUP_PRECACHED                     "Precached"
#define TEXTURE_GROUP_CUBE_MAP                      "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET                 "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED                   "Unaccounted textures"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER           "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP     "Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR    "Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD    "World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS   "Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER    "Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER          "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER         "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER                  "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL                    "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS                 "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS                "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE         "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS                 "Morph Targets"
#ifdef NDEBUG
#define strenc( s ) std::string( cx_make_encrypted_string( s ) )
#define charenc( s ) strenc( s ).c_str()
#define wstrenc( s ) std::wstring( strenc( s ).begin(), strenc( s ).end() )
#define wcharenc( s ) wstrenc( s ).c_str()
#else
#define strenc( s ) ( s )
#define charenc( s ) ( s )
#define wstrenc( s ) ( s )
#define wcharenc( s ) ( s )
#endif

#ifdef NDEBUG
#define XorStr( s ) ( XorCompileTime::XorString< sizeof( s ) - 1, __COUNTER__ >( s, std::make_index_sequence< sizeof( s ) - 1>() ).decrypt() )
#else
#define XorStr( s ) ( s )
#endif




int currentfov;
Vector LastAngleAA;
bool Resolver::didhitHS;
CUserCmd* Globals::UserCmd;
IClientEntity* Globals::Target;
int Globals::Shots;
bool Globals::change;
int Globals::TargetID;
std::map<int, QAngle>Globals::storedshit;
int Globals::missedshots;

typedef void(__thiscall* LockCursor)(void*);
typedef void(__thiscall* DrawModelEx_)(void*, void*, void*, const ModelRenderInfo_t&, matrix3x4*);
typedef void(__thiscall* PaintTraverse_)(PVOID, unsigned int, bool, bool);
typedef bool(__thiscall* InPrediction_)(PVOID);
typedef void(__stdcall *FrameStageNotifyFn)(ClientFrameStage_t);
typedef bool(__thiscall *FireEventClientSideFn)(PVOID, IGameEvent*);
typedef void(__thiscall* RenderViewFn)(void*, CViewSetup&, CViewSetup&, int, int);
using OverrideViewFn = void(__fastcall*)(void*, void*, CViewSetup*);
typedef float(__stdcall *oGetViewModelFOV)();
typedef void(__thiscall *SceneEnd_t)(void *pEcx);
typedef int(__thiscall* DoPostScreenEffects_t)(IClientModeShared*, int);
//typedef MDLHandle_t(__thiscall *_FindMDL)(void*, const char*);

PaintTraverse_ oPaintTraverse;
DrawModelEx_ oDrawModelExecute;
FrameStageNotifyFn oFrameStageNotify;
SceneEnd_t pSceneEnd;
DoPostScreenEffects_t o_DoPostScreenEffects;
OverrideViewFn oOverrideView;
FireEventClientSideFn oFireEventClientSide;
RenderViewFn oRenderView;
LockCursor oLockCursor;
//_FindMDL			oFindMDL;


void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce);
bool __stdcall Hooked_InPrediction();
bool __fastcall Hooked_FireEventClientSide(PVOID ECX, PVOID EDX, IGameEvent *Event);
void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld);
bool __stdcall CreateMoveClient_Hooked(float frametime, CUserCmd* pCmd);
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage);
void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup);
void __fastcall	hkSceneEnd(void *pEcx, void *pEdx);
//MDLHandle_t		__fastcall		hkFindMDL(void* ecx, void* edx, const char* FilePath);
float __stdcall GGetViewModelFOV();
int __stdcall Hooked_DoPostScreenEffects(int a1); 
void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw);
void __stdcall Hooked_LockCursor()
{
	bool xd = Menu::Window.m_bIsOpen;
	if (xd) {
		Interfaces::Surface->unlockcursor();
		return;
	}
	oLockCursor(Interfaces::Surface);
}

namespace Hooks
{
	Utilities::Memory::VMTManager VMTPanel;
	Utilities::Memory::VMTManager VMTClient;
	Utilities::Memory::VMTManager VMTClientMode;
	Utilities::Memory::VMTManager VMTModelRender;
	Utilities::Memory::VMTManager VMTPrediction;
	Utilities::Memory::VMTManager VMTRenderView;
	Utilities::Memory::VMTManager VMTDIRECTX;
	Utilities::Memory::VMTManager VMTEventManager;
//	Utilities::Memory::VMTManager VMTFindMdl;
};

void Hooks::UndoHooks()
{
	VMTPanel.RestoreOriginal();
	VMTPrediction.RestoreOriginal();
	VMTModelRender.RestoreOriginal();
	VMTDIRECTX.RestoreOriginal();
//	VMTFindMdl.RestoreOriginal();
	VMTClientMode.RestoreOriginal();
}

void Hooks::Initialise()
{
	VMTClientMode.Initialise((DWORD*)Interfaces::ClientMode);
	o_DoPostScreenEffects = (DoPostScreenEffects_t)VMTClientMode.HookMethod((DWORD)Hooked_DoPostScreenEffects, 44);
//	Interfaces::Engine->ExecuteClientCmd("clear");
//	Interfaces::CVar->ConsoleColorPrintf(Color(0, 255, 0, 255), ("\n poseidon.cc is injected \n"));
	VMTPanel.Initialise((DWORD*)Interfaces::Panels);
	oPaintTraverse = (PaintTraverse_)VMTPanel.HookMethod((DWORD)&PaintTraverse_Hooked, Offsets::VMT::Panel_PaintTraverse);
	VMTPrediction.Initialise((DWORD*)Interfaces::Prediction);
	VMTPrediction.HookMethod((DWORD)&Hooked_InPrediction, 14);
	VMTModelRender.Initialise((DWORD*)Interfaces::ModelRender);
	oDrawModelExecute = (DrawModelEx_)VMTModelRender.HookMethod((DWORD)&Hooked_DrawModelExecute, Offsets::VMT::ModelRender_DrawModelExecute);
	VMTClientMode.Initialise((DWORD*)Interfaces::ClientMode);
	VMTClientMode.HookMethod((DWORD)CreateMoveClient_Hooked, 24);
	pSceneEnd = (SceneEnd_t)VMTEventManager.HookMethod((DWORD)&hkSceneEnd, 9);
	oOverrideView = (OverrideViewFn)VMTClientMode.HookMethod((DWORD)&Hooked_OverrideView, 18);
	VMTClientMode.HookMethod((DWORD)&GGetViewModelFOV, 35);
	VMTClient.Initialise((DWORD*)Interfaces::Client);
	oFrameStageNotify = (FrameStageNotifyFn)VMTClient.HookMethod((DWORD)&Hooked_FrameStageNotify, 37);
	VMTEventManager.Initialise((DWORD*)Interfaces::EventManager);
	oFireEventClientSide = (FireEventClientSideFn)VMTEventManager.HookMethod((DWORD)&Hooked_FireEventClientSide, 9);
	VMTEventManager.Initialise((DWORD*)Interfaces::Surface);
	oLockCursor = (LockCursor)VMTEventManager.HookMethod((DWORD)Hooked_LockCursor, 67);
//	VMTFindMdl.Initialise((DWORD*)Interfaces::MdlCache);
//	oFindMDL = (_FindMDL)VMTFindMdl.HookMethod((DWORD)&hkFindMDL, 10);
	Interfaces::Engine->ClientCmd_Unrestricted("clear");
}

void MovementCorrection(CUserCmd* pCmd)
{

}

float clip(float n, float lower, float upper)
{

	return (std::max)(lower, (std::min)(n, upper));
}

int LagCompBreak() {
	IClientEntity *pLocalPlayer = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	Vector velocity = pLocalPlayer->GetVelocity();
	velocity.z = 0;
	float speed = velocity.Length();
	if (speed > 0.f) {
		auto distance_per_tick = speed *
			Interfaces::Globals->interval_per_tick;
		int choked_ticks = std::ceilf(65.f / distance_per_tick);
		return std::min<int>(choked_ticks, 14);
	}
	return 1;
}

BYTE bMoveData[0x200];
void Prediction(CUserCmd* pCmd, IClientEntity* LocalPlayer)
{

	if (Interfaces::MoveHelper && Menu::Window.RageBotTab.AimbotEnable.GetState() && LocalPlayer->IsAlive())
	{

		float curtime = Interfaces::Globals->curtime;
		float frametime = Interfaces::Globals->frametime;
		int iFlags = LocalPlayer->GetFlags();

		Interfaces::Globals->curtime = (float)LocalPlayer->GetTickBase() * Interfaces::Globals->interval_per_tick;
		Interfaces::Globals->frametime = Interfaces::Globals->interval_per_tick;
		Interfaces::MoveHelper->SetHost(LocalPlayer);

		Interfaces::Prediction1->SetupMove(LocalPlayer, pCmd, nullptr, bMoveData);
		Interfaces::GameMovement->ProcessMovement(LocalPlayer, bMoveData);
		Interfaces::Prediction1->FinishMove(LocalPlayer, pCmd, bMoveData);

		Interfaces::MoveHelper->SetHost(0);

		Interfaces::Globals->curtime = curtime;
		Interfaces::Globals->frametime = frametime;
		*LocalPlayer->GetPointerFlags() = iFlags;
	}
}

int kek = 0;
int autism = 0;

// Hook FindMDL to replace models.
/*MDLHandle_t __fastcall hkFindMDL(void* thisptr, void* edx, const char* FilePath)
{

	Removed because it was justins code (Penguware.xyz)

original_function:
	return oFindMDL(thisptr, FilePath);
}*/

int __stdcall Hooked_DoPostScreenEffects(int a1)
{

	return o_DoPostScreenEffects(Interfaces::ClientMode, a1);
}

void __fastcall  hkSceneEnd(void *pEcx, void *pEdx)
{
	Hooks::VMTRenderView.GetMethod<SceneEnd_t>(9)(pEcx);

	IClientEntity* local_player = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (!local_player || !Interfaces::Engine->IsInGame() || !Interfaces::Engine->IsConnected())
		return pSceneEnd(pEcx);

	pSceneEnd(pEcx);



	if (local_player)
	{
		for (int i = 1; i <= Interfaces::Globals->maxClients; i++)
		{
			auto ent = Interfaces::EntList->GetClientEntity(i);

			if (ent)
			{
				if (ent->GetClientClass()->m_ClassID != (int)CSGOClassID::CCSPlayer && ent->IsAlive() && !ent->IsDormant())
					continue;
				if (Menu::Window.VisualsTab.fakelag_ghost.GetIndex() != 0 && Interfaces::pInput->m_fCameraInThirdPerson)
				{
					float color[4] = { 0.8f, 0.8f, 0.8f, 0.6f };

					switch (Menu::Window.VisualsTab.fakelag_ghost.GetIndex())
					{
					case 1:
					{
						IMaterial * estrogen = Interfaces::MaterialSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL, true);
						if (!estrogen || estrogen->IsErrorMaterial() || !local_player || !local_player->IsAlive() || ent->GetVelocity().Length() < 0.5)
							return;

						Vector OrigAngle = local_player->GetAbsAngles_2();
						Vector OrigOrigin = local_player->GetAbsOrigin3();

						local_player->SetAbsOriginal(c_beam->cham_origin);
						local_player->SetAbsAngles(c_beam->cham_angle);

						Interfaces::RenderView->SetColorModulation(color);
						Interfaces::RenderView->SetBlend(0.3f);

						Interfaces::ModelRender->ForcedMaterialOverride(estrogen);

						local_player->draw_model(0x1, 255);
						Interfaces::ModelRender->ForcedMaterialOverride(nullptr);

						local_player->SetAbsAngles(OrigAngle);
						local_player->SetAbsOriginal(OrigOrigin);
					}
					break;

					case 2:
					{
						IMaterial * estrogen = Interfaces::MaterialSystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_OTHER, true);
						if (!estrogen || estrogen->IsErrorMaterial() || !local_player || !local_player->IsAlive())
							return;

						Vector OrigAngle = local_player->GetAbsAngles_2();
						Vector OrigOrigin = local_player->GetAbsOrigin3();

						local_player->SetAbsOriginal(c_beam->cham_origin);
						local_player->SetAbsAngles(c_beam->cham_angle);

						Interfaces::RenderView->SetColorModulation(color);
						Interfaces::RenderView->SetBlend(0.6f);

						Interfaces::ModelRender->ForcedMaterialOverride(estrogen);

						local_player->draw_model(0x1, 255);
						Interfaces::ModelRender->ForcedMaterialOverride(nullptr);

						local_player->SetAbsAngles(OrigAngle);
						local_player->SetAbsOriginal(OrigOrigin);
					}
					break;
					}
				}
			}
		}
	}
}


float NormalizeYaw(float value)
{
	while (value > 180)
		value -= 360.f;
	while (value < -180)
		value += 360.f;
	return value;
}

bool __stdcall CreateMoveClient_Hooked(float frametime, CUserCmd* pCmd)
{
	if (!pCmd->command_number)
		return true;

	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal)
	{


		PVOID pebp;
		__asm mov pebp, ebp;
		bool* pbSendPacket = (bool*)(*(DWORD*)pebp - 0x1C);
		bool& bSendPacket = *pbSendPacket;

		IPrediction->StartPrediction(pCmd);
		{
		//	CMBacktracking::Get().StartLagCompensation(pLocal);
		//	if (Menu::Window.RageBotTab.extrapolation.GetState())
		//		linear_extraps.run();

			Hacks::MoveHacks(pCmd, bSendPacket);
		}
		IPrediction->EndPrediction(pCmd);

		Vector origView = pCmd->viewangles;
		Vector viewforward, viewright, viewup, aimforward, aimright, aimup;
		Vector qAimAngles;
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);

		for (int i = 1; i < Interfaces::Engine->GetMaxClients(); i++) {
			IClientEntity* pBaseEntity = Interfaces::EntList->GetClientEntity(i);
			if (pBaseEntity && !pBaseEntity->IsDormant() && pBaseEntity != hackManager.pLocal()) globalsh.OldSimulationTime[i] = pBaseEntity->GetSimulationTime();
			if (pBaseEntity && !pBaseEntity->IsDormant() && pBaseEntity != hackManager.pLocal() && Menu::Window.RageBotTab.AccuracyBacktracking.GetState()) DataManager.UpdatePlayerPos();
		}

		IClientEntity* pEntityity;
		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && pLocal && pLocal->IsAlive())
		{
			Hacks::MoveHacks(pCmd, bSendPacket);
			ResolverSetup::GetInst().CM(pEntityity);
		}
		backtracking->legitBackTrack(pCmd, pLocal);

		if (Menu::Window.RageBotTab.AimbotEnable.GetState())
			pCmd->buttons |= IN_BULLRUSH;

		if (Menu::Window.MiscTab.FakeLagEnable.GetState())
		{
			static int ticks = 0;
			static int ticks1 = 0;
			static int iTick = 0;
			static int iTick1 = 0;
			static int iTick2 = 0;
			int ticksMax = 16;
			int value = Menu::Window.MiscTab.FakeLagChoke.GetValue();
			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 2)
			{
				if (ticks >= ticksMax)
				{
					bSendPacket = true;
					ticks = 0;
				}
				else
				{
					int packetsToChoke;
					if (pLocal->GetVelocity().Length() > 0.f)
					{
						packetsToChoke = (int)((128.f / Interfaces::Globals->interval_per_tick) / pLocal->GetVelocity().Length()) + 1;
						if (packetsToChoke >= 15)
							packetsToChoke = 14;
						if (packetsToChoke < value)
							packetsToChoke = value;
					}
					else
						packetsToChoke = 0;

					bSendPacket = ticks < 18 - packetsToChoke;;
				}
				ticks++;
			}

			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 4)
			{
				if (!(pLocal->GetFlags() & FL_ONGROUND))
				{
					if (ticks1 >= ticksMax)
					{
						bSendPacket = true;
						ticks1 = 0;
					}
					else
					{

						int packetsToChoke;
						if (pLocal->GetVelocity().Length() > 0.f)
						{
							packetsToChoke = (int)((128.f / Interfaces::Globals->interval_per_tick) / pLocal->GetVelocity().Length()) + 1;
							if (packetsToChoke >= 15)
								packetsToChoke = 14;
							if (packetsToChoke < value)
								packetsToChoke = value;
						}
						else
							packetsToChoke = 0;

						bSendPacket = ticks1 < 18 - packetsToChoke;;
					}
					ticks1++;
				}
			}
			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 1)
			{


				if (iTick < value) {
					bSendPacket = false;
					iTick++;
				}
				else {
					bSendPacket = true;
					iTick = 0;
				}
			}
			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 3)
			{

				if (!(pLocal->GetFlags() & FL_ONGROUND))
				{
					if (iTick1 < value) {
						bSendPacket = false;
						iTick1++;
					}
					else {
						bSendPacket = true;
						iTick1 = 0;
					}
				}
			}
			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 5)
			{

				value = LagCompBreak();
				if (iTick2 < value) {
					bSendPacket = false;
					iTick2++;
				}
				else {
					bSendPacket = true;
					iTick2 = 0;
				}

			}
			if (Menu::Window.MiscTab.FakeLagEnable.GetState() && value > 0 && Menu::Window.MiscTab.FakeLagTyp.GetIndex() == 6)
			{
				if (!pLocal)
					return;
				bool send_packet;
				int amount;
				float ExtrapolatedSpeed;
				int WishTicks;
				float VelocityY;
				float VelocityX;
				int WishTicks_1;
				signed int AdaptTicks;
				WishTicks = (amount + 0.5) - 1;

				VelocityY = pLocal->GetVelocity().y;
				VelocityX = pLocal->GetVelocity().x;
				WishTicks_1 = 0;
				AdaptTicks = 2;
				ExtrapolatedSpeed = sqrt((VelocityX * VelocityX) + (VelocityY * VelocityY))
					* Interfaces::Globals->interval_per_tick;

				while ((WishTicks_1 * ExtrapolatedSpeed) <= 68.0)
				{
					if (((AdaptTicks - 1) * ExtrapolatedSpeed) > 68.0)
					{
						++WishTicks_1;
						break;
					}
					if ((AdaptTicks * ExtrapolatedSpeed) > 68.0)
					{
						WishTicks_1 += 2;
						break;
					}
					if (((AdaptTicks + 1) * ExtrapolatedSpeed) > 68.0)
					{
						WishTicks_1 += 3;
						break;
					}
					if (((AdaptTicks + 2) * ExtrapolatedSpeed) > 68.0)
					{
						WishTicks_1 += 4;
						break;
					}
					AdaptTicks += 5;
					WishTicks_1 += 5;
					if (AdaptTicks > 16)
						break;
				}

				bool should_choke;

				if (*(int*)(uintptr_t(Interfaces::Client) + 0x4D28) < WishTicks_1 && *(int*)(uintptr_t(Interfaces::Client) + 0x4D28) < 15)
					should_choke = true;
				else
					should_choke = false;

				if (should_choke)
					send_packet = (WishTicks_1 <= *(int*)(uintptr_t(Interfaces::Client) + 0x4D28));

				else
					send_packet = (4 <= *(int*)(uintptr_t(Interfaces::Client) + 0x4D28));
			}
		}


		
		/*if (pCmd->buttons & IN_DUCK)  
			{
				static bool counter = false;
				static int counters = 0;
				if (counters == 9)
				{
					counters = 0;
					counter = !counter;
				}
				counters++;
				if (counter)
				{
					pCmd->buttons |= IN_DUCK;
					bSendPacket = true;
				}
				else
					pCmd->buttons &= ~IN_DUCK;
			}*/
		

		static bool abc = false;

		qAimAngles.Init(0.0f, GetAutostrafeView().y, 0.0f);
		AngleVectors(qAimAngles, &viewforward, &viewright, &viewup);
		qAimAngles.Init(0.0f, pCmd->viewangles.y, 0.0f);
		AngleVectors(qAimAngles, &aimforward, &aimright, &aimup);
		Vector vForwardNorm;		Normalize(viewforward, vForwardNorm);
		Vector vRightNorm;			Normalize(viewright, vRightNorm); 
		Vector vUpNorm;				Normalize(viewup, vUpNorm);

		float forward = pCmd->forwardmove;
		float right = pCmd->sidemove;
		float up = pCmd->upmove;
		if (forward > 450) forward = 450;
		if (right > 450) right = 450;
		if (up > 450) up = 450;
		if (forward < -450) forward = -450;
		if (right < -450) right = -450;
		if (up < -450) up = -450;
		pCmd->forwardmove = DotProduct(forward * vForwardNorm, aimforward) + DotProduct(right * vRightNorm, aimforward) + DotProduct(up * vUpNorm, aimforward);
		pCmd->sidemove = DotProduct(forward * vForwardNorm, aimright) + DotProduct(right * vRightNorm, aimright) + DotProduct(up * vUpNorm, aimright);
		pCmd->upmove = DotProduct(forward * vForwardNorm, aimup) + DotProduct(right * vRightNorm, aimup) + DotProduct(up * vUpNorm, aimup);

		if (Menu::Window.MiscTab.OtherSafeMode.GetState())
		{
			GameUtils::NormaliseViewAngle(pCmd->viewangles);

			if (pCmd->viewangles.z != 0.0f)
			{
				pCmd->viewangles.z = 0.00;
			}

			if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
			{
				Utilities::Log("Having to re-normalise!");
				GameUtils::NormaliseViewAngle(pCmd->viewangles);
				Beep(750, 800);
				if (pCmd->viewangles.x < -89 || pCmd->viewangles.x > 89 || pCmd->viewangles.y < -180 || pCmd->viewangles.y > 180)
				{
					pCmd->viewangles = origView;
					pCmd->sidemove = right;
					pCmd->forwardmove = forward;
				}
			}
		}

		if (pCmd->viewangles.x > 90)
		{
			pCmd->forwardmove = -pCmd->forwardmove;
		}

		if (pCmd->viewangles.x < -90)
		{
			pCmd->forwardmove = -pCmd->forwardmove;
		}
		// LBY
		LBYThirdpersonAngle = Vector(pLocal->GetEyeAnglesXY()->x, pLocal->GetLowerBodyYaw(), pLocal->GetEyeAnglesXY()->z);

		if (bSendPacket == true) {
			LastAngleAA = pCmd->viewangles;
		}
		else if (bSendPacket == false) {
			LastAngleAAReal = pCmd->viewangles;
		}

		lineLBY = pLocal->GetLowerBodyYaw();
		if (bSendPacket == true) {
			lineFakeAngle = pCmd->viewangles.y;
		}
		else if (bSendPacket == false) {
			lineRealAngle = pCmd->viewangles.y;
		}

	}
	return false;
}


void __fastcall PaintTraverse_Hooked(PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce)
{
	if (Menu::Window.VisualsTab.Active.GetState() && Menu::Window.VisualsTab.OtherNoScope.GetState() && strcmp("HudZoom", Interfaces::Panels->GetName(vguiPanel)) == 0)
		return;

	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	CUserCmd* cmdlist = *(CUserCmd**)((DWORD)Interfaces::pInput + 0xEC);
	CUserCmd* pCmd = cmdlist;
	RECT scrn = Render::GetViewport();

	static unsigned int FocusOverlayPanel = 0;
	static bool FoundPanel = false;

	if (!FoundPanel)
	{

		PCHAR szPanelName = (PCHAR)Interfaces::Panels->GetName(vguiPanel);
		if (strstr(szPanelName, XorStr("MatSystemTopPanel")))
		{

			FocusOverlayPanel = vguiPanel;
			FoundPanel = true;
		}

	}
	else if (FocusOverlayPanel == vguiPanel)
	{
		{
			//Render::Text(7, 7, Color(80, 140, 240, 255), Render::Fonts::ESP, ("poseidon.cc"));

			if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && Menu::Window.VisualsTab.Indicators.GetState())
			{

				IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
				RECT scrn = Render::GetViewport();
				bool breaklagcomp = false;
			
				int w, h;
				int centerW, centerh, topH;
				Interfaces::Engine->GetScreenSize(w, h);
				centerW = w / 2;
				centerh = h / 2;

				float lspeed;
				float pitchmeme;
				float inaccuracy;
				float lineLBY2;


				if (Menu::Window.VisualsTab.cheatinfo.GetState())
				{
					char jew[64];

					float blob = Interfaces::client_state->chokedcommands;





				}

				if (GetAsyncKeyState(Menu::Window.SettingsTab.minimalwalk.GetKey()))
				{
					Render::Text(9, scrn.bottom - 51, Color(0, 250, 30, 255), Render::Fonts::LBY, "SLOWWALK");
				}
				else
				{
					Render::Text(9, scrn.bottom - 51, Color(255, 0, 20, 255), Render::Fonts::LBY, "SLOWWALK");
				}

				if (Menu::Window.VisualsTab.Indicators.GetState())
				{
					Render::Text(9, scrn.bottom - 91, breaklagcomp ? Color(0, 255, 30, 255) : Color(255, 0, 30, 255), Render::Fonts::LBY, "LC");
				}
			}

		}

		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
			Hacks::DrawHacks();

		Menu::DoUIFrame();

		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && Menu::Window.VisualsTab.OtherHitmarker.GetState())
			hitmarker::singleton()->on_paint();
	}
	oPaintTraverse(pPanels, vguiPanel, forceRepaint, allowForce);
}

bool __stdcall Hooked_InPrediction()
{

	bool result;
	static InPrediction_ origFunc = (InPrediction_)Hooks::VMTPrediction.GetOriginalFunction(14);
	static DWORD *ecxVal = Interfaces::Prediction;
	result = origFunc(ecxVal);

	if (Menu::Window.VisualsTab.OtherNoVisualRecoil.GetState() && (DWORD)(_ReturnAddress()) == Offsets::Functions::dwCalcPlayerView)
	{

		IClientEntity* pLocalEntity = NULL;

		float* m_LocalViewAngles = NULL;

		__asm
		{
			MOV pLocalEntity, ESI
			MOV m_LocalViewAngles, EBX
		}

		Vector viewPunch = pLocalEntity->localPlayerExclusive()->GetViewPunchAngle();
		Vector aimPunch = pLocalEntity->localPlayerExclusive()->GetAimPunchAngle();

		m_LocalViewAngles[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		m_LocalViewAngles[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		m_LocalViewAngles[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
		return true;
	}
	return result;
}

player_info_t GetInfo(int Index) {
	player_info_t Info;
	Interfaces::Engine->GetPlayerInfo(Index, &Info);
	return Info;
}

typedef void(__cdecl* MsgFn)(const char* msg, va_list);
void Msg(const char* msg, ...)
{

	if (msg == nullptr)
		return; //If no string was passed, or it was null then don't do anything
	static MsgFn fn = (MsgFn)GetProcAddress(GetModuleHandle("tier0.dll"), "Msg"); //This gets the address of export "Msg" in the dll "tier0.dll". The static keyword means it's only called once and then isn't called again (but the variable is still there)
	char buffer[989];
	va_list list; //Normal varargs stuff http://stackoverflow.com/questions/10482960/varargs-to-printf-all-arguments
	va_start(list, msg);

	vsprintf(buffer, msg, list);
	va_end(list);

	fn(buffer, list); //Calls the function, we got the address above.
}

char* HitgroupToName(int hitgroup)
{
	switch (hitgroup)
	{
	case 1:
		return "Head";
	case 6:
		return "Arm";
	case 7:
		return "Leg";
	case 3:
		return "Stomach";
	default:
		return "Chest";
	}
};

int Kills2 = 0;
int Kills = 0;
bool RoundInfo = false;
size_t Delay = 0;
bool flipAA;
bool __fastcall Hooked_FireEventClientSide(PVOID ECX, PVOID EDX, IGameEvent *Event)
{

	std::string event_name = Event->GetName();
	if (!strcmp(Event->GetName(), "round_prestart"))
	{
	}

	if (!strcmp(Event->GetName(), "round_start"))
	{
	}
	if (!strcmp(Event->GetName(), "player_death"))
	{

		if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 2)
		{

			if (!strcmp(Event->GetName(), "player_hurt"))
			{

				int deadfag = Event->GetInt("userid");
				int attackingfag = Event->GetInt("attacker");
				IClientEntity* pLocal = hackManager.pLocal();
				if (Interfaces::Engine->GetPlayerForUserID(deadfag) != Interfaces::Engine->GetLocalPlayer() && Interfaces::Engine->GetPlayerForUserID(attackingfag) == Interfaces::Engine->GetLocalPlayer())
				{
					IClientEntity* hittedplayer = (IClientEntity*)(Interfaces::Engine->GetPlayerForUserID(deadfag));
					int hit = Event->GetInt("hitgroup");
					if (hit == 1 && hittedplayer && deadfag && attackingfag)
					{
						Resolver::didhitHS = true;
						Globals::missedshots = 0;
					}
					else
					{
						Resolver::didhitHS = false;
						Globals::missedshots++;
					}
				}
			}

		}
		if (!strcmp(Event->GetName(), "game_newmap"))
		{
		}
	}

	if (Menu::Window.RageBotTab.FlipAA.GetState())
	{

		if (!strcmp(Event->GetName(), "player_hurt"))
		{

			int deadfag = Event->GetInt("userid");
			int attackingfag = Event->GetInt("attacker");
			IClientEntity* pLocal = hackManager.pLocal();
			if (Interfaces::Engine->GetPlayerForUserID(deadfag) == Interfaces::Engine->GetLocalPlayer() && Interfaces::Engine->GetPlayerForUserID(attackingfag) != Interfaces::Engine->GetLocalPlayer())
			{

				flipAA = true;

			}
			else
			{

				flipAA = false;
			}

		}
	}
	if (Menu::Window.VisualsTab.Logs.GetState())
	{
		if (!strcmp(Event->GetName(), "player_hurt"))
		{

			int attackerid = Event->GetInt("attacker");
			int entityid = Interfaces::Engine->GetPlayerForUserID(attackerid);
			if (entityid == Interfaces::Engine->GetLocalPlayer())
			{

				int nUserID = Event->GetInt("attacker");
				int nDead = Event->GetInt("userid");
				if (nUserID || nDead)
				{

					player_info_t killed_info = GetInfo(Interfaces::Engine->GetPlayerForUserID(nDead));
					player_info_t killer_info = GetInfo(Interfaces::Engine->GetPlayerForUserID(nUserID));
					std::string before = ("[poseidon.cc] ");
					std::string two = ("Hit ");
					std::string three = killed_info.name;
					std::string four = (" in the ");
					std::string five = HitgroupToName(Event->GetInt("hitgroup"));
					std::string sixa = " for ";
					std::string sevena = Event->GetString("dmg_health");
					std::string damage = " damage";
					std::string sixb = " (";
					std::string sevenb = Event->GetString("health");
					std::string ate = " health remaining.)";
					std::string newline = "\n";
					if (Menu::Window.VisualsTab.Logs.GetState())
					{
						Msg((before + two + three + four + five + sixa + sevena + damage + sixb + sevenb + ate + newline).c_str());
					}

				}
			}

		}

	}

	return oFireEventClientSide(ECX, Event);
}

namespace index {
	constexpr auto ListLeavesInBox = 6;
}


struct RenderableInfo_t {
	IClientRenderable* m_pRenderable;
	void* m_pAlphaProperty;
	int m_EnumCount;
	int m_nRenderFrame;
	unsigned short m_FirstShadow;
	unsigned short m_LeafList;
	short m_Area;
	uint16_t m_Flags;   // 0x0016
	uint16_t m_Flags2; // 0x0018
	Vector m_vecBloatedAbsMins;
	Vector m_vecBloatedAbsMaxs;
	Vector m_vecAbsMins;
	Vector m_vecAbsMaxs;
	int pad;
};

#define MAX_COORD_FLOAT ( 16384.0f )
#define MIN_COORD_FLOAT ( -MAX_COORD_FLOAT )
int __fastcall hkListLeavesInBox(void* bsp, void* edx, Vector& mins, Vector& maxs, unsigned short* pList, int listMax) {
	typedef int(__thiscall * ListLeavesInBox)(void*, const Vector&, const Vector&, unsigned short*, int);


	// get current renderable info from stack ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L1470 )
	auto info = *(RenderableInfo_t**)((uintptr_t)_AddressOfReturnAddress() + 0x14);
	if (!info || !info->m_pRenderable)


	// fix render order, force translucent group ( https://www.unknowncheats.me/forum/2429206-post15.html )
	// AddRenderablesToRenderLists: https://i.imgur.com/hcg0NB5.png ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L2473 )
	info->m_Flags &= ~0x100;
	info->m_Flags2 |= 0xC0;

	// extend world space bounds to maximum ( https://github.com/pmrowla/hl2sdk-csgo/blob/master/game/client/clientleafsystem.cpp#L707 )
	static const Vector map_min = Vector(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
	static const Vector map_max = Vector(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);
	return 1;
}

void __fastcall Hooked_DrawModelExecute(void* thisptr, int edx, void* ctx, void* state, const ModelRenderInfo_t &pInfo, matrix3x4 *pCustomBoneToWorld)
{


	Color color;
	float flColor[3] = { 0.f };
	static IMaterial* CoveredLit = CreateMaterial(true);
	static IMaterial* OpenLit = CreateMaterial(false);
	static IMaterial* CoveredFlat = CreateMaterial(true, false);
	static IMaterial* OpenFlat = CreateMaterial(false, false);
	static IMaterial* Chrome = CreateMaterial("$envmap env_cube");
	bool DontDraw = false;

	if (Menu::Window.VisualsTab.Active.GetState())
	{
		const char* ModelName = Interfaces::ModelInfo->GetModelName((model_t*)pInfo.pModel);
		IClientEntity* pModelEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(pInfo.entity_index);
		IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		int ChamsStyle = Menu::Window.VisualsTab.OptionsChams.GetIndex();
		int HandsStyle = Menu::Window.VisualsTab.OtherNoHands.GetIndex();
	

		if (Menu::Window.VisualsTab.Active.GetState())
		{
			if (pLocal->IsScoped())
			{
				Interfaces::RenderView->SetBlend(0.3);
			}

		}

		if (Menu::Window.VisualsTab.Active.GetState() && strstr(ModelName, "models/player"))
		{
			if (pLocal && pModelEntity && ChamsStyle != 0)
			{

				IMaterial *material2 = Interfaces::MaterialSystem->FindMaterial("vgui/achievements/glow", TEXTURE_GROUP_OTHER);
				IMaterial *material1 = Interfaces::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER);

				if ((Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() && pModelEntity->GetTeamNum() != pLocal->GetTeamNum()))
				{


					IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
					IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;

					if (pModelEntity->IsAlive() && pModelEntity->GetHealth() > 0 /*&& pModelEntity->GetTeamNum() != local->GetTeamNum()*/)
					{
						float alpha = 1.f;

						if (pModelEntity->HasGunGameImmunity())
							alpha = 0.5f;

						if (pModelEntity->GetTeamNum() != pLocal->GetTeamNum())
						{
							flColor[0] = 60.f / 255.f;
							flColor[1] = 120.f / 255.f;
							flColor[2] = 180.f / 255.f;
						}
						else
						{
							flColor[0] = 60.f / 255.f;
							flColor[1] = 120.f / 255.f;
							flColor[2] = 180.f / 255.f;
						}

						if (Menu::Window.VisualsTab.OptionsChams.GetIndex() == 1 || Menu::Window.VisualsTab.OptionsChams.GetIndex() == 2 && !pLocal->IsScoped())
						{
							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(covered);
							oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
						}

						if (pModelEntity->GetTeamNum() == pLocal->GetTeamNum())
						{
							flColor[0] = 150.f / 255.f;
							flColor[1] = 200.f / 255.f;
							flColor[2] = 60.f / 255.f;
						}
						else
						{
							flColor[0] = 150.f / 255.f;
							flColor[1] = 200.f / 255.f;
							flColor[2] = 60.f / 255.f;
						}
						if (Menu::Window.VisualsTab.OptionsChams.GetIndex() == 3)
						{
							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(OpenLit);
						}
						else {
							Interfaces::RenderView->SetColorModulation(flColor);
							Interfaces::RenderView->SetBlend(alpha);
							Interfaces::ModelRender->ForcedMaterialOverride(open);
						}
					}
					else
					{
						color.SetColor(255, 255, 255, 255);
						ForceMaterial(color, open);
					}
				}
			}
		}
		else if (strstr(ModelName, "arms"))
		{
			/*
			models/player/ct_fbi/ct_fbi_glass - platinum
			models/inventory_items/cologne_prediction/cologne_prediction_glass - glass
			models/inventory_items/trophy_majors/crystal_clear - crystal
			models/inventory_items/trophy_majors/gold - gold
			models/gibs/glass/glass - dark chrome
			models/inventory_items/trophy_majors/gloss - plastic/glass
			vgui/achievements/glow - glow
			*/
			IMaterial *material = Interfaces::MaterialSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_OTHER);
			IMaterial *material1 = Interfaces::MaterialSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_OTHER);

			if (HandsStyle != 0 && pLocal && pLocal->IsAlive())
			{
				if (HandsStyle == 1)
				{
					IMaterial* Hands = Interfaces::MaterialSystem->FindMaterial(ModelName, "Model textures");
					Hands->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
					Interfaces::ModelRender->ForcedMaterialOverride(Hands);
				}
				else if (HandsStyle == 2)
				{
					DontDraw = true;
				}
				else if (HandsStyle == 3)
				{
					IMaterial *covered = ChamsStyle == 1 ? CoveredLit : CoveredFlat;
					IMaterial *open = ChamsStyle == 1 ? OpenLit : OpenFlat;
					if (pLocal)
					{
						if (pLocal->IsAlive())
						{
							int alpha = pLocal->HasGunGameImmunity() ? 150 : 255;

							if (pLocal->GetTeamNum() == 2)
								color.SetColor(185, 255, 70, alpha);
							else
								color.SetColor(185, 255, 70, alpha);

							ForceMaterial(color, covered);
							oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);

							if (pLocal->GetTeamNum() == 2)
								color.SetColor(185, 255, 70, alpha);
							else
								color.SetColor(185, 255, 70, alpha);
						}
						else
						{
							color.SetColor(185, 255, 70, 255);
						}

						ForceMaterial(color, covered);
					}
				}
				else
				{
					static int counter = 0;
					static float colors[3] = { 1.f, 0.f, 0.f };

					if (colors[counter] >= 1.0f)
					{
						colors[counter] = 1.0f;
						counter += 1;
						if (counter > 2)
							counter = 0;
					}
					else
					{
						int prev = counter - 1;
						if (prev < 0) prev = 2;
						colors[prev] -= 0.05f;
						colors[counter] += 0.05f;
					}

					Interfaces::RenderView->SetColorModulation(colors);
					Interfaces::RenderView->SetBlend(0.3);
					Interfaces::ModelRender->ForcedMaterialOverride(CoveredLit);
				}
			}
		}

		IClientEntity* local_player = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		if (local_player)
		{
			for (int i = 1; i <= Interfaces::Globals->maxClients; i++)
			{
				auto ent = Interfaces::EntList->GetClientEntity(i);

				if (ent)
				{
					if (ent->GetClientClass()->m_ClassID != (int)CSGOClassID::CCSPlayer && ent->IsAlive() && !ent->IsDormant())
						continue;
					if (Menu::Window.VisualsTab.fakelag_ghost.GetIndex() != 0 && Interfaces::pInput->m_fCameraInThirdPerson)
					{
						float color[4] = { 0.8f, 0.8f, 0.8f, 0.6f };

						switch (Menu::Window.VisualsTab.fakelag_ghost.GetIndex())
						{
						case 1:
						{
							IMaterial * estrogen = Interfaces::MaterialSystem->FindMaterial("debug/debugambientcube", TEXTURE_GROUP_MODEL, true);
							if (!estrogen || estrogen->IsErrorMaterial() || !local_player || !local_player->IsAlive() || ent->GetVelocity().Length() < 0.5)
								return;

							Vector OrigAngle = local_player->GetAbsAngles_2();
							Vector OrigOrigin = local_player->GetAbsOrigin3();

							local_player->SetAbsOriginal(c_beam->cham_origin);
							local_player->SetAbsAngles(c_beam->cham_angle);

							Interfaces::RenderView->SetColorModulation(color);
							Interfaces::RenderView->SetBlend(0.3f);

							Interfaces::ModelRender->ForcedMaterialOverride(estrogen);

							local_player->draw_model(0x1, 255);
							Interfaces::ModelRender->ForcedMaterialOverride(nullptr);

							local_player->SetAbsAngles(OrigAngle);
							local_player->SetAbsOriginal(OrigOrigin);
						}
						break;

						case 2:
						{
							IMaterial * estrogen = Interfaces::MaterialSystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_OTHER, true);
							if (!estrogen || estrogen->IsErrorMaterial() || !local_player || !local_player->IsAlive())
								return;

							Vector OrigAngle = local_player->GetAbsAngles_2();
							Vector OrigOrigin = local_player->GetAbsOrigin3();

							local_player->SetAbsOriginal(c_beam->cham_origin);
							local_player->SetAbsAngles(c_beam->cham_angle);

							Interfaces::RenderView->SetColorModulation(color);
							Interfaces::RenderView->SetBlend(0.6f);

							Interfaces::ModelRender->ForcedMaterialOverride(estrogen);

							local_player->draw_model(0x1, 255);
							Interfaces::ModelRender->ForcedMaterialOverride(nullptr);

							local_player->SetAbsAngles(OrigAngle);
							local_player->SetAbsOriginal(OrigOrigin);
						}
						break;
						}
					}
				}
			}
		}
	}

	if (!DontDraw)
		oDrawModelExecute(thisptr, ctx, state, pInfo, pCustomBoneToWorld);
	Interfaces::ModelRender->ForcedMaterialOverride(NULL);
}

int RandomInt(int min, int max)
{
	return rand() % max + min;

}

static std::vector<const char*> smoke_materials = {
        "particle/beam_smoke_01",
        "particle/particle_smokegrenade",
        "particle/particle_smokegrenade1",
        "particle/particle_smokegrenade2",
        "particle/particle_smokegrenade3",
        "particle/particle_smokegrenade_sc",
        "particle/smoke1/smoke1",
        "particle/smoke1/smoke1_ash",
        "particle/smoke1/smoke1_nearcull",
        "particle/smoke1/smoke1_nearcull2",
        "particle/smoke1/smoke1_snow",
        "particle/smokesprites_0001",
        "particle/smokestack",
        "particle/vistasmokev1/vistasmokev1",
        "particle/vistasmokev1/vistasmokev1_emods",
        "particle/vistasmokev1/vistasmokev1_nearcull",
        "particle/vistasmokev1/vistasmokev1_nearcull_fog",
        "particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
        "particle/vistasmokev1/vistasmokev1_smokegrenade",
        "particle/vistasmokev1/vistasmokev4_emods_nocull",
        "particle/vistasmokev1/vistasmokev4_nearcull",
        "particle/vistasmokev1/vistasmokev4_nocull"
 };

/*std::vector<const char*> smoke_materials =
{
	"particle/beam_smoke_01",
	"particle/particle_smokegrenade",
	"particle/particle_smokegrenade1",
	"particle/particle_smokegrenade2",
	"particle/particle_smokegrenade3",
	"particle/particle_smokegrenade_sc",
	"particle/smoke1/smoke1",
	"particle/smoke1/smoke1_ash",
	"particle/smoke1/smoke1_nearcull",
	"particle/smoke1/smoke1_nearcull2",
	"particle/smoke1/smoke1_snow",
	"particle/smokesprites_0001",
	"particle/smokestack",
	"particle/vistasmokev1/vistasmokev1",
	"particle/vistasmokev1/vistasmokev1_emods",
	"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	"particle/vistasmokev1/vistasmokev1_fire",
	"particle/vistasmokev1/vistasmokev1_nearcull",
	"particle/vistasmokev1/vistasmokev1_nearcull_fog",
	"particle/vistasmokev1/vistasmokev1_nearcull_nodepth",
	"particle/vistasmokev1/vistasmokev1_smokegrenade",
	"particle/vistasmokev1/vistasmokev4_emods_nocull",
	"particle/vistasmokev1/vistasmokev4_nearcull",
	"particle/vistasmokev1/vistasmokev4_nocull"
};*/


float Bolbilize(float Yaw)
{
	if (Yaw > 180)
	{
		Yaw -= (round(Yaw / 360) * 360.f);
	}
	else if (Yaw < -180)
	{
		Yaw += (round(Yaw / 360) * -360.f);
	}
	return Yaw;
}

Vector CalcAngle69(Vector dst, Vector src)
{
	Vector angles;

	double delta[3] = { (src.x - dst.x), (src.y - dst.y), (src.z - dst.z) };
	double hyp = sqrt(delta[1] * delta[1] + delta[2] * delta[2]);
	angles.x = (float)(atan(delta[2] / hyp) * 180.0 / 3.14159265);
	angles.y = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	angles.z = 0.0f;

	if (delta[0] >= 1.9)
	{
		angles.y += 180.0f;
	}

	return angles;
}
//Vector LastAngleAA;

namespace resolvokek
{
	int Shots;
	int missedshots;
	float RealAngle;
	float FakeAngle;
	Vector AimPoint;
	bool shouldflip;
	bool ySwitch;
	float NextTime;
	int resolvemode = 1;
	float fakeAngle;
	float OldSimulationTime[65];
	bool error;
}

auto smoke_count = *(DWORD*)(Utilities::Memory::FindPatternV2("client_panorama", "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0") + 0x8);
#define SETANGLE 180
void  __stdcall Hooked_FrameStageNotify(ClientFrameStage_t curStage)
{

	DWORD eyeangles = NetVar.GetNetVar(0xBFEA4E7B);
	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	IClientEntity* pEntityity = nullptr;

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_RENDER_START)
	{

		static bool rekt1 = false;
		if (Menu::Window.MiscTab.OtherThirdperson.GetState() && pLocal->IsAlive() && pLocal->IsScoped() == 0)
		{
			if (!rekt1)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				rekt1 = true;

			}
		}
		else if (!Menu::Window.MiscTab.OtherThirdperson.GetState())
		{
			rekt1 = false;

		}

		static bool rekt = false;
		if (!Menu::Window.MiscTab.OtherThirdperson.GetState() || pLocal->IsAlive() == 0)
		{
			if (!rekt)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("firstperson");
				rekt = true;

			}
		}
		else if (Menu::Window.MiscTab.OtherThirdperson.GetState() || pLocal->IsAlive() == 0)
		{
			rekt = false;
		}

		static bool meme = false;
		if (Menu::Window.MiscTab.OtherThirdperson.GetState() && pLocal->IsScoped() == 0)
		{
			if (!meme)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				meme = true;
			}
		}
		else if (pLocal->IsScoped())
		{

			meme = false;
		}

		static bool kek = false;
		if (Menu::Window.MiscTab.OtherThirdperson.GetState() && pLocal->IsAlive())
		{
			if (!kek)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				kek = true;
			}
		}
		else if (pLocal->IsAlive() == 0)
		{
			kek = false;
		}	if (pLocal->IsAlive() && Menu::Window.MiscTab.OtherThirdperson.GetState())
		{
			Vector thirdpersonMode;

			switch (Menu::Window.MiscTab.ThirdpersonAngle.GetIndex())
			{
			case 0:

				thirdpersonMode = LastAngleAAReal;
				break;
			case 1:
				thirdpersonMode = LastAngleAA;
				break;
			case 2:
				thirdpersonMode = LBYThirdpersonAngle;
				break;
			}

			static bool rekt = false;
			if (!rekt)
			{
				ConVar* sv_cheats = Interfaces::CVar->FindVar("sv_cheats");
				SpoofedConvar* sv_cheats_spoofed = new SpoofedConvar(sv_cheats);
				sv_cheats_spoofed->SetInt(1);
				rekt = true;
			}


			static bool kek = false;

			if (!kek)
			{
				Interfaces::Engine->ClientCmd_Unrestricted("thirdperson");
				kek = true;
			}

			static bool toggleThirdperson;
			static float memeTime;
			int ThirdPersonKey = Menu::Window.SettingsTab.ThirdPersonKeyBind.GetKey();
			if (ThirdPersonKey >= 0 && GUI.GetKeyState(ThirdPersonKey) && abs(memeTime - Interfaces::Globals->curtime) > 0.5)
			{
				toggleThirdperson = !toggleThirdperson;
				memeTime = Interfaces::Globals->curtime;
			}


			if (toggleThirdperson)
			{
				Interfaces::pInput->m_fCameraInThirdPerson = true;
				if (*(bool*)((DWORD)Interfaces::pInput + 0xAD))
					*(Vector*)((DWORD)pLocal + 0x31D8) = thirdpersonMode;
			}
			else {
				// No Thirdperson
				static Vector vecAngles;
				Interfaces::Engine->GetViewAngles(vecAngles);
				Interfaces::pInput->m_fCameraInThirdPerson = false;
				Interfaces::pInput->m_vecCameraOffset = Vector(vecAngles.x, vecAngles.y, 0);
			}


		}
		else if (pLocal->IsAlive() == 0)
		{

			kek = false;

		}

		if (!Menu::Window.MiscTab.OtherThirdperson.GetState()) {

			// No Thirdperson
			static Vector vecAngles;
			Interfaces::Engine->GetViewAngles(vecAngles);
			Interfaces::pInput->m_fCameraInThirdPerson = false;
			Interfaces::pInput->m_vecCameraOffset = Vector(vecAngles.x, vecAngles.y, 0);
		}

		static bool leo = false;
		if (Menu::Window.VisualsTab.Nightmode.GetState())
		{
			if (!leo)
			{
				ConVar* staticdrop = Interfaces::CVar->FindVar("r_DrawSpecificStaticProp");
				SpoofedConvar* staticdrop_spoofed = new SpoofedConvar(staticdrop);
				staticdrop_spoofed->SetInt(0);

				{
					for (MaterialHandle_t i = Interfaces::MaterialSystem->FirstMaterial(); i != Interfaces::MaterialSystem->InvalidMaterial(); i = Interfaces::MaterialSystem->NextMaterial(i))
					{
						IMaterial *pMaterial = Interfaces::MaterialSystem->GetMaterial(i);

						if (!pMaterial)
							continue;

						if (!strcmp(pMaterial->GetTextureGroupName(), "World textures"))
						{
							pMaterial->ColorModulation(0.1f, 0.1f, 0.1f);
						}
						if (!strcmp(pMaterial->GetTextureGroupName(), "StaticProp textures"))
						{
							pMaterial->ColorModulation(0.3f, 0.3f, 0.3f);
						}
					}
				}
			}
			leo = true;
			Menu::Window.VisualsTab.Nightmode.SetState(false);
		}
		else
		{
			leo = false;
		}

		if (curStage == FRAME_RENDER_START)
		{
			for (int i = 1; i <= Interfaces::Globals->maxClients; i++)
			{
				if (i == Interfaces::Engine->GetLocalPlayer()) continue;

				IClientEntity* pCurEntity = Interfaces::EntList->GetClientEntity(i);
				if (!pCurEntity) continue;

				*(int*)((uintptr_t)pCurEntity + 0xA30) = Interfaces::Globals->framecount;
				*(int*)((uintptr_t)pCurEntity + 0xA28) = 0;
			}
			if (Menu::Window.VisualsTab.OtherNoSmoke.GetState())
			{
				for (auto matName : smoke_materials)
				{
					IMaterial* mat = Interfaces::MaterialSystem->FindMaterial(matName, "Other textures");
					mat->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
					*(int*)smoke_count = 0;
				}
			}
		}

	}


	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		{
			if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 3)
			{
				resolvokek::resolvemode = 3;
			}


		}

		if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame() && curStage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{

			IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
			ResolverSetup::GetInst().FSN(pEntityity, curStage);

			if (!pLocal->m_bIsControllingBot()) {
				UINT *hWeapons = (UINT*)((DWORD)pLocal + 0x2DF8);
				if (hWeapons) {
					player_info_t pLocalInfo;
					Interfaces::Engine->GetPlayerInfo(Interfaces::Engine->GetLocalPlayer(), &pLocalInfo);
					for (int i = 0; hWeapons[i]; i++) {
						CBaseAttributableItem* pWeapon = (CBaseAttributableItem*)Interfaces::EntList->GetClientEntityFromHandle((HANDLE)hWeapons[i]);
						if (!pWeapon)
							continue;
						int nWeaponIndex = *pWeapon->GetItemDefinitionIndex();
						if (pLocalInfo.xuidlow != *pWeapon->GetOriginalOwnerXuidLow())
							continue;
						if (pLocalInfo.xuidhigh != *pWeapon->GetOriginalOwnerXuidHigh())
							continue;
						*pWeapon->GetAccountID() = pLocalInfo.xuidlow;
					}
				}
			}

			for (int i = 0; i <= Interfaces::EntList->GetHighestEntityIndex(); i++)
			{
				IClientEntity *pEntityity = Interfaces::EntList->GetClientEntity(i);

				if (pEntityity)
				{
					if (pEntityity == nullptr)
						return;

					ULONG hOwnerEntity = *(PULONG)((DWORD)pEntityity + 0x148);

					IClientEntity* pOwner = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)hOwnerEntity);

					if (pOwner)
					{

						if (pOwner == nullptr)
							return;

						if (pOwner == pLocal)
						{
							C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)pEntityity;

							ClientClass *pClass = Interfaces::Client->GetAllClasses();

						}
					}
				}

			}
		}
	}
	oFrameStageNotify(curStage);
}

void __fastcall Hooked_OverrideView(void* ecx, void* edx, CViewSetup* pSetup)
{

	IClientEntity* localplayer1 = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!localplayer1)
		return;

	if (!Interfaces::Engine->IsConnected())
		return;
	if (!Interfaces::Engine->IsInGame())
		return;

	if (localplayer1)
	{
		if (!localplayer1->IsScoped() && Menu::Window.VisualsTab.OtherFOV.GetValue() > 0)
			pSetup->fov += Menu::Window.VisualsTab.OtherFOV.GetValue();
	}
	currentfov = pSetup->fov;
	oOverrideView(ecx, edx, pSetup);
}

void GetViewModelFOV(float& fov)
{
	IClientEntity* localplayer = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (Interfaces::Engine->IsConnected() && Interfaces::Engine->IsInGame())
	{

		if (!localplayer)
			return;


		if (Menu::Window.VisualsTab.Active.GetState())
			fov += Menu::Window.VisualsTab.OtherViewmodelFOV.GetValue();
	}
}

float __stdcall GGetViewModelFOV()
{
	float fov = Hooks::VMTClientMode.GetMethod<oGetViewModelFOV>(35)();


	return fov;
}

void __fastcall Hooked_RenderView(void* ecx, void* edx, CViewSetup &setup, CViewSetup &hudViewSetup, int nClearFlags, int whatToDraw)
{
	static DWORD oRenderView = Hooks::VMTRenderView.GetOriginalFunction(6);

	IClientEntity* pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	__asm
	{
		PUSH whatToDraw
		PUSH nClearFlags
		PUSH hudViewSetup
		PUSH setup
		MOV ECX, ecx
		CALL oRenderView
	}
}