#pragma once
#include "Globalshhh.h"
#include "MiscDefinitions.h"
#include "ClientRecvProps.h"
#include "offsets.h"
#include "Vector.h"

#define TEAM_CS_T 2
#define TEAM_CS_CT 3

#define BONE_USED_BY_HITBOX			0x00000100

#define ptr( x, x1, x2 ) *(x*)( (DWORD)x1 + (DWORD)x2 )

#define TICK_INTERVAL			(Interfaces::Globals->interval_per_tick)

#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )

class IClientRenderable;
class IClientNetworkable;
class IClientUnknown;
class IClientThinkable;
class IClientEntity;
class CSWeaponInfo;



enum class TeamID : int
{
	TEAM_UNASSIGNED,
	TEAM_SPECTATOR,
	TEAM_TERRORIST,
	TEAM_COUNTER_TERRORIST,
};

class CSWeaponInfo
{
public:
	char _0x0000[20];
	__int32 max_clip;			//0x0014 
	char _0x0018[12];
	__int32 max_reserved_ammo;	//0x0024 
	char _0x0028[96];
	char* szBulletType; //0x0080 
	char* weapon_name;			//0x008C 
	char _0x0090[56];
	int iWeaponType;
	char _0x0094[4];
	__int32 price;				//0x00D0 
	__int32 reward;				//0x00D4 
	char _0x00D8[20];
	BYTE full_auto;				//0x00EC 
	char _0x00ED[3];
	int	damage;				//0x00F0 
	float armor_ratio;			//0x00F4 
	__int32 bullets;			//0x00F8 
	float penetration;		//0x00FC 
	char _0x0100[8];
	float range;				//0x0108 
	float range_modifier;		//0x010C 
	char _0x0110[16];
	BYTE silencer;				//0x0120 
	char _0x0121[15];
	float max_speed;			//0x0130 
	float max_speed_alt;		//0x0134 
	char _0x0138[76];
	__int32 recoil_seed;		//0x0184 
	char _0x0188[32];
};//Size=0x240

