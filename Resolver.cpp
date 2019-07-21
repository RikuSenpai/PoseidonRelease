#include "Resolver.h"
#include "Ragebot.h"
#include "Hooks.h"
#include "EdgyLagComp.h"
//BackTrack* backtracking = new BackTrack();

void LowerBodyYawFix(IClientEntity* pEntity)
{
	if (Menu::Window.RageBotTab.LBYCorrection.GetState())
	{
		if (!pEntity) return;
		if (pEntity->GetClientClass()->m_ClassID != (int)CSGOClassID::CCSPlayer) return;
		if (!pEntity->IsAlive() || !pEntity->GetActiveWeaponHandle()) return;
		if (Interfaces::Engine->GetLocalPlayer()) return;

		auto EyeAngles = pEntity->GetEyeAnglesXY();
		if (pEntity->GetVelocity().Length() > 1 && (pEntity->GetFlags() & (int)pEntity->GetFlags() & FL_ONGROUND))
			EyeAngles->y = pEntity->GetLowerBodyYaw();
	}
}

C_BaseCombatWeapon* IClientEntity::GetWeapon2()
{
	static int iOffset = GET_NETVAR("DT_BaseCombatCharacter", "m_hActiveWeapon");
	ULONG pWeepEhandle = *(PULONG)((DWORD)this + iOffset);
	return (C_BaseCombatWeapon*)(Interfaces::EntList->GetClientEntityFromHandle((HANDLE)pWeepEhandle));
}

int IClientEntity::get_choked_ticks()
{
	if (!this || !this->IsAlive())
		return 0;

	float flSimulationTime = this->GetSimulationTime();
	float flOldSimulationTime = this->m_flOldSimulationTime();
	float flSimDiff = flSimulationTime - flOldSimulationTime;

	return TIME_TO_TICKS(max(0, flSimDiff /*- latency*/));
}

int IClientEntity::sequence_activity(IClientEntity* pEntity, int sequence)
{
	const model_t* pModel = pEntity->GetModel();
	if (!pModel)
		return 0;

	auto hdr = Interfaces::ModelInfo->GetStudiomodel(pEntity->GetModel());

	if (!hdr)
		return -1;

	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Utilities::Memory::FindPatternV2("client_panorama.dll", "55 8B EC 53 8B 5D 08 56 8B F1 83"));

	return get_sequence_activity(pEntity, hdr, sequence);
}

bool IClientEntity::IsKnifeorNade()
{
	if (!this)
		return false;
	if (!this->IsAlive())
		return false;

	C_BaseCombatWeapon *pWeapon = (C_BaseCombatWeapon*)this->GetWeapon2();

	if (!pWeapon)
		return false;

	std::string WeaponName = pWeapon->GetName();

	if (WeaponName == "weapon_knife")
		return true;
	else if (WeaponName == "weapon_incgrenade")
		return true;
	else if (WeaponName == "weapon_decoy")
		return true;
	else if (WeaponName == "weapon_flashbang")
		return true;
	else if (WeaponName == "weapon_hegrenade")
		return true;
	else if (WeaponName == "weapon_smokegrenade")
		return true;
	else if (WeaponName == "weapon_molotov")
		return true;

	return false;
}

float NormalizeYaw180(float yaw)
{
	if (yaw > 180)
		yaw -= (round(yaw / 360) * 360.f);
	else if (yaw < -180)
		yaw += (round(yaw / 360) * -360.f);

	return yaw;
}

float approach(float cur, float target, float inc) {
	inc = abs(inc);

	if (cur < target)
		return min(cur + inc, target);
	if (cur > target)
		return max(cur - inc, target);

	return target;
}

float angle_difference(float a, float b) {
	auto diff = NormalizeYaw180(a - b);

	if (diff < 180)
		return diff;
	return diff - 360;
}

float approach_angle(float cur, float target, float inc) {
	auto diff = angle_difference(target, cur);
	return approach(cur, cur + diff, inc);
}

CRageBot * c_ragebot;

#define M_PI 3.14159265358979323846


Vector CalcAngleToEnt(const Vector& vecSource, const Vector& vecDestination)
{
	Vector qAngles;
	Vector delta = Vector((vecSource[0] - vecDestination[0]), (vecSource[1] - vecDestination[1]), (vecSource[2] - vecDestination[2]));
	float hyp = sqrtf(delta[0] * delta[0] + delta[1] * delta[1]);
	qAngles[0] = (float)(atan(delta[2] / hyp) * (180.0f / M_PI));
	qAngles[1] = (float)(atan(delta[1] / delta[0]) * (180.0f / M_PI));
	qAngles[2] = 0.f;
	if (delta[0] >= 0.f)
		qAngles[1] += 180.f;

	return qAngles;
}



extra s_extra;
bool is_slow_walking(IClientEntity* entity) {
	float velocity_2D[64], old_velocity_2D[64];

	if (entity->GetVelocity().Length2D() != velocity_2D[entity->GetIndex()] && entity->GetVelocity().Length2D() != NULL) {
		old_velocity_2D[entity->GetIndex()] = velocity_2D[entity->GetIndex()];
		velocity_2D[entity->GetIndex()] = entity->GetVelocity().Length2D();
	}

	if (velocity_2D[entity->GetIndex()] > 0.1) {
		int tick_counter[64];

		if (velocity_2D[entity->GetIndex()] == old_velocity_2D[entity->GetIndex()])
			++tick_counter[entity->GetIndex()];
		else
			tick_counter[entity->GetIndex()] = 0;

		while (tick_counter[entity->GetIndex()] > (1 / Interfaces::Globals->interval_per_tick) * fabsf(0.1f))// should give use 100ms in ticks if their speed stays the same for that long they are definetely up to something..
			return true;

	}
	return false;
}

