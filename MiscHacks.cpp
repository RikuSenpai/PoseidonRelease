#define _CRT_SECURE_NO_WARNINGS

#include "MiscHacks.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include <chrono>
#include <time.h>

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

inline float bitsToFloat(unsigned long i)
{
	return *reinterpret_cast<float*>(&i);
}

inline float FloatNegate(float f)
{
	return bitsToFloat(FloatBits(f) ^ 0x80000000);
}

Vector AutoStrafeView;

void CMiscHacks::Init()
{

}

void CMiscHacks::Draw()
{
	if (!Interfaces::Engine->IsConnected() || !Interfaces::Engine->IsInGame())
		return;

	switch (Menu::Window.MiscTab.NameChanger.GetIndex())
	{
	case 0:
		break;
	case 1:
		Namespam();

		break;
	case 2:
		NoName();

		break;
	case 3:
		NameSteal();
		break;

	case 4:
		ChatSpamInterwebz();
		break;


	}
}
inline float FastSqrt(float x)
{
	unsigned int i = *(unsigned int*)&x;
	i += 127 << 23;
	i >>= 1;
	return *(float*)&i;
}
#define square( x ) ( x * x )
void CMiscHacks::MinimalWalk(CUserCmd* cmd, float speed)
{
//Removed For Leak
}

float CMiscHacks::get_gun(C_BaseCombatWeapon* weapon)
{

	if (!weapon)
		return 0.f;

	if (weapon->isAuto())
		return 40.f;

	else if (weapon->isScout())
		return 70.f;

	else if (weapon->isAwp())
		return 30.f;

	else
		return 34.f;
}

int CMiscHacks::GetFPS()
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