enum class CSGOClassID {
	CAI_BaseNPC = 0,
	CAK47,
	CBaseAnimating,
	CBaseAnimatingOverlay,
	CBaseAttributableItem,
	CBaseButton,
	CBaseCombatCharacter,
	CBaseCombatWeapon,
	CBaseCSGrenade,
	CBaseCSGrenadeProjectile,
	CBaseDoor,
	CBaseEntity,
	CBaseFlex,
	CBaseGrenade,
	CBaseParticleEntity,
	CBasePlayer,
	CBasePropDoor,
	CBaseTeamObjectiveResource,
	CBaseTempEntity,
	CBaseToggle,
	CBaseTrigger,
	CBaseViewModel,
	CBaseVPhysicsTrigger,
	CBaseWeaponWorldModel,
	CBeam,
	CBeamSpotlight,
	CBoneFollower,
	CBRC4Target,
	CBreachCharge,
	CBreachChargeProjectile,
	CBreakableProp,
	CBreakableSurface,
	CBumpMine,
	CBumpMineProjectile,
	CC4,
	CCascadeLight,
	CChicken,
	CColorCorrection,
	CColorCorrectionVolume,
	CCSGameRulesProxy,
	CCSPlayer,
	CCSPlayerResource,
	CCSRagdoll,
	CCSTeam,
	CDangerZone,
	CDangerZoneController,
	CDEagle,
	CDecoyGrenade,
	CDecoyProjectile,
	CDrone,
	CDronegun,
	CDynamicLight,
	CDynamicProp,
	CEconEntity,
	CEconWearable,
	CEmbers,
	CEntityDissolve,
	CEntityFlame,
	CEntityFreezing,
	CEntityParticleTrail,
	CEnvAmbientLight,
	CEnvDetailController,
	CEnvDOFController,
	CEnvGasCanister,
	CEnvParticleScript,
	CEnvProjectedTexture,
	CEnvQuadraticBeam,
	CEnvScreenEffect,
	CEnvScreenOverlay,
	CEnvTonemapController,
	CEnvWind,
	CFEPlayerDecal,
	CFireCrackerBlast,
	CFireSmoke,
	CFireTrail,
	CFish,
	CFists,
	CFlashbang,
	CFogController,
	CFootstepControl,
	CFunc_Dust,
	CFunc_LOD,
	CFuncAreaPortalWindow,
	CFuncBrush,
	CFuncConveyor,
	CFuncLadder,
	CFuncMonitor,
	CFuncMoveLinear,
	CFuncOccluder,
	CFuncReflectiveGlass,
	CFuncRotating,
	CFuncSmokeVolume,
	CFuncTrackTrain,
	CGameRulesProxy,
	CGrassBurn,
	CHandleTest,
	CHEGrenade,
	CHostage,
	CHostageCarriableProp,
	CIncendiaryGrenade,
	CInferno,
	CInfoLadderDismount,
	CInfoMapRegion,
	CInfoOverlayAccessor,
	CItem_Healthshot,
	CItemCash,
	CItemDogtags,
	CKnife,
	CKnifeGG,
	CLightGlow,
	CMaterialModifyControl,
	CMelee,
	CMolotovGrenade,
	CMolotovProjectile,
	CMovieDisplay,
	CParadropChopper,
	CParticleFire,
	CParticlePerformanceMonitor,
	CParticleSystem,
	CPhysBox,
	CPhysBoxMultiplayer,
	CPhysicsProp,
	CPhysicsPropMultiplayer,
	CPhysMagnet,
	CPhysPropAmmoBox,
	CPhysPropLootCrate,
	CPhysPropRadarJammer,
	CPhysPropWeaponUpgrade,
	CPlantedC4,
	CPlasma,
	CPlayerPing,
	CPlayerResource,
	CPointCamera,
	CPointCommentaryNode,
	CPointWorldText,
	CPoseController,
	CPostProcessController,
	CPrecipitation,
	CPrecipitationBlocker,
	CPredictedViewModel,
	CProp_Hallucination,
	CPropCounter,
	CPropDoorRotating,
	CPropJeep,
	CPropVehicleDriveable,
	CRagdollManager,
	CRagdollProp,
	CRagdollPropAttached,
	CRopeKeyframe,
	CSCAR17,
	CSceneEntity,
	CSensorGrenade,
	CSensorGrenadeProjectile,
	CShadowControl,
	CSlideshowDisplay,
	CSmokeGrenade,
	CSmokeGrenadeProjectile,
	CSmokeStack,
	CSnowball,
	CSnowballPile,
	CSnowballProjectile,
	CSpatialEntity,
	CSpotlightEnd,
	CSprite,
	CSpriteOriented,
	CSpriteTrail,
	CStatueProp,
	CSteamJet,
	CSun,
	CSunlightShadowControl,
	CSurvivalSpawnChopper,
	CTablet,
	CTeam,
	CTeamplayRoundBasedRulesProxy,
	CTEArmorRicochet,
	CTEBaseBeam,
	CTEBeamEntPoint,
	CTEBeamEnts,
	CTEBeamFollow,
	CTEBeamLaser,
	CTEBeamPoints,
	CTEBeamRing,
	CTEBeamRingPoint,
	CTEBeamSpline,
	CTEBloodSprite,
	CTEBloodStream,
	CTEBreakModel,
	CTEBSPDecal,
	CTEBubbles,
	CTEBubbleTrail,
	CTEClientProjectile,
	CTEDecal,
	CTEDust,
	CTEDynamicLight,
	CTEEffectDispatch,
	CTEEnergySplash,
	CTEExplosion,
	CTEFireBullets,
	CTEFizz,
	CTEFootprintDecal,
	CTEFoundryHelpers,
	CTEGaussExplosion,
	CTEGlowSprite,
	CTEImpact,
	CTEKillPlayerAttachments,
	CTELargeFunnel,
	CTEMetalSparks,
	CTEMuzzleFlash,
	CTEParticleSystem,
	CTEPhysicsProp,
	CTEPlantBomb,
	CTEPlayerAnimEvent,
	CTEPlayerDecal,
	CTEProjectedDecal,
	CTERadioIcon,
	CTEShatterSurface,
	CTEShowLine,
	CTesla,
	CTESmoke,
	CTESparks,
	CTESprite,
	CTESpriteSpray,
	CTest_ProxyToggle_Networkable,
	CTestTraceline,
	CTEWorldDecal,
	CTriggerPlayerMovement,
	CTriggerSoundOperator,
	CVGuiScreen,
	CVoteController,
	CWaterBullet,
	CWaterLODControl,
	CWeaponAug,
	CWeaponAWP,
	CWeaponBaseItem,
	CWeaponBizon,
	CWeaponCSBase,
	CWeaponCSBaseGun,
	CWeaponCycler,
	CWeaponElite,
	CWeaponFamas,
	CWeaponFiveSeven,
	CWeaponG3SG1,
	CWeaponGalil,
	CWeaponGalilAR,
	CWeaponGlock,
	CWeaponHKP2000,
	CWeaponM249,
	CWeaponM3,
	CWeaponM4A1,
	CWeaponMAC10,
	CWeaponMag7,
	CWeaponMP5Navy,
	CWeaponMP7,
	CWeaponMP9,
	CWeaponNegev,
	CWeaponNOVA,
	CWeaponP228,
	CWeaponP250,
	CWeaponP90,
	CWeaponSawedoff,
	CWeaponSCAR20,
	CWeaponScout,
	CWeaponSG550,
	CWeaponSG552,
	CWeaponSG556,
	CWeaponShield,
	CWeaponSSG08,
	CWeaponTaser,
	CWeaponTec9,
	CWeaponTMP,
	CWeaponUMP45,
	CWeaponUSP,
	CWeaponXM1014,
	CWorld,
	CWorldVguiText,
	DustTrail,
	MovieExplosion,
	ParticleSmokeGrenade,
	RocketTrail,
	SmokeTrail,
	SporeExplosion,
	SporeTrail,
};

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

enum moveTypes
{
	MOVETYPE_NONE = 0,			// never moves
	MOVETYPE_ISOMETRIC,					// For players -- in TF2 commander view, etc.
	MOVETYPE_WALK,						// Player only - moving on the ground
	MOVETYPE_STEP,						// gravity, special edge handling -- monsters use this
	MOVETYPE_FLY,						// No gravity, but still collides with stuff
	MOVETYPE_FLYGRAVITY,				// flies through the air + is affected by gravity
	MOVETYPE_VPHYSICS,					// uses VPHYSICS for simulation
	MOVETYPE_PUSH,						// no clip to world, push and crush
	MOVETYPE_NOCLIP,					// No gravity, no collisions, still do velocity/avelocity
	MOVETYPE_LADDER,					// Used by players only when going onto a ladder
	MOVETYPE_OBSERVER,					// Observer movement, depends on player's observer mode
	MOVETYPE_CUSTOM,					// Allows the entity to describe its own physics
	MOVETYPE_LAST = MOVETYPE_CUSTOM,	// should always be defined as the last item in the list
	MOVETYPE_MAX_BITS = 4
};

enum class CSGOHitboxID : int
{
	Head = 0,
	Neck,
	Pelvis,
	Stomach,
	LowerChest,
	Chest,
	UpperChest,
	RightThigh,
	LeftThigh,
	RightShin,
	LeftShin,
	RightFoot,
	LeftFoot,
	RightHand,
	LeftHand,
	RightUpperArm,
	RightLowerArm,
	LeftUpperArm,
	LeftLowerArm,
	Max,
};

