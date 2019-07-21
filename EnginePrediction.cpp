#include "EnginePrediction.h"
#include "NetVars.h"
#include "MathFunctions.h"
#include "Hacks.h"
#include "EnginePrediction.h"

void CPredictionSystem::StartPrediction(CUserCmd* pCmd)
{
	//i removed my one and added mirrors as i dont wanna leak mine

	static int flTickBase;

	if (pCmd)
	{
		if (pCmd->hasbeenpredicted)
			flTickBase = hackManager.pLocal()->GetTickBase();
		else
			++flTickBase;
	}

	static bool bInit = false;
	if (!bInit) {
		m_pPredictionRandomSeed = *(int**)(Utilities::Memory::FindPatternV2("client_panorama.dll", "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 2);
		bInit = true;
	}

	//	*m_pPredictionRandomSeed = MD5_PseudoRandom(Hacks.CurrentCmd->command_number) & 0x7FFFFFFF; /* no thanks */

	m_flOldCurtime = Interfaces::Globals->curtime;
	m_flOldFrametime = Interfaces::Globals->frametime;

	if (!Interfaces::GameMovement)
		return;

	Interfaces::Globals->curtime = flTickBase * Interfaces::Globals->interval_per_tick;
	Interfaces::Globals->frametime = Interfaces::Globals->interval_per_tick;

	Interfaces::GameMovement->StartTrackPredictionErrors(hackManager.pLocal());

	memset(&m_MoveData, 0, sizeof(m_MoveData));

	Interfaces::MoveHelper->SetHost(hackManager.pLocal());
	Interfaces::Prediction1->SetupMove(hackManager.pLocal(), pCmd, Interfaces::MoveHelper, &m_MoveData);
	Interfaces::GameMovement->ProcessMovement(hackManager.pLocal(), &m_MoveData);
	Interfaces::Prediction1->FinishMove(hackManager.pLocal(), pCmd, &m_MoveData);
}
void CPredictionSystem::EndPrediction(CUserCmd* pCmd)
{
	//i removed my one and added mirrors as i dont wanna leak mine
	if (!Interfaces::GameMovement)
		return;

	Interfaces::GameMovement->FinishTrackPredictionErrors(hackManager.pLocal());
	Interfaces::MoveHelper->SetHost(0);

	*m_pPredictionRandomSeed = -1;

	Interfaces::Globals->curtime = m_flOldCurtime;
	Interfaces::Globals->frametime = m_flOldFrametime;
}




