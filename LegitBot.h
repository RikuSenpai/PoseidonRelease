#pragma once

#include "Hacks.h"

class CLegitBot : public CHack
{
public:

	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool& bSendPacket);
private:
	// Targetting
	int GetTargetCrosshair();
	bool TargetMeetsRequirements(IClientEntity* pEntityity);
	bool TargetMeetsTriggerRequirements(IClientEntity* pEntityity);
	float FovToPlayer(Vector ViewOffSet, Vector View, IClientEntity* pEntityity, int HitBox);
	bool AimAtPoint(IClientEntity* pLocal, Vector point, CUserCmd *pCmd);

	void SyncWeaponSettings();

	// Functionality
	void DoAimbot(CUserCmd *pCmd);
	void DoTrigger(CUserCmd *pCmd);

	// Aimbot
	bool IsLocked;
	int TargetID;
	int HitBox;
	Vector AimPoint;
	bool Multihitbox;
	float StartAim;
	float Aimtime;
	int besthitbox;

	float Speed;
	float FoV;
	float RecoilControl;

	bool shoot;

};

class LegitAA
{
public:
	void LegitAA2(CUserCmd * cmd, bool moving);
}; extern LegitAA * legitaa;