enum ItemDefinitionIndex : short
{
	WEAPON_NONE = 0,
	WEAPON_DEAGLE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_GLOCK,
	WEAPON_AK47 = 7,
	WEAPON_AUG,
	WEAPON_AWP,
	WEAPON_FAMAS,
	WEAPON_G3SG1,
	WEAPON_GALILAR = 13,
	WEAPON_M249,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,
	WEAPON_XM1014,
	WEAPON_BIZON,
	WEAPON_MAG7,
	WEAPON_NEGEV,
	WEAPON_SAWEDOFF,
	WEAPON_TEC9,
	WEAPON_TASER,
	WEAPON_HKP2000,
	WEAPON_MP7,
	WEAPON_MP9,
	WEAPON_NOVA,
	WEAPON_P250,
	WEAPON_SCAR20 = 38,
	WEAPON_SG556,
	WEAPON_SSG08,
	WEAPON_KNIFE_CT,
	WEAPON_KNIFE,
	WEAPON_FLASHBANG,
	WEAPON_HEGRENADE,
	WEAPON_SMOKEGRENADE,
	WEAPON_MOLOTOV,
	WEAPON_DECOY,
	WEAPON_INCGRENADE,
	WEAPON_C4,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER,
	WEAPON_USP_SILENCER,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,
	WEAPON_BREACHCHARGE,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE,
	WEAPON_HAMMER,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB,
	WEAPON_DIVERSION,
	WEAPON_FRAG_GRENADE,
	WEAPON_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT,
	WEAPON_KNIFE_KARAMBIT,
	WEAPON_KNIFE_M9_BAYONET,
	WEAPON_KNIFE_TACTICAL,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY,
	WEAPON_KNIFE_PUSH,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER
};
class AnimationLayer
{
public:
	char  pad_0000[20];
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void *m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Siz
class CBaseAnimState
{
public:
	char pad[4];
	char bUnknown; //0x4
	char pad2[91];
	void* pBaseEntity; //0x60
	void* pActiveWeapon; //0x64
	void* pLastActiveWeapon; //0x68
	float m_flLastClientSideAnimationUpdateTime; //0x6C
	int m_iLastClientSideAnimationUpdateFramecount; //0x70
	float m_flEyePitch; //0x74
	float m_flEyeYaw; //0x78
	float m_flPitch; //0x7C
	float m_flGoalFeetYaw; //0x80
	float m_flCurrentFeetYaw; //0x84
	float m_flCurrentTorsoYaw; //0x88
	float m_flUnknownVelocityLean; //0x8C //changes when moving/jumping/hitting ground
	float m_flLeanAmount; //0x90
	char pad4[4]; //NaN
	float m_flFeetCycle; //0x98 0 to 1
	float m_flFeetYawRate; //0x9C 0 to 1
	float m_fUnknown2;
	float m_fDuckAmount; //0xA4
	float m_fLandingDuckAdditiveSomething; //0xA8
	float m_fUnknown3; //0xAC
	vec_t m_vOrigin; //0xB0, 0xB4, 0xB8
	vec_t m_vLastOrigin; //0xBC, 0xC0, 0xC4
	float m_vVelocityX; //0xC8
	float m_vVelocityY; //0xCC
	char pad5[4];
	float m_flUnknownFloat1; //0xD4 Affected by movement and direction
	char pad6[8];
	float m_flUnknownFloat2; //0xE0 //from -1 to 1 when moving and affected by direction
	float m_flUnknownFloat3; //0xE4 //from -1 to 1 when moving and affected by direction
	float m_unknown; //0xE8
	float speed_2d; //0xEC
	float flUpVelocity; //0xF0
	float m_flSpeedNormalized; //0xF4 //from 0 to 1
	float m_flFeetSpeedForwardsOrSideWays; //0xF8 //from 0 to 2. something  is 1 when walking, 2.something when running, 0.653 when crouch walking
	float m_flFeetSpeedUnknownForwardOrSideways; //0xFC //from 0 to 3. something
	float m_flTimeSinceStartedMoving; //0x100
	float m_flTimeSinceStoppedMoving; //0x104
	unsigned char m_bOnGround; //0x108
	int8_t m_bInHitGroundAnimation; //0x109
	char pad7[10];
	float m_flLastOriginZ; //0x114
	float m_flHeadHeightOrOffsetFromHittingGroundAnimation; //0x118 from 0 to 1, is 1 when standing
	float m_flStopToFullRunningFraction; //0x11C from 0 to 1, doesnt change when walking or crouching, only running
	char pad8[4]; //NaN
	float m_flUnknownFraction; //0x124 affected while jumping and running, or when just jumping, 0 to 1
	char pad9[4]; //NaN
	float m_flUnknown3;
	char pad10[528];
	float* feetyaw() //rofl
	{
		return reinterpret_cast<float*>((DWORD)this + 0x0080);
	}
};

class ICollideable
{
public:
	virtual void pad0();
	virtual const Vector& OBBMins() const;
	virtual const Vector& OBBMaxs() const;
};

class ScriptCreatedItem
{
public:
	CPNETVAR_FUNC(short*, ItemDefinitionIndex, 0xE67AB3B8); //m_iItemDefinitionIndex
	CPNETVAR_FUNC(int*, ItemIDHigh, 0x714778A); //m_iItemIDHigh
	CPNETVAR_FUNC(int*, ItemIDLow, 0x3A3DFC74); //m_iItemIDLow
	CPNETVAR_FUNC(int*, AccountID, 0x24abbea8); //m_iAccountID
};

class AttributeContainer
{
public:
	CPNETVAR_FUNC(ScriptCreatedItem*, m_Item, 0x7E029CE5);
};
class C_BaseCombatWeapon
{
public:
	CNETVAR_FUNC(float, GetNextPrimaryAttack, 0xDB7B106E); //m_flNextPrimaryAttack
	CNETVAR_FUNC(int, GetAmmoInClip, 0x97B6F70C); //m_iClip1
	CNETVAR_FUNC(HANDLE, GetOwnerHandle, 0xC32DF98D); //m_hOwner
	CNETVAR_FUNC(float, GetAccuracyPenalty, 0xE2958A63); //m_fAccuracyPenalty
	CNETVAR_FUNC(Vector, GetOrigin, 0x1231CE10); //m_vecOrigin
	CPNETVAR_FUNC(int*, FallbackPaintKit, 0xADE4C870); // m_nFallbackPaintKit
	CPNETVAR_FUNC(int*, FallbackSeed, 0xC2D0683D); // m_nFallbackSeed
	CPNETVAR_FUNC(float*, FallbackWear, 0xA263576C); //m_flFallbackWear
	CPNETVAR_FUNC(int*, FallbackStatTrak, 0x1ED78768); //m_nFallbackStatTrak
	CPNETVAR_FUNC(int*, OwnerXuidLow, 0xAD8D897F);
	CPNETVAR_FUNC(int*, OwnerXuidHigh, 0x90511E77);
	CPNETVAR_FUNC(int*, ViewModelIndex, 0x7F7C89C1);
	CPNETVAR_FUNC(int*, ModelIndex, 0x27016F83);
	CPNETVAR_FUNC(int*, WorldModelIndex, 0x4D8AD9F3);
	CPNETVAR_FUNC(char*, szCustomName, 0x0);
	CPNETVAR_FUNC(AttributeContainer*, m_AttributeManager, 0xCFFCE089);
	CNETVAR_FUNC(int, GetZoomLevel, 0x26553F1A);


