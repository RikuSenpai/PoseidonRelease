#include "RageBot.h"
#include "RenderManager.h"
#include "Resolver.h"
#include "Autowall.h"
#include <iostream>
#include "UTIL Functions.h"
#include "Vector.h"
#include "Vector2D.h"
#include "Entities.h"
#include "MiscHacks.h"
#include "LegitBot.h"
#include <chrono>
#include "EdgyLagComp.h"

//Definitions--------------------------------------------------------------------------------------------------------
#define square( x ) ( x * x )


antiaim_helper * c_helper;
CMiscHacks * misc_hacks;
LegitAA * legitaa;

static bool jitter = false;
static bool jitter2 = false;


inline float FastSqrt(float x)
{
	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;
	return *(float*)&i;
}

void ClampMovement(CUserCmd* pCommand, float fMaxSpeed)
{
	if (fMaxSpeed <= 0.f)
		return;
	float fSpeed = (float)(FastSqrt(square(pCommand->forwardmove) + square(pCommand->sidemove) + square(pCommand->upmove)));
	if (fSpeed <= 0.f)
		return;
	if (pCommand->buttons & IN_DUCK)
		fMaxSpeed *= 2.94117647f;
	if (fSpeed <= fMaxSpeed)
		return;
	float fRatio = fMaxSpeed / fSpeed;
	pCommand->forwardmove *= fRatio;
	pCommand->sidemove *= fRatio;
	pCommand->upmove *= fRatio;
}

#define TICKS_TO_TIME(t) (Interfaces::Globals->interval_per_tick * (t))

#define RandomInt(min, max) (rand() % (max - min + 1) + min)
#define	MASK_ALL				(0xFFFFFFFF)
#define	MASK_SOLID				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE) 			/**< everything that is normally solid */
#define	MASK_PLAYERSOLID		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_PLAYERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE) 	/**< everything that blocks player movement */
#define	MASK_NPCSOLID			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTERCLIP|CONTENTS_WINDOW|CONTENTS_MONSTER|CONTENTS_GRATE) /**< blocks npc movement */
#define	MASK_WATER				(CONTENTS_WATER|CONTENTS_MOVEABLE|CONTENTS_SLIME) 							/**< water physics in these contents */
#define	MASK_OPAQUE				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_OPAQUE) 							/**< everything that blocks line of sight for AI, lighting, etc */
#define MASK_OPAQUE_AND_NPCS	(MASK_OPAQUE|CONTENTS_MONSTER)										/**< everything that blocks line of sight for AI, lighting, etc, but with monsters added. */
#define	MASK_VISIBLE			(MASK_OPAQUE|CONTENTS_IGNORE_NODRAW_OPAQUE) 								/**< everything that blocks line of sight for players */
#define MASK_VISIBLE_AND_NPCS	(MASK_OPAQUE_AND_NPCS|CONTENTS_IGNORE_NODRAW_OPAQUE) 							/**< everything that blocks line of sight for players, but with monsters added. */
#define	MASK_SHOT				(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_HITBOX) 	/**< bullets see these as solid */
#define MASK_SHOT_HULL			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_MONSTER|CONTENTS_WINDOW|CONTENTS_DEBRIS|CONTENTS_GRATE) 	/**< non-raycasted weapons see this as solid (includes grates) */
#define MASK_SHOT_PORTAL		(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW) 							/**< hits solids (not grates) and passes through everything else */
#define MASK_SHOT_BRUSHONLY			(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_DEBRIS) // non-raycasted weapons see this as solid (includes grates)
#define MASK_SOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_GRATE) 					/**< everything normally solid, except monsters (world+brush only) */
#define MASK_PLAYERSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_PLAYERCLIP|CONTENTS_GRATE) 			/**< everything normally solid for player movement, except monsters (world+brush only) */
#define MASK_NPCSOLID_BRUSHONLY	(CONTENTS_SOLID|CONTENTS_MOVEABLE|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE) 			/**< everything normally solid for npc movement, except monsters (world+brush only) */
#define MASK_NPCWORLDSTATIC		(CONTENTS_SOLID|CONTENTS_WINDOW|CONTENTS_MONSTERCLIP|CONTENTS_GRATE) 					/**< just the world, used for route rebuilding */
#define MASK_SPLITAREAPORTAL	(CONTENTS_WATER|CONTENTS_SLIME) 		

//BackTrack* backtracking = new BackTrack();

inline float RandomFloat(float min, float max)
{
	static auto fn = (decltype(&RandomFloat))(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
	return fn(min, max);
}

int GetFPS()
{
	static int fps = 0;
	static int count = 0;
	using namespace std::chrono;
	auto now = high_resolution_clock::now();
	static auto last = high_resolution_clock::now();
	count++;

	if (duration_cast<milliseconds>(now - last).count() > 1000)
	{
		fps = count;
		count = 0;
		last = now;
	}

	return fps;
}

static bool dir = false;
static bool back = false;
static bool up = false;

float stand_range = Menu::Window.RageBotTab.desync_range_stand.GetValue();
float move_range = Menu::Window.RageBotTab.desync_range_move.GetValue();


void NormalizeNum(Vector &vIn, Vector &vOut)
{
	float flLen = vIn.Length();
	if (flLen == 0) {
		vOut.Init(0, 0, 1);
		return;
	}
	flLen = 1 / flLen;
	vOut.Init(vIn.x * flLen, vIn.y * flLen, vIn.z * flLen);
}

float antiaim_helper::fov_ent(Vector ViewOffSet, Vector View, IClientEntity* entity, int hitbox)
{
	const float MaxDegrees = 180.0f;
	Vector Angles = View, Origin = ViewOffSet;
	Vector Delta(0, 0, 0), Forward(0, 0, 0);
	Vector AimPos = GetHitboxPosition(entity, hitbox);
	AngleVectors(Angles, &Forward);
	VectorSubtract(AimPos, Origin, Delta);
	NormalizeNum(Delta, Delta);
	float DotProduct = Forward.Dot(Delta);
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int antiaim_helper::closest()
{
	
	int closest_entity;
	int index = -1;
	float lowest_fov = 180.f;

	IClientEntity* local_player = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (!local_player)
		return -1;

	if (!local_player->IsAlive())
		return -1;

	Vector local_position = local_player->GetAbsOrigin() + local_player->GetViewOffset();
	Vector angles;
	Interfaces::Engine->GetViewAngles(angles);
	for (int i = 1; i <= Interfaces::Globals->maxClients; i++)
	{
		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		if (!entity || entity->GetHealth() <= 0 || entity->GetTeamNum() == local_player->GetTeamNum() || entity->IsDormant() || entity == local_player)
			continue;

		float fov = fov_ent(local_position, angles, entity, 0);
		if (fov < lowest_fov)
		{
			lowest_fov = fov;
			index = i;
			closest_entity = i; //crashes
		}

	}
	
	return index;

}

Vector hitbox_location(IClientEntity* obj, int hitbox_id)
{
	matrix3x4 bone_matrix[128];

	if (obj->SetupBones(bone_matrix, 128, 0x00000100, 0.0f)) {
		if (obj->GetModel()) {
			auto studio_model = Interfaces::ModelInfo->GetStudiomodel(obj->GetModel());
			if (studio_model) {
				auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
				if (hitbox) {
					auto min = Vector{}, max = Vector{};

					VectorTransform(hitbox->bbmin, bone_matrix[hitbox->bone], min);
					VectorTransform(hitbox->bbmax, bone_matrix[hitbox->bone], max);

					return (min + max) / 2.0f;
				}
			}
		}
	}
	return Vector{};
}

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
	CTraceFilterSkipTwoEntities(void *pPassEnt1, void *pPassEnt2)
	{
		passentity1 = pPassEnt1;
		passentity2 = pPassEnt2;
	}

	bool ShouldHitEntity(IClientEntity* pEntityityHandle, int contentsMask)
	{
		return !(pEntityityHandle == passentity1 || pEntityityHandle == passentity2);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void *passentity1;
	void *passentity2;
};

bool CRageBot::enemy_is_slow_walking(IClientEntity * entity)
{
	C_BaseCombatWeapon* weapon = entity->GetWeapon2();
	if (!weapon)
		return false;

	float speed = entity->GetVelocity().Length2D();
	float get_gun = misc_hacks->get_gun(weapon);

	if (speed - get_gun < 34) // if it's more or less the same.
	{
		return true;
	}
	return false;
}

//CRageBot * c_ragebot;


//Definition End --------------------------------------------------------------------------------------------------------

void CRageBot::Init()
{

	IsAimStepping = false;
	IsLocked = false;
	TargetID = -1;
}

void CRageBot::Draw()
{

}

bool IsAbleToShoot(IClientEntity* pLocal)
{
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (!pLocal)return false;
	if (!pWeapon)return false;
	float flServerTime = pLocal->GetTickBase() * Interfaces::Globals->interval_per_tick;
	return (!(pWeapon->GetNextPrimaryAttack() > flServerTime));
}

float hitchance(IClientEntity* pLocal, C_BaseCombatWeapon* pWeapon)
{
	float hitchance = 101;
	if (!pWeapon) return 0;
	if (Menu::Window.RageBotTab.AccuracyHitchance.GetValue() > 1)
	{
		float inaccuracy = pWeapon->GetInaccuracy();
		if (inaccuracy == 0) inaccuracy = 0.0000001;
		inaccuracy = 1 / inaccuracy;
		hitchance = inaccuracy;
	}
	return hitchance;
}

bool CanOpenFire() 
{
	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	if (!pLocalEntity)
		return false;

	C_BaseCombatWeapon* entwep = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocalEntity->GetActiveWeaponHandle());

	float flServerTime = (float)pLocalEntity->GetTickBase() * Interfaces::Globals->interval_per_tick;
	float flNextPrimaryAttack = entwep->GetNextPrimaryAttack();

	std::cout << flServerTime << " " << flNextPrimaryAttack << std::endl;

	return !(flNextPrimaryAttack > flServerTime);
}

template<class T, class U>
inline T clamp(T in, U low, U high)
{
	if (in <= low)
		return low;
	else if (in >= high)
		return high;
	else
		return in;
}

float InterpolationFix()
{
	int ud_rate = Interfaces::CVar->FindVar("cl_updaterate")->GetFloat();
	ConVar *min_ud_rate = Interfaces::CVar->FindVar("sv_minupdaterate");
	ConVar *max_ud_rate = Interfaces::CVar->FindVar("sv_maxupdaterate");
	if (min_ud_rate && max_ud_rate)
		ud_rate = max_ud_rate->GetFloat();
	float ratio = Interfaces::CVar->FindVar("cl_interp_ratio")->GetFloat();
	if (ratio == 0)
		ratio = 1.0f;
	float lerp = Interfaces::CVar->FindVar("cl_interp")->GetFloat();
	ConVar *c_min_ratio = Interfaces::CVar->FindVar("sv_client_min_interp_ratio");
	ConVar *c_max_ratio = Interfaces::CVar->FindVar("sv_client_max_interp_ratio");
	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1)
		ratio = clamp(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());
	return max(lerp, (ratio / ud_rate));
}

