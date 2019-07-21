#include "EdgyLagComp.h"
#include "RageBot.h"

#define TICK_INTERVAL			(Interfaces::Globals->interval_per_tick)

#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

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

void BackTrack::Update(int tick_count)
{
	if (!Menu::Window.RageBotTab.AccuracyBacktracking.GetState())
		return;

	latest_tick = tick_count;
	for (int i = 0; i < 64; i++)
	{

		UpdateRecord(i);
	}
}

bool BackTrack::IsTickValid(int tick)
{

	int delta = latest_tick - tick;
	float deltaTime = delta * Interfaces::Globals->interval_per_tick;
	return (fabs(deltaTime) <= 0.2f);
}

void BackTrack::UpdateRecord(int i)
{
	IClientEntity* pEntityity = Interfaces::EntList->GetClientEntity(i);
	if (pEntityity && pEntityity->IsAlive() && !pEntityity->IsDormant())
	{
		float lby = pEntityity->GetLowerBodyYaw();
		if (lby != records[i].lby)
		{

			records[i].tick_count = latest_tick;
			records[i].lby = lby;
			records[i].headPosition = GetHitboxPosition(pEntityity, 0);
		}
	}
	else
	{

		records[i].tick_count = 0;
	}
}

bool BackTrack::RunLBYBackTrack(int i, CUserCmd* cmd, Vector& aimPoint)
{
	if (IsTickValid(records[i].tick_count))
	{
		aimPoint = records[i].headPosition;
		cmd->tick_count = records[i].tick_count;
		return true;
	}
	return false;
}

float BackTrack::GetLerpTime() {
	static ConVar* cl_interp = Interfaces::CVar->FindVar("cl_interp");
	static ConVar* cl_updaterate = Interfaces::CVar->FindVar("cl_updaterate");
	static ConVar* cl_interp_ratio = Interfaces::CVar->FindVar("cl_interp_ratio");
	static ConVar* sv_maxupdaterate = Interfaces::CVar->FindVar("sv_maxupdaterate");
	static ConVar* sv_minupdaterate = Interfaces::CVar->FindVar("sv_minupdaterate");
	static ConVar* sv_client_min_interp_ratio = Interfaces::CVar->FindVar("sv_client_min_interp_ratio");
	static ConVar* sv_client_max_interp_ratio = Interfaces::CVar->FindVar("sv_client_max_interp_ratio");
	auto Interp = cl_interp->GetFloat();
	auto UpdateRate = cl_updaterate->GetFloat();
	auto InterpRatio = static_cast<float>(cl_interp_ratio->GetInt());
	auto MaxUpdateRate = static_cast<float>(sv_maxupdaterate->GetInt());
	auto MinUpdateRate = static_cast<float>(sv_minupdaterate->GetInt());
	auto ClientMinInterpRatio = sv_client_min_interp_ratio->GetFloat();
	auto ClientMaxInterpRatio = sv_client_max_interp_ratio->GetFloat();
	if (ClientMinInterpRatio > InterpRatio)
		InterpRatio = ClientMinInterpRatio;
	if (InterpRatio > ClientMaxInterpRatio)
		InterpRatio = ClientMaxInterpRatio;
	if (MaxUpdateRate <= UpdateRate)
		UpdateRate = MaxUpdateRate;
	if (MinUpdateRate > UpdateRate)
		UpdateRate = MinUpdateRate;
	auto v20 = InterpRatio / UpdateRate;
	if (v20 <= Interp)
		return Interp;
	else
		return v20;
}

#define TICKS_TO_TIME(t) (Interfaces::Globals->interval_per_tick * (t))

bool BackTrack::good_tick(int tick)
{
	auto nci = Interfaces::Engine->GetNetChannelInfo();
	if (!nci) return false;

	float correct = clamp(nci->GetLatency(FLOW_OUTGOING) + GetLerpTime(), 0.f, 1.f);
	float delta_time = correct - (Interfaces::Globals->curtime - TICKS_TO_TIME(tick));
	return fabsf(delta_time) < 0.2f;
}

void BackTrack::legitBackTrack(CUserCmd* cmd, IClientEntity* pLocal)
{
	if (Menu::Window.LegitBotTab.AimbotBacktrack.GetState())
	{

		int bestTargetIndex = -1;
		int tickxd = Menu::Window.LegitBotTab.TickModulation.GetValue();
		float bestFov = FLT_MAX;
		player_info_t info;
		if (!pLocal->IsAlive())
			return;

		for (int i = 0; i < Interfaces::Engine->GetMaxClients(); i++)
		{
			auto entity = (IClientEntity*)Interfaces::EntList->GetClientEntity(i);

			if (!entity || !pLocal)
				continue;

			if (entity == pLocal)
				continue;

			if (!Interfaces::Engine->GetPlayerInfo(i, &info))
				continue;

			if (entity->IsDormant())
				continue;

			if (entity->GetTeamNum() == pLocal->GetTeamNum())
				continue;

			if (entity->IsAlive())
			{

				float simtime = entity->GetSimulationTime();
				Vector hitboxPos = GetHitboxPosition(entity, 0);

				//headPositions[i][cmd->command_number % 13] = backtrackData{ simtime, hitboxPos };
				headPositions[i][cmd->command_number % tickxd] = backtrackData{ simtime, hitboxPos };
				Vector ViewDir = AngleVector(cmd->viewangles + (pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f));
				float FOVDistance = DistPointToLine(hitboxPos, pLocal->GetEyePosition(), ViewDir);

				if (bestFov > FOVDistance)
				{
					bestFov = FOVDistance;
					bestTargetIndex = i;
				}
			}

		}

		float bestTargetSimTime;
		if (bestTargetIndex != -1)
		{
			float tempFloat = FLT_MAX;
			Vector ViewDir = AngleVector(cmd->viewangles + (pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f));
			for (int t = 0; t < 12; ++t)
			{
				float tempFOVDistance = DistPointToLine(headPositions[bestTargetIndex][t].hitboxPos, pLocal->GetEyePosition(), ViewDir);
				if (tempFloat > tempFOVDistance && headPositions[bestTargetIndex][t].simtime > pLocal->GetSimulationTime() - 1)
				{
					tempFloat = tempFOVDistance;
					bestTargetSimTime = headPositions[bestTargetIndex][t].simtime;
				}
			}
			if (cmd->buttons & IN_ATTACK)
			{

				cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);
			}
		}

	}

}

BackTrack* backtracking = new BackTrack();
backtrackData headPositions[64][12];