	template< class T >
	inline T GetFieldValue(int offset)
	{
		return *(T*)((DWORD)this + offset);
	}

	float GetLastShotTime()
	{
		static int m_fLastShotTime = GET_NETVAR("DT_WeaponCSBase", "m_fLastShotTime");
		return GetFieldValue<float>(m_fLastShotTime);
	}

	float GetInaccuracy()
	{
		typedef float(__thiscall* oInaccuracy)(PVOID);
		return call_vfunc< oInaccuracy >(this, 476)
			(this);
	}

	float GetSpread()
	{
		typedef float(__thiscall *OrigFn)(void *);
		return call_vfunc<OrigFn>(this, 446)(this);
	}
	short* GetItemDefinitionIndex()
	{
		return (short*)m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	}
	bool isAuto()
	{
		short weapon_id = *this->GetItemDefinitionIndex();
		return weapon_id == WEAPON_SCAR20 || WEAPON_G3SG1;
	}
	bool isAwp()
	{
		short weapon_id = *this->GetItemDefinitionIndex();
		return weapon_id == WEAPON_AWP;
	}
	bool isScout()
	{
		short weapon_id = *this->GetItemDefinitionIndex();
		return weapon_id == WEAPON_SSG08;
	}

	int* GetEntityQuality()
	{
		return (int*)((DWORD)this + 0x2D70 + 0x40 + 0x1DC);  
	}

	int GetWeaponID2()
	{
		return *(int*)((DWORD)this + 0x2F98);
	}

	std::string GetName()
	{
		///TODO: Test if szWeaponName returns proper value for m4a4 / m4a1-s or it doesnt recognize them.
		return std::string(this->GetCSWpnData()->weapon_name);
	}

	CBaseAnimState *AnimState()
	{
		return *reinterpret_cast<CBaseAnimState**>(uintptr_t(this) + 0x3900);
	}

	void PreDataUpdate(int updateType)
	{
		PVOID pNetworkable = (PVOID)((DWORD)(this) + 0x8);
		typedef void(__thiscall* OriginalFn)(PVOID, int);
		return call_vfunc<OriginalFn>(pNetworkable, 6)(pNetworkable, updateType);
	}


	bool isZeus27()
	{
		short weapon_id = *this->GetItemDefinitionIndex();
		return weapon_id == WEAPON_TASER;
	}

	void SetModelIndex(int modelIndex) {
		typedef void(__thiscall* OriginalFn)(PVOID, int);
		return call_vfunc<OriginalFn>(this, 75)(this, modelIndex);
	}

	void UpdateAccPenalty()
	{
		typedef void(__thiscall *OrigFn)(void *);
		return call_vfunc<OrigFn>(this, 477)(this);
	}

	bool IsScoped(int x = 0)
	{
		return GetZoomLevel() > 0;
	}

	CSWeaponInfo* GetCSWpnData()
	{
		if (!this)
			return nullptr;

		typedef CSWeaponInfo*(__thiscall* OriginalFn)(void*);
		return call_vfunc< OriginalFn >(this, 454)(this);
		return nullptr;
	}
};

class CCSBomb
{
public:
	CNETVAR_FUNC(HANDLE, GetOwnerHandle, 0xC32DF98D); //m_hOwner 0x29BC
	CNETVAR_FUNC(float, GetC4BlowTime, 0xB5E0CA1C); //m_flC4Blow
	CNETVAR_FUNC(float, GetC4DefuseCountDown, 0x2994); //m_flDefuseCountDown 0x2994
};

class CLocalPlayerExclusive
{
public:
	CNETVAR_FUNC(Vector, GetViewPunchAngle, 0x68F014C0);//m_viewPunchAngle
	CNETVAR_FUNC(Vector, GetAimPunchAngle, 0xBF25C290);//m_aimPunchAngle
	CNETVAR_FUNC(Vector, GetAimPunchAngleVel, 0x8425E045);//m_aimPunchAngleVel
};

class CollisionProperty
{
public:
	CNETVAR_FUNC(Vector, GetMins, 0xF6F78BAB);//m_vecMins
	CNETVAR_FUNC(Vector, GetMaxs, 0xE47C6FC4);//m_vecMaxs
	CNETVAR_FUNC(unsigned char, GetSolidType, 0xB86722A1);//m_nSolidType
	CNETVAR_FUNC(unsigned short, GetSolidFlags, 0x63BB24C1);//m_usSolidFlags
	CNETVAR_FUNC(int, GetSurroundsType, 0xB677A0BB); //m_nSurroundType