void CRageBot::Move(CUserCmd *pCmd, bool &bSendPacket)
{

	IClientEntity* pLocalEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (!pLocalEntity || !Menu::Window.RageBotTab.Active.GetState() || !Interfaces::Engine->IsConnected() || !Interfaces::Engine->IsInGame())
		return;

	if (Menu::Window.RageBotTab.AntiAimEnable.GetState())
	{
		static int ChokedPackets = -1;

		C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
		if (!pWeapon)
			return;

		if (ChokedPackets < 1 && pLocalEntity->GetLifeState() == LIFE_ALIVE && pCmd->buttons & IN_ATTACK && CanOpenFire() && GameUtils::IsBallisticWeapon(pWeapon))
		{
			bSendPacket = false;
		}
		else
		{
			if (pLocalEntity->GetLifeState() == LIFE_ALIVE)
			{
				DoAntiAim(pCmd, bSendPacket);

			}
			ChokedPackets = -1;
		}
	}

	if (Menu::Window.RageBotTab.AimbotEnable.GetState())
		DoAimbot(pCmd, bSendPacket);

	if (Menu::Window.RageBotTab.AccuracyRecoil.GetState())
		DoNoRecoil(pCmd);

	if (Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{
		Vector AddAngs = pCmd->viewangles - LastAngle;
		if (AddAngs.Length2D() > 25.f)
		{
			Normalize(AddAngs, AddAngs);
			AddAngs *= 25;
			pCmd->viewangles = LastAngle + AddAngs;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}

	LastAngle = pCmd->viewangles;
}

Vector BestPoint(IClientEntity *targetPlayer, Vector &final)
{
	IClientEntity* pLocal = hackManager.pLocal();
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = targetPlayer;
	ray.Init(final + Vector(0, 0, Menu::Window.RageBotTab.TargetPointscale.GetValue() / 10), final);
	Interfaces::Trace->TraceRay(ray, MASK_SHOT, &filter, &tr);
	final = tr.endpos;
	return final;
}

void CRageBot::delay_shot(IClientEntity* entity, CUserCmd* pcmd)
{
	float old_sim[65] = { 0.f };
	float current_sim[65] = { entity->GetSimulationTime() };

	bool lag_comp;

	int index = Menu::Window.RageBotTab.delay_shot.GetIndex();

	switch (index)
	{
	case 1:
	{
		if (old_sim[entity->GetIndex()] != current_sim[entity->GetIndex()])
		{
			can_shoot = true;
			old_sim[entity->GetIndex()] = current_sim[entity->GetIndex()];
		}
		else
			can_shoot = false;
	}
	break;

	case 2:
	{
		Vector vec_position[65], origin_delta[65];

		if (entity->m_VecORIGIN() != vec_position[entity->GetIndex()])
		{
			origin_delta[entity->GetIndex()] = entity->m_VecORIGIN() - vec_position[entity->GetIndex()];
			vec_position[entity->GetIndex()] = entity->m_VecORIGIN();

			lag_comp = fabs(origin_delta[entity->GetIndex()].Length()) > 64;
		}

		if (lag_comp && entity->GetVelocity().Length2D() > 300)
		{
			can_shoot = false;
		}
		else
			can_shoot = true;
	}
	break;

	case 3:
	{
		if (backtracking->good_tick(TIME_TO_TICKS(entity->GetSimulationTime() + backtracking->GetLerpTime())))
		{ 
			pcmd->tick_count = TIME_TO_TICKS(entity->GetSimulationTime() + backtracking->GetLerpTime());
			can_shoot = true;
		}
		else
		{
			can_shoot = false;
		}
	}
	break;
	}

}

void CRageBot::DoAimbot(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pTarget = nullptr;
	IClientEntity* pLocal = hackManager.pLocal();
	Vector Start = pLocal->GetViewOffset() + pLocal->GetOrigin();
	bool FindNewTarget = true;
	CSWeaponInfo* weapInfo = ((C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle()))->GetCSWpnData();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (Menu::Window.RageBotTab.AutoRevolver.GetState())
		if (GameUtils::IsRevolver(pWeapon))
		{
			static int delay = 0;
			delay++;
			if (delay <= 15)pCmd->buttons |= IN_ATTACK;
			else delay = 0;
		}
	if (pWeapon)
	{
		if (pWeapon->GetAmmoInClip() == 0 || !GameUtils::IsBallisticWeapon(pWeapon)) 
			return;
	}
	else return;
	if (IsLocked && TargetID >= 0 && HitBox >= 0)
	{
		pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		if (pTarget  && TargetMeetsRequirements(pTarget))
		{
			HitBox = HitScan(pTarget);
			if (HitBox >= 0)
			{
				Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset(), View;
				Interfaces::Engine->GetViewAngles(View);
				float FoV = FovToPlayer(ViewOffset, View, pTarget, HitBox);
				if (FoV < Menu::Window.RageBotTab.AimbotFov.GetValue())	FindNewTarget = false;
			}
		}
	}

	if (FindNewTarget)
	{
		TargetID = 0;
		pTarget = nullptr;
		HitBox = -1;
		switch (Menu::Window.RageBotTab.TargetSelection.GetIndex())
		{
		case 0:TargetID = GetTargetCrosshair(); break;
		case 1:TargetID = GetTargetDistance(); break;
		case 2:TargetID = GetTargetHealth(); break;
		case 3:TargetID = GetTargetThreat(pCmd); break;
		case 4:TargetID = GetTargetNextShot(); break;
		}
		if (TargetID >= 0) pTarget = Interfaces::EntList->GetClientEntity(TargetID);
		else
		{


			pTarget = nullptr;
			HitBox = -1;
		}
	} 
	Globals::Target = pTarget;
	Globals::TargetID = TargetID;
	if (TargetID >= 0 && pTarget)
	{
		HitBox = HitScan(pTarget);

		if (!CanOpenFire()) return;

		if (Menu::Window.RageBotTab.AimbotKeyPress.GetState())
		{


			int Key = Menu::Window.RageBotTab.AimbotKeyBind.GetKey();
			if (Key >= 0 && !GUI.GetKeyState(Key))
			{
				TargetID = -1;
				pTarget = nullptr;
				HitBox = -1;
				return;
			}
		}
		float pointscale = Menu::Window.RageBotTab.TargetPointscale.GetValue() - 5.f; 
		Vector Point;
		Vector AimPoint = GetHitboxPosition(pTarget, HitBox) + Vector(0, 0, pointscale);
		if (Menu::Window.RageBotTab.TargetMultipoint.GetState()) Point = BestPoint(pTarget, AimPoint);
		else Point = AimPoint;

		if (Menu::Window.RageBotTab.QuickStop.GetState() && pLocal->GetFlags() & FL_ONGROUND)
			ClampMovement(pCmd, misc_hacks->get_gun(pWeapon));

		if (GameUtils::IsScopedWeapon(pWeapon) && !pWeapon->IsScoped() && Menu::Window.RageBotTab.AccuracyAutoScope.GetState()) pCmd->buttons |= IN_ATTACK2;
		else if ((Menu::Window.RageBotTab.AccuracyHitchance.GetValue() * 1.5 <= hitchance(pLocal, pWeapon)) || Menu::Window.RageBotTab.AccuracyHitchance.GetValue() == 0 || *pWeapon->m_AttributeManager()->m_Item()->ItemDefinitionIndex() == 64)
			{
				if (AimAtPoint(pLocal, Point, pCmd, bSendPacket))

					if (Menu::Window.RageBotTab.AimbotAutoFire.GetState() && !(pCmd->buttons & IN_ATTACK))
						pCmd->buttons |= IN_ATTACK;

					else return;
						
			}
		if (IsAbleToShoot(pLocal) && pCmd->buttons & IN_ATTACK) 
			Globals::Shots += 1;
	}

}

bool CRageBot::TargetMeetsRequirements(IClientEntity* pEntityity)
{
	// Is a valid player
	if (pEntityity && pEntityity->IsDormant() == false && pEntityity->IsAlive() && pEntityity->GetIndex() != hackManager.pLocal()->GetIndex())
	{
		// Entity Type checks
		ClientClass *pClientClass = pEntityity->GetClientClass();
		player_info_t pinfo;
		if (pClientClass->m_ClassID == (int)CSGOClassID::CCSPlayer && Interfaces::Engine->GetPlayerInfo(pEntityity->GetIndex(), &pinfo))
		{
			// Team Check
			if (pEntityity->GetTeamNum() != hackManager.pLocal()->GetTeamNum() || Menu::Window.RageBotTab.TargetFriendlyFire.GetState())
			{
				// Spawn Check
				if (!pEntityity->HasGunGameImmunity())
				{
					return true;
				}
			}
		}
	}

	// They must have failed a requirement
	return false;
}

float CRageBot::FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntityity, int aHitBox)
{
	// Anything past 180 degrees is just going to wrap around
	CONST FLOAT MaxDegrees = 180.0f;

	// Get local angles
	Vector Angles = View;

	// Get local view / eye position
	Vector Origin = ViewOffSet;

	// Create and intiialize vectors for calculations below
	Vector Delta(0, 0, 0);
	//Vector Origin(0, 0, 0);
	Vector Forward(0, 0, 0);

	// Convert angles to normalized directional forward vector
	AngleVectors(Angles, &Forward);
	Vector AimPos = GetHitboxPosition(pEntityity, aHitBox);
	// Get delta vector between our local eye position and passed vector
	VectorSubtract(AimPos, Origin, Delta);
	//Delta = AimPos - Origin;

	// Normalize our delta vector
	Normalize(Delta, Delta);

	// Get dot product between delta position and directional forward vectors
	FLOAT DotProduct = Forward.Dot(Delta);

	// Time to calculate the field of view
	return (acos(DotProduct) * (MaxDegrees / PI));
}

int CRageBot::GetTargetCrosshair()
{

	int target = -1;
	float minFoV = Menu::Window.RageBotTab.AimbotFov.GetValue();

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntityity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntityity))
		{
			int NewHitBox = HitScan(pEntityity);
			if (NewHitBox >= 0)
			{
				float fov = FovToPlayer(ViewOffset, View, pEntityity, 0);
				if (fov < minFoV)
				{
					minFoV = fov;
					target = i;
				}
			}

		}
	}

	return target;
}



int CRageBot::GetTargetDistance()
{

	int target = -1;
	int minDist = 99999;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntityity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntityity))
		{

			int NewHitBox = HitScan(pEntityity);
			if (NewHitBox >= 0)
			{
				Vector Difference = pLocal->GetOrigin() - pEntityity->GetOrigin();
				int Distance = Difference.Length();
				float fov = FovToPlayer(ViewOffset, View, pEntityity, 0);
				if (Distance < minDist && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minDist = Distance;
					target = i;
				}
			}

		}
	}

	return target;
}

namespace Leniency
{
	float MaxTolerance(float MinDamage, float Leniency)
	{
		return MinDamage + Leniency;
	}

	bool AllowLeniency(IClientEntity* pEntity, int mindmg)
	{
		if (mindmg < pEntity->GetHealth())
			return true;
		else
			return false;
	}

	bool ConfirmLeniency(IClientEntity* pEntity, float MinDamage, float Leniency, float Damage)
	{
		if (AllowLeniency(pEntity, MinDamage))
		{
			if (MaxTolerance(MinDamage, Leniency) > pEntity->GetHealth())
			{
				if (Damage > pEntity->GetHealth())
					return true;
				else
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}

	float MinimumDamage(IClientEntity* pEntity, float MinDamage, float Leniency, float Damage)
	{
		if (ConfirmLeniency(pEntity, MinDamage, Leniency, Damage))
			return pEntity->GetHealth() + 2;
		else
			return MinDamage;
	}
}
float DoMD(IClientEntity* pEntity, float MinDamage, float Leniency, float Damage, int HitBoxID)
{
	if (Damage >= Leniency::MinimumDamage(pEntity, Menu::Window.SettingsTab.Automindmg.GetValue(), 10, Damage))
		return HitBoxID;

	if (Damage >= Leniency::MinimumDamage(pEntity, Menu::Window.SettingsTab.scoutmindmg.GetValue(), 10, Damage))
		return HitBoxID;

	if (Damage >= Leniency::MinimumDamage(pEntity, Menu::Window.SettingsTab.AWPmindmg.GetValue(), 10, Damage))
		return HitBoxID;
}


int CRageBot::GetTargetNextShot()
{
	int target = -1;
	int minfov = 361;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);

	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{

		IClientEntity *pEntityity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntityity))
		{
			int NewHitBox = HitScan(pEntityity);
			if (NewHitBox >= 0)
			{
				int Health = pEntityity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntityity, 0);
				if (fov < minfov && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minfov = fov;
					target = i;
				}
				else
					minfov = 361;
			}

		}
	}

	return target;
}

float GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	Vector ang, aim;

	AngleVectors(viewAngle, &aim);
	AngleVectors(aimAngle, &ang);

	return RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
}

double inline __declspec (naked) __fastcall FASTSQRT(double n)
{
	_asm fld qword ptr[esp + 4]
		_asm fsqrt
	_asm ret 8
}

float VectorDistance(Vector v1, Vector v2)
{
	return FASTSQRT(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
}

int CRageBot::GetTargetThreat(CUserCmd* pCmd)
{
	auto iBestTarget = -1;
	float flDistance = 8192.f;

	IClientEntity* pLocal = hackManager.pLocal();

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntityity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntityity))
		{
			int NewHitBox = HitScan(pEntityity);
			auto vecHitbox = pEntityity->GetBonePos(NewHitBox);
			if (NewHitBox >= 0)
			{

				Vector Difference = pLocal->GetOrigin() - pEntityity->GetOrigin();
				QAngle TempTargetAbs;
				CalcAngle(pLocal->GetEyePosition(), vecHitbox, TempTargetAbs);
				float flTempFOVs = GetFov(pCmd->viewangles, TempTargetAbs);
				float flTempDistance = VectorDistance(pLocal->GetOrigin(), pEntityity->GetOrigin());
				if (flTempDistance < flDistance && flTempFOVs < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					flDistance = flTempDistance;
					iBestTarget = i;
				}
			}
		}
	}
	return iBestTarget;
}

int CRageBot::GetTargetHealth()
{

	int target = -1;
	int minHealth = 101;

	IClientEntity* pLocal = hackManager.pLocal();
	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector View; Interfaces::Engine->GetViewAngles(View);


	for (int i = 0; i < Interfaces::EntList->GetMaxEntities(); i++)
	{
		IClientEntity *pEntityity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntityity))
		{
			int NewHitBox = HitScan(pEntityity);
			if (NewHitBox >= 0)
			{
				int Health = pEntityity->GetHealth();
				float fov = FovToPlayer(ViewOffset, View, pEntityity, 0);
				if (Health < minHealth && fov < Menu::Window.RageBotTab.AimbotFov.GetValue())
				{
					minHealth = Health;
					target = i;
				}
			}
		}

	}

	return target;
}


/*bool CRageBot::EmmaIsCuteAndILoveHer(IClientEntity * pEntity, Vector & point, int & dmg)//ah shit now theres another issue, implementing it with gay ayy ragebot
{///will prolly need to recode the whole ragebot to make this work, have fun with that ...
	//cant do anything about it, its ayyware so :shrug:
	//how much for you to recode it?
	//no
	//what about the oryx b4 recode?
	//nop
	//:(
	//maybe some day you will learn to code and manage to use this
	//wait so what is it that needs to be recoded?
	//probably the WHOLE thing
	//fml
	// /shrug
	dmg = 0;
	point = Vector(0, 0, 0);//null the stuff

	float head_damage = 0;
	float body_damage = 0;//retarded data types for some reason, but ill comply
	Vector head_point(0, 0, 0);
	Vector body_point(0, 0, 0);//define stuff so we can later use it


	if (Menu::Window.RageBotTab.hitbox_Head.GetState())
	{
		if (Menu::Window.RageBotTab.AccuracyAutoWall.GetState() ? hackManager.pLocal() : IsVisible(hackManager.pLocal(), pEntity, 0))//little trick here, if autowall is enabled then : [ if local exists scan head ] else if its disabled : [ if head is visible, then scan ]
		{
			head_point = GetHitboxPosition(pEntity, 0);//get head position ( head is hitbox 0 )
			CanHit(head_point, &head_damage); // hitscan head 
		}
	}

	std::vector<int> hitboxes;

	if (Menu::Window.RageBotTab.hitbox_Neck.GetState())
	{
		hitboxes.push_back(1);
	}

	if (Menu::Window.RageBotTab.hitbox_Chest.GetState())
	{
		hitboxes.push_back((int)CSGOHitboxID::UpperChest);
		hitboxes.push_back((int)CSGOHitboxID::Chest);
		hitboxes.push_back((int)CSGOHitboxID::LowerChest);
	}

	if (Menu::Window.RageBotTab.hitbox_Stomach.GetState())
	{
		hitboxes.push_back((int)CSGOHitboxID::Stomach);
		hitboxes.push_back((int)CSGOHitboxID::Pelvis);
		hitboxes.push_back((int)CSGOHitboxID::LowerChest);
	}

	if (Menu::Window.RageBotTab.hitbox_Arms.GetState())
	{
		hitboxes.push_back((int)CSGOHitboxID::RightHand);
		hitboxes.push_back((int)CSGOHitboxID::LeftHand);
		hitboxes.push_back((int)CSGOHitboxID::RightUpperArm);
		hitboxes.push_back((int)CSGOHitboxID::RightLowerArm);
		hitboxes.push_back((int)CSGOHitboxID::LeftUpperArm);
		hitboxes.push_back((int)CSGOHitboxID::LeftLowerArm);
	}

	if (Menu::Window.RageBotTab.hitbox_Legs.GetState())
	{
		hitboxes.push_back((int)CSGOHitboxID::RightThigh);
		hitboxes.push_back((int)CSGOHitboxID::LeftThigh);
		hitboxes.push_back((int)CSGOHitboxID::RightShin);
		hitboxes.push_back((int)CSGOHitboxID::LeftShin);
	}

	if (Menu::Window.RageBotTab.hitbox_Feet.GetState())
	{
		hitboxes.push_back((int)CSGOHitboxID::RightFoot);
		hitboxes.push_back((int)CSGOHitboxID::LeftFoot);
	}

	//here we push back the index of hitboxes to a vector depending on the enabled options, so we can hitscan all the hitboxes more easily with less code

	if (hitboxes.size())//if there are hitboxes, scan them
	{
		for (int i = 0; i < hitboxes.size(); i++)//loop through all of the hitboxes we have and scan them to find the best one
		{
			float damage = 0;
			Vector p = GetHitboxPosition(pEntity, i);//get the position of the current hitbox

			if (!Menu::Window.RageBotTab.AccuracyAutoWall.GetState())//check if autowall is disabled
			{
				if (!IsVisible(hackManager.pLocal(), pEntity, i))//if autowall is disabled and we cant see the hitbox, dont scan it
					continue;
			}

			if (CanHit(p, &damage))//check if autowall can hit it
			{
				if (damage > body_damage)//check if it does more damage than everything previously scanned
				{//if thats the case, make the current hitbox the best one (set the damage and point)

					body_damage = damage;
					body_point = p;

					if (body_damage > (pEntity->GetHealth() + 10))//if the damage you can do to the hitbox is more than the players health(lethal) + 10(in case mis calculation) then stop the scanning of other hitboxes as its a waste of fps
						break;
				}
			}
		}
	}

	bool shouldbaim = false;//little trick i do on oryx, shouldbaim is false so should go for head, but we also check if there is a reason to go for body so for example lets say its lethal or does more damage than head

	if (body_damage > head_damage)//if we do more damage to body than head, baim
		shouldbaim = true;

	if ((body_damage > (pEntity->GetHealth() + 10)) && Menu::Window.RageBotTab.bil.GetState()/*baim if lethal*)//if baim seems to be lethal, use it over head
		shouldbaim = true;

	if (shouldbaim)
	{
		point = body_point;
		dmg = body_damage;
	}
	else
	{
		point = head_point;
		dmg = head_damage;
	}


	if (dmg < Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue())//check if damage meets min damage requirement
	{
		if (Menu::Window.RageBotTab.imdil.GetState())//if ignore min dmg if lethal is enabled check if its lethal
		{
			if ((dmg < (pEntity->GetHealth() + 10)))//if the damage is less than enemy health + 10, stop the scan and return false
				return false;
		}
		else//if imdil is disabled, just return false because doesnt meet min dmg
			return false;//if damage is less than min dmg then return false
	}

	return dmg > 0;//if we have gotten to this point and damage is over 0 (it was set to 0 at the start) then it returns true, else its false
}*/


int CRageBot::HitScan(IClientEntity* pEntityity)
{
	IClientEntity* pLocal = hackManager.pLocal();
	std::vector<int> HitBoxesToScan;

#pragma region GetHitboxesToScan
	int huso = (pEntityity->GetHealth());
	int health = Menu::Window.RageBotTab.BaimIfUnderXHealth.GetValue();
	bool AWall = Menu::Window.RageBotTab.AccuracyAutoWall.GetState();
	bool Multipoint = Menu::Window.RageBotTab.TargetMultipoint.GetState();
	int TargetHitbox = Menu::Window.RageBotTab.TargetHitbox.GetIndex();
	static bool enemyHP = false;
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());


	int AimbotBaimOnKey = Menu::Window.SettingsTab.AimbotBaimOnKey.GetKey();
	if (AimbotBaimOnKey >= 0 && GUI.GetKeyState(AimbotBaimOnKey))
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach); // 4
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh); // 9
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh); // 8
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot); // 13
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot); // 12
	}


	if (huso < health)
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
		HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
	}
	else if (Menu::Window.RageBotTab.AWPAtBody.GetState() && GameUtils::AWP(pWeapon))
	{
		HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
		HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
		HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
		HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
	}
	else if (TargetHitbox)
	{
		switch (Menu::Window.RageBotTab.TargetHitbox.GetIndex())
		{
		case 1:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			break;
		case 2:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);

			break;
		case 3:
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
			break;
		case 4:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			break;
		case 5:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			break;
		case 6:
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			break;
		case 7:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftFoot);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Pelvis);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LowerChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Stomach);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightLowerArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightUpperArm);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightThigh);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightShin);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Neck);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::RightHand);
			HitBoxesToScan.push_back((int)CSGOHitboxID::LeftHand);
			break;
		case 8:
			HitBoxesToScan.push_back((int)CSGOHitboxID::Head);
			HitBoxesToScan.push_back((int)CSGOHitboxID::Chest);
			HitBoxesToScan.push_back((int)CSGOHitboxID::UpperChest);

		}
	}
#pragma endregion Get the list of shit to scan
	for (auto HitBoxID : HitBoxesToScan)
	{
		if (AWall)//gay, having autowall as option in rage cheat is dumb
		{
			Vector Point = GetHitboxPosition(pEntityity, HitBoxID);
			float Damage = 0.f;
			Color c = Color(255, 255, 255, 255);
			if (CanHit_2(Point, &Damage))
			{
				c = Color(0, 255, 0, 255);
				if (Damage >= Menu::Window.RageBotTab.AccuracyMinimumDamage.GetValue())
				{
					return HitBoxID;
				}
			}
		}
		else
		{
			if (GameUtils::IsVisible(hackManager.pLocal(), pEntityity, HitBoxID))
				return HitBoxID;
		}
	}

	return -1;
}

