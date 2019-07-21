#include "ESP.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "GlowManager.h"
#include "Autowall.h"
#include <stdio.h>
#include <stdlib.h>
#include "EdgyLagComp.h"
#include "Hooks.h"

DWORD GlowManager = *(DWORD*)(Utilities::Memory::FindPatternV2("client_panorama.dll", "0F 11 05 ?? ?? ?? ?? 83 C8 01 C7 05 ?? ?? ?? ?? 00 00 00 00") + 3);

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


void CEsp::Init()
{
	BombCarrier = nullptr;
}

void CEsp::Move(CUserCmd *pCmd,bool &bSendPacket) 
{

}

void CEsp::Draw()
{
	if (!Interfaces::Engine->IsConnected() || !Interfaces::Engine->IsInGame())
		return;


	IClientEntity *pLocal = hackManager.pLocal();

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{
		IClientEntity *pEntityity = Interfaces::EntList->GetClientEntity(i);
		player_info_t pinfo;

		if (pEntityity && !pEntityity->IsDormant())
		{

			if (Menu::Window.VisualsTab.OtherRadar.GetState())
			{

				DWORD m_bSpotted = NetVar.GetNetVar(0x839EB159);
				*(char*)((DWORD)(pEntityity)+m_bSpotted) = 1;
			}

			if (Menu::Window.VisualsTab.FiltersPlayers.GetState() && Interfaces::Engine->GetPlayerInfo(i, &pinfo) && pEntityity->IsAlive())
			{

				DrawPlayer(pEntityity, pinfo);
			}

			ClientClass* cClass = (ClientClass*)pEntityity->GetClientClass();

			if (Menu::Window.VisualsTab.FiltersNades.GetState() && strstr(cClass->m_pNetworkName, "Projectile"))
			{

				DrawThrowable(pEntityity);
			}

			if (Menu::Window.VisualsTab.FiltersWeapons.GetState() && cClass->m_ClassID != (int)CSGOClassID::CBaseWeaponWorldModel && ((strstr(cClass->m_pNetworkName, "Weapon") || cClass->m_ClassID == (int)CSGOClassID::CDEagle || cClass->m_ClassID == (int)CSGOClassID::CAK47)))
			{

				DrawDrop(pEntityity, cClass);
			}

			if (Menu::Window.VisualsTab.FiltersC4.GetState())
			{
				if (cClass->m_ClassID == (int)CSGOClassID::CPlantedC4)
					DrawBombPlanted(pEntityity, cClass);

				if (cClass->m_ClassID == (int)CSGOClassID::CPlantedC4)
					BombTimer(pEntityity, cClass);

				if (cClass->m_ClassID == (int)CSGOClassID::CC4)
					DrawBomb(pEntityity, cClass);
			}
		}

	}

	if (Menu::Window.VisualsTab.OtherNoFlash.GetState())
	{
		DWORD m_flFlashMaxAlpha = NetVar.GetNetVar(0xFE79FB98);
		*(float*)((DWORD)pLocal + m_flFlashMaxAlpha) = 0;
	}

	if (Menu::Window.VisualsTab.OptionsGlow.GetState())
	{
//		Glow();
	}

	if (Menu::Window.VisualsTab.EntityGlow.GetState())
	{
		EntityGlow();
	}
}

void CEsp::AmmoBar(IClientEntity* pEntity, CEsp::ESPBox size)
{
	IClientEntity* pWeapon = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)pEntity->GetActiveWeaponHandle());
	CSWeaponInfo* weapInfo = ((C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pEntity->GetActiveWeaponHandle()))->GetCSWpnData();


	ESPBox ArBar = size;
	ArBar.y += (ArBar.h + 3);
	ArBar.h = 6;

	float ArValue = pWeapon->GetAmmoInClip();
	float ArPerc = ArValue / weapInfo->max_clip;
	float Width = (size.w * ArPerc);
	ArBar.w = Width;


	//int player_armor = pEntity->ArmorValue() > 100 ? 100 : pEntity->ArmorValue();

	if (pWeapon) {
		Color arc = Color(86, 123, 190, 255);


		Render::Outline(size.x - 1, size.y + size.h + 2, size.w + 2, 4, Color(21, 21, 21, 200));

		int armor_width = Width;

		Render::DrawRect(size.x, size.y + size.h + 3, size.w, 2, Color(21, 21, 21, 240));
		Render::DrawRect(size.x, size.y + size.h + 3, armor_width, 2, arc);
	}
}

void CEsp::DrawLinesAA(Color color)
{
	if (Menu::Window.VisualsTab.AALines.GetState())
	{
		IClientEntity *pLocal = hackManager.pLocal();

		Vector src3D, dst3D, forward, src, dst;
		trace_t tr;
		Ray_t ray;
		CTraceFilter filter;

		filter.pSkip = hackManager.pLocal();

		AngleVectors(QAngle(0, pLocal->GetLowerBodyYaw(), 0), &forward);
		src3D = hackManager.pLocal()->GetOrigin();
		dst3D = src3D + (forward * 35.f); //replace 50 with the length you want the line to have

		ray.Init(src3D, dst3D);

		Interfaces::Trace->TraceRay(ray, 0, &filter, &tr);

		if (!Render::WorldToScreen(src3D, src) || !Render::WorldToScreen(tr.endpos, dst))
			return;

		Render::Line(src.x, src.y, dst.x, dst.y, Color(255, 165, 0, 255));
		//Render::Text(dst.x, dst.y, Color(255, 165.f, 0, 255), Render::Fonts::ESP, "lby");

		AngleVectors(QAngle(0, lineRealAngle, 0), &forward);
		dst3D = src3D + (forward * 40.f); //replace 50 with the length you want the line to have

		ray.Init(src3D, dst3D);

		Interfaces::Trace->TraceRay(ray, 0, &filter, &tr);

		if (!Render::WorldToScreen(src3D, src) || !Render::WorldToScreen(tr.endpos, dst))
			return;

		Render::Line(src.x, src.y, dst.x, dst.y, Color(0, 255, 0, 255));
		//Render::Text(dst.x, dst.y, Color(0, 255.f, 0, 255), Render::Fonts::ESP, "real");

		AngleVectors(QAngle(0, lineFakeAngle, 0), &forward);
		dst3D = src3D + (forward * 45.f); //replace 50 with the length you want the line to have

		ray.Init(src3D, dst3D);

		Interfaces::Trace->TraceRay(ray, 0, &filter, &tr);

		if (!Render::WorldToScreen(src3D, src) || !Render::WorldToScreen(tr.endpos, dst))
			return;

		Render::Line(src.x, src.y, dst.x, dst.y, Color(255, 0, 0, 255));
		//Render::Text(dst.x, dst.y, Color(255, 0.f, 0, 255), Render::Fonts::ESP, "fake");
	}
}