void ABS(IClientEntity * player)
{
	CUserCmd* pCmd;
	for (int i = 0; i < Interfaces::Engine->GetMaxClients(); ++i)
	{
		IClientEntity* pLocal = hackManager.pLocal();
		IClientEntity *player = (IClientEntity*)Interfaces::EntList->GetClientEntity(i);

		if (!player || player->IsDormant() || player->GetHealth() < 1 || (DWORD)player == (DWORD)pLocal)
			continue;

		if (!player)
			continue;

		if (pLocal)
			continue;
		
			if (pLocal && player && pLocal->IsAlive())
			{
				if (Menu::Window.RageBotTab.AdvancedResolver.GetState())
				{
					Vector* eyeAngles = player->GetEyeAnglesXY();
					if (eyeAngles->x < -179.f) eyeAngles->x += 360.f;
					else if (eyeAngles->x > 90.0 || eyeAngles->x < -90.0) eyeAngles->x = 89.f;
					else if (eyeAngles->x > 89.0 && eyeAngles->x < 91.0) eyeAngles->x -= 90.f;
					else if (eyeAngles->x > 179.0 && eyeAngles->x < 181.0) eyeAngles->x -= 180;
					else if (eyeAngles->x > -179.0 && eyeAngles->x < -181.0) eyeAngles->x += 180;
					else if (fabs(eyeAngles->x) == 0) eyeAngles->x = std::copysign(89.0f, eyeAngles->x);
				}
			}
	}
}

void preso(IClientEntity * player)
{
	switch (Menu::Window.RageBotTab.preso.GetIndex())
	{
	case 1:
	{
		player->GetEyeAnglesXY()->x = 89;
		//	resolver->resolved_pitch = 89.f;
	}
	break;
	case 2:
	{
		player->GetEyeAnglesXY()->x = -89;
		//	resolver->resolved_pitch = -89.f;
	}
	break;
	case 3:
	{
		player->GetEyeAnglesXY()->x = 0;
		//	resolver->resolved_pitch = 0.f;
	}
	break;
	}

}


ResolverSetup * resolver = new ResolverSetup();

int total_missed[64];

void ResolverSetup::Resolve(IClientEntity* pEntity)
{
	bool MeetsLBYReq;
	if (pEntity->GetFlags() & FL_ONGROUND)
		MeetsLBYReq = true;
	else
		MeetsLBYReq = false;

	bool IsMoving;
	if (pEntity->GetVelocity().Length2D() >= 0.5)
		IsMoving = true;
	else
		IsMoving = false;

	if (is_slow_walking(pEntity))
	{
		s_extra.current_flag[pEntity->GetIndex()] = correction_flags::SLOW_WALK;
		resolver->enemy_slowwalk = true;
	}
	else

	ResolverSetup::NewANgles[pEntity->GetIndex()] = *pEntity->GetEyeAnglesXY();
	ResolverSetup::newlby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
	ResolverSetup::newsimtime = pEntity->GetSimulationTime();
	ResolverSetup::newdelta[pEntity->GetIndex()] = pEntity->GetEyeAnglesXY()->y;
	ResolverSetup::newlbydelta[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
	ResolverSetup::finaldelta[pEntity->GetIndex()] = ResolverSetup::newdelta[pEntity->GetIndex()] - ResolverSetup::storeddelta[pEntity->GetIndex()];
	ResolverSetup::finallbydelta[pEntity->GetIndex()] = ResolverSetup::newlbydelta[pEntity->GetIndex()] - ResolverSetup::storedlbydelta[pEntity->GetIndex()];
	if (newlby == storedlby)
		ResolverSetup::lbyupdated = false;
	else
		ResolverSetup::lbyupdated = true;

	if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 0)
	{

	}
	else if (Menu::Window.RageBotTab.AimbotResolver.GetIndex() == 1)
	{
		//Removed for leak
		//Use mirrors or whatever /shrug
	}

		ABS(pEntity);
	}


void ResolverSetup::StoreFGE(IClientEntity* pEntity)
{
	ResolverSetup::storedanglesFGE = pEntity->GetEyeAnglesXY()->y;
	ResolverSetup::storedlbyFGE = pEntity->GetLowerBodyYaw();
	ResolverSetup::storedsimtimeFGE = pEntity->GetSimulationTime();
}

void ResolverSetup::StoreThings(IClientEntity* pEntity)
{
	ResolverSetup::StoredAngles[pEntity->GetIndex()] = *pEntity->GetEyeAnglesXY();
	ResolverSetup::storedlby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
	ResolverSetup::storedsimtime = pEntity->GetSimulationTime();
	ResolverSetup::storeddelta[pEntity->GetIndex()] = pEntity->GetEyeAnglesXY()->y;
	ResolverSetup::storedlby[pEntity->GetIndex()] = pEntity->GetLowerBodyYaw();
}

void ResolverSetup::CM(IClientEntity* pEntity)
{
	for (int x = 1; x < Interfaces::Engine->GetMaxClients(); x++)
	{

		pEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(x);

		if (!pEntity
			|| pEntity == hackManager.pLocal()
			|| pEntity->IsDormant()
			|| !pEntity->IsAlive())
			continue;

		ResolverSetup::StoreThings(pEntity);
	}
}

void ResolverSetup::FSN(IClientEntity* pEntity, ClientFrameStage_t stage)
{
	if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		for (int i = 1; i < Interfaces::Engine->GetMaxClients(); i++)
		{

			pEntity = (IClientEntity*)Interfaces::EntList->GetClientEntity(i);

			if (!pEntity
				|| pEntity == hackManager.pLocal()
				|| pEntity->IsDormant()
				|| !pEntity->IsAlive())
				continue;

			ResolverSetup::Resolve(pEntity);
		}
	}
}