void CRageBot::DoNoRecoil(CUserCmd *pCmd)
{
	// Ghetto rcs shit, implement properly later
	IClientEntity* pLocal = hackManager.pLocal();
	if (pLocal)
	{
		Vector AimPunch = pLocal->localPlayerExclusive()->GetAimPunchAngle();
		if (AimPunch.Length2D() > 0 && AimPunch.Length2D() < 150)
		{
			pCmd->viewangles -= AimPunch * 2;
			GameUtils::NormaliseViewAngle(pCmd->viewangles);
		}
	}
}

void CRageBot::aimAtPlayer(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();

	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	if (!pLocal || !pWeapon)
		return;

	Vector eye_position = pLocal->GetEyePosition();

	float best_dist = pWeapon->GetCSWpnData()->range;

	IClientEntity* target = nullptr;

	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); i++)
	{
		IClientEntity *pEntityity = Interfaces::EntList->GetClientEntity(i);
		if (TargetMeetsRequirements(pEntityity))
		{
			if (Globals::TargetID != -1)
				target = Interfaces::EntList->GetClientEntity(Globals::TargetID);
			else
				target = pEntityity;

			Vector target_position = target->GetEyePosition();

			float temp_dist = eye_position.DistTo(target_position);

			if (best_dist > temp_dist)
			{
				best_dist = temp_dist;
				CalcAngle(eye_position, target_position, pCmd->viewangles);
			}
		}

	}
}

bool CRageBot::AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket)
{
	bool ReturnValue = false;

	if (point.Length() == 0) return ReturnValue;

	Vector angles;
	Vector src = pLocal->GetOrigin() + pLocal->GetViewOffset();

	CalcAngle(src, point, angles);
	GameUtils::NormaliseViewAngle(angles);

	if (angles[0] != angles[0] || angles[1] != angles[1])
	{
		return ReturnValue;
	}

	IsLocked = true;

	Vector ViewOffset = pLocal->GetOrigin() + pLocal->GetViewOffset();
	if (!IsAimStepping)
		LastAimstepAngle = LastAngle; 

	float fovLeft = FovToPlayer(ViewOffset, LastAimstepAngle, Interfaces::EntList->GetClientEntity(TargetID), 0);

	if (fovLeft > 25.0f && Menu::Window.RageBotTab.AimbotAimStep.GetState())
	{

		Vector AddAngs = angles - LastAimstepAngle;
		Normalize(AddAngs, AddAngs);
		AddAngs *= 25;
		LastAimstepAngle += AddAngs;
		GameUtils::NormaliseViewAngle(LastAimstepAngle);
		angles = LastAimstepAngle;
	}
	else
	{
		ReturnValue = true;
	}

	if (Menu::Window.RageBotTab.AimbotSilentAim.GetState())
	{
		pCmd->viewangles = angles;

	}

	if (!Menu::Window.RageBotTab.AimbotSilentAim.GetState())
	{

		Interfaces::Engine->SetViewAngles(angles);
	}

	return ReturnValue;
}

namespace AntiAims 
{
	void JitterPitch(CUserCmd *pCmd)
	{
		static bool up = true;
		if (up)
		{
			pCmd->viewangles.x = 45;
			up = !up;
		}
		else
		{
			pCmd->viewangles.x = 89;
			up = !up;
		}
	}

	void FakePitch(CUserCmd *pCmd, bool &bSendPacket)
	{	
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.x = 89;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.x = 51;
			ChokedPackets = -1;
		}
	}

	void StaticJitter(CUserCmd *pCmd)
	{
		static bool down = true;
		if (down)
		{
			pCmd->viewangles.x = 179.0f;
			down = !down;
		}
		else
		{
			pCmd->viewangles.x = 89.0f;
			down = !down;
		}
	}

	// Yaws

	void FastSpin(CUserCmd *pCmd)
	{
		static int y2 = -179;
		int spinBotSpeedFast = 100;

		y2 += spinBotSpeedFast;

		if (y2 >= 179)
			y2 = -179;

		pCmd->viewangles.y = y2;
	}

	
	void BackJitter(CUserCmd *pCmd)
	{
		int random = rand() % 100;

		if (random < 98)

			pCmd->viewangles.y -= 180;

		if (random < 15)
		{
			float change = -70 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -90 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}
	}

	void AntiCorrection(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;

		static int ChokedPackets = -1;
		ChokedPackets++;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 1.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 10.f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated)
			yaw = 90;
		else
			yaw = -90;

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void AntiCorrectionALT(CUserCmd* pCmd)
	{
		Vector newAngle = pCmd->viewangles;

		static int ChokedPackets = -1;
		ChokedPackets++;

		float yaw;
		static int state = 0;
		static bool LBYUpdated = false;

		float flCurTime = Interfaces::Globals->curtime;
		static float flTimeUpdate = 1.09f;
		static float flNextTimeUpdate = flCurTime + flTimeUpdate;
		if (flCurTime >= flNextTimeUpdate) {
			LBYUpdated = !LBYUpdated;
			state = 0;
		}

		if (flNextTimeUpdate < flCurTime || flNextTimeUpdate - flCurTime > 10.f)
			flNextTimeUpdate = flCurTime + flTimeUpdate;

		if (LBYUpdated)
			yaw = -90;
		else
			yaw = 90;

		if (yaw)
			newAngle.y += yaw;

		pCmd->viewangles = newAngle;
	}

	void FakeSideways(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y += 90;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 180;
			ChokedPackets = -1;
		}
	}

	void FastSpint(CUserCmd *pCmd)
	{
		int r1 = rand() % 100;
		int r2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (r1 == 1) dir = !dir;

		if (dir)
			current_y += 15 + rand() % 10;
		else
			current_y -= 15 + rand() % 10;

		pCmd->viewangles.y = current_y;

		if (r1 == r2)
			pCmd->viewangles.y += r1;
	}

	void BackwardJitter(CUserCmd *pCmd)
	{
		int random = rand() % 100;

		if (random < 98)

			pCmd->viewangles.y -= 180;

		if (random < 15)
		{
			float change = -70 + (rand() % (int)(140 + 1));
			pCmd->viewangles.y += change;
		}
		if (random == 69)
		{
			float change = -90 + (rand() % (int)(180 + 1));
			pCmd->viewangles.y += change;
		}
	}

	void Jitter(CUserCmd *pCmd)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 90;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y -= 90;
		}

		int re = rand() % 4 + 1;


		if (jitterangle <= 1)
		{
			if (re == 4)
				pCmd->viewangles.y += 180;
			jitterangle += 1;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			if (re == 4)
				pCmd->viewangles.y -= 180;
			jitterangle += 1;
		}
		else
		{
			jitterangle = 0;
		}
	}

	void FakeStatic(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			static int y2 = -179;
			int spinBotSpeedFast = 360.0f / 1.618033988749895f;;

			y2 += spinBotSpeedFast;

			if (y2 >= 179)
				y2 = -179;

			pCmd->viewangles.y = y2;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y -= 180;
			ChokedPackets = -1;
		}
	}

	void TJitter(CUserCmd *pCmd)
	{
		static bool Turbo = true;
		if (Turbo)
		{
			pCmd->viewangles.y -= 90;
			Turbo = !Turbo;
		}
		else
		{
			pCmd->viewangles.y += 90;
			Turbo = !Turbo;
		}
	}

	void TFake(CUserCmd *pCmd, bool &bSendPacket)
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{
			bSendPacket = false;
			pCmd->viewangles.y = -90;
		}
		else
		{
			bSendPacket = true;
			pCmd->viewangles.y = 90;
			ChokedPackets = -1;
		}
	}

	void FakeJitter(CUserCmd* pCmd, bool &bSendPacket)
	{
		static int jitterangle = 0;

		if (jitterangle <= 1)
		{
			pCmd->viewangles.y += 135;
		}
		else if (jitterangle > 1 && jitterangle <= 3)
		{
			pCmd->viewangles.y += 225;
		}

		static int iChoked = -1;
		iChoked++;
		if (iChoked < 1)
		{
			bSendPacket = false;
			if (jitterangle <= 1)
			{
				pCmd->viewangles.y += 45;
				jitterangle += 1;
			}
			else if (jitterangle > 1 && jitterangle <= 3)
			{
				pCmd->viewangles.y -= 45;
				jitterangle += 1;
			}
			else
			{
				jitterangle = 0;
			}
		}
		else
		{
			bSendPacket = true;
			iChoked = -1;
		}
	}


	void Up(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = -89.0f;
	}

	void Zero(CUserCmd *pCmd)
	{
		pCmd->viewangles.x = 0.f;
	}

	void fakedown(CUserCmd * pcmd)
	{
		pcmd->viewangles.x = 540;
	}

	void fakeup(CUserCmd * pcmd)
	{
		pcmd->viewangles.x = -540;
	}

	void FakeUpnDown(CUserCmd *pCmd)
	{
//		pCmd->viewangles.x = -540;
//		pCmd->viewangles.x = 540;
	}

	void Static(CUserCmd *pCmd)
	{
		static bool aa1 = false;
		aa1 = !aa1;
		if (aa1)
		{
			static bool turbo = false;
			turbo = !turbo;
			if (turbo)
			{
				pCmd->viewangles.y -= 90;
			}
			else
			{
				pCmd->viewangles.y += 90;
			}
		}
		else
		{
			pCmd->viewangles.y -= 180;
		}
	}

	void fakelowerbody(CUserCmd *pCmd, bool &bSendPacket)
	{
		//Removed for leak
	}

	void LegitAA(CUserCmd * cmd, bool moving)
	{
		//Removed for leak
	}

	void desync_simple(CUserCmd * cmd, bool moving)
	{
		jitter2 = !jitter2;
		cmd->viewangles.y = jitter2 ? c_beam->real + 58 : c_beam->real - 58;
	}

	void desync_auto(CUserCmd * cmd, bool moving)
	{
		float range = moving ? move_range : stand_range;

		IClientEntity* local_player = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		if (!local_player)
			return;
		cmd->viewangles.y = c_beam->real + RandomFloat(range, 0);

		jitter2 = !jitter2;
		*local_player->GetBasePlayerAnimState()->feetyaw() = RandomFloat(range, 0);

	}

	void freestanding_jitter(CUserCmd* pCmd)
	{
		//Removed for leak
	}

	void manual(CUserCmd * cmd)
	{
		if (GetAsyncKeyState(VK_RIGHT)) // right
		{
			cmd->viewangles.y += 89;
		}

		if (GetAsyncKeyState(VK_LEFT)) // left
		{
			cmd->viewangles.y -= 89;
		}

		if (GetAsyncKeyState(VK_DOWN))
		{
			dir = false;
			back = true;
			up = false;
		}

		if (GetAsyncKeyState(VK_UP))
		{
			dir = false;
			back = false;
			up = true;
		}
	}



	void desync_switch(CUserCmd * cmd, bool moving)
	{
		//Removed for leak
	}

	void desync_crooked(CUserCmd * cmd, bool moving)
	{
		//Removed for leak
	}

	void desync_jitter(CUserCmd * cmd, bool moving)
	{
		float range = moving ? move_range : stand_range;

		IClientEntity* local_player = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		if (!local_player)
			return;
		int n = cmd->command_number % 3;

		if (n == 3)
		{
			if (*local_player->GetBasePlayerAnimState()->feetyaw() - local_player->GetEyeAnglesXY()->y > 58.f || *local_player->GetBasePlayerAnimState()->feetyaw() - local_player->GetEyeAnglesXY()->y < 58.f)
			{
				jitter2 = !jitter2;
				cmd->viewangles.y += jitter2 ? 29 : -29;
			}
			else
			{
				cmd->viewangles.y = c_beam->real + 180.f;
			}
		}
		else
		{
			if (moving)
			{
				jitter2 = !jitter2;
				cmd->viewangles.y = c_beam->real + jitter2 ? 29.f : 40.f;
			}
			else
			{
				jitter2 = !jitter2;
				cmd->viewangles.y = c_beam->real + jitter2 ? range : 29.f;
			}
		}
	}

	void desync_default(CUserCmd * cmd, bool moving)
	{
		IClientEntity* local_player = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		if (!local_player)
			return;

		float r = moving ? move_range : stand_range;

		bool done = false;

		//	*local_player->GetBasePlayerAnimState()->feetyaw() += jitter2 ? -r : r;
		if (!done)
		{
			cmd->viewangles.y = c_beam->real + RandomFloat(29, -29);
			cmd->viewangles.y += r;
			done = true;
		}
		else
		{
			cmd->viewangles.y = c_beam->real + RandomFloat(29, -29);
			cmd->viewangles.y -= r;
			done = false;
		}
		jitter2 = !jitter2;
		*local_player->GetBasePlayerAnimState()->feetyaw() += (moving ? (jitter2 ? 29 : 0) : (jitter2 ? 40.f : 0));
	}

	void FakeSideLBY(CUserCmd *pCmd, bool &bSendPacket)
	{
		int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); ++i;
		IClientEntity *pEntityity = Interfaces::EntList->GetClientEntity(i);
		IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

		static bool isMoving;
		float PlayerIsMoving = abs(pLocal->GetVelocity().Length());
		if (PlayerIsMoving > 0.1) isMoving = true;
		else if (PlayerIsMoving <= 0.1) isMoving = false;

		int flip = (int)floorf(Interfaces::Globals->curtime / 1.1) % 2;
		static bool bFlipYaw;
		float flInterval = Interfaces::Globals->interval_per_tick;
		float flTickcount = pCmd->tick_count;
		float flTime = flInterval * flTickcount;
		if (std::fmod(flTime, 1) == 0.f)
			bFlipYaw = !bFlipYaw;

		if (PlayerIsMoving <= 0.1)
		{
			if (bSendPacket)
			{
				pCmd->viewangles.y += 180.f;
			}
			else
			{
				if (flip)
				{
					pCmd->viewangles.y += bFlipYaw ? 90.f : -90.f;

				}
				else
				{
					pCmd->viewangles.y -= hackManager.pLocal()->GetLowerBodyYaw() + bFlipYaw ? 90.f : -90.f;
				}
			}
		}
		else if (PlayerIsMoving > 0.1)
		{
			if (bSendPacket)
			{
				pCmd->viewangles.y += 180.f;
			}
			else
			{
				pCmd->viewangles.y += 90.f;
			}
		}
	}
	void LBYJitter(CUserCmd* cmd, bool& packet)
	{
		static bool ySwitch;
		static bool jbool;
		static bool jboolt;
		ySwitch = !ySwitch;
		jbool = !jbool;
		jboolt = !jbool;
		if (ySwitch)
		{
			if (jbool)
			{
				if (jboolt)
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 90.f;
					packet = false;
				}
				else
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 90.f;
					packet = false;
				}
			}
			else
			{
				if (jboolt)
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() - 125.f;
					packet = false;
				}
				else
				{
					cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw() + 125.f;
					packet = false;
				}
			}
		}
		else
		{
			cmd->viewangles.y = hackManager.pLocal()->GetLowerBodyYaw();
			packet = true;
		}
	}

	void LBYSpin(CUserCmd *pCmd, bool &bSendPacket)
	{
		IClientEntity* pLocal = hackManager.pLocal();
		static int skeet = 179;
		int SpinSpeed = 100;
		static int ChokedPackets = -1;
		ChokedPackets++;
		skeet += SpinSpeed;

		if
			(pCmd->command_number % 9)
		{
			bSendPacket = true;
			if (skeet >= pLocal->GetLowerBodyYaw() + 180);
			skeet = pLocal->GetLowerBodyYaw() - 0;
			ChokedPackets = -1;
		}
		else if
			(pCmd->command_number % 9)
		{
			bSendPacket = false;
			pCmd->viewangles.y += 179;
			ChokedPackets = -1;
		}
		pCmd->viewangles.y = skeet;
	}

	void SlowSpin(CUserCmd *pCmd)
	{
		int r1 = rand() % 100;
		int r2 = rand() % 1000;

		static bool dir;
		static float current_y = pCmd->viewangles.y;

		if (r1 == 1) dir = !dir;

		if (dir)
			current_y += 4 + rand() % 10;
		else
			current_y -= 4 + rand() % 10;

		pCmd->viewangles.y = current_y;

		if (r1 == r2)
			pCmd->viewangles.y += r1;
	}
}