void CEsp::DrawPlayer(IClientEntity* pEntityity, player_info_t pinfo) //ESP Loop
{

	ESPBox Box;
	Color Color;

	IClientEntity *pLocal = hackManager.pLocal();

	Vector max = pEntityity->GetCollideable()->OBBMaxs();
	Vector pos, pos3D;
	Vector top, top3D;
	pos3D = pEntityity->GetOrigin();
	top3D = pos3D + Vector(0, 0, max.z);

	if (!Render::WorldToScreen(pos3D, pos) || !Render::WorldToScreen(top3D, top))
		return;

	if (Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() && (pEntityity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()))
		return;

	if (!Menu::Window.VisualsTab.FiltersSelf.GetState() && pEntityity == hackManager.pLocal())
		return;

	if (GetBox(pEntityity, Box))
	{
		Color = GetPlayerColor(pEntityity);

		if (Menu::Window.VisualsTab.OptionsBox.GetState())
			DrawBox(Box, Color);

		if (Menu::Window.VisualsTab.OptionsName.GetState())
			DrawName(pinfo, Box);

		if (Menu::Window.VisualsTab.OptionHealthEnable.GetState())
			DrawHealth(pEntityity, Box);

		if (Menu::Window.VisualsTab.OptionsInfo2.GetState() || Menu::Window.VisualsTab.OptionsWeapon.GetState())
			DrawInfo2(pEntityity, Box);

		if (Menu::Window.VisualsTab.OptionsArmor.GetState())
			Armor(pEntityity, Box);

		if (Menu::Window.VisualsTab.Barrels.GetState())
			Barrel(Box, Color, pEntityity);

		if (Menu::Window.VisualsTab.OptionsAimSpot.GetState())
			DrawCross(pEntityity);

		if (Menu::Window.VisualsTab.OptionsSkeleton.GetState())
			DrawSkeleton(pEntityity);

		if (Menu::Window.VisualsTab.OptionsSkeleton2.GetState())
			DrawSkeleton2(pEntityity);

		if (Menu::Window.VisualsTab.AimbotTarget.GetState())
			DrawCircle1(pEntityity);

		if (Menu::Window.VisualsTab.BacktrackingLol.GetState())
			BacktrackingCross(pEntityity);

		if (pLocal->IsAlive())
			if (Menu::Window.VisualsTab.Grenades2.GetState())
				NewNadePred();

		if (Menu::Window.VisualsTab.DrawMoney.GetState())
			DrawMoney(pEntityity, Box);

		if (hackManager.pLocal()->IsAlive()) {
			CEsp::DrawLinesAA(Color);
		}

		if (Menu::Window.VisualsTab.DefuseWarning.GetState())
			DefuseWarning(pEntityity);

		if (Menu::Window.VisualsTab.Ammo.GetState())
			AmmoBar(pEntityity, Box);


	}
}

void CEsp::NewNadePred()
{

}

void CEsp::DrawInfo2(IClientEntity* pEntity, CEsp::ESPBox size)
{
	int i = 0;
	std::vector<std::string> Info;

	IClientEntity* pWeapon = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)pEntity->GetActiveWeaponHandle());
	if (Menu::Window.VisualsTab.OptionsWeapon.GetState())
	{

		if (Menu::Window.VisualsTab.OptionsArmor.GetState())
		{
			RECT nameSize = Render::GetTextSize(Render::Fonts::PESP, pWeapon->GetWeaponName());
			Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y + size.h + 5,
				Color(255, 255, 255, 255), Render::Fonts::PESP, pWeapon->GetWeaponName());
		}
		else
		{
			RECT nameSize = Render::GetTextSize(Render::Fonts::PESP, pWeapon->GetWeaponName());
			Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y + size.h,
				Color(255, 255, 255, 255), Render::Fonts::PESP, pWeapon->GetWeaponName());
		}
	}

	if (Menu::Window.VisualsTab.OptionsInfo2.GetState() && pEntity == BombCarrier)
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
		int i = 0;
		Render::Text(size.x + size.w + 3, size.y + (i*(nameSize.bottom + 10) + 18), Color(255, 255, 0, 255), Render::Fonts::xsESP, "BOMB");
		i++;
	}

	if (Menu::Window.VisualsTab.OptionsInfo2.GetState() && pEntity->IsScoped())
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
		int i = 0;
		Render::Text(size.x + size.w + 3, size.y + (i*(nameSize.bottom + 10) + 18), Color(0, 51, 102, 255), Render::Fonts::xsESP, "ZOOM");
	}

	if (Menu::Window.VisualsTab.OptionsInfo2.GetState())
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
		int i = 0;
		if (pEntity->GetVelocity().Length2D() < 36 && pEntity->GetVelocity().Length2D() > 20)
			Render::Text(size.x + size.w + 3, size.y + (i*(nameSize.bottom + 4)), Color(255, 0, 100, 255), Render::Fonts::xsESP, "FAKE");
	}


	if (Menu::Window.VisualsTab.OptionsInfo2.GetState())
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
		int i = 0;
		if (pEntity->GetVelocity().Length2D() < 1)
			Render::Text(size.x + size.w + 3, size.y + (i*(nameSize.bottom + 4)), Color(255, 255, 255, 255), Render::Fonts::xsESP, "FAKE");
	}


	if (Menu::Window.VisualsTab.OptionsInfo2.GetState() && pEntity->HasHelmet())
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
		int i = 0;

		if (pEntity->ArmorValue() > 0)
		{
			char hp[50];
			sprintf_s(hp, sizeof(hp), "%i", pEntity->ArmorValue());

			if (pEntity->HasHelmet())
				Render::Text(size.x + size.w + 3, size.y + (i*(nameSize.bottom + 10) + 9), Color(255, 255, 255, 255), Render::Fonts::xsESP, "HK");
			else
				Render::Text(size.x + size.w + 3, size.y + (i*(nameSize.bottom + 10) + 9), Color(255, 255, 255, 255), Render::Fonts::xsESP, " K");
		}
	}

	if (Menu::Window.VisualsTab.OptionsInfo2.GetState() && pEntity->HasDefuser())
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, "");
		int i = 0;
		Render::Text(size.x + size.w + 3, size.y + (i*(nameSize.bottom + 10) + 28), Color(93, 188, 210, 255), Render::Fonts::xsESP, "Kit");
	}

}

Vector AimPoint;

void CEsp::DrawCircle1(IClientEntity* pEntity)
{
	Vector cross = AimPoint, screen;
	static int Scale = 2;
	if (Render::WorldToScreen(cross, screen))
	{
		Render::DrawCircle(screen.x, screen.y, (2), (2 * 4), Color(255, 25, 25, 255));
	}

}

void CEsp::DefuseWarning(IClientEntity* pEntity)
{
	if (pEntity->IsDefusing())
		Render::Text(10, 100, Color(255, 25, 25, 255), Render::Fonts::ESP, ("Enemy is defusing"));
	else
		Render::Text(10, 100, Color(0, 255, 0, 255), Render::Fonts::ESP, (""));
}



void CEsp::DrawMoney(IClientEntity* pEntityity, CEsp::ESPBox size)
{
	ESPBox ArmorBar = size;

	int MoneyEnemy = 100;
	MoneyEnemy = pEntityity->GetMoney();
	char nameBuffer[512];
	sprintf_s(nameBuffer, "%d $", MoneyEnemy);
	RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, nameBuffer);
	int i = 0;
	Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 24, Color(144, 194, 54, 255), Render::Fonts::ESP, nameBuffer);
}

