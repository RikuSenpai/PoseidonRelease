#include "laggycompensation.h"
#include "RageBot.h"

#define TICK_INTERVAL			(Interfaces::Globals->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME(t) (Interfaces::Globals->interval_per_tick * (t))

struct tick_record
{
	Vector m_vecOrigin;//0 //4 // 8
	bool bLowerBodyYawUpdated;
	bool bFakelagging;
	float m_flSimulationTime; //12
	float m_flAnimTime;
	int m_nSequence;//16
	float m_flCycle; //20
	Vector m_angEyeAngles;//x->24
	float m_flUpdateTime;//36
	Vector m_vecVelocity;//40 //44 //48
	float m_flPoseParameter[24];//52
	float backtrack_time;
	bool shot_in_that_record = false;
	bool needs_extrapolation = false;
	/*my addition*/
	matrix3x4 boneMatrix[128];
};
struct player_record
{
	//std::vector<tick_record> records;
	std::vector<tick_record> records;
	Vector EyeAngles;
	float LowerBodyYawTarget;
	int Flags;
	float unknown;
};

struct simulation_record
{
	Vector origin;
	Vector velocity;
	Vector acceleration;
	float simulation_time;
	float simulation_time_increasment_per_tick;
	float update_time;

};

CMBacktracking * cm_backtrack = new CMBacktracking();
template<class T, class U>
T CMBacktracking::clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

void LagRecord2::SaveRecord(IClientEntity* player)
{
	m_nFlags = player->getFlags();
	m_flSimulationTime = player->GetSimulationTime();
}
void LagRecord2::ExtrapolatePosition(IClientEntity* pPlayer, Vector& position, float &simtime, Vector velocity)
{
/*	auto AirAccelerate = [](IClientEntity* pPlayer, Vector &wishdir, float wishspeed, float accel, Vector &velo)
	{
		float wishspd = wishspeed;
		if (wishspd > 30.f)
			wishspd = 30.f;
		float currentspeed = DotProduct(pPlayer->GetVelocity(), wishdir);
		float addspeed = wishspd - currentspeed;
		if (addspeed <= 0)
			return;
		float accelspeed = accel * wishspeed * Interfaces::Globals->frametime * pPlayer->GetFriction();
		if (accelspeed > addspeed)
			accelspeed = addspeed;
		velo += accelspeed * wishdir;
	};
	auto GetSomeStrangeSimTime = [](simulation_record pre_latest_record, simulation_record latest_record, float difference) -> float
	{
		float v2; // ST00_4@1
		float v3; // xmm2_4@1
		float v4; // ST00_4@1
		float v5; // xmm2_4@1
		float result; // xmm0_4@2
		v2 = atan2(latest_record.velocity.x, latest_record.velocity.y);
		v3 = v2 * 57.295776;
		v4 = atan2(pre_latest_record.velocity.x, pre_latest_record.velocity.y);
		v5 = v3 - (v4 * 57.295776);
		if (v5 <= 180.0)
		{
			if (v5 < -180.0)
				v5 = v5 + 360.0;
			result = v5 / (difference/*latest_record.simulation_time - pre_latest_record.simulation_time*);
		}
		else
		{
			result = (v5 - 360.0) / (difference/*latest_record.simulation_time - pre_latest_record.simulation_time*);
		}
		return result;
	};
	int index = pPlayer->GetIndex();
	simulation_record latest_record = this->SimRecord[index][0];
	simulation_record pre_latest_record = this->SimRecord[index][1];
	int latency_ticks = GetLatencyTicks();
	Vector vel = velocity;
	float s_time = simtime;

	if (ragebot->cmd == nullptr)
		return;

	INetChannelInfo* nci = Interfaces::engine->GetNetChannelInfo();
	float latency = nci->GetAvgLatency(0) + nci->GetAvgLatency(1);
	float ticks_per_second = 1.0f / Interfaces::globals->interval_per_tick;
	float server_time = ((floorf(((latency)*ticks_per_second) + 0.5) + ragebot->cmd->tick_count + 1) * Interfaces::globals->interval_per_tick);
	float server_time_difference = server_time - s_time;
	if (server_time_difference > 1.0f)
		server_time_difference = 1.0f;
	float sim_time_difference = Interfaces::globals->curtime - latest_record.update_time;
	//proper method
	int ticks_choked = /*TIME_TO_TICKS(latest_record.simulation_time - pre_latest_record.simulation_time_increasment_per_tick);*TIME_TO_TICKS(sim_time_difference);
	if (ticks_choked <= 15)
	{
		latency_ticks = 1;
		if (ticks_choked < 1)
			ticks_choked = 1;
	}
	else
		ticks_choked = 15;
	int total_difference = floorf((server_time_difference * ticks_per_second) + 0.5) - ticks_choked;
	vel.z -= Interfaces::cvar->FindVar("sv_gravity")->GetFloat() * Interfaces::globals->interval_per_tick;
	Ray_t ray;
	trace_t tr;
	CTraceWorldOnly filter;
	if (total_difference < 0)
	{
		return;
	}
	else
	{
		do
		{
			int chokes = ticks_choked;
			//test, aw has that do statement
			do
			{
				ray.Init(position, position + (vel* Interfaces::globals->interval_per_tick), hackManager.pLocal()->GetMins(), hackManager.pLocal()->GetMaxs());
				Interfaces::trace->TraceRay(ray, MASK_SOLID, &filter, &tr);
				if (tr.fraction > 0)
				{
					position += vel * (Interfaces::globals->interval_per_tick);
					simtime += Interfaces::globals->interval_per_tick;
				}
				else
				{
					return;
				}

				chokes--;

			} while (chokes);
			total_difference -= ticks_choked;
		} while (total_difference >= 0);
	}
*/
}