	bool IsSolid()
	{
		return (GetSolidType() != SOLID_NONE) && ((GetSolidFlags() & FSOLID_NOT_SOLID) == 0);
	}
};

class IClientRenderable
{
public:
	//virtual void*					GetIClientUnknown() = 0;
	virtual Vector const&			GetRenderOrigin(void) = 0;
	virtual Vector const&			GetRenderAngles(void) = 0;
	virtual bool					ShouldDraw(void) = 0;
	virtual bool					IsTransparent(void) = 0;
	virtual bool					UsesPowerOfTwoFrameBufferTexture() = 0;
	virtual bool					UsesFullFrameBufferTexture() = 0;
	virtual void					GetShadowHandle() const = 0;
	virtual void*					RenderHandle() = 0;
	virtual const model_t*				GetModel() const = 0;
	virtual int						DrawModel(int flags) = 0;
	virtual int						GetBody() = 0;
	virtual int                     GetSkin() = 0;
	virtual void					ComputeFxBlend() = 0;

	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		typedef bool(__thiscall* oSetupBones)(PVOID, matrix3x4*, int, int, float);
		return call_vfunc< oSetupBones>(this, 13)(this, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}
};

class IClientNetworkable
{
public:
	virtual IClientUnknown*	GetIClientUnknown() = 0;
	virtual void			Release() = 0;
	virtual ClientClass*	GetClientClass() = 0;// FOR NETVARS FIND YOURSELF ClientClass* stuffs
	virtual void			NotifyShouldTransmit( /* ShouldTransmitState_t state*/) = 0;
	virtual void			OnPreDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			OnDataChanged( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PreDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			PostDataUpdate( /*DataUpdateType_t updateType*/) = 0;
	virtual void			unknown();
	virtual bool			IsDormant(void) = 0;
	virtual int				GetIndex(void) const = 0;
	virtual void			ReceiveMessage(int classID /*, bf_read &msg*/) = 0;
	virtual void*			GetDataTableBasePtr() = 0;
	virtual void			SetDestroyedOnRecreateEntities(void) = 0;
};

class IClientUnknown
{
public:
	ICollideable* GetCollideable()
	{
		return (ICollideable*)((DWORD)this + 0x31C);
	}
	virtual IClientNetworkable*	GetClientNetworkable() = 0;
	virtual IClientRenderable*	GetClientRenderable() = 0;
	virtual IClientEntity*		GetIClientEntity() = 0;
	virtual IClientEntity*		GetBaseEntity() = 0;
	virtual IClientThinkable*	GetClientThinkable() = 0;
	IClientRenderable* GetRenderable()
	{
		return reinterpret_cast<IClientRenderable*>((DWORD)this + 0x4);
	}
};

class IClientThinkable
{
public:
	virtual IClientUnknown*		GetIClientUnknown() = 0;
	virtual void				ClientThink() = 0;
	virtual void*				GetThinkHandle() = 0;
	virtual void				SetThinkHandle(void* hThink) = 0;
	virtual void				Release() = 0;
};


inline void**& getvtable(void* inst, size_t offset = 0)
{

	return *reinterpret_cast<void***>((size_t)inst + offset);
}
inline const void** getvtable(const void* inst, size_t offset = 0)
{
	if (!inst && !offset)
		return NULL;
	return *reinterpret_cast<const void***>((size_t)inst + offset);
}
template< typename Fn >
inline Fn fvv(const void* inst, size_t index, size_t offset = 0)
{
	if (!inst && offset == 0)
		return NULL;

	return reinterpret_cast<Fn>(getvtable(inst, offset)[index]);
}
class CBaseViewModel : public IClientUnknown, public IClientRenderable, public IClientNetworkable {
public:
	inline int GetModelIndex() {
		return *(int*)((DWORD)this + 0x258);
	}
	inline void SetModelIndex(int val) {
		*(int*)((DWORD)this + 0x258) = val;
	}
	inline DWORD GetOwner() {
		return *(PDWORD)((DWORD)this + 0x29CC);
	}
	inline DWORD GetWeapon() {
		return *(PDWORD)((DWORD)this + 0x29C8);
	}
	inline void SetWeaponModel(const char* Filename, IClientUnknown* Weapon) {
		return fvv<void(__thiscall*)(void*, const char*, IClientUnknown*)>(this, 243)(this, Filename, Weapon);
	}
};

class CBaseAttributableItem : public IClientUnknown, public IClientRenderable, public IClientNetworkable {
public:
	inline short* GetItemDefinitionIndex() {
		return (short*)((DWORD)this + 0x2FAA);
	}
	inline int* GetItemIDHigh() {
		return (int*)((DWORD)this + 0x2FC0);
	}
	inline int* GetAccountID() {
		return (int*)((DWORD)this + 0x2FC8);
	}
	inline int* GetEntityQuality() {
		return (int*)((DWORD)this + 0x2FAC);
	}
	inline char* GetCustomName() {
		return (char*)((DWORD)this + 0x303C);
	}
	inline int* GetOriginalOwnerXuidLow() {
		return (int*)((DWORD)this + 0x31B0);
	}
	inline int* GetOriginalOwnerXuidHigh() {
		return (int*)((DWORD)this + 0x31B4);
	}
	inline int* GetFallbackPaintKit() {
		return (int*)((DWORD)this + 0x31B8);
	}
	inline int* GetFallbackSeed() {
		return (int*)((DWORD)this + 0x31BC);
	}
	inline float* GetFallbackWear() {
		return (float*)((DWORD)this + 0x31C0);
	}
	inline int* GetFallbackStatTrak() {
		return (int*)((DWORD)this + 0x31C4);
	}
	inline int GetModelIndex() {
		return *(int*)((DWORD)this + 0x258);
	}
	inline void SetModelIndex(int nModelIndex) {
		*(int*)((DWORD)this + 0x258) = nModelIndex;
	}

};

class __declspec (novtable)IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
public:
	virtual void			Release(void) = 0;
	virtual void			blahblahpad(void) = 0;
	virtual Vector&	GetAbsOrigin(void) const = 0;//in broken place use GetOrigin Below
	virtual const Vector&	GetAbsAngles(void) const = 0;