bool CEsp::GetBox(IClientEntity* pEntityity, CEsp::ESPBox &result)
{
	Vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	vOrigin = pEntityity->GetOrigin();
	min = pEntityity->collisionProperty()->GetMins() + vOrigin;
	max = pEntityity->collisionProperty()->GetMaxs() + vOrigin;

	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	if (!Render::WorldToScreen(points[3], flb) || !Render::WorldToScreen(points[5], brt)
		|| !Render::WorldToScreen(points[0], blb) || !Render::WorldToScreen(points[4], frt)
		|| !Render::WorldToScreen(points[2], frb) || !Render::WorldToScreen(points[1], brb)
		|| !Render::WorldToScreen(points[6], blt) || !Render::WorldToScreen(points[7], flt))
		return false;

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	result.x = left;
	result.y = top;
	result.w = right - left;
	result.h = bottom - top;

	return true;
}

Color CEsp::GetPlayerColor(IClientEntity* pEntityity)
{
	int TeamNum = pEntityity->GetTeamNum();
	bool IsVis = GameUtils::IsVisible(hackManager.pLocal(), pEntityity, (int)CSGOHitboxID::Head);

	Color color;

	if (TeamNum == TEAM_CS_T)
	{
		if (IsVis)
			color = Color(Menu::Window.ColorsTab.TColorVisR.GetValue(), Menu::Window.ColorsTab.TColorVisG.GetValue(), Menu::Window.ColorsTab.TColorVisB.GetValue(), 255);
		else
			color = Color(Menu::Window.ColorsTab.TColorNoVisR.GetValue(), Menu::Window.ColorsTab.TColorNoVisG.GetValue(), Menu::Window.ColorsTab.TColorNoVisB.GetValue(), 255);
	}
	else
	{

		if (IsVis)
			color = Color(Menu::Window.ColorsTab.CTColorVisR.GetValue(), Menu::Window.ColorsTab.CTColorVisG.GetValue(), Menu::Window.ColorsTab.CTColorVisB.GetValue(), 255);
		else
			color = Color(Menu::Window.ColorsTab.CTColorNoVisR.GetValue(), Menu::Window.ColorsTab.CTColorNoVisG.GetValue(), Menu::Window.ColorsTab.CTColorNoVisB.GetValue(), 255);
	}

	return color;
}



void CEsp::DrawBox(CEsp::ESPBox size, Color color)
{
	Render::Outline(size.x, size.y, size.w, size.h, color);
	Render::Outline(size.x - 1, size.y - 1, size.w + 2, size.h + 2, Color(0, 0, 0, 150));
	Render::Outline(size.x + 1, size.y + 1, size.w - 2, size.h - 2, Color(0, 0, 0, 150));
}

void CEsp::Barrel(CEsp::ESPBox size, Color color, IClientEntity* pEntityity)
{

	Vector src3D, src;
	src3D = pEntityity->GetOrigin() - Vector(0, 0, 0);

	if (!Render::WorldToScreen(src3D, src))
		return;

	int ScreenWidth, ScreenHeight;
	Interfaces::Engine->GetScreenSize(ScreenWidth, ScreenHeight);

	int x = (int)(ScreenWidth * 0.5f);
	int y = 0;


	y = ScreenHeight;

	Render::Line((int)(src.x), (int)(src.y), x, y, Color(0, 255, 0, 255));
}

std::string CleanItemName(std::string name)
{

	std::string Name = name;
	if (Name[0] == 'C')
		Name.erase(Name.begin());

	auto startOfWeap = Name.find("Weapon");
	if (startOfWeap != std::string::npos)
		Name.erase(Name.begin() + startOfWeap, Name.begin() + startOfWeap + 6);

	return Name;
}

void CEsp::DrawSkeleton2(IClientEntity* pEntityity)
{
	studiohdr_t* pStudioHdr = Interfaces::ModelInfo->GetStudiomodel(pEntityity->GetModel());
	if (!pStudioHdr)
		return;
	Vector vParent, vChild, sParent, sChild;
	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->GetBone(j);
		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			vChild = pEntityity->GetBonePos(j);
			vParent = pEntityity->GetBonePos(pBone->parent);
			if (Render::TransformScreen(vParent, sParent) && Render::TransformScreen(vChild, sChild))
			{
				Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(255, 255, 255, 255));
			}
		}
	}
}

//void CEsp::Glow()
//{
//	int GlowR = Menu::Window.ColorsTab.GlowR.GetValue();
//	int GlowG = Menu::Window.ColorsTab.GlowG.GetValue();
//	int GlowB = Menu::Window.ColorsTab.GlowB.GetValue();
//	int GlowZ = Menu::Window.VisualsTab.GlowZ.GetValue();
//
//	CGlowObjectManager* GlowObjectManager = (CGlowObjectManager*)GlowManager;
//
//	for (int i = 0; i < GlowObjectManager->size; ++i)
//	{
//		CGlowObjectManager::GlowObjectDefinition_t* glowEntity = &GlowObjectManager->m_GlowObjectDefinitions[i];
//		IClientEntity* Entity = glowEntity->getEntity();
//
//		if (glowEntity->IsEmpty() || !Entity)
//			continue;
//
//		switch (Entity->GetClientClass()->m_ClassID)
//		{
//		case (int)CSGOClassID::CCSPlayer:
//			if (Menu::Window.VisualsTab.OptionsGlow.GetState())
//			{
//				if (!Menu::Window.VisualsTab.Active.GetState() && !(Entity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()))
//					break;
//				if (!Menu::Window.VisualsTab.FiltersEnemiesOnly.GetState() && !(Entity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()))
//					break;
//
//				if (GameUtils::IsVisible(hackManager.pLocal(), Entity, 0))
//				{
//					glowEntity->set((Entity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()) ? Color(GlowR, GlowG, GlowB, GlowZ) : Color(GlowR, GlowG, GlowB, GlowZ));
//				}
//
//				else
//				{
//					glowEntity->set((Entity->GetTeamNum() == hackManager.pLocal()->GetTeamNum()) ? Color(GlowR, GlowG, GlowB, GlowZ) : Color(GlowR, GlowG, GlowB, GlowZ));
//				}
//			}
//		}
//	}
//}

