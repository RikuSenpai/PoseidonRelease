#pragma once

#include "Hacks.h"
extern float lineLBY;
extern float lineRealAngle;
extern float lineFakeAngle;

class CEsp : public CHack
{
public:
	void Init();
	void Draw();
	void Move(CUserCmd *pCmd, bool &bSendPacket);
private:

	IClientEntity *BombCarrier;

	struct ESPBox
	{
		int x, y, w, h;
	};

	void DrawPlayer(IClientEntity* pEntityity, player_info_t pinfo);
	void NewNadePred();
	void DrawCircle1(IClientEntity * pEntity);
	void DefuseWarning(IClientEntity * pEntity);
	void drawBacktrackedSkelet(IClientEntity * base, player_info_t pinfo);
	void DrawAutowallCrosshair();
	Color GetPlayerColor(IClientEntity* pEntityity);
	void DrawLinesAA(Color color);
	bool GetBox(IClientEntity* pEntityity, ESPBox &result);
	void DrawBox(ESPBox size, Color color);
	void Barrel(CEsp::ESPBox size, Color color, IClientEntity * pEntityity);
	void DrawSkeleton2(IClientEntity * pEntityity);
	void DrawWeapon(IClientEntity * pEntityity, CEsp::ESPBox size);
	void Ammo(IClientEntity * pEntityity, player_info_t pinfo, CEsp::ESPBox size);
	void EntityGlow();
	void BacktrackingCross(IClientEntity * pEntityity);
	void BombTimer(IClientEntity * pEntityity, ClientClass * cClass);
	CEsp::ESPBox GetBOXX(IClientEntity * pEntityity);
	void DrawHealth(IClientEntity * pEntityity, CEsp::ESPBox size);
	void AmmoBar(IClientEntity* pEntity, CEsp::ESPBox size);
	void DrawName(player_info_t pinfo, ESPBox size);
	void DrawInfo2(IClientEntity * pEntityity, CEsp::ESPBox size);
	void DrawCross(IClientEntity* pEntityity);
	void DrawSkeleton(IClientEntity* pEntityity);
	void DrawMoney(IClientEntity * pEntityity, CEsp::ESPBox size);
	void Armor(IClientEntity * pEntityity, CEsp::ESPBox size);
	void BoxAndText(IClientEntity * entity, std::string text);
	void DrawThrowable(IClientEntity * throwable);
	void DrawDrop(IClientEntity* pEntityity, ClientClass* cClass);
	void DrawBombPlanted(IClientEntity* pEntityity, ClientClass* cClass);
	void DrawBomb(IClientEntity* pEntityity, ClientClass* cClass);
}; extern CEsp c_esp;