	Vector& GetAbsOrigin3()
	{
		//	typedef Vector(__thiscall* oLocal)(PVOID);
		//	return call_vfunc< oLocal >(this,2)(this);

		typedef Vector&(__thiscall *o_getAbsOriginal)(void*);
		return call_vfunc<o_getAbsOriginal>(this, 10)(this);
	}

	Vector GetAbsAngles_2()
	{
		typedef Vector&(__thiscall *o_getAbsAechse)(void*);
		return call_vfunc<o_getAbsAechse>(this, 11)(this);
	}

	int GetGlowIndex()
	{
		return *(int*)(this + 0xA3F8);
	}
	CNETVAR_FUNC(int, GetTeamNum, 0xC08B6C6E); //m_iTeamNum
	CNETVAR_FUNC(int, team, 0xC08B6C6E); //m_iTeamNum
	CNETVAR_FUNC(int, GetAmmoInClip, 0x97B6F70C);

	CPNETVAR_FUNC(Vector*, GetOriginPtr, 0x1231CE10); //m_vecOrigin
	CPNETVAR_FUNC(CLocalPlayerExclusive*, localPlayerExclusive, 0x7177BC3E);// m_Local
	CPNETVAR_FUNC(CollisionProperty*, collisionProperty, 0xE477CBD0);//m_Collision
	CNETVAR_FUNC(float, GetLowerBodyYaw, 0xE6996CCF); //m_flLowerBodyYawTarget
	CNETVAR_FUNC(int, GetFlags, 0xE456D580); //m_fFlags
	CNETVAR_FUNC(int, GetHealth, 0xA93054E3); //m_iHealth
	CNETVAR_FUNC(float, GetTargetYaw, 0xE6996CCF);
	CNETVAR_FUNC(Vector, GetOrigin, 0x1231CE10); //m_vecOrigin 0x0134
	CNETVAR_FUNC(Vector, GetRotation, 0x6BEA197A); //m_angRotation
	
	CNETVAR_FUNC(int*, GetPointerFlags, 0xE456D580); //m_fFlags
	CNETVAR_FUNC(HANDLE, GetOwnerHandle, 0xC32DF98D); //m_hOwner
	CNETVAR_FUNC(int, GetMaxHealth, 0xC52E1C28); //m_iMaxHealth
	CNETVAR_FUNC(Vector, GetVecOrigin, 0x134); //m_vecVelocity[0]
	
	CNETVAR_FUNC(bool, IsDefusing, 0xA2C14106); //m_bIsDefusing
	CNETVAR_FUNC(float, GetFlashDuration, 0x4B5938D5); //m_flFlashDuration
	CNETVAR_FUNC(float , GetFlashAlpha, 0xFE79FB98); //m_flFlashMaxAlpha
	CNETVAR_FUNC(unsigned char, GetLifeState, 0xD795CCFC); //m_lifeState
	CNETVAR_FUNC(HANDLE, GetActiveWeaponHandle, 0xB4FECDA3); //m_hActiveWeapon
	CNETVAR_FUNC(int, GetTickBase, 0xD472B079); //m_nTickBase
	CNETVAR_FUNC(Vector, GetViewOffset, 0xA9F74931); //m_vecViewOffset[0]
	CNETVAR_FUNC(Vector, GetViewPunch, 0x68F014C0);
	CNETVAR_FUNC(int, GetMoney, 0xF4B3E183); //m_iAccount
	CNETVAR_FUNC(Vector, GetPunch, 0xBF25C290);
	CNETVAR_FUNC(Vector, GetVelocity, 0x40C1CA24); //m_vecVelocity[0]
	CNETVAR_FUNC(bool, HasGunGameImmunity, 0x6AD6FA0D); //m_bGunGameImmunity
	CNETVAR_FUNC(int, ArmorValue, 0x3898634); //m_ArmorValue
	CNETVAR_FUNC(bool, HasHelmet, 0x7B97F18A); //m_bHasHelmet
	CNETVAR_FUNC(int, GetObserverMode, 0x2441D093); // m_iObserverMode
	CNETVAR_FUNC(HANDLE, GetObserverTargetHandle, 0x8660FD83); //m_hObserverTarget
	CNETVAR_FUNC(bool, HasDefuser, 0x32D0F325); //m_bHasDefuser
	CNETVAR_FUNC(int, GetShotsFired, 0x3F2F6C66); //m_nTickBase
	CNETVAR_FUNC(float, GetSimulationTime, 0xC4560E44); //m_flSimulationTime
	CNETVAR_FUNC(float, GetAnimTime, 0xD27E8416);
	CNETVAR_FUNC(bool, IsScoped, 0x61B9C22C); //m_bIsScoped

	Vector m_VecORIGIN()
	{
		return *(Vector*)((DWORD)this + 0x00000134);
	}

	//C_BaseCombatWeapon* GetWeapon2();
	bool IsKnifeorNade();

/*	float calculateDesyncDelta()
	{

		IClientEntity * pEntity;

		float maxDeg, v48;
		auto animState = pEntity->AnimState();

		if (!animState)
			return 0.f;

		float v11 = animState->m_flFeetSpeedUnknownForwardOrSideways;

		if (animState->m_flFeetSpeedForwardsOrSideWays >= 0.f)
			v48 = fminf(animState->m_flFeetSpeedForwardsOrSideWays, 1.f);
		else
			v48 = 0.f;

		maxDeg = ((animState->m_flStopToFullRunningFraction * -.30000001f) - .19999999f) * v48 + 1.f;

		if (animState->m_fDuckAmount > 0.f)
		{
			if (v11 >= 0.0f)
				v11 = fminf(v11, 1.0f);
			else
				v11 = 0.0f;

			maxDeg = maxDeg + ((animState->m_fDuckAmount * v11) * (.5f - maxDeg));
		}

		return *(float*)(((uintptr_t)animState + 0x334)) * maxDeg;
	}*/