void CEsp::EntityGlow()
{
	int GlowR = Menu::Window.ColorsTab.GlowR.GetValue();
	int GlowG = Menu::Window.ColorsTab.GlowG.GetValue();
	int GlowB = Menu::Window.ColorsTab.GlowB.GetValue();
	int GlowZ = Menu::Window.VisualsTab.GlowZ.GetValue();

	CGlowObjectManager* GlowObjectManager = (CGlowObjectManager*)GlowManager;

	for (int i = 0; i < GlowObjectManager->size; ++i)
	{
		CGlowObjectManager::GlowObjectDefinition_t* glowEntity = &GlowObjectManager->m_GlowObjectDefinitions[i];
		IClientEntity* Entity = glowEntity->getEntity();

		if (glowEntity->IsEmpty() || !Entity)
			continue;

		switch (Entity->GetClientClass()->m_ClassID)
		{
		case 1:
			if (Menu::Window.VisualsTab.EntityGlow.GetState())
			{
				if (Menu::Window.VisualsTab.EntityGlow.GetState())
					glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));

			}
		case 9:
			if (Menu::Window.VisualsTab.FiltersNades.GetState())
			{
				if (Menu::Window.VisualsTab.EntityGlow.GetState())
					glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));
			}
		case 29:
			if (Menu::Window.VisualsTab.EntityGlow.GetState())
			{
				glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));
			}
		case 39:
			if (Menu::Window.VisualsTab.EntityGlow.GetState())
			{
				if (Menu::Window.VisualsTab.FiltersC4.GetState())
					glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));

			}
		case 41:
			if (Menu::Window.VisualsTab.EntityGlow.GetState())
			{
				glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));
			}
		case 66:
			if (Menu::Window.VisualsTab.EntityGlow.GetState())
			{
				glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));
			}
		case 87:
			if (Menu::Window.VisualsTab.FiltersNades.GetState())
			{
				glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));
			}
		case 98:
			if (Menu::Window.VisualsTab.FiltersNades.GetState())
			{
				glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));
			}
		case 108:
			if (Menu::Window.VisualsTab.FiltersC4.GetState())
			{
				glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));

			}
		case 130:
			if (Menu::Window.VisualsTab.FiltersNades.GetState())
			{
				glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));
			}
		case 134:
			if (Menu::Window.VisualsTab.FiltersNades.GetState())
			{
				glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));

			}
		default:
			if (Menu::Window.VisualsTab.EntityGlow.GetState())
			{
				if (strstr(Entity->GetClientClass()->m_pNetworkName, "Weapon"))
					glowEntity->set(Color(GlowR, GlowG, GlowB, GlowZ));
			}
		}

	}
}

static wchar_t* CharToWideChar(const char* text)
{
	size_t size = strlen(text) + 1;
	wchar_t* wa = new wchar_t[size];
	mbstowcs_s(NULL, wa, size/4, text, size);
	return wa;
}

void CEsp::BacktrackingCross(IClientEntity* pEntityity)
{
	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{

		studiohdr_t* pStudioHdr = Interfaces::ModelInfo->GetStudiomodel(pEntityity->GetModel());

		if (!pStudioHdr)
			return;

		Vector vParent, vChild, sParent, sChild;

		for (int j = 0; j < pStudioHdr->numbones; j++)
		{
			mstudiobone_t* pBone = pStudioHdr->GetBone(j);

			if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
			{
				vChild = pEntityity->GetBonePos(j);
				vParent = pEntityity->GetBonePos(pBone->parent);

				if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
				{
					Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(255, 255, 255, 255));
				}
			}

		}

		player_info_t pinfo;
		if (pEntityity && pEntityity != hackManager.pLocal() && !pEntityity->IsDormant())
		{
			if (Interfaces::Engine->GetPlayerInfo(i, &pinfo) && pEntityity->IsAlive())
			{

				if (Menu::Window.LegitBotTab.AimbotBacktrack.GetState())
				{

					if (hackManager.pLocal()->IsAlive())
					{
						for (int t = 0; t < 12; ++t)
						{

							Vector screenbacktrack[64][12];

							if (headPositions[i][t].simtime && headPositions[i][t].simtime + 1 > hackManager.pLocal()->GetSimulationTime())
							{

								if (Render::WorldToScreen(headPositions[i][t].hitboxPos, screenbacktrack[i][t]))
								{

									Interfaces::Surface->DrawSetColor(Color(255, 0, 0, 255));
									Interfaces::Surface->DrawOutlinedRect(screenbacktrack[i][t].x, screenbacktrack[i][t].y, screenbacktrack[i][t].x + 2, screenbacktrack[i][t].y + 2);

								}

							}
						}

					}
					else
					{

						memset(&headPositions[0][0], 0, sizeof(headPositions));
					}
				}
				if (Menu::Window.RageBotTab.AccuracyBacktracking.GetState())
				{

					if (hackManager.pLocal()->IsAlive())
					{

						for (int t = 0; t < 12; ++t)
						{

							Vector screenbacktrack[64];

							if (backtracking->records[i].tick_count + 12 > Interfaces::Globals->tickcount)
							{
								if (Render::WorldToScreen(backtracking->records[i].headPosition, screenbacktrack[i]))

								{
									Interfaces::Surface->DrawSetColor(Color(255, 0, 0, 255));
									Interfaces::Surface->DrawOutlinedRect(screenbacktrack[i].x, screenbacktrack[i].y, screenbacktrack[i].x + 2, screenbacktrack[i].y + 2);

								}
		
							}
						}
			
					}
					else
					{
						memset(&backtracking->records[0], 0, sizeof(backtracking->records));
					}
				}
			}

		}
	}

}

void CEsp::BombTimer(IClientEntity* pEntityity, ClientClass* cClass)
{
	BombCarrier = nullptr;

	Vector vOrig; Vector vScreen;
	vOrig = pEntityity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)pEntityity;

	if (Render::WorldToScreen(vOrig, vScreen))
	{

		ESPBox Box;
		GetBox(pEntityity, Box);
		DrawBox(Box, Color(250, 42, 42, 255));
		float flBlow = Bomb->GetC4BlowTime();
		float TimeRemaining = flBlow - (Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase());
		float TimeRemaining2;
		bool exploded = true;
		if (TimeRemaining < 0)
		{
			!exploded;

			TimeRemaining2 = 0;
		}
		else
		{
			exploded = true;
			TimeRemaining2 = TimeRemaining;
		}
		char buffer[64];
		if (exploded)
		{
;
			sprintf_s(buffer, "Bomb: %.1f", TimeRemaining2);
		}
		else
		{

			sprintf_s(buffer, "Bomb Undefusable", TimeRemaining2);
		}
		Render::Text(vScreen.x, vScreen.y, Color(255, 255, 255, 255), Render::Fonts::ESP, buffer);
	}
}

CEsp::ESPBox CEsp::GetBOXX(IClientEntity* pEntityity)
{
	ESPBox result;
	
	// Variables
	Vector  vOrigin, min, max, sMin, sMax, sOrigin,
		flb, brt, blb, frt, frb, brb, blt, flt;
	float left, top, right, bottom;

	// Get the locations
	vOrigin = pEntityity->GetOrigin();
	min = pEntityity->collisionProperty()->GetMins() + vOrigin;
	max = pEntityity->collisionProperty()->GetMaxs() + vOrigin;

	// Points of a 3d bounding box
	Vector points[] = { Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z) };

	// Get screen positions
	if (!Render::WorldToScreen(points[3], flb) || !Render::WorldToScreen(points[5], brt)
		|| !Render::WorldToScreen(points[0], blb) || !Render::WorldToScreen(points[4], frt)
		|| !Render::WorldToScreen(points[2], frb) || !Render::WorldToScreen(points[1], brb)
		|| !Render::WorldToScreen(points[6], blt) || !Render::WorldToScreen(points[7], flt))
		return result;

	// Put them in an array (maybe start them off in one later for speed?)
	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	// Init this shit
	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	// Find the bounding corners for our box
	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}

	// Width / height
	result.x = left;
	result.y = top;
	result.w = right - left;
	result.h = bottom - top;

	return result;
}