void LagRecord2::UpdateExtrapolationRecords(IClientEntity* pPlayer)
{
/*	int index = pPlayer->GetIndex();
	if (pPlayer->GetSimulationTime() == this->SimRecord[index][0].simulation_time)
		return;
	for (int i = 7; i > 0; i--)
	{
		this->SimRecord[index][i].acceleration = this->SimRecord[index][i - 1].acceleration;
		this->SimRecord[index][i].origin = this->SimRecord[index][i - 1].origin;
		this->SimRecord[index][i].simulation_time = this->SimRecord[index][i - 1].simulation_time;
		this->SimRecord[index][i].update_time = this->SimRecord[index][i - 1].update_time;
		this->SimRecord[index][i].velocity = this->SimRecord[index][i - 1].velocity;
	}
	this->SimRecord[index][0].simulation_time = pPlayer->GetSimulationTime();
	this->SimRecord[index][0].update_time = Interfaces::globals->curtime;
	this->SimRecord[index][0].origin = pPlayer->GetOrigin();
	int lost_ticks = TIME_TO_TICKS(this->SimRecord[index][0].simulation_time) - TIME_TO_TICKS(this->SimRecord[index][1].simulation_time);
	this->SimRecord[index][0].simulation_time_increasment_per_tick = (this->SimRecord[index][0].simulation_time - this->SimRecord[index][1].simulation_time) / lost_ticks;
	/*calculate velocity by ourselves*
	Vector velocity = this->SimRecord[index][0].origin - this->SimRecord[index][1].origin;
	/*divide through lost ticks to get the velocity per tick
	velocity /= lost_ticks;
	this->SimRecord[index][0].velocity = pPlayer->GetVelocity();//velocity;*/
}
void CMBacktracking::AnimationFix(IClientEntity *player)
{
	auto &lag_records = this->m_LagRecord[player->GetIndex()];
	CMBacktracking::Get().RemoveBadRecords(player->GetIndex(), lag_records);
	auto entity = player;
	auto player_index = entity->GetIndex() - 1;
	if (lag_records.size() < 2)
		return;
	player->client_side_animation() = true;
	auto old_curtime = Interfaces::Globals->curtime;
	auto old_frametime = Interfaces::Globals->frametime;
	auto old_ragpos = entity->get_ragdoll_pos();

	Interfaces::Globals->curtime = entity->GetSimulationTime();
	Interfaces::Globals->frametime = Interfaces::Globals->interval_per_tick;

	auto player_animation_state = reinterpret_cast<DWORD*>(entity + 0x3884);
	auto player_model_time = reinterpret_cast<int*>(player_animation_state + 112);
	if (player_animation_state != nullptr && player_model_time != nullptr)
		if (*player_model_time == Interfaces::Globals->framecount)
			*player_model_time = Interfaces::Globals->framecount - 1;
	entity->get_ragdoll_pos() = old_ragpos;
	entity->UpdateClientSideAnimation();
	Interfaces::Globals->curtime = old_curtime;
	Interfaces::Globals->frametime = old_frametime;
	player->client_side_animation() = false;
}