	CPNETVAR_FUNC(AttributeContainer*, m_AttributeManager, 0xCFFCE089);
	short* GetItemDefinitionIndex()
	{
		return (short*)m_AttributeManager()->m_Item()->ItemDefinitionIndex();
	}

	char* IClientEntity::GetWeaponName()
	{
		int WeaponId = *this->GetItemDefinitionIndex();
		switch (WeaponId)
		{
		case WEAPON_KNIFE_CT:
			return "KNIFE CT";
		case WEAPON_KNIFE_T:
			return "KNIFE T";
		case 500:
		case 505:
		case 506:
		case 507:
		case 508:
		case 509:
		case 512:
		case 514:
		case 515:
		case 516:
			return "";
		case WEAPON_DEAGLE:
			return "DEAGLE";
		case WEAPON_ELITE:
			return "ELITE";
		case WEAPON_FIVESEVEN:
			return "FIVESEVEN";
		case WEAPON_GLOCK:
			return "GLOCK";
		case WEAPON_HKP2000:
			return "HKP2000";
		case WEAPON_P250:
			return "P250";
		case WEAPON_USP_SILENCER:
			return "USP-S";
		case WEAPON_TEC9:
			return "TEC9";
		case WEAPON_CZ75A:
			return "CZ75A";
		case WEAPON_REVOLVER:
			return "REVOLVER";
		case WEAPON_MAC10:
			return "MAC-10";
		case WEAPON_UMP45:
			return "UMP45";
		case WEAPON_BIZON:
			return "BIZON";
		case WEAPON_MP7:
			return "MP7";
		case WEAPON_MP9:
			return "MP9";
		case WEAPON_P90:
			return "P90";
		case WEAPON_GALILAR:
			return "GALILAR";
		case WEAPON_FAMAS:
			return "FAMAS";
		case WEAPON_M4A1_SILENCER:
			return "M4A1-S";
		case WEAPON_M4A1:
			return "M4A1";
		case WEAPON_AUG:
			return "AUG";
		case WEAPON_SG556:
			return "SG556";
		case WEAPON_AK47:
			return "Ak47";
		case WEAPON_G3SG1:
			return "G3SG1";
		case WEAPON_SCAR20:
			return "SCAR20";
		case WEAPON_AWP:
			return "AWP";
		case WEAPON_SSG08:
			return "SSG08";
		case WEAPON_XM1014:
			return "Xm1014";
		case WEAPON_SAWEDOFF:
			return "SAWEDOFF";
		case WEAPON_MAG7:
			return "MAG7";
		case WEAPON_NOVA:
			return "NOVA";
		case WEAPON_NEGEV:
			return "NEGEV";
		case WEAPON_M249:
			return "M249";
		case WEAPON_TASER:
			return "TASER";
		case WEAPON_FLASHBANG:
			return "Flashbang";
		case WEAPON_HEGRENADE:
			return "Grenade";
		case WEAPON_SMOKEGRENADE:
			return "Smoke";
		case WEAPON_MOLOTOV:
			return "Molotov";
		case WEAPON_DECOY:
			return "Decoy";
		case WEAPON_INCGRENADE:
			return "Incgrenade";
			/*case WEAPON_KNIFE_M9_BAYONET:
				return "M9 BAYONET KNIFE";
			case WEAPON_KNIFE_TACTICAL:
				return "KARAMBIT KNIFE";
			case WEAPON_KNIFE_FALCHION:
				return "FALCHION KNIFE";
			case WEAPON_KNIFE_BOWIE:
				return "BOWIE KNIFE";
			case WEAPON_KNIFE_BUTTERFLY:
				return "BUTTERFLY KNIFE";
			case WEAPON_KNIFE_PUSH:
				return "SHADOW KNIFE";*/
		case WEAPON_C4:
			return "C4";
		default:
			return " ";
		}
	}

	bool isValidPlayer()
	{
		if (!this)
			return false;
		if (!IsAlive())
			return false;
		//	if (!(GetHealth() > 0))
		//		return false;
		player_info_t info;
		return true;
	}

	void SetLowerBodyYaw(float value)
	{
		static int m_flLowerBodyYawTarget = GET_NETVAR("DT_CSPlayer", "m_flLowerBodyYawTarget");
		*reinterpret_cast<float*>(uintptr_t(this) + m_flLowerBodyYawTarget) = value;
	}

	void ClientAnimations(bool value)
	{
		static int m_bClientSideAnimation = GET_NETVAR("DT_BaseAnimating", "m_bClientSideAnimation");
		*reinterpret_cast<bool*>(uintptr_t(this) + m_bClientSideAnimation) = value;
	}
	bool IsAlive()
	{
		return (GetLifeState() == LIFE_ALIVE && GetHealth() > 0);
	}

	CBaseAnimState* GetBasePlayerAnimState()
	{
		return *(CBaseAnimState**)((DWORD)this + 0x3900);
	}

	AnimationLayer *AnimOverlays()
	{
		return *reinterpret_cast<AnimationLayer**>(uintptr_t(this) + 0x2980);
	}

	bool m_bIsControllingBot() {
		return *(bool*)((uintptr_t)this + 0xB995);
	}
		

	int GetNumAnimOverlays()
	{
		return *(int*)((DWORD)this + 0x298C);
	}

	AnimationLayer* get_anim_overlays()
	{
		// to find offset: use 9/12/17 dll
		// sig: 55 8B EC 51 53 8B 5D 08 33 C0
		return *(AnimationLayer**)((DWORD)this + 0x2980); //0x2970
	}

	AnimationLayer& GetAnimOverlay(int Index)
	{
		return (*(AnimationLayer**)((DWORD)this + 0x2980))[Index];
	}

	CBaseAnimState *AnimState()
	{
		return *reinterpret_cast<CBaseAnimState**>(uintptr_t(this) + 0x3900);
	}


	bool IsMoving()
	{
		return GetVelocity().Length2D() > 0.1f;
	}

