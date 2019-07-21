#pragma once

#include "Hacks.h"

Vector GetAutostrafeView();

class CMiscHacks : public CHack
{
public:

	void Init();
	void Draw();
	void MinimalWalk(CUserCmd * cmd, float speed);
	void Move(CUserCmd *pCmd, bool &bSendPacket);
	float get_gun(C_BaseCombatWeapon* weapon);
	int GetFPS();
private:
	void AutoPistol(CUserCmd * pCmd);
	void SetClanTag(const char * tag, const char * name);
	void Namespam();
	void NoName();
	void NameSteal();
	void Crasher();
	void ChatSpamInterwebz();
	void AutoJump(CUserCmd *pCmd);
	void LegitStrafe(CUserCmd *pCmd);
	void RageStrafe(CUserCmd *pCmd);
	int CircleFactor = 0;
}; extern CMiscHacks * misc_hacks;