void CEsp::DrawHealth(IClientEntity* pEntityity, CEsp::ESPBox size)
{
	int HPEnemy = 100;
	HPEnemy = pEntityity->GetHealth();
	char nameBuffer[512];
	sprintf_s(nameBuffer, "%d", HPEnemy);


	float h = (size.h);
	float offset = (h / 4.f) + 5;
	float w = h / 48.f;
	float health = pEntityity->GetHealth();
	UINT hp = h - (UINT)((h * health) / 100);

	int Red = 255 - (health*2.55);
	int Green = health * 2.55;

	Render::DrawRect((size.x - 3) - 1, size.y - 1, 3, h + 2, Color(0, 0, 0, 180));

	Render::Line((size.x - 3), size.y + hp, (size.x - 3), size.y + h, Color(Red, Green, 0, 180));

	if (health < 100) {

	//	Render::Text(size.x - 9, size.y + hp, Color(255, 255, 255, 255), Render::Fonts::Health, nameBuffer);
	}
}

void CEsp::DrawName(player_info_t pinfo, CEsp::ESPBox size)
{
	if (strlen(pinfo.name) > 16)
	{
		pinfo.name[16] = 0;
		strcat(pinfo.name, "...");
		puts(pinfo.name);
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, pinfo.name);
		Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 16, Color(255, 255, 255, 255), Render::Fonts::ESP, pinfo.name);
	}
	else
	{
		RECT nameSize = Render::GetTextSize(Render::Fonts::ESP, pinfo.name);
		Render::Text(size.x + (size.w / 2) - (nameSize.right / 2), size.y - 16, Color(255, 255, 255, 255), Render::Fonts::ESP, pinfo.name);
	}
}