float CMBacktracking::GetLerpTime()
{
	int ud_rate = Interfaces::CVar->FindVar("cl_updaterate")->GetInt();
	ConVar *min_ud_rate = Interfaces::CVar->FindVar("sv_minupdaterate");
	ConVar *max_ud_rate = Interfaces::CVar->FindVar("sv_maxupdaterate");

	if (min_ud_rate && max_ud_rate)
		ud_rate = max_ud_rate->GetInt();

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

bool CMBacktracking::IsTickValid(int tick)
{
	static auto sv_maxunlag = Interfaces::CVar->FindVar("sv_maxunlag");
	INetChannelInfo* nci = Interfaces::Engine->GetNetChannelInfo();

	if (!nci || !sv_maxunlag)
		return false;

	float correct = clamp(nci->GetLatency(FLOW_OUTGOING) + GetLerpTime(), 0.f, sv_maxunlag->GetFloat());
	float deltaTime = correct - (Interfaces::Globals->curtime - TICKS_TO_TIME(tick));
	return fabsf(deltaTime) < 0.2f;
}


bool CMBacktracking::IsPlayerValid(IClientEntity* player)
{
	if (!player)
		return false;
	if (!hackManager.pLocal())
		return false;
	if (!hackManager.pLocal()->IsAlive())
		return false;
	if (!player->IsPlayer())
		return false;

	if (player == hackManager.pLocal())
		return false;

	if (player->IsDormant())
		return false;

	if (!player->IsAlive())
		return false;

	if (player->GetTeamNum() == hackManager.pLocal()->GetTeamNum())
		return false;

	if (player->HasGunGameImmunity())
		return false;

	return true;
}


int CMBacktracking::GetPriorityLevel(IClientEntity *player, LagRecord2* lag_record)
{
	int priority = 0;
	return priority;
}

void CMBacktracking::FrameUpdatePostEntityThink()
{
	static auto sv_unlag = Interfaces::CVar->FindVar("sv_unlag");

	if (Interfaces::Globals->maxClients <= 1 || !sv_unlag)
	{
		CMBacktracking::Get().ClearHistory();
		return;
	}

	for (int i = 1; i <= Interfaces::Globals->maxClients; i++)
	{
		IClientEntity *player = Interfaces::EntList->GetClientEntity(i);

		auto &lag_records = this->m_LagRecord[i];

		if (!IsPlayerValid(player))
		{
			if (lag_records.size() > 0)
				lag_records.clear();

			continue;
		}

		int32_t ent_index = player->GetIndex();
		float_t sim_time = player->GetSimulationTime();

		LagRecord2 cur_lagrecord;

		RemoveBadRecords(ent_index, lag_records);

		if (lag_records.size() > 0)
		{
			auto &tail = lag_records.back();

			if (tail.m_flSimulationTime == sim_time)
				continue;
		}

		cur_lagrecord.SaveRecord(player); // first let's create the record

		if (!lag_records.empty()) // apply specific stuff that is needed
		{
			auto &temp_lagrecord = lag_records.back();
			RemoveBadRecords(ent_index, lag_records);
			int32_t priority_level = GetPriorityLevel(player, &temp_lagrecord);

			cur_lagrecord.m_iPriority = priority_level;

			RemoveBadRecords(ent_index, lag_records);

		}

		lag_records.emplace_back(cur_lagrecord);
	}
}
bool CMBacktracking::StartLagCompensation(IClientEntity* player)
{
	backtrack_records.clear();
	auto& m_LagRecords = this->m_LagRecord[player->GetIndex()];
	m_RestoreLagRecord[player->GetIndex()].second.SaveRecord(player);
	CMBacktracking::Get().RemoveBadRecords(player->GetIndex(), m_LagRecords);
	LagRecord2 newest_record = LagRecord2();
	for (auto it : m_LagRecords)
	{
		if (it.m_flSimulationTime > newest_record.m_flSimulationTime)
			newest_record = it;
		if (it.m_iPriority >= 1)
			backtrack_records.emplace_back(it);
	}
	backtrack_records.emplace_back(newest_record);
	std::sort(backtrack_records.begin(), backtrack_records.end(), [](LagRecord2 const &a, LagRecord2 const &b) { return a.m_iPriority > b.m_iPriority; });
	return backtrack_records.size() > 0;
}

void CMBacktracking::RemoveBadRecords(int Idx, std::deque<LagRecord2>& records)
{
	auto& m_LagRecords = records; // Should use rbegin but can't erase
	for (auto lag_record = m_LagRecords.begin(); lag_record != m_LagRecords.end(); lag_record++)
	{
		if (!IsTickValid(TIME_TO_TICKS(lag_record->m_flSimulationTime)))
		{
			m_LagRecords.erase(lag_record);
			if (!m_LagRecords.empty())
				lag_record = m_LagRecords.begin();
			else
				break;
		}
	}
}

bool CMBacktracking::FindViableRecord(IClientEntity *player, LagRecord2* record)
{
	return true;
}

void CMBacktracking::FinishLagCompensation(IClientEntity *player)
{
	int idx = player->GetIndex();
	player->SetFlags(m_RestoreLagRecord[idx].second.m_nFlags);
}