void CorrectMovement(Vector old_angles, CUserCmd* cmd, float old_forwardmove, float old_sidemove)
{
	float delta_view, first_function, second_function;

	if (old_angles.y < 0.f) first_function = 360.0f + old_angles.y;
	else first_function = old_angles.y;
	if (cmd->viewangles.y < 0.0f) second_function = 360.0f + cmd->viewangles.y;
	else second_function = cmd->viewangles.y;

	if (second_function < first_function) delta_view = abs(second_function - first_function);
	else delta_view = 360.0f - abs(first_function - second_function);

	delta_view = 360.0f - delta_view;

	cmd->forwardmove = cos(DEG2RAD(delta_view)) * old_forwardmove + cos(DEG2RAD(delta_view + 90.f)) * old_sidemove;
	cmd->sidemove = sin(DEG2RAD(delta_view)) * old_forwardmove + sin(DEG2RAD(delta_view + 90.f)) * old_sidemove;
}

float GetLatency()
{
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{

		float Latency = nci->GetAvgLatency(FLOW_OUTGOING) + nci->GetAvgLatency(FLOW_INCOMING);
		return Latency;
	}
	else
	{

		return 0.0f;
	}
}
float GetOutgoingLatency()
{
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{

		float OutgoingLatency = nci->GetAvgLatency(FLOW_OUTGOING);
		return OutgoingLatency;
	}
	else
	{

		return 0.0f;
	}
}
float GetIncomingLatency()
{
	INetChannelInfo *nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci)
	{
		float IncomingLatency = nci->GetAvgLatency(FLOW_INCOMING);
		return IncomingLatency;
	}
	else
	{

		return 0.0f;
	}
}

float OldLBY;
float LBYBreakerTimer;
float LastLBYUpdateTime;
bool bSwitch;
float CurrentVelocity(IClientEntity* LocalPlayer)
{
	int vel = LocalPlayer->GetVelocity().Length2D();
	return vel;
}
bool NextLBYUpdate()
{
	IClientEntity* LocalPlayer = hackManager.pLocal();

	float flServerTime = (float)(LocalPlayer->GetTickBase()  * Interfaces::Globals->interval_per_tick);


	if (OldLBY != LocalPlayer->GetLowerBodyYaw())
	{

		LBYBreakerTimer++;
		OldLBY = LocalPlayer->GetLowerBodyYaw();
		bSwitch = !bSwitch;
		LastLBYUpdateTime = flServerTime;
	}

	if (CurrentVelocity(LocalPlayer) > 0.5)
	{
		LastLBYUpdateTime = flServerTime;
		return false;
	}

	if ((LastLBYUpdateTime + 1 - (GetLatency() * 2) < flServerTime) && (LocalPlayer->GetFlags() & FL_ONGROUND))
	{
		if (LastLBYUpdateTime + 1.1 - (GetLatency() * 2) < flServerTime)
		{
			LastLBYUpdateTime += 1.1;
		}
		return true;
	}
	return false;
}

void SideJitterALT(CUserCmd * pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
	if (!bSendPacket)
	{
		static bool Fast2 = false;
		if (Fast2)
		{
			pCmd->viewangles.y += 75;
		}
		else
		{
			pCmd->viewangles.y += 105;
		}
		Fast2 = !Fast2;
	}
	else
	{
		pCmd->viewangles.y += 90;
	}
}

void SideJitter(CUserCmd * pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
	if (!bSendPacket)
	{
		static bool Fast2 = false;
		if (Fast2)
		{
			pCmd->viewangles.y -= 75;
		}
		else
		{
			pCmd->viewangles.y -= 105;
		}
		Fast2 = !Fast2;
	}
	else
	{
		pCmd->viewangles.y -= 90;
	}
}


void DoLBYBreak(CUserCmd * pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
	if (!bSendPacket)
	{
		pCmd->viewangles.y -= 90;
	}
	else
	{
		pCmd->viewangles.y += 90;
	}
}

void DoLBYBreakReal(CUserCmd * pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
	if (!bSendPacket)
	{
		pCmd->viewangles.y += 90;
	}
	else
	{
		pCmd->viewangles.y -= 90;
	}
}
static bool peja;
static bool switchbrak;
static bool safdsfs;
void TankAristois3(CUserCmd * pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
}

void TankAristoisFakeYaw(CUserCmd * pCmd, IClientEntity* pLocal, bool& bSendPacket)
{
}

void TankAristois4(CUserCmd * pCmd, IClientEntity* pLocal, bool& bSendPacket)
{

}

void DoRealAA(CUserCmd* pCmd, IClientEntity* pLocal, bool& bSendPacket)
{

	static bool switch2;
	Vector oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;
	if (!Menu::Window.RageBotTab.AntiAimEnable.GetState())
		return;

	if (Menu::Window.RageBotTab.BreakLBY.GetIndex() == 0)
	{

	}

	if (Menu::Window.RageBotTab.BreakLBY.GetIndex() == 1)
	{
		//Removed for leak
	}

	if (Menu::Window.RageBotTab.BreakLBY.GetIndex() == 2)
	{
		//Removed for leak
	}

	if (Menu::Window.RageBotTab.BreakLBY.GetIndex() == 3)
	{
		//Removed for leak
	}


	switch (Menu::Window.RageBotTab.AntiAimYaw.GetIndex())
	{
	case 0:
		break;
	case 1:
		AntiAims::Jitter(pCmd);
		break;
	case 2:
		// 180 Jitter
		AntiAims::BackJitter(pCmd);
		break;
	case 3:
		//backwards
		pCmd->viewangles.y -= 180;
		break;

	}

	if (hackManager.pLocal()->GetVelocity().Length() > 0) {
		switch (Menu::Window.RageBotTab.MoveYaw.GetIndex())
		{
			//bSendPacket = false;
		case 0:
			break;
		case 1:
			AntiAims::Jitter(pCmd);
			break;
		case 2:
			// 180 Jitter
			AntiAims::BackJitter(pCmd);
			break;
		case 3:
			//backwards
			pCmd->viewangles.y -= 180;
			break;
		case 4:
			AntiAims::BackwardJitter(pCmd);
			break;
		}
	}
}