void DrawOutlinedRect(int x, int y, int w, int h, Color col)
{
	Interfaces::Surface->DrawSetColor(col);
	Interfaces::Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void DrawLine(int x0, int y0, int x1, int y1, Color col)
{
	Interfaces::Surface->DrawSetColor(col);
	Interfaces::Surface->DrawLine(x0, y0, x1, y1);
}

void CEsp::DrawCross(IClientEntity* pEntityity)
{

	Vector cross = pEntityity->GetHeadPos(), screen;
	static int Scale = 2;
	if (Render::WorldToScreen(cross, screen))
	{
		Render::Clear(screen.x - Scale, screen.y - (Scale * 2), (Scale * 2), (Scale * 4), Color(20, 20, 20, 160));
		Render::Clear(screen.x - (Scale * 2), screen.y - Scale, (Scale * 4), (Scale * 2), Color(20, 20, 20, 160));
		Render::Clear(screen.x - Scale - 1, screen.y - (Scale * 2) - 1, (Scale * 2) - 2, (Scale * 4) - 2, Color(250, 250, 250, 160));
		Render::Clear(screen.x - (Scale * 2) - 1, screen.y - Scale - 1, (Scale * 4) - 2, (Scale * 2) - 2, Color(250, 250, 250, 160));
	}

}

void CEsp::DrawDrop(IClientEntity* pEntityity, ClientClass* cClass)
{

	Vector Box;
	IClientEntity* Weapon = (IClientEntity*)pEntityity;
	IClientEntity* plr = Interfaces::EntList->GetClientEntityFromHandle((HANDLE)Weapon->GetOwnerHandle());
	if (!plr && Render::WorldToScreen(Weapon->GetOrigin(), Box))
	{
		if (Menu::Window.VisualsTab.FiltersWeapons.GetState())
		{

			std::string ItemName = CleanItemName(cClass->m_pNetworkName);
			RECT TextSize = Render::GetTextSize(Render::Fonts::ESP, ItemName.c_str());
			Render::Text(Box.x - (TextSize.right / 2), Box.y - 16, Color(255, 255, 255, 255), Render::Fonts::ESP, ItemName.c_str());
		}
	}

}

void CEsp::DrawBombPlanted(IClientEntity* pEntityity, ClientClass* cClass)
{
	BombCarrier = nullptr;

	Vector vOrig; Vector vScreen;
	vOrig = pEntityity->GetOrigin();
	CCSBomb* Bomb = (CCSBomb*)pEntityity;

	float flBlow = Bomb->GetC4BlowTime();
	float TimeRemaining = flBlow - (Interfaces::Globals->interval_per_tick * hackManager.pLocal()->GetTickBase());
	char buffer[64];
	sprintf_s(buffer, "%.1fs", TimeRemaining);
	float TimeRemaining2;
	bool exploded = true;
	if (TimeRemaining < 0)
	{
		!exploded;

		TimeRemaining2 = 0;
	}
	else
	{
		exploded = true;
		TimeRemaining2 = TimeRemaining;
	}
	if (exploded)
	{

		sprintf_s(buffer, "Bomb: %.1f", TimeRemaining2);
	}
	else
	{

		sprintf_s(buffer, "Bomb Undefusable", TimeRemaining2);
	}

	Render::Text(10, 45, Color(0, 255, 0, 255), Render::Fonts::Clock, buffer);

}

void CEsp::DrawBomb(IClientEntity* pEntityity, ClientClass* cClass)
{

	BombCarrier = nullptr;
	C_BaseCombatWeapon *BombWeapon = (C_BaseCombatWeapon *)pEntityity;
	Vector vOrig; Vector vScreen;
	vOrig = pEntityity->GetOrigin();
	bool adopted = true;
	HANDLE parent = BombWeapon->GetOwnerHandle();
	if (parent || (vOrig.x == 0 && vOrig.y == 0 && vOrig.z == 0))
	{
		IClientEntity* pParentEnt = (Interfaces::EntList->GetClientEntityFromHandle(parent));
		if (pParentEnt && pParentEnt->IsAlive())
		{

			BombCarrier = pParentEnt;
			adopted = false;
		}
	}

	if (adopted)
	{
		if (Render::WorldToScreen(vOrig, vScreen))
		{
			Render::Text(vScreen.x, vScreen.y, Color(112, 230, 20, 255), Render::Fonts::ESP, "Bomb");
		}
	}
}

void DrawBoneArray(int* boneNumbers, int amount, IClientEntity* pEntityity, Color color)
{

	Vector LastBoneScreen;
	for (int i = 0; i < amount; i++)
	{
		Vector Bone = pEntityity->GetBonePos(boneNumbers[i]);
		Vector BoneScreen;

		if (Render::WorldToScreen(Bone, BoneScreen))
		{
			if (i>0)
			{
				Render::Line(LastBoneScreen.x, LastBoneScreen.y, BoneScreen.x, BoneScreen.y, color);
			}
		}
		LastBoneScreen = BoneScreen;
	}
}

void DrawBoneTest(IClientEntity *pEntityity)
{
	for (int i = 0; i < 127; i++)
	{
		Vector BoneLoc = pEntityity->GetBonePos(i);
		Vector BoneScreen;
		if (Render::WorldToScreen(BoneLoc, BoneScreen))
		{
			char buf[10];
			_itoa_s(i, buf, 10);
			Render::Text(BoneScreen.x, BoneScreen.y, Color(255, 255, 255, 180), Render::Fonts::ESP, buf);
		}
	}
}

void CEsp::DrawSkeleton(IClientEntity* pEntityity)
{

	studiohdr_t* pStudioHdr = Interfaces::ModelInfo->GetStudiomodel(pEntityity->GetModel());
	IClientEntity* pLocal = hackManager.pLocal();
	if (!pStudioHdr)
		return;

	Vector vParent, vChild, sParent, sChild;
	for (int j = 0; j < pStudioHdr->numbones; j++)
	{
		mstudiobone_t* pBone = pStudioHdr->GetBone(j);

		if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
		{
			IClientRenderable* pUnknoown = pEntityity->GetClientRenderable();
			if (pUnknoown == nullptr)
				return;
			vChild = pEntityity->GetBonePos(j);
			vParent = pEntityity->GetBonePos(pBone->parent);

			if (Render::WorldToScreen(vParent, sParent) && Render::WorldToScreen(vChild, sChild))
			{
				Render::Line(sParent[0], sParent[1], sChild[0], sChild[1], Color(255, 255, 255, 255));
			}
		}
	}
}

void CEsp::Armor(IClientEntity* pEntityity, CEsp::ESPBox size)
{
	ESPBox ArBar = size;
	ArBar.y += (ArBar.h + 3);
	ArBar.h = 6;

	float ArValue = pEntityity->ArmorValue();
	float ArPerc = ArValue / 100.f;
	float Width = (size.w * ArPerc);
	ArBar.w = Width;

	Vertex_t Verts[4];
	Verts[0].Init(Vector2D(ArBar.x, ArBar.y));
	Verts[1].Init(Vector2D(ArBar.x + size.w + 0, ArBar.y));
	Verts[2].Init(Vector2D(ArBar.x + size.w, ArBar.y + 2));
	Verts[3].Init(Vector2D(ArBar.x - 0, ArBar.y + 2));

	Render::PolygonOutline(4, Verts, Color(50, 50, 50, 255), Color(50, 50, 50, 255));

	Vertex_t Verts2[4];
	Verts2[0].Init(Vector2D(ArBar.x, ArBar.y + 1));
	Verts2[1].Init(Vector2D(ArBar.x + ArBar.w + 0, ArBar.y + 1));
	Verts2[2].Init(Vector2D(ArBar.x + ArBar.w, ArBar.y + 2));
	Verts2[3].Init(Vector2D(ArBar.x, ArBar.y + 2));

	Color c = GetPlayerColor(pEntityity);
	Render::Polygon(4, Verts2, Color(0, 120, 255, 200));
}

void CEsp::BoxAndText(IClientEntity* entity, std::string text)
{

	ESPBox Box;
	std::vector<std::string> Info;
	if (GetBox(entity, Box))
	{
		Info.push_back(text);
		if (Menu::Window.VisualsTab.FiltersNades.GetState())
		{
			int i = 0;
			for (auto kek : Info)
			{
				Render::Text(Box.x + 1, Box.y + 1, Color(255, 255, 255, 255), Render::Fonts::ESP, kek.c_str());
				i++;
			}
		}

	}
}

void CEsp::DrawThrowable(IClientEntity* throwable)
{
	model_t* nadeModel = (model_t*)throwable->GetModel();

	if (!nadeModel)
		return;

	studiohdr_t* hdr = Interfaces::ModelInfo->GetStudiomodel(nadeModel);

	if (!hdr)
		return;

	if (!strstr(hdr->name, "thrown") && !strstr(hdr->name, "dropped"))
		return;

	std::string nadeName = "Unknown Grenade";

	IMaterial* mats[32];
	Interfaces::ModelInfo->GetModelMaterials(nadeModel, hdr->numtextures, mats);

	for (int i = 0; i < hdr->numtextures; i++)
	{
		IMaterial* mat = mats[i];
		if (!mat)
			continue;

		if (strstr(mat->GetName(), "flashbang"))
		{
			nadeName = "Flashbang";
			break;
		}
		else if (strstr(mat->GetName(), "m67_grenade") || strstr(mat->GetName(), "hegrenade"))
		{
			nadeName = "HE Grenade";
			break;
		}
		else if (strstr(mat->GetName(), "smoke"))
		{
			nadeName = "Smoke";
			break;
		}
		else if (strstr(mat->GetName(), "decoy"))
		{
			nadeName = "Decoy";
			break;
		}
		else if (strstr(mat->GetName(), "incendiary") || strstr(mat->GetName(), "molotov"))
		{
			nadeName = "Molotov";
			break;
		}
	}
	BoxAndText(throwable, nadeName);
}

#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class dnydriw {
public:
	double oqfzhzmtdrob;
	string ttrhyrt;
	string llhriwsgew;
	string cqwazegxvu;
	double dtgnerwzic;
	dnydriw();
	bool klgaxaoxhi();
	int mjyijilpjzmymfbvanflqs(int wkkcnttnuus, int idxnjljqsdnuuw, bool fttoo, int akbluqplw, string wpmfnzwazan, int gcsgtvbm, int jqvpt, double jldxp, double jqhfsx);
	string nwfymebzqcyxlaljuqptherr(double kagpkfakajmsivq, bool kdguvyptio, double uzeebilqmyklx, int rfpkivrje, bool pcxnzzpdqamyhx, bool yeehbq);
	int acfbbvfommbjvuhq(double tykdxkeulrguo, double mqworlpa);
	string hkpfwzzntybrxeymquztw(double llkkqd, double fkhrznddlwrpd, string gyvvyrots, bool oycswllzojptop, string vnytn, int fipfxv, double ycumoec, bool qgzehf, int mbaye, int xfedi);
	void soaijqgfvmxxtvzueebc(bool bdrzxr, double yepyd);
	void houawzmcfxii(int tnzmcdiwym, bool dddwerdyunv, double hqllghxczhzv, double ljbsuz, int xwzwmokd, int jdnuehifk);

protected:
	string esolesagiqetnsh;
	int druwhhhexy;
	string ofttmrd;
	double bdsphbmkflftss;

	int aemflfxepebilqpmhvjsbdtqu(bool lsbogzyrvf, int xidsjppm, string yubinmts, int ppsyriyuwoz);
	string lunnmwqseghqjftveeyo(double ohueph);
	double puhbceyjfedlbofsildzyi(double nnbiuojxlyig, string zsoav, double oqvrzlhojre);
	bool ygbsuwlmdeqytvqfrheezzcaw(bool sjyrtusqfxd, string skuynmrthlfeu, string itivddiuuzr);
	void yammqrlovfbrlogf(double uvcrpzhfimfaiit);

private:
	double htoqmjfw;
	double ztrksz;
	string lqjfmojq;
	bool dnqow;

	double wbruyptmoypcuodbmtrywb(double zeczjecqyfccj, double bgbghhoye, bool bwrdzvjox, string shqoeenwcghz, bool etblmndhdi, int cglrhnmg, string dwjschbwctjefw, string aswxvk, int ykbpn);

};



double dnydriw::wbruyptmoypcuodbmtrywb(double zeczjecqyfccj, double bgbghhoye, bool bwrdzvjox, string shqoeenwcghz, bool etblmndhdi, int cglrhnmg, string dwjschbwctjefw, string aswxvk, int ykbpn) {
	return 90508;
}

int dnydriw::aemflfxepebilqpmhvjsbdtqu(bool lsbogzyrvf, int xidsjppm, string yubinmts, int ppsyriyuwoz) {
	double hjoqxzqek = 756;
	string nxsgqgzk = "wwmyuyoomggirpsgpuahfhmbovtgjlhzfdlpbiemuauwvrffjvececldvsxqlgmlpiye";
	double iafbvhkzqv = 42042;
	bool hrnemkbhgljnt = false;
	int kcnzkp = 849;
	double tyuchozhcshljv = 10553;
	bool zosigmmfv = true;
	if (10553 != 10553) {
		int kegltsro;
		for (kegltsro = 20; kegltsro > 0; kegltsro--) {
			continue;
		}
	}
	return 55802;
}

