#pragma once

extern bool DoUnload;

#include "Vector.h"
#include "Vector2D.h"
#include "Utilities.h"
#include "IMDLCache.h"


namespace resolvokek
{
	extern int Shots;
	extern int missedshots;
	extern float RealAngle;
	extern float FakeAngle;
	extern bool shouldflip;
	extern bool ySwitch;
	extern float NextTime;
	extern int resolvemode;
	extern float fakeAngle;
	extern float OldSimulationTime[65];
	extern bool error;
}


namespace Hooks
{
	void Initialise();
	void UndoHooks();

	extern Utilities::Memory::VMTManager VMTPanel; 
	extern Utilities::Memory::VMTManager VMTFindMdl;
	extern Utilities::Memory::VMTManager VMTClient; 
	extern Utilities::Memory::VMTManager VMTClientMode;
	extern Utilities::Memory::VMTManager VMTModelRender;
	extern Utilities::Memory::VMTManager VMTPrediction; 
	extern Utilities::Memory::VMTManager VMTRenderView;
	extern Utilities::Memory::VMTManager VMTEventManager;
	extern Utilities::Memory::VMTManager VMTCreateMove;
};

namespace Resolver
{
	extern bool didhitHS;
}

class AnimatedClanTag
{
public:
	//	void Tick();
};

extern AnimatedClanTag *animatedClanTag;

class beam
{
public:
	float real = 0.f;
	Vector cham_origin;
	Vector cham_angle;
};
extern beam *c_beam;
extern bool flipAA;	