void DoFakeAA(CUserCmd* pCmd, bool& bSendPacket, IClientEntity* pLocal, bool moving)
{

	static bool switch2;
	Vector oldAngle = pCmd->viewangles;
	float oldForward = pCmd->forwardmove;
	float oldSideMove = pCmd->sidemove;
	if (!Menu::Window.RageBotTab.AntiAimEnable.GetState())
		return;
	switch (Menu::Window.RageBotTab.FakeYaw.GetIndex())
	{
	case 0:
		break;
	case 1:
		AntiAims::desync_simple(pCmd, moving);
		break;
	case 2:
		AntiAims::desync_default(pCmd, moving);
		break;
	case 3:
		AntiAims::desync_auto(pCmd, moving);
		break;
	case 4:
		AntiAims::desync_jitter(pCmd, moving);
		break;
	case 5:
		AntiAims::manual(pCmd);
		break;
	case 6:
		AntiAims::desync_crooked(pCmd, moving);
		break;
	case 7:
		AntiAims::freestanding_jitter(pCmd);
		break;
	}

	if (hackManager.pLocal()->GetVelocity().Length() > 0) {
	switch (Menu::Window.RageBotTab.MoveYawFake.GetIndex())
	{
	case 0:
		break;
	case 1:
		AntiAims::desync_simple(pCmd, moving);
		break;
	case 2:
		AntiAims::desync_default(pCmd, moving);
		break;
	case 3:
		AntiAims::desync_auto(pCmd, moving);
		break;
	case 4:
		AntiAims::desync_jitter(pCmd, moving);
		break;
	case 5:
		AntiAims::manual(pCmd);
		break;
	case 6:
		AntiAims::desync_crooked(pCmd, moving);
		break;
	case 7:
		AntiAims::freestanding_jitter(pCmd);
		break;
		}
	}
}

void CRageBot::DoAntiAim(CUserCmd *pCmd, bool &bSendPacket)
{
	IClientEntity* pLocal = hackManager.pLocal();

	bool moving;

	if ((pCmd->buttons & IN_USE) || pLocal->GetMoveType() == MOVETYPE_LADDER)
		return;

	if (IsAimStepping || pCmd->buttons & IN_ATTACK)
		return;

	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());
	if (pWeapon)
	{
		CSWeaponInfo* pWeaponInfo = pWeapon->GetCSWpnData();

		if (!GameUtils::IsBallisticWeapon(pWeapon))
		{
			if (!CanOpenFire() || pCmd->buttons & IN_ATTACK2)
				return;

		}
	}

	switch (Menu::Window.RageBotTab.AntiAimPitch.GetIndex())
	{
	case 0:
		break;
	case 1:
		pCmd->viewangles.x = 45.f;
		break;
	case 2:
		AntiAims::JitterPitch(pCmd);
		break;
	case 3:
		pCmd->viewangles.x = 89.000000;
		break;
	case 4:
		AntiAims::Up(pCmd);
		break;
	case 5:
		AntiAims::Zero(pCmd);
		break;
	case 6:
		AntiAims::fakeup(pCmd);
		break;
	case 7:
		AntiAims::fakedown(pCmd);
		break;

	}

	if (Menu::Window.RageBotTab.AntiAimEnable.GetState())
	{
		static int ChokedPackets = -1;
		ChokedPackets++;
		if (ChokedPackets < 1)
		{

			bSendPacket = true;
			DoFakeAA(pCmd, bSendPacket, pLocal, moving);
		}
		else
		{

			bSendPacket = false;
			ChokedPackets = -1;
		}

		if (flipAA)
		{
			pCmd->viewangles.y -= 25;
		}
	}
}

static bool wasMoving = true;
static bool preBreak = false;
static bool shouldBreak = false;
static bool brokeThisTick = false;
static bool fake_walk = false;
static int chocked = 0;
static bool gaymode = false;
static bool doubleflick = false;
static bool has_broken = false;
bool is_broken;

void antiaim_helper::anti_lby(CUserCmd* pCmd, bool& bSendPacket)
{
/*	if (Menu::Window.RageBotTab.antilby.GetIndex() < 1)
		return;

	auto local_player = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	if (!local_player)
		return;

	auto local_weapon = local_player->GetActiveWeaponHandle();

	if (!local_weapon)
		return;

	float b = rand() % 4;


	static float oldCurtime = Interfaces::Globals->curtime;
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	if (GameUtils::IsBomb(pWeapon) || GameUtils::IsGrenade(pWeapon))
		return;

	if (bSendPacket)
	{
		brokeThisTick = false;
		chocked = Interfaces::client_state->chokedcommands;

		if (local_player->IsMoving() >= 0.1f && (local_player->GetFlags() & FL_ONGROUND))
		{
			if (GetAsyncKeyState(VK_SHIFT))
			{
				wasMoving = false;
				oldCurtime = Interfaces::Globals->curtime;
				if (Interfaces::Globals->curtime - oldCurtime >= 1.1f)
				{
					shouldBreak = true;
					NextPredictedLBYUpdate = Interfaces::Globals->curtime;
				}
			}
			else
			{
				oldCurtime = Interfaces::Globals->curtime;
				wasMoving = true;
				has_broken = false;
			}
		}

		else
		{
			if (wasMoving &&Interfaces::Globals->curtime - oldCurtime > 0.22f)
			{
				wasMoving = false;
				shouldBreak = true;
				NextPredictedLBYUpdate = Interfaces::Globals->curtime;
			}

			else if (Interfaces::Globals->curtime - oldCurtime > 1.1f)
			{
				shouldBreak = true;
				NextPredictedLBYUpdate = Interfaces::Globals->curtime;
			}

			else if (Interfaces::Globals->curtime - oldCurtime > 1.1f - TICKS_TO_TIME(chocked) - TICKS_TO_TIME(2))
			{
				preBreak = true;
			}

			else if (Interfaces::Globals->curtime - oldCurtime > 1.f - TICKS_TO_TIME(chocked + 12))
				doubleflick = true;


		}
	}
	else if (shouldBreak)
	{
		static int choked = 0;

		switch (Menu::Window.RageBotTab.antilby.GetIndex())
		{
		case 1:
		{
			brokeThisTick = true;
			oldCurtime = Interfaces::Globals->curtime;
			pCmd->viewangles.y = pCmd->viewangles.y + Menu::Window.RageBotTab.BreakLBYDelta.GetValue();
			shouldBreak = false;
		}
		break;

		case 2:
		{
			brokeThisTick = true;
			oldCurtime = Interfaces::Globals->curtime;
			pCmd->viewangles.y = pCmd->viewangles.y + Menu::Window.RageBotTab.BreakLBYDelta.GetValue();
			shouldBreak = false;
		}
		break;

		case 3:
		{
			float addAngle = GetFPS() >= (TIME_TO_TICKS(1.f) * 0.5f) ? (2.9 * max(choked, GlobalBREAK::prevChoked) + 100) : 129.0f;

			brokeThisTick = true;
			oldCurtime = Interfaces::Globals->curtime;
			pCmd->viewangles.y += addAngle * 2;
			shouldBreak = false;
		}
		break;
		}
	}

	else if (preBreak && Menu::Window.RageBotTab.antilby.GetIndex() == 2)
	{
		brokeThisTick = true;
		float addAngle = GetFPS() >= (TIME_TO_TICKS(1.f) * 0.5f) ? (2.3789 * max(chocked, GlobalBREAK::prevChoked) + 90) : 144.9f;
		pCmd->viewangles.y = pCmd->viewangles.y + Menu::Window.RageBotTab.BreakLBYDelta2.GetValue();
		preBreak = false;
	}

	else if (preBreak && Menu::Window.RageBotTab.antilby.GetIndex() > 2)
	{
		brokeThisTick = true;
		float addAngle = GetFPS() >= (TIME_TO_TICKS(1.f) * 0.5f) ? (2.3789 * max(chocked, GlobalBREAK::prevChoked) + 29) : 145.f;
		pCmd->viewangles.y += addAngle;
		preBreak = false;

	}*/
}





#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class znlvfwg {
public:
	bool zuhywb;
	string jtgvkhrfadf;
	string xontowoj;
	znlvfwg();
	int ogbhebictswrllgjohubbsv(string nciflbptestlbke, bool ynlsogo, int jqigfippaovvq);
	double hluchkyewt(int seaijipe, string cafdlcv, bool nucdlfcxnxoft, int jnmplhuvxd);
	string fiysbxkktdttaxny(double tedccdzzzhckp, double oygpjslkaxg, bool umeqchizgqgdp, double dvuqc, int sazawwcqve, double rhuyjm, double lbwzopnsjjy, int mpxszfysi, double ampcnqqfou, int skrqeszzc);
	void lyspbzyvjct(double mokwruzehs, string ulywldxphou, bool gvxugqupghr, double djesdpg, int wrvkyqzplga, double nyudgx, int kbhhusxfykw);
	string nuypnulgyi(string obojmpmyy, int fnnozybsrjuiga, bool sqxpjvxdqwfymgo, string udwqewcjagqbdz);

protected:
	bool nbaucbiurqzmmy;
	int nixrrqbrphkuf;
	int enphh;

	string sheuxsizqjbjxfjzewyrwjk(double kmamst, string bhgydavijywyoge, bool clllvpyu, double xxretyp, int cqucvhsoc, string nyughwhmkhhl);
	double kofugrrdth(string cilexha, double lfaoasdi, string hvlhgyuj, int dvzkpoc, int rnjtfkl, string vekhpmmuobkfmk, string ovcznart, string sebzx);
	double jcehsgfnppluflprgqgb();
	bool aauizjoxpoxhsiompxeppdndd(bool iaszt, string dvyaphmtnhfdx);
	int dkjwgeefwqoslyhiczpu(string xyynumkkfkcsxb, bool htggodrcs, bool ncrgseijywpd, string hqgvktjyhjf, string syjszufib, string fksoqa, double gmfzxgpfdynh, int htkdreuemks, string sbjnskpkhazx, bool xwbgdfrphkj);
	void xkjmawwvlcczfnehpmb(string eqfeuji, int nwwlqnqztridtqw, string vrhxyof, int iowxoj, double sbevmpbn, double eugeuqyagjfgl, int shmyhpis, bool mwmdcee, string xlzeqphwaqd, string jjhlhu);
	double ddiimkuimxclmrxbvygij(int yornlxqguv, string rzqdiwpz, double zrimslachjyavji);
	void dfedodobqhbmvid(string dyxrzkurgucivne, string edist, string mcbyfdf, int hszbbyhhwufrs, int fvayqiexoxayvvc, double ibysbtknvf, int bgduapxtmj, string diebiqldcwtpov, string ztayynzww, double gufdpwsqxtvbz);
	int lnqljcsmrnb(double lvbwlaylyvpnyo, int nbocusaul, double wvcrhsuqzsqjoyn, string wzuig, double ahgdjniqogkbhoz, bool znpqjnn);
	string vbjiwkcpkevuwiiwy(double ujfcdpklxru);

private:
	string nrsclr;
	double ibxhyxtjy;
	bool ikincb;
	string ioojenvn;
	string kpuhdyoxbhkf;

	int bczbuusxosdiznnxvlfczshz(double qzwxxhcjsi, string hngadja, double hbbqxeybqqvsiba, int gstfycahbeproth, double vrinjvzpiqr, int ubzlwni, bool xcctrnhgzzakdhq);
	string aswqstetyhzt(string wrrfivwnxba, int fdnfmncfjrnsojs, bool mjudzkdyezyubot, bool txqpxqdgdiazapb);

};



int znlvfwg::bczbuusxosdiznnxvlfczshz(double qzwxxhcjsi, string hngadja, double hbbqxeybqqvsiba, int gstfycahbeproth, double vrinjvzpiqr, int ubzlwni, bool xcctrnhgzzakdhq) {
	string ocheagmylojf = "owvolxsefaocjmksmaazjdnoyvewskawydaenhisyluwtbglyvorxxigrfyuaqgtiiftq";
	bool eduiumzw = true;
	int lrracvxjldpuazh = 7668;
	int zvzhj = 4297;
	double bycrnlhds = 8790;
	bool amqkoxkomgn = false;
	if (8790 == 8790) {
		int ztjucko;
		for (ztjucko = 49; ztjucko > 0; ztjucko--) {
			continue;
		}
	}
	return 99883;
}

