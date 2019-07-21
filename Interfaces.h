#pragma once

#include "CommonIncludes.h"
#include "SDK.h"
#include "gameUI.hpp"
#include "IGameUIFuncs.h"
#include "IMDLCache.h"

class IMoveHelper : public Interface
{
public:
	void SetHost(IClientEntity* pEntity)
	{
		typedef void(__thiscall* SetHostFn)(void*, IClientEntity*);
		return VFunc<SetHostFn>(1)(this, pEntity);
	}
};

class CMoveData {
public:
	bool m_bFirstRunOfFunctions : 1;
	bool m_bGameCodeMovedPlayer : 1;
	bool m_bNoAirControl : 1;

	unsigned long m_nPlayerHandle;
	int m_nImpulseCommand;
	QAngle m_vecViewAngles;
	QAngle m_vecAbsViewAngles;
	int m_nButtons;
	int m_nOldButtons;
	float m_flForwardMove;
	float m_flSideMove;
	float m_flUpMove;

	float m_flMaxSpeed;
	float m_flClientMaxSpeed;

	Vector m_vecVelocity;
	Vector m_vecOldVelocity;
	float somefloat;
	QAngle m_vecAngles;
	QAngle m_vecOldAngles;

	float m_outStepHeight;
	Vector m_outWishVel;
	Vector m_outJumpVel;

	Vector m_vecConstraintCenter;
	float m_flConstraintRadius;
	float m_flConstraintWidth;
	float m_flConstraintSpeedFactor;
	bool m_bConstraintPastRadius;

	void SetAbsOrigin(const Vector& vec);
	const Vector& GetOrigin() const;

private:
	Vector m_vecAbsOrigin;
};

class IPrediction : public Interface
{
public:
	void	SetupMove(IClientEntity *player, CUserCmd *pCmd, IMoveHelper *pHelper, void *move)
	{
		typedef void(__thiscall* SetupMoveFn)(void*, IClientEntity*, CUserCmd*, IMoveHelper*, void*);
		return VFunc<SetupMoveFn>(20)(this, player, pCmd, pHelper, move);
	}

	void	FinishMove(IClientEntity *player, CUserCmd *pCmd, void *move)
	{
		typedef void(__thiscall* FinishMoveFn)(void*, IClientEntity*, CUserCmd*, void*);
		return VFunc<FinishMoveFn>(21)(this, player, pCmd, move);
	}
};

class IGameMovement : public Interface
{
public:
	void ProcessMovement(IClientEntity *pPlayer, void *pMove)
	{
		typedef void(__thiscall* ProcessMovementFn)(void*, IClientEntity*, void*);
		return VFunc<ProcessMovementFn>(1)(this, pPlayer, pMove);
	}

	void StartTrackPredictionErrors(IClientEntity *pPlayer)
	{
		typedef void(__thiscall *o_StartTrackPredictionErrors)(void*, void*);
		return VFunc<o_StartTrackPredictionErrors>(3)(this, pPlayer);
	}

	void FinishTrackPredictionErrors(IClientEntity *pPlayer)
	{
		typedef void(__thiscall *o_FinishTrackPredictionErrors)(void*, void*);
		return VFunc<o_FinishTrackPredictionErrors>(4)(this, pPlayer);
	}
};

namespace Interfaces
{
	void peahjhekjwfheuwfhwe();
	void dwieohfeiwufhiwuef();
	void Initialise();

	extern IInputSystem* InputSystem;
	extern IBaseClientDLL* Client;
	extern IVEngineClient* Engine;
	extern CGameUI* gameUI;
	extern IPanel* Panels;
	extern IClientEntityList* EntList;
	extern ISurface* Surface;
	extern IVDebugOverlay* DebugOverlay;
	extern IClientModeShared *ClientMode;
	extern CGlobalVarsBase *Globals;
	extern DWORD *Prediction;
	extern CMaterialSystem* MaterialSystem;
	extern CClientState *client_state;
	extern CVRenderView* RenderView;
	extern IVModelRender* ModelRender;
	extern CModelInfo* ModelInfo;
	extern CGameUI* gameUI;
//	extern CGlowObjectManager* glow_manager;
	extern IEngineTrace* Trace;
	extern IPhysicsSurfaceProps* PhysProps;
//	extern IMDLCache* MdlCache;
	extern ICVar *CVar;
	extern CInput* pInput;
	extern IGameEventManager2 *EventManager;
	extern HANDLE __FNTHANDLE;
	extern IMoveHelper* MoveHelper;
	extern IPrediction *Prediction1;
	extern IGameMovement* GameMovement;
};