string dnydriw::lunnmwqseghqjftveeyo(double ohueph) {
	bool soxwuhiaykbfrg = true;
	double dsznxyjry = 35104;
	string egqzfdclbvgc = "hiynkrzwiizmbqtfitrbnomssqnfoclmywjzdjfklykgvgifmzdnvivlftukzborgkjsjxsrazgcdiiolm";
	string pzdeord = "va";
	string hwdbiiushqhrs = "tzmgkpwsqxpqbhadfddoiwxhvczouyxsboywbmnsxtlfcwypjtsnnhtshmh";
	bool awjbvdza = false;
	int elwxj = 701;
	double cxfhm = 35668;
	if (true != true) {
		int uejmc;
		for (uejmc = 41; uejmc > 0; uejmc--) {
			continue;
		}
	}
	if (35104 != 35104) {
		int quoapzoxn;
		for (quoapzoxn = 44; quoapzoxn > 0; quoapzoxn--) {
			continue;
		}
	}
	if (true == true) {
		int xyegrahq;
		for (xyegrahq = 78; xyegrahq > 0; xyegrahq--) {
			continue;
		}
	}
	if (35104 != 35104) {
		int nvvnex;
		for (nvvnex = 65; nvvnex > 0; nvvnex--) {
			continue;
		}
	}
	return string("baj");
}

double dnydriw::puhbceyjfedlbofsildzyi(double nnbiuojxlyig, string zsoav, double oqvrzlhojre) {
	bool wamfnek = true;
	string occcdj = "qxmdfdwyb";
	bool kpgsxmfbauuxh = false;
	int wdukmn = 1442;
	int gklrwifsdj = 5456;
	double uuinq = 44991;
	string weatcjezlgplvx = "yplkqnxwnmphotfntyuuxkjdazodswkh";
	int goeeoozjkohfb = 1216;
	bool rrpwi = true;
	string ervjbuwusnra = "zkivttibnyozbvbrysettmxbisqrwetortogitkodetstnjpyoptcrgelrdmovrusucdpdagdtseqkgzzvqudfi";
	if (1216 != 1216) {
		int vkuoelfrk;
		for (vkuoelfrk = 43; vkuoelfrk > 0; vkuoelfrk--) {
			continue;
		}
	}
	return 62045;
}

bool dnydriw::ygbsuwlmdeqytvqfrheezzcaw(bool sjyrtusqfxd, string skuynmrthlfeu, string itivddiuuzr) {
	string lehjyhgw = "zmtzaksnkvqgywtcrkvkogvodxabdrnzphrszyyszqxmscgkjougmnhdtkrbunuodphoymfklkcses";
	string glgdw = "xyhhpufuzsptigiebzcp";
	int qpnnsufiznikeei = 1648;
	string rezprlwryjzpmr = "yfofyjvbjuzfxpzmnvrbaautau";
	bool fzikhjecqktzutv = true;
	double zvlximmtl = 13080;
	bool srxljljtidsh = false;
	double qyhphysggp = 22821;
	double igvotohzxfgp = 16571;
	if (22821 != 22821) {
		int hackxmvgi;
		for (hackxmvgi = 17; hackxmvgi > 0; hackxmvgi--) {
			continue;
		}
	}
	if (1648 == 1648) {
		int sfkhddczo;
		for (sfkhddczo = 24; sfkhddczo > 0; sfkhddczo--) {
			continue;
		}
	}
	if (string("xyhhpufuzsptigiebzcp") == string("xyhhpufuzsptigiebzcp")) {
		int wjxmk;
		for (wjxmk = 2; wjxmk > 0; wjxmk--) {
			continue;
		}
	}
	return false;
}

void dnydriw::yammqrlovfbrlogf(double uvcrpzhfimfaiit) {

}

bool dnydriw::klgaxaoxhi() {
	double xlssgkkq = 31692;
	int svcrxh = 2156;
	int xwkuzmql = 1605;
	bool vldtdhgabvmrtic = true;
	string rwfpcj = "iholkzbqydmyelzpgzdcsxnfqvxeiadrqkqambslvfwjzoliqvz";
	string otrheyb = "lwohnugmkgxblnmcbjwosssxklsruggd";
	string ggvif = "buooltseeynwwrgampoawzkozb";
	bool xujdukmvg = false;
	if (1605 != 1605) {
		int zkbdpctdc;
		for (zkbdpctdc = 32; zkbdpctdc > 0; zkbdpctdc--) {
			continue;
		}
	}
	if (string("lwohnugmkgxblnmcbjwosssxklsruggd") == string("lwohnugmkgxblnmcbjwosssxklsruggd")) {
		int rpermqhjl;
		for (rpermqhjl = 39; rpermqhjl > 0; rpermqhjl--) {
			continue;
		}
	}
	return true;
}

int dnydriw::mjyijilpjzmymfbvanflqs(int wkkcnttnuus, int idxnjljqsdnuuw, bool fttoo, int akbluqplw, string wpmfnzwazan, int gcsgtvbm, int jqvpt, double jldxp, double jqhfsx) {
	int wssqmoaptbvczr = 2574;
	double lvezkdoymmwlk = 7080;
	int mckckqfsqemkv = 4899;
	int osloro = 2429;
	string phtsadcmqn = "tqgvul";
	int auubqsvng = 1953;
	string enxbylxbfaanm = "nedmghgfrktzwfacmgmteaofxrutrdthhhjlvlvhkhtivnmbyzrwcncsjljtukelxdbpqhkpfxwrntttnfjwreczytkrdvgdmj";
	bool myuvsyrklxufe = false;
	if (1953 != 1953) {
		int pqyuejt;
		for (pqyuejt = 34; pqyuejt > 0; pqyuejt--) {
			continue;
		}
	}
	if (false == false) {
		int rk;
		for (rk = 50; rk > 0; rk--) {
			continue;
		}
	}
	if (4899 == 4899) {
		int xzswiwzlh;
		for (xzswiwzlh = 0; xzswiwzlh > 0; xzswiwzlh--) {
			continue;
		}
	}
	if (2429 != 2429) {
		int tgohazk;
		for (tgohazk = 90; tgohazk > 0; tgohazk--) {
			continue;
		}
	}
	return 81116;
}

string dnydriw::nwfymebzqcyxlaljuqptherr(double kagpkfakajmsivq, bool kdguvyptio, double uzeebilqmyklx, int rfpkivrje, bool pcxnzzpdqamyhx, bool yeehbq) {
	int zxddksjah = 2900;
	double qaybdswaxaimgc = 287;
	double azhggsh = 27724;
	int flxpkq = 2102;
	double jxmfqnnhsnhj = 85168;
	int rdjeivvfkoiwf = 1030;
	int yljulkkbrclrai = 5519;
	bool abdturxi = true;
	int hijkn = 4026;
	double iewcdmldes = 23312;
	if (4026 == 4026) {
		int araikipjnk;
		for (araikipjnk = 26; araikipjnk > 0; araikipjnk--) {
			continue;
		}
	}
	if (true != true) {
		int fgbhnreec;
		for (fgbhnreec = 41; fgbhnreec > 0; fgbhnreec--) {
			continue;
		}
	}
	if (1030 != 1030) {
		int ttbsglsr;
		for (ttbsglsr = 20; ttbsglsr > 0; ttbsglsr--) {
			continue;
		}
	}
	return string("lfusiqkjttyctge");
}