void CMiscHacks::Move(CUserCmd *pCmd, bool &bSendPacket)
{
	if (Menu::Window.VisualsTab.Logs.GetState())
	{

		ConVar* Developer = Interfaces::CVar->FindVar("developer");
		*(float*)((DWORD)&Developer->fnChangeCallback + 0xC) = NULL;
		Developer->SetValue("1");

		ConVar* con_filter_enable = Interfaces::CVar->FindVar("con_filter_enable");
		*(float*)((DWORD)&con_filter_enable->fnChangeCallback + 0xC) = NULL;
		con_filter_enable->SetValue("2");

		ConVar* con_filter_text = Interfaces::CVar->FindVar("con_filter_text");
		*(float*)((DWORD)&con_filter_text->fnChangeCallback + 0xC) = NULL;
		con_filter_text->SetValue("");
	}
	
	if (!Interfaces::Engine->IsConnected() || !Interfaces::Engine->IsInGame())
		return;

	IClientEntity *pLocal = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	
	if (Menu::Window.MiscTab.OtherAutoJump.GetState())
		AutoJump(pCmd);

	static bool counter = false;

	bool once = false;
	if (GetAsyncKeyState(Menu::Window.SettingsTab.fake_crouch.GetKey()))
	{
		if (Menu::Window.MiscTab.fake_crouch.GetState())
		{
			unsigned int chokegoal = 6;
			auto choke = *(int*)(uintptr_t(Interfaces::client_state) + 0x4D28);
			bool mexican_tryhard = choke >= chokegoal;

			if (pLocal->GetFlags() & FL_ONGROUND)
			{
				if (mexican_tryhard)
					pCmd->buttons |= IN_DUCK;
				else
					pCmd->buttons &= ~IN_DUCK;
			}
		}
	}

	Interfaces::Engine->GetViewAngles(AutoStrafeView);
	switch (Menu::Window.MiscTab.OtherAutoStrafe.GetIndex())
	{
	case 0:
		break;
	case 1:
		LegitStrafe(pCmd);
		break;

	case 2:
		RageStrafe(pCmd);
		break;

	}

	if (Menu::Window.MiscTab.AutoPistol.GetState())
		AutoPistol(pCmd);

	//IClientEntity* pLocal = hackManager.pLocal();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	if (Menu::Window.SettingsTab.WeaponCheck.GetState())
	{

		if (GameUtils::IsScout(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue((float)Menu::Window.SettingsTab.scoutmindmg.GetValue());
		}
		else if (GameUtils::IsAWP(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue((float)Menu::Window.SettingsTab.AWPmindmg.GetValue());
		}
		else if (GameUtils::IsAuto(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue((float)Menu::Window.SettingsTab.Automindmg.GetValue());
		}
		else if (GameUtils::IsR8(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue((float)Menu::Window.SettingsTab.R8mindmg.GetValue());
		}
		else if (GameUtils::IsPistol(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue((float)Menu::Window.SettingsTab.Pistolmindmg.GetValue());
		}
		else if (GameUtils::IsRifle(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue((float)Menu::Window.SettingsTab.Riflemindmg.GetValue());
		}
		else if (GameUtils::IsMP(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyMinimumDamage.SetValue((float)Menu::Window.SettingsTab.MPmindmg.GetValue());
		}





		if (GameUtils::IsScout(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyHitchance.SetValue((float)Menu::Window.SettingsTab.scoutChance.GetValue());
		}
		else if (GameUtils::IsAWP(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyHitchance.SetValue((float)Menu::Window.SettingsTab.AWPChance.GetValue());
		}
		else if (GameUtils::IsAuto(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyHitchance.SetValue((float)Menu::Window.SettingsTab.AutoChance.GetValue());
		}
		else if (GameUtils::IsR8(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyHitchance.SetValue((float)Menu::Window.SettingsTab.R8Chance.GetValue());
		}
		else if (GameUtils::IsPistol(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyHitchance.SetValue((float)Menu::Window.SettingsTab.PistolChance.GetValue());
		}
		else if (GameUtils::IsRifle(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyHitchance.SetValue((float)Menu::Window.SettingsTab.RifleChance.GetValue());
		}
		else if (GameUtils::IsMP(pWeapon))
		{

			Menu::Window.RageBotTab.AccuracyHitchance.SetValue((float)Menu::Window.SettingsTab.MPChance.GetValue());
		}
	}
}

static __declspec(naked) void __cdecl Invoke_NET_SetConVar(void* pfn, const char* cvar, const char* value)
{
	__asm 
	{
		push    ebp
			mov     ebp, esp
			and     esp, 0FFFFFFF8h
			sub     esp, 44h
			push    ebx
			push    esi
			push    edi
			mov     edi, cvar
			mov     esi, value
			jmp     pfn
	}
}

void DECLSPEC_NOINLINE NET_SetConVar(const char* value, const char* cvar)
{
	static DWORD setaddr = Utilities::Memory::FindPattern("engine.dll", (PBYTE)"\x8D\x4C\x24\x1C\xE8\x00\x00\x00\x00\x56", "xxxxx????x");
	if (setaddr != 0) 
	{
		void* pvSetConVar = (char*)setaddr;
		Invoke_NET_SetConVar(pvSetConVar, cvar, value);
	}
}

void change_name(const char* name)
{
	if (Interfaces::Engine->IsInGame() && Interfaces::Engine->IsConnected())
		NET_SetConVar(name, "name");
}

void CMiscHacks::AutoPistol(CUserCmd* pCmd)
{
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(hackManager.pLocal()->GetActiveWeaponHandle());

	if (pWeapon)
	{

		if (GetAsyncKeyState(Menu::Window.SettingsTab.minimalwalk.GetKey()))
		{
			MinimalWalk(pCmd, get_gun(pWeapon));
		}

		if (GameUtils::IsBomb(pWeapon))
		{
			return;
		}
		if (!GameUtils::IsNotPistol(pWeapon))
		{
			return;
		}
		if (GameUtils::IsGrenade(pWeapon))
		{
			return;
		}

	}
	static bool WasFiring = false;

	if (GameUtils::IsPistol)
	{
		if (pCmd->buttons & IN_ATTACK)
		{
			if (WasFiring)
			{
				pCmd->buttons &= ~IN_ATTACK;
			}
		}
		WasFiring = pCmd->buttons & IN_ATTACK ? true : false;
	}
}

void CMiscHacks::Namespam()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;

	if (wasSpamming)
	{
		static bool useSpace = true;
		if (useSpace)
		{
			change_name("�poseidon");
			useSpace = !useSpace;
		}
		else
		{
			change_name("�poseidon");
			useSpace = !useSpace;
		}

	}

	start_t = clock();
}

void CMiscHacks::NoName()
{

	change_name("\n���");
}

void CMiscHacks::NameSteal()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	std::vector < std::string > Names;

	for (int i = 0; i < Interfaces::EntList->GetHighestEntityIndex(); i++)
	{

		IClientEntity *entity = Interfaces::EntList->GetClientEntity(i);

		player_info_t pInfo;

		if (entity && hackManager.pLocal()->GetTeamNum() == entity->GetTeamNum() && entity != hackManager.pLocal())
		{
			ClientClass* cClass = (ClientClass*)entity->GetClientClass();

			if (cClass->m_ClassID == (int)CSGOClassID::CCSPlayer)
			{
				if (Interfaces::Engine->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.name, "GOTV"))
						Names.push_back(pInfo.name);
				}
			}

		}
	}

	static bool wasSpamming = true;

	int randomIndex = rand() % Names.size();
	char buffer[128];
	sprintf_s(buffer, "%s ", Names[randomIndex].c_str());

	if (wasSpamming)
	{

		change_name(buffer);
	}
	else
	{

		change_name("p$i 1337");
	}

	start_t = clock();
}


void CMiscHacks::ChatSpamInterwebz()
{
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.001)
		return;

	static bool wasSpamming = true;
	//static std::string nameBackup = "INTERWEBZ";

	if (wasSpamming)
	{

		static bool useSpace = true;
		if (useSpace)
		{
			change_name("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n----------------------------poseidon----------------------------");
			useSpace = !useSpace;
		}
		else
		{

			change_name("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n----------------------------poseidon----------------------------");
			useSpace = !useSpace;
		}
	}

	start_t = clock();
}

void CMiscHacks::AutoJump(CUserCmd *pCmd)
{
	if (pCmd->buttons & IN_JUMP && GUI.GetKeyState(VK_SPACE))
	{
		int iFlags = hackManager.pLocal()->GetFlags();
		if (!(iFlags & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;

		if (hackManager.pLocal()->GetVelocity().Length() <= 50)
		{
			pCmd->forwardmove = 450.f;
		}

	}
}

void CMiscHacks::LegitStrafe(CUserCmd *pCmd)
{
	IClientEntity* pLocal = hackManager.pLocal();
	if (!(pLocal->GetFlags() & FL_ONGROUND))
	{
		pCmd->forwardmove = 0.0f;

		if (pCmd->mousedx < 0)
		{
			pCmd->sidemove = -450.0f;
		}
		else if (pCmd->mousedx > 0)
		{
			pCmd->sidemove = 450.0f;
		}

	}
}

void CMiscHacks::RageStrafe(CUserCmd *pCmd)
{

	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());

	static bool bDirection = true;

	static float move = 450.f;
	float s_move = move * 0.5065f;
	static float strafe = pCmd->viewangles.y;
	float rt = pCmd->viewangles.y, rotation;

	if ((pCmd->buttons & IN_JUMP) || !(pLocal->GetFlags() & FL_ONGROUND))
	{

		pCmd->forwardmove = move * 0.015f;
		pCmd->sidemove += (float)(((pCmd->tick_count % 2) * 2) - 1) * s_move;

		if (pCmd->mousedx)
			pCmd->sidemove = (float)clamp(pCmd->mousedx, -1, 1) * s_move;

		rotation = strafe - rt;

		strafe = rt;

		IClientEntity* pLocal = hackManager.pLocal();
		static bool bDirection = true;

		bool bKeysPressed = true;

		if (GUI.GetKeyState(0x41) || GUI.GetKeyState(0x57) || GUI.GetKeyState(0x53) || GUI.GetKeyState(0x44))
			bKeysPressed = false;
		if (pCmd->buttons & IN_ATTACK)
			bKeysPressed = false;

		float flYawBhop = 0.f;

		float sdmw = pCmd->sidemove;
		float fdmw = pCmd->forwardmove;

		static float move = 450.f;
		float s_move = move * 0.5276f;
		static float strafe = pCmd->viewangles.y;

		if (Menu::Window.MiscTab.OtherAutoStrafe.GetIndex() == 2 && !GetAsyncKeyState(VK_RBUTTON))
		{
			if (pLocal->GetVelocity().Length() > 45.f)
			{
				float x = 30.f, y = pLocal->GetVelocity().Length(), z = 0.f, a = 0.f;

				z = x / y;
				z = fabsf(z);

				a = x * z;

				flYawBhop = a;
			}

			if ((GetAsyncKeyState(VK_SPACE) && !(pLocal->GetFlags() & FL_ONGROUND)) && bKeysPressed)
			{

				if (bDirection)
				{
					AutoStrafeView -= flYawBhop;
					GameUtils::NormaliseViewAngle(AutoStrafeView);
					pCmd->sidemove = -450;
					bDirection = false;
				}
				else
				{
					AutoStrafeView += flYawBhop;
					GameUtils::NormaliseViewAngle(AutoStrafeView);
					pCmd->sidemove = 430;
					bDirection = true;
				}

				if (pCmd->mousedx < 0)
				{
					pCmd->forwardmove = 22;
					pCmd->sidemove = -450;
				}

				if (pCmd->mousedx > 0)
				{
					pCmd->forwardmove = +22;
					pCmd->sidemove = 450;
				}
			}
		}
	}
}

Vector GetAutostrafeView()
{
	return AutoStrafeView;
}
