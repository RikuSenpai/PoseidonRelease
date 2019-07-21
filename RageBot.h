#pragma once

#include "Hacks.h"

class CRageBot : public CHack
{
public:
	void Init();
	bool enemy_is_slow_walking(IClientEntity * entity);
	void Draw();
	bool can_autowall = false;
	void Move(CUserCmd *pCmd, bool &bSendPacket);
private:
	int GetTargetCrosshair();
	int GetTargetDistance();
	int GetTargetNextShot();
	int GetTargetThreat(CUserCmd * pCmd);
	int GetTargetHealth();
//	bool EmmaIsCuteAndILoveHer(IClientEntity * pEntity, Vector & point, int & dmg);
	bool TargetMeetsRequirements(IClientEntity* pEntityity);
	void aimAtPlayer(CUserCmd * pCmd);
	float FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntityity, int HitBox);
	int HitScan(IClientEntity* pEntityity);
	bool AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd, bool &bSendPacket);

	void delay_shot(IClientEntity * entity, CUserCmd * pcmd);

	bool can_shoot = false;
	bool shot_this_tick;
	bool there_is_a_target = false;

	void DoAimbot(CUserCmd *pCmd, bool &bSendPacket);
	void DoNoRecoil(CUserCmd *pCmd);
	void DoAntiAim(CUserCmd *pCmd, bool &bSendPacket);

	bool stand_done = false;
	bool move_done = false;


	bool IsAimStepping;
	Vector LastAimstepAngle;
	Vector LastAngle;

	bool IsLocked;
	int TargetID;
	int HitBox;
	Vector AimPoint;
}; extern CRageBot * c_ragebot;

class antiaim_helper
{
public:

	void anti_lby(CUserCmd * cmd, bool & bSendPacket);

	float fov_ent(Vector ViewOffSet, Vector View, IClientEntity * entity, int hitbox);

	int closest();

	void yes();

private:
//	int closest_entity = 0;
	float NextPredictedLBYUpdate = 0.f;
};
extern antiaim_helper * c_helper;