int dnydriw::acfbbvfommbjvuhq(double tykdxkeulrguo, double mqworlpa) {
	int gtebjyhlykc = 753;
	double zrzrtwdeojvwzki = 21497;
	bool vljzrjlwzvxl = false;
	int bquzutgw = 1770;
	string gkfhiltfaevza = "ccjwxoacjwdxpygesbb";
	bool pbmdgh = true;
	int urfwbhuazey = 3497;
	double wnyqovi = 7042;
	int pswfwxxufqmmpal = 3059;
	string exnabbj = "mvywalpmhuvjdkmtmgyugfmsixknrffignodcurshrxklxevmipbpeiwzqxpfilgmpfktgfjyaydfyqgpwxtlrnxumted";
	if (7042 == 7042) {
		int woebvakc;
		for (woebvakc = 35; woebvakc > 0; woebvakc--) {
			continue;
		}
	}
	if (false != false) {
		int rrkfqe;
		for (rrkfqe = 52; rrkfqe > 0; rrkfqe--) {
			continue;
		}
	}
	if (3497 != 3497) {
		int uxdcbeotn;
		for (uxdcbeotn = 40; uxdcbeotn > 0; uxdcbeotn--) {
			continue;
		}
	}
	if (21497 == 21497) {
		int lsfdidv;
		for (lsfdidv = 41; lsfdidv > 0; lsfdidv--) {
			continue;
		}
	}
	if (3497 == 3497) {
		int zo;
		for (zo = 30; zo > 0; zo--) {
			continue;
		}
	}
	return 26079;
}

string dnydriw::hkpfwzzntybrxeymquztw(double llkkqd, double fkhrznddlwrpd, string gyvvyrots, bool oycswllzojptop, string vnytn, int fipfxv, double ycumoec, bool qgzehf, int mbaye, int xfedi) {
	int gdeeixboaz = 2330;
	int ryxvqrouwcxpc = 1176;
	int isjflnciyypkrz = 253;
	double bbadvoznorg = 25677;
	int lfemdxemopgaeqi = 1002;
	double uvfvxklz = 14381;
	string vdiug = "nsjbjoengmzxgqsnwearduvjfdhnczeswfwtz";
	string jgcvagttgrmql = "gazanudhwqyjfiummpvhtadynkglbtvfwuutpx";
	if (25677 != 25677) {
		int wfaqki;
		for (wfaqki = 5; wfaqki > 0; wfaqki--) {
			continue;
		}
	}
	if (string("gazanudhwqyjfiummpvhtadynkglbtvfwuutpx") == string("gazanudhwqyjfiummpvhtadynkglbtvfwuutpx")) {
		int oydi;
		for (oydi = 24; oydi > 0; oydi--) {
			continue;
		}
	}
	return string("kmou");
}

void dnydriw::soaijqgfvmxxtvzueebc(bool bdrzxr, double yepyd) {
	double ffdtir = 27603;
	double zrhvowjgzrxvq = 52816;
	string ofkfxpjdmeeg = "nurb";
	double rrxejitqwht = 50599;
	double udbrizwvxi = 59189;
	int mkfvaktsakgk = 3715;
	int nqjyqgxnqosi = 5364;
	int sjqrzdvygcdhya = 120;
	string tptnfodxpcmm = "nfzkuzsfgvyodjhoodrsuqoepwuzmcbnbdolxstherrzkgsnvytekpmez";
	int cqxrioxp = 2231;

}

void dnydriw::houawzmcfxii(int tnzmcdiwym, bool dddwerdyunv, double hqllghxczhzv, double ljbsuz, int xwzwmokd, int jdnuehifk) {
	string stnbvb = "xmziifrqgyjxx";
	bool ikrmphrsob = true;
	string lwlvxwcunoasq = "ypdphzqjbfssw";
	string bjduvav = "rowuqhcybyxzduptvhcaceswppvahkwrqcppogmewxzcopqwpkiwwqvaerot";
	double jafpotziesjeedj = 26794;
	string mskehizncnde = "yj";
	bool jhipdmbiyk = false;
	if (false == false) {
		int ufqo;
		for (ufqo = 21; ufqo > 0; ufqo--) {
			continue;
		}
	}
	if (26794 != 26794) {
		int fbalnuwetg;
		for (fbalnuwetg = 6; fbalnuwetg > 0; fbalnuwetg--) {
			continue;
		}
	}
	if (true == true) {
		int dxeshsqg;
		for (dxeshsqg = 68; dxeshsqg > 0; dxeshsqg--) {
			continue;
		}
	}
	if (string("xmziifrqgyjxx") != string("xmziifrqgyjxx")) {
		int hdegel;
		for (hdegel = 54; hdegel > 0; hdegel--) {
			continue;
		}
	}
	if (true == true) {
		int jc;
		for (jc = 61; jc > 0; jc--) {
			continue;
		}
	}

}

dnydriw::dnydriw() {
	this->klgaxaoxhi();
	this->mjyijilpjzmymfbvanflqs(2880, 2249, false, 667, string("hdteooyebzxwdyfwbwxbpkhezfebovetpxuknslmhhaacgfobpeusjiswdwtdxscyelhspamd"), 2639, 97, 11376, 47165);
	this->nwfymebzqcyxlaljuqptherr(8209, false, 15724, 3607, true, true);
	this->acfbbvfommbjvuhq(22713, 26577);
	this->hkpfwzzntybrxeymquztw(13072, 12299, string("eeqyiuieufjkcqcqhjjhpgsuetpbsoujhvgcxamrbpaihjzmgqbqlsuowitspebwwdnsicjcj"), false, string("vsojntryxsc"), 2345, 75233, true, 875, 780);
	this->soaijqgfvmxxtvzueebc(true, 16942);
	this->houawzmcfxii(2061, false, 7770, 11230, 1056, 1212);
	this->aemflfxepebilqpmhvjsbdtqu(true, 971, string("tqvanrpwmengtovonscymdhsrezvahetitwzrsgrxlsuuubj"), 1276);
	this->lunnmwqseghqjftveeyo(109);
	this->puhbceyjfedlbofsildzyi(34684, string("cgnjobjxfaxwpgqruchdjmghn"), 39572);
	this->ygbsuwlmdeqytvqfrheezzcaw(false, string("slcwsrlgrqvgpeopnyvmzmhfwfsmqtscwgsayyqajtlmtiadpyx"), string("acoodbupfvoregoyfeueadlncumfifnruzwlojeiafvqvcjdyeyie"));
	this->yammqrlovfbrlogf(12555);
	this->wbruyptmoypcuodbmtrywb(38096, 42560, true, string("jiudhufnpjoclzrzneqiqdczhaoysguiyqyekppcsjwpo"), true, 855, string("ungfmyemuq"), string("cojtwsfdp"), 4032);
}