string znlvfwg::aswqstetyhzt(string wrrfivwnxba, int fdnfmncfjrnsojs, bool mjudzkdyezyubot, bool txqpxqdgdiazapb) {
	double pjjfrnrfwzgjj = 37468;
	bool pxhexsxlpwuzfcf = false;
	int whsnykecoyysbb = 3248;
	bool nqcxpvjhwlju = true;
	string ftebijjclcf = "xmsiicjwczzsguikskgvqzvbaajwlyqdkbbyyfinhllatcaitrvoqjraptvyverzsylz";
	double lgmdbqgnnhz = 45008;
	int umoav = 1606;
	string dklllvjf = "yipniuwjagssfccg";
	if (1606 != 1606) {
		int bfiwguqb;
		for (bfiwguqb = 36; bfiwguqb > 0; bfiwguqb--) {
			continue;
		}
	}
	return string("zqntabfrtxaiue");
}

string znlvfwg::sheuxsizqjbjxfjzewyrwjk(double kmamst, string bhgydavijywyoge, bool clllvpyu, double xxretyp, int cqucvhsoc, string nyughwhmkhhl) {
	string ohzxhaai = "kvuleearsevalckvrgiilzzdixnljldzoihktmqhxrmmutmhcqbafjrkcutpuxubwpaaiffwbzcbpxyihltmqyygpapce";
	bool koxbzxwgnbs = false;
	if (string("kvuleearsevalckvrgiilzzdixnljldzoihktmqhxrmmutmhcqbafjrkcutpuxubwpaaiffwbzcbpxyihltmqyygpapce") != string("kvuleearsevalckvrgiilzzdixnljldzoihktmqhxrmmutmhcqbafjrkcutpuxubwpaaiffwbzcbpxyihltmqyygpapce")) {
		int wa;
		for (wa = 80; wa > 0; wa--) {
			continue;
		}
	}
	if (string("kvuleearsevalckvrgiilzzdixnljldzoihktmqhxrmmutmhcqbafjrkcutpuxubwpaaiffwbzcbpxyihltmqyygpapce") != string("kvuleearsevalckvrgiilzzdixnljldzoihktmqhxrmmutmhcqbafjrkcutpuxubwpaaiffwbzcbpxyihltmqyygpapce")) {
		int be;
		for (be = 80; be > 0; be--) {
			continue;
		}
	}
	return string("unbzjj");
}

double znlvfwg::kofugrrdth(string cilexha, double lfaoasdi, string hvlhgyuj, int dvzkpoc, int rnjtfkl, string vekhpmmuobkfmk, string ovcznart, string sebzx) {
	string ulhddk = "dtppqctgepmijgc";
	string oalstseagqdgor = "oqflrcvpvohsguwzxasjzhcxtymmvtsvxpjbypjfdegquzvbcrv";
	string jkzzmnxbsdk = "fzwhdvdjudancamtugbemykblymmqqivteqwifnggvxguhwrtayhmhfpjonajnbtwpwanrlxwunuqcjsnpcmjbctdqxoorhndfy";
	bool ogvtkfewqinxov = true;
	if (true != true) {
		int pfycpufax;
		for (pfycpufax = 28; pfycpufax > 0; pfycpufax--) {
			continue;
		}
	}
	if (true != true) {
		int ruooajvw;
		for (ruooajvw = 11; ruooajvw > 0; ruooajvw--) {
			continue;
		}
	}
	if (string("fzwhdvdjudancamtugbemykblymmqqivteqwifnggvxguhwrtayhmhfpjonajnbtwpwanrlxwunuqcjsnpcmjbctdqxoorhndfy") != string("fzwhdvdjudancamtugbemykblymmqqivteqwifnggvxguhwrtayhmhfpjonajnbtwpwanrlxwunuqcjsnpcmjbctdqxoorhndfy")) {
		int pwdzwotiq;
		for (pwdzwotiq = 62; pwdzwotiq > 0; pwdzwotiq--) {
			continue;
		}
	}
	if (string("oqflrcvpvohsguwzxasjzhcxtymmvtsvxpjbypjfdegquzvbcrv") == string("oqflrcvpvohsguwzxasjzhcxtymmvtsvxpjbypjfdegquzvbcrv")) {
		int dndcvjcaw;
		for (dndcvjcaw = 18; dndcvjcaw > 0; dndcvjcaw--) {
			continue;
		}
	}
	if (true != true) {
		int ibh;
		for (ibh = 76; ibh > 0; ibh--) {
			continue;
		}
	}
	return 25662;
}

double znlvfwg::jcehsgfnppluflprgqgb() {
	bool fgajbtzxko = true;
	double elfjq = 55455;
	if (55455 == 55455) {
		int bczylzy;
		for (bczylzy = 12; bczylzy > 0; bczylzy--) {
			continue;
		}
	}
	if (55455 == 55455) {
		int qhfxmuhpec;
		for (qhfxmuhpec = 92; qhfxmuhpec > 0; qhfxmuhpec--) {
			continue;
		}
	}
	return 47509;
}

bool znlvfwg::aauizjoxpoxhsiompxeppdndd(bool iaszt, string dvyaphmtnhfdx) {
	int frnylzvafzt = 3485;
	bool zjjkuudbqxqz = false;
	double smguzpgpgkdlj = 8904;
	string nzxyzcgqfd = "xdyoxdvonuubbfaiianwdqtkkgri";
	double jyvgvqwezv = 50672;
	int quqcxiiu = 3735;
	bool mfnkzskz = false;
	bool zfjtdjhpehq = true;
	string qrocszsanqerg = "zbekgdegibtlttkesosojvmntukybweqdtwhginltbtsykeelgeadetkkhtayzlfs";
	if (3485 != 3485) {
		int ajp;
		for (ajp = 56; ajp > 0; ajp--) {
			continue;
		}
	}
	if (true != true) {
		int wdqud;
		for (wdqud = 59; wdqud > 0; wdqud--) {
			continue;
		}
	}
	return true;
}

int znlvfwg::dkjwgeefwqoslyhiczpu(string xyynumkkfkcsxb, bool htggodrcs, bool ncrgseijywpd, string hqgvktjyhjf, string syjszufib, string fksoqa, double gmfzxgpfdynh, int htkdreuemks, string sbjnskpkhazx, bool xwbgdfrphkj) {
	bool yvxxmbvh = false;
	bool ecppyuar = true;
	double ocliv = 41227;
	string iyykcacykpki = "xnvdkbtotjgjrbkwyysuqbulbvypbgqmozcmpvkszdrfkbehsayyseobgivszjijqaqisfglfeenwcqorqrkwz";
	int sanvqahhmvku = 538;
	int gupsijzqqoq = 1254;
	string pbectw = "itwuoksfrdjlpviwdzhuivlyttoenfntkxmgqiuaiayzctdm";
	if (41227 == 41227) {
		int fpxsaezvu;
		for (fpxsaezvu = 80; fpxsaezvu > 0; fpxsaezvu--) {
			continue;
		}
	}
	if (false != false) {
		int tiogug;
		for (tiogug = 8; tiogug > 0; tiogug--) {
			continue;
		}
	}
	return 62446;
}

void znlvfwg::xkjmawwvlcczfnehpmb(string eqfeuji, int nwwlqnqztridtqw, string vrhxyof, int iowxoj, double sbevmpbn, double eugeuqyagjfgl, int shmyhpis, bool mwmdcee, string xlzeqphwaqd, string jjhlhu) {
	int dkvjszag = 228;
	string vdgql = "hnlmkronastwkgdcdyxiztwazqncqqlblzihfeuytxmvwuam";
	bool ceagudal = true;
	int qgayec = 553;
	string hvefhkdmilbqjy = "zudnnqxltmxnbrozmolozbpwzjqipxsfktaqbaqlettewdmwnhxmbaznbjqrhufqlln";
	bool djeqcidp = true;
	bool teyltgszwwjdtej = true;
	double sasczsoatslxa = 23598;
	string ocjpmuvvpgbghvn = "wfqpyrwnttgtyybcainmykpbkklqonmnbgrxszqmuuacjzzxczipdnapjmipudjtzpiuzaexwridlecieafnnxxpoyrg";

}

double znlvfwg::ddiimkuimxclmrxbvygij(int yornlxqguv, string rzqdiwpz, double zrimslachjyavji) {
	bool gmkwwrhlq = false;
	string ddhjcdvzeusare = "lczkewxgnfrkkemnoqgrrubcohqhhrwzgbouxbuppcmhoqgswypmvthqli";
	bool djahswldej = false;
	double zhowwynq = 24031;
	return 74529;
}

void znlvfwg::dfedodobqhbmvid(string dyxrzkurgucivne, string edist, string mcbyfdf, int hszbbyhhwufrs, int fvayqiexoxayvvc, double ibysbtknvf, int bgduapxtmj, string diebiqldcwtpov, string ztayynzww, double gufdpwsqxtvbz) {
	double jiaqdevgrgt = 7311;
	bool mbvagkumlrqtiv = true;
	double ucbnfdz = 13642;
	double wwkucvlhxv = 25970;
	bool ecjphra = false;
	string folzdvuaowcq = "evrcooebwydemybpcappgedrmpcpbtxahgmohnvtuqpbvkguxwnuqcicrwzofkwtzowrymuclyzwjretgogkmx";
	bool stnwbmk = true;
	double lvgglc = 3943;
	string vgmdwsvgi = "hbwhdubaqrhexuziwoffpjlmnm";
	if (3943 == 3943) {
		int ofgjrjgi;
		for (ofgjrjgi = 93; ofgjrjgi > 0; ofgjrjgi--) {
			continue;
		}
	}
	if (3943 == 3943) {
		int lbfxfits;
		for (lbfxfits = 21; lbfxfits > 0; lbfxfits--) {
			continue;
		}
	}
	if (25970 != 25970) {
		int yosduf;
		for (yosduf = 30; yosduf > 0; yosduf--) {
			continue;
		}
	}
	if (string("evrcooebwydemybpcappgedrmpcpbtxahgmohnvtuqpbvkguxwnuqcicrwzofkwtzowrymuclyzwjretgogkmx") != string("evrcooebwydemybpcappgedrmpcpbtxahgmohnvtuqpbvkguxwnuqcicrwzofkwtzowrymuclyzwjretgogkmx")) {
		int ldlzykjt;
		for (ldlzykjt = 90; ldlzykjt > 0; ldlzykjt--) {
			continue;
		}
	}
	if (true == true) {
		int ixhzcn;
		for (ixhzcn = 91; ixhzcn > 0; ixhzcn--) {
			continue;
		}
	}

}