	int GetMoveType() 
	{
		if (!this)
			return 0;

		return ptr(int, this, 0x25C);
	}

	QAngle* GetEyeAnglesPointer()
	{
		return reinterpret_cast<QAngle*>((DWORD)this + (DWORD)0x528C);
	}

	QAngle GetEyeAngles()
	{
		return *reinterpret_cast<QAngle*>((DWORD)this + (DWORD)0x528C);
	}

	int IClientEntity::GetChokedTicks() {
		if (this->GetSimulationTime() > globalsh.OldSimulationTime[this->GetIndex()])
			return fabs(this->GetSimulationTime() - globalsh.OldSimulationTime[this->GetIndex()]);
		return 0;
	}

	QAngle* GetEyeAnglesXY()
	{
		return (QAngle*)((DWORD)this + GET_NETVAR("DT_CSPlayer", "m_angEyeAngles"));
	}

	Vector GetBonePos(int i)
	{
		matrix3x4 boneMatrix[128];
		if (this->SetupBones(boneMatrix, 128, BONE_USED_BY_HITBOX, GetTickCount64()))
		{
			return Vector(boneMatrix[i][0][3], boneMatrix[i][1][3], boneMatrix[i][2][3]);
		}
		return Vector(0, 0, 0);
	}

	Vector GetHeadPos()
	{
		return this->GetBonePos(8);
	}

	Vector GetHeadAngle()
	{
		return GetAbsOrigin() + GetViewOffset();
	}

	bool IsPlayer()
	{
		return GetClientClass()->m_ClassID == (int)CSGOClassID::CCSPlayer;
	}

	Vector GetOrigin2() {
		return *(Vector*)((DWORD)this + 0x138);
	}

	Vector GetOrigin_likeajew() // m_vecOrigin
	{
		return *(Vector*)((DWORD)this + GET_NETVAR("DT_BaseEntity", "m_vecOrigin"));
	}
	int draw_model(int flags, uint8_t alpha) {
		using fn = int(__thiscall*)(void*, int, uint8_t);
		return call_vfunc< fn >(GetRenderable(), 9)(GetRenderable(), flags, alpha);
	}
	void IClientEntity::SetAbsAngles(Vector angle)
	{
		using SetAbsAnglesFn = void(__thiscall*)(void*, const Vector &angle);
		static SetAbsAnglesFn SetAbsAngles;

		if (!SetAbsAngles)
			SetAbsAngles = (SetAbsAnglesFn)(Utilities::Memory::FindPattern("client_panorama.dll", (BYTE*)"\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x64\x53\x56\x57\x8B\xF1\xE8", "xxxxxxxxxxxxxxx"));

		SetAbsAngles(this, angle);
	}
	void IClientEntity::SetAbsOriginal(Vector origin)
	{
		using SetAbsOriginFn = void(__thiscall*)(void*, const Vector &origin);
		static SetAbsOriginFn SetAbsOrigin;
		if (!SetAbsOrigin)
			SetAbsOrigin = (SetAbsOriginFn)((DWORD)Utilities::Memory::FindPatternV2(("client_panorama.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		SetAbsOrigin(this, origin);
	}

	float IClientEntity::m_flOldSimulationTime()
	{
		static int m_flSimulationTime = 0x264;
		return *(float*)((DWORD)this + (m_flSimulationTime + 0x4));
	}

	Vector GetViewAngles2() {
		return *(Vector*)((DWORD)this + 0x108);
	}

	template< class T >
	inline T GetFieldValue(int offset)
	{
		return *(T*)((DWORD)this + offset);
	}

	void UpdateClientSideAnimation()
	{
		typedef void(__thiscall *o_updateClientSideAnimation)(void*);
		call_vfunc<o_updateClientSideAnimation>(this, 222)(this);
	}
	void SetFlags(int flag)
	{
		*reinterpret_cast<int*>(uintptr_t(this) + 0x00000100) = flag;
	}

	Vector get_ragdoll_pos()
	{
		auto gay = GET_NETVAR("DT_Ragdoll", "m_ragPos");
		return GetFieldValue<Vector>(gay);
	}

	int &getFlags()
	{
		return *(int*)((DWORD)this + 0x00000100);
	}

	bool& client_side_animation()
	{
		static int m_bBombDefused = GET_NETVAR("DT_BaseAnimating", "m_bClientSideAnimation");
		return *(bool*)((DWORD)this + m_bBombDefused);
	}

	Vector GetAbsOrigin2() {
		__asm {
			MOV ECX, this
			MOV EAX, DWORD PTR DS : [ECX]
			CALL DWORD PTR DS : [EAX + 0x28]
		}
	}
	Vector GetAbsAngles2() {
		__asm {
			MOV ECX, this;
			MOV EAX, DWORD PTR DS : [ECX];
			CALL DWORD PTR DS : [EAX + 0x2C]
		}
	}

	Vector GetEyePosition() {
		Vector Origin = *(Vector*)((DWORD)this + 0x138);
		Vector View = *(Vector*)((DWORD)this + 0x108);
		return(Origin + View);
	}
	Vector GetAimPunch() {
		return *(Vector*)((DWORD)this + 0x302C);
	}
	bool IsImmune() {
		return *(bool*)((DWORD)this + 0x3928);
	}
	ClientClass *GetClientClass2() {
		PVOID Networkable = (PVOID)((DWORD)(this) + 0x8);
		using Original = ClientClass*(__thiscall*)(PVOID);
		return call_vfunc<Original>(Networkable, 2)(Networkable);
	}
	HANDLE GetWeaponHandle() {
		return *(HANDLE*)((DWORD)this + 0x2EF8);
	}

	HANDLE* Weapons() {
		return (HANDLE*)((DWORD)this + 0x2DF8);
	}

	C_BaseCombatWeapon * GetWeapon2();

	int get_choked_ticks();

	int sequence_activity(IClientEntity * pEntity, int sequence);

};

typedef unsigned short MaterialHandle_t;