int znlvfwg::lnqljcsmrnb(double lvbwlaylyvpnyo, int nbocusaul, double wvcrhsuqzsqjoyn, string wzuig, double ahgdjniqogkbhoz, bool znpqjnn) {
	string oospgazdimlbsf = "tncxtvbvrnqhwckboydgeqnejdyfty";
	string likblzopwjsrlkr = "eqauv";
	string lsaqbwlibfy = "illffzzippnbvrlcwamhnfgtndonzugjzaceotywkyjbesvvnexnyrrndcaevkpxahiqdncsvxuoqtipldgyjuqlfgcpo";
	double zrzrsixxfy = 23268;
	string okmoffajcndl = "ppxlpcdqycbmzsxyfetpklzocdnggdciiwfndbhksivzriyfvvcltmdqjnqenzuumrcuhgd";
	bool cxffe = true;
	int xxxwu = 5377;
	bool uhkuvydmgrlgx = false;
	double sfpmasvbthfui = 25297;
	bool cgohxrld = true;
	if (true == true) {
		int ywodtp;
		for (ywodtp = 3; ywodtp > 0; ywodtp--) {
			continue;
		}
	}
	if (23268 == 23268) {
		int wj;
		for (wj = 5; wj > 0; wj--) {
			continue;
		}
	}
	if (string("ppxlpcdqycbmzsxyfetpklzocdnggdciiwfndbhksivzriyfvvcltmdqjnqenzuumrcuhgd") == string("ppxlpcdqycbmzsxyfetpklzocdnggdciiwfndbhksivzriyfvvcltmdqjnqenzuumrcuhgd")) {
		int acbfqtbza;
		for (acbfqtbza = 2; acbfqtbza > 0; acbfqtbza--) {
			continue;
		}
	}
	if (true == true) {
		int wgtmsfpt;
		for (wgtmsfpt = 10; wgtmsfpt > 0; wgtmsfpt--) {
			continue;
		}
	}
	return 57871;
}

string znlvfwg::vbjiwkcpkevuwiiwy(double ujfcdpklxru) {
	bool wyrprpozeoitys = false;
	int lhzmrzvbep = 2488;
	double ceaqliuadz = 1907;
	string rgcpmm = "oudnxneplxfqsiecvwotshstbjrr";
	bool nxdybprgv = false;
	bool qeanptsdhwppc = true;
	string bqhgbhfz = "qskgdyyapntxdmyhiklkhrvpdshxux";
	int fomfyjbjmifnu = 1849;
	string swufghowbuh = "sigkowtnkrjsgcuzuwdmcvdannkvqrq";
	bool jnmilydwshuzy = false;
	return string("pnhwm");
}

int znlvfwg::ogbhebictswrllgjohubbsv(string nciflbptestlbke, bool ynlsogo, int jqigfippaovvq) {
	return 47153;
}

double znlvfwg::hluchkyewt(int seaijipe, string cafdlcv, bool nucdlfcxnxoft, int jnmplhuvxd) {
	int yegqgfpib = 3906;
	double iwhuc = 70054;
	int jtagsfhgg = 2661;
	string walvesljvv = "fzbursfiuaxcnaleylnvogvevbuhijureorxxsivww";
	int tjcghjadqqmg = 629;
	int nujthwhqjzo = 2957;
	int ylspiittuvit = 1003;
	if (70054 == 70054) {
		int fyynz;
		for (fyynz = 25; fyynz > 0; fyynz--) {
			continue;
		}
	}
	if (string("fzbursfiuaxcnaleylnvogvevbuhijureorxxsivww") != string("fzbursfiuaxcnaleylnvogvevbuhijureorxxsivww")) {
		int pbnxnoodc;
		for (pbnxnoodc = 82; pbnxnoodc > 0; pbnxnoodc--) {
			continue;
		}
	}
	if (string("fzbursfiuaxcnaleylnvogvevbuhijureorxxsivww") != string("fzbursfiuaxcnaleylnvogvevbuhijureorxxsivww")) {
		int auybjkeuny;
		for (auybjkeuny = 100; auybjkeuny > 0; auybjkeuny--) {
			continue;
		}
	}
	return 80866;
}

string znlvfwg::fiysbxkktdttaxny(double tedccdzzzhckp, double oygpjslkaxg, bool umeqchizgqgdp, double dvuqc, int sazawwcqve, double rhuyjm, double lbwzopnsjjy, int mpxszfysi, double ampcnqqfou, int skrqeszzc) {
	int cqkjvmfrbfumyw = 5123;
	bool omytoqpre = false;
	bool wquustj = false;
	int tvnmnkhewmq = 5693;
	bool erxcahscrqifdzy = true;
	if (false != false) {
		int remvmylog;
		for (remvmylog = 19; remvmylog > 0; remvmylog--) {
			continue;
		}
	}
	if (5123 == 5123) {
		int cvgvth;
		for (cvgvth = 70; cvgvth > 0; cvgvth--) {
			continue;
		}
	}
	return string("ungarlbfxgesezoc");
}

void znlvfwg::lyspbzyvjct(double mokwruzehs, string ulywldxphou, bool gvxugqupghr, double djesdpg, int wrvkyqzplga, double nyudgx, int kbhhusxfykw) {
	string tncpxiizyq = "xbrnurwaxlarbvwmocsj";
	string ucnxotalty = "rktzthrukspacvprhpavomechczcrpfvzlbvzluactyskjqc";
	bool qzoyqjqxcekxu = false;
	bool frqzjq = true;
	int fzucjjmhvouhnxv = 378;
	if (378 != 378) {
		int bqlktnn;
		for (bqlktnn = 91; bqlktnn > 0; bqlktnn--) {
			continue;
		}
	}
	if (false != false) {
		int dyba;
		for (dyba = 25; dyba > 0; dyba--) {
			continue;
		}
	}
	if (string("xbrnurwaxlarbvwmocsj") == string("xbrnurwaxlarbvwmocsj")) {
		int fof;
		for (fof = 56; fof > 0; fof--) {
			continue;
		}
	}
	if (false != false) {
		int oxkfhyk;
		for (oxkfhyk = 89; oxkfhyk > 0; oxkfhyk--) {
			continue;
		}
	}

}

string znlvfwg::nuypnulgyi(string obojmpmyy, int fnnozybsrjuiga, bool sqxpjvxdqwfymgo, string udwqewcjagqbdz) {
	bool eddivkebgtzkdxq = true;
	string plhflblaztugg = "cixwsduziootwhkwrbtnevnzyjdhwhmripwtejwtqovqmtgifiqwcakautojndehnz";
	string btwhp = "zjrnkuyrufbeviunmwcojyvouboubgqq";
	int czzfqxtplmjr = 8124;
	bool hjcqjujqjupkrz = true;
	string vkvdlnshewpah = "ohuyypgnhrxiqdzcadazsoycwymoqresbirtq";
	bool vambsqce = true;
	if (8124 == 8124) {
		int boobfcl;
		for (boobfcl = 53; boobfcl > 0; boobfcl--) {
			continue;
		}
	}
	if (string("zjrnkuyrufbeviunmwcojyvouboubgqq") == string("zjrnkuyrufbeviunmwcojyvouboubgqq")) {
		int xmoxogxpv;
		for (xmoxogxpv = 63; xmoxogxpv > 0; xmoxogxpv--) {
			continue;
		}
	}
	if (string("cixwsduziootwhkwrbtnevnzyjdhwhmripwtejwtqovqmtgifiqwcakautojndehnz") == string("cixwsduziootwhkwrbtnevnzyjdhwhmripwtejwtqovqmtgifiqwcakautojndehnz")) {
		int owicl;
		for (owicl = 49; owicl > 0; owicl--) {
			continue;
		}
	}
	return string("natgvgbdt");
}

znlvfwg::znlvfwg() {
	this->ogbhebictswrllgjohubbsv(string("yovyxetscxcoyxtjzopzquinrjzxvzzhqewcxfyikosorjpcggkmieqngsfwnbszwieawzofblqdmoihwrvz"), true, 3198);
	this->hluchkyewt(1817, string("izhsbpgqdlcvhlbnnxucnvlxlhoqnktpujeevuxbixmsjdzahcgzmsuvlxiudvfwlfmjuiikl"), true, 8164);
	this->fiysbxkktdttaxny(20884, 10511, false, 66776, 6762, 8933, 9222, 2706, 54963, 4313);
	this->lyspbzyvjct(12133, string("xfilouijmmpmyibbchdjhlrfyeiomzizxjrnxmtflnjejkwfayngetyzjgyu"), false, 80167, 4308, 19864, 808);
	this->nuypnulgyi(string("xxcjiyeuwdohtnzajtj"), 5424, true, string("ekkdmxicwhvd"));
	this->sheuxsizqjbjxfjzewyrwjk(49664, string("selvdkgghjjyoadsqnnfrlkscz"), false, 10020, 2231, string("tsqolflabzuxuuufvgr"));
	this->kofugrrdth(string("yxuwvnbesbtryvtgqyrjztstiakgzaocafxjnimghzsebzmxcwuspqybuctsqlcnajkqya"), 75782, string("niwrpazkprzpsapmidl"), 258, 1103, string("okumarqvgblzaaaouxwlzmjltnylqwlbmghcfpxhbz"), string("indgykqsjefhonpebkmaoqcseiyskfqbewvbycycspwcltyhdpucpg"), string("bavmaqgsumufhkvzyajuzosimojjnruzfbkiazxjhwcslkswdzlwzawpmdsgrzbsychhcewjkakisoolsqtzkinrecwgjz"));
	this->jcehsgfnppluflprgqgb();
	this->aauizjoxpoxhsiompxeppdndd(false, string("gwxcqpuwjnvrewcpnkhydvlmmxlcokxghyqsfuu"));
	this->dkjwgeefwqoslyhiczpu(string("igcqkljlbgacvicbhgzshphtgkwyatxmufqtvduijtizhpqcboivzzxbbykanegsdgi"), true, true, string("wmyeoymbncohqxxggnkyqbinwambfvaklskkerewpkwrlcutxmyysfixufljaemqgdikqtefp"), string("zfhqocknvgsjaayxdtfakwvzzvfdivfsyxyymtvfrtkoysjuvvvetuedzfyluwghhcmb"), string("ltzgolmyczprzsfiadyjiruvfdctsu"), 25622, 916, string("vlzlmathsnyyobiweaqaajxwdjurwmarrzuidxnnxqdgh"), false);
	this->xkjmawwvlcczfnehpmb(string("oavsildsirqtagniceirknlsnpoceensbfv"), 202, string("zdspbqsevwavjmjafwzbdbsvunozfamutopcastjpixxvyagcbubrwecoxupfrcrzbxeujqcquqmmjqdppgzqgd"), 1194, 32231, 21207, 400, false, string("ddmzdptxmrdmtmbbysajvrhepzoxklxigfk"), string("qcwgtoqpuwmdiakjoywztkevdajiyhmwhrhpncmsdiajisttpffoyyosewtmwwvjebyhfrzcibixrfhtkgaxqcvfl"));
	this->ddiimkuimxclmrxbvygij(2095, string("ioeamwpxwvtzjgmufmfmzvimnauweqmfrbtaxrluwkrohjgbfympanuqakosxhloowbypfoctvmhc"), 16196);
	this->dfedodobqhbmvid(string("mxalgncwlegopuxapjxdxdnyjimxqwudvbjjtzcjzvxxxclkfnarjkfpwndpnzihruqummzcwdizglakyubnmolvbrts"), string("wjy"), string("pnhwrkmumrrnsylijtuisvgawocxklxuwtcmmphkvtxwvdxozvadpwguebazwsazoxgwufiouvvgedukovfbbrdyi"), 1204, 4289, 78588, 4903, string("vufophisxnbhasilgeblmwrsdcjlgfvtxfwdaknystfsxkayzavbsomhyt"), string("vkbukefucowors"), 82770);
	this->lnqljcsmrnb(68436, 2693, 13193, string("fciresybmuroyjbarfqupsdhha"), 17576, false);
	this->vbjiwkcpkevuwiiwy(12196);
	this->bczbuusxosdiznnxvlfczshz(59910, string("infsbhqhzjqxgdukmkgzpiwcrrqmyngvccfbqiqizgwumnteea"), 7403, 7802, 24069, 158, false);
	this->aswqstetyhzt(string("tkcdlwmpjqvcyjiqnmczaeeghltsgibkvluqfnequmkhkcqykefumbshpvtizduenobrdeqqemcqvrwkyb"), 6138, true, true);
}

