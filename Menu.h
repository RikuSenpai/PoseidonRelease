#pragma once

#include "GUI.h"
#include "Controls.h"

class CRageBotTab : public CTab
{
public:
	void Setup();

	CLabel ActiveLabel;
	CCheckBox Active;

	CGroupBox AimbotGroup;
	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CCheckBox LBYCorrection;
	CSlider BreakLBYDelta;
	CSlider BreakLBYDelta2;
	CSlider freerange;
	CComboBox antilby;
	CCheckBox AccuracyBacktracking;
	CCheckBox extrapolation;
	CSlider  AimbotFov;
	CCheckBox QuickStop;
	CCheckBox AimbotSilentAim;
	CCheckBox PVSFix;
	CComboBox AimbotResolver;
	CCheckBox AdvancedResolver;
	CCheckBox NoSpread;
	CComboBox preso;
	CCheckBox AutoRevolver;
	CSlider AddFakeYaw;
	CSlider CustomPitch;
	CCheckBox LBY;
	CComboBox FakeYaw;
	CComboBox MoveYaw;
	CComboBox MoveYawFake;
	CCheckBox LowerbodyFix;
	CCheckBox PreferBodyAim;
	CCheckBox AWPAtBody;
	CSlider BaimIfUnderXHealth;
	CSlider XShots;
	CCheckBox EnginePrediction;
	CCheckBox AimbotAimStep;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CKeyBind  AimbotStopKey;
	CKeyBind manualleft;
	CKeyBind manualright;
	CKeyBind manualback;
	CKeyBind manualfront;







	//HITBOXES
	CCheckBox hitbox_Head;
	CCheckBox hitbox_Neck;
	CCheckBox hitbox_Chest;
	CCheckBox hitbox_Stomach;
	CCheckBox hitbox_Arms;
	CCheckBox hitbox_Legs;
	CCheckBox hitbox_Feet;
	//HITBOXES


	CCheckBox bil;
	CCheckBox imdil;









	CGroupBox TargetGroup;
	CComboBox TargetSelection;
	CCheckBox TargetFriendlyFire;
	CComboBox TargetHitbox;
	CComboBox TargetHitscan;
	CCheckBox TargetMultipoint;
	CSlider   TargetPointscale;

	CGroupBox AccuracyGroup;
	CCheckBox AccuracyRecoil;
	CCheckBox AccuracyAutoWall;
	CSlider AccuracyMinimumDamage;
	CCheckBox AccuracyAutoStop;
	CCheckBox AccuracyAutoCrouch;
	CCheckBox AccuracyAutoScope;
	CSlider   AccuracyHitchance;
	CComboBox AccuracyResolver;
	CComboBox delay_shot;

	CGroupBox AntiAimGroup;
	CSlider desync_range_stand;
	CSlider desync_range_move;
	CCheckBox AntiAimEnable;
	CComboBox AntiAimPitch;
	CComboBox AntiAimYaw;
	CCheckBox FlipAA;
	CComboBox BreakLBY;
	CCheckBox AntiAimEdge;
	CSlider AntiAimOffset;
	CCheckBox AntiAimKnife;
	CCheckBox AntiAimTarget;
};

class CLegitBotTab : public CTab
{
public:
	void Setup();

	// Master Switch
	CLabel ActiveLabel;
	CCheckBox Active;

	CGroupBox AimbotGroup;

	CCheckBox AimbotEnable;
	CCheckBox AimbotAutoFire;
	CSlider   TickModulation;
	CCheckBox AimbotBacktrack;
	CCheckBox AimbotFriendlyFire;
	CCheckBox AimbotKeyPress;
	CKeyBind  AimbotKeyBind;
	CCheckBox AimbotSmokeCheck;
	CCheckBox LegitAA;
	CCheckBox BackTrack;

	CGroupBox TriggerGroup;
	CCheckBox TriggerEnable;
	CCheckBox TriggerKeyPress;
	CKeyBind  TriggerKeyBind;
	CCheckBox TriggerHitChance;
	CSlider   TriggerHitChanceAmmount;
	CSlider   TriggerDelay;
	CSlider   TriggerBurst;
	CSlider   TriggerBreak;
	CCheckBox TriggerRecoil;

	CGroupBox TriggerFilterGroup;
	CCheckBox TriggerHead;
	CCheckBox TriggerChest;
	CCheckBox TriggerStomach;
	CCheckBox TriggerArms;
	CCheckBox TriggerLegs;
	CCheckBox TriggerTeammates;
	CCheckBox TriggerFlashCheck;
	CCheckBox TriggerSmokeCheck;

	CGroupBox WeaponMainGroup;
	CSlider   WeaponMainSpeed;
	CSlider   WeaponMainFoV;
	CSlider WeaponMainRecoil;
	CComboBox WeaponMainHitbox;
	CSlider WeaponMainAimtime;
	CSlider WeaoponMainStartAimtime;

	CGroupBox WeaponPistGroup;
	CSlider   WeaponPistSpeed;
	CSlider   WeaponPistFoV;
	CSlider WeaponPistRecoil;
	CComboBox WeaponPistHitbox;
	CSlider WeaponPistAimtime;
	CSlider WeaoponPistStartAimtime;

	CGroupBox WeaponSnipGroup;
	CSlider   WeaponSnipSpeed;
	CSlider   WeaponSnipFoV;
	CSlider WeaponSnipRecoil;
	CComboBox WeaponSnipHitbox;
	CSlider WeaponSnipAimtime;
	CSlider WeaoponSnipStartAimtime;

	CGroupBox WeaponMpGroup;
	CSlider   WeaponMpSpeed;
	CSlider   WeaponMpFoV;
	CSlider WeaponMpRecoil;
	CComboBox WeaponMpHitbox;
	CSlider WeaponMpAimtime;
	CSlider WeaoponMpStartAimtime;

	CGroupBox WeaponShotgunGroup;
	CSlider   WeaponShotgunSpeed;
	CSlider   WeaponShotgunFoV;
	CSlider WeaponShotgunRecoil;
	CComboBox WeaponShotgunHitbox;
	CSlider WeaponShotgunAimtime;
	CSlider WeaoponShotgunStartAimtime;

	CGroupBox WeaponMGGroup;
	CSlider   WeaponMGSpeed;
	CSlider   WeaponMGFoV;
	CSlider WeaponMGRecoil;
	CComboBox WeaponMGHitbox;
	CSlider WeaponMGAimtime;
	CSlider WeaoponMGStartAimtime;
};

class CVisualTab : public CTab
{
public:
	void Setup();

	CLabel ActiveLabel;
	CCheckBox Active;
	CComboBox fakelag_ghost;
	CCheckBox pLocalOpacity;
	CCheckBox ChamsLocal;
	CCheckBox cheatinfo;
	CCheckBox Nightmode;
	CGroupBox OptionsGroup;
	CGroupBox InfoGroup;
	CCheckBox DrawBullets;
	CGroupBox RemovalsGroup;
	CSlider Glowz_lcl;
	CGroupBox ChamsGroup;
	CCheckBox BackTrackBones;
	CGroupBox BoxGroup;
	CCheckBox OptionsBox;
	CCheckBox OptionsName;
	CCheckBox HitmarkerSound;
	CSlider blend_local;
	CCheckBox Logs;
	CCheckBox OtherHitmarker;

	CGroupBox ModelsGroup;
	CCheckBox ModelsKawaiiDesu;
	CComboBox ModelsKnife;
	CComboBox ModelsArms;
	CComboBox ModelsAK47;
	CComboBox ModelsM4A4;
	CComboBox ModelsM4A1s;
	CComboBox ModelsAWP;
	CComboBox ModelsDeagle;

	CCheckBox OptionsWeapon;
	CCheckBox Indicators;
	CCheckBox OptionHealthEnable;
	CCheckBox OptionsInfo2;
	CCheckBox ResolverInfo;
	CCheckBox BulletTrace;
	CCheckBox Barrels;
	CCheckBox AALines;
	CCheckBox ChamsVisibleOnly;
	CCheckBox Watermark;
	CCheckBox WelcomeMessage;
	CCheckBox GrenadePrediction;
	CCheckBox LocalPlayerESP;
	CCheckBox CanHit;
	CCheckBox OptionsHelmet;
	CCheckBox NightMode;
	CCheckBox OptionsKit;
	CCheckBox SpecList;
	CCheckBox IsScoped;
	CCheckBox CompRank;
	CCheckBox HasDefuser;
	CCheckBox GrenadeTrace;
	CCheckBox Distance;
	CCheckBox DiscoLights;
	CCheckBox Money;
	CCheckBox OptionsDefuse;
	CComboBox OptionsChams;
	CCheckBox OptionsArmor;
	CSlider  GlowZ;
	CCheckBox OptionsSkeleton;
	CCheckBox OptionsSkeleton2;
	CCheckBox AimbotTarget;
	CCheckBox BacktrackingLol;
	CCheckBox Grenades2;
	CCheckBox DrawMoney;
	CCheckBox DefuseWarning;
	CCheckBox Ammo;
	CCheckBox OptionsAimSpot;
	CCheckBox OptionsFilled;
	CCheckBox OtherNoScope;
	CCheckBox OptionsCompRank;

	CGroupBox FiltersGroup;
	CCheckBox FiltersAll;
	CCheckBox FiltersPlayers;
	CCheckBox FiltersSelf;
	CCheckBox FiltersEnemiesOnly;
	CCheckBox FiltersWeapons;
	CCheckBox FiltersNades;
	CCheckBox FiltersC4;
	CGroupBox GlowGroup;
	CGroupBox RenderGroup;
	CGroupBox OtherGroup;
	CCheckBox OtherSpreadCrosshair;
	CCheckBox Walldmg;
	CComboBox OtherRecoilCrosshair;
	CCheckBox OptionsGlow;
	CCheckBox EntityGlow;
	CCheckBox DisablePostProcess;
	CCheckBox OtherRadar;
	CCheckBox OtherNoVisualRecoil;
	CCheckBox OtherNoSky; 
	CCheckBox OtherNoFlash; 
	CCheckBox OtherNoSmoke;
	CCheckBox OtherAsusWalls;
	CComboBox OtherNoHands;
	CCheckBox fakeangle_ghost;
	CSlider OtherViewmodelFOV;
	CSlider OtherFOV;
};

class CColorTab : public CTab
{
public:
	void Setup();

	CGroupBox RGBGroup;
	CSlider   MenuR;
	CSlider   MenuG;
	CSlider  MenuB;

//	CColorSelector GlowEnemy;

	CGroupBox CTVisibleGroup;
	CComboBox ColorsPicker;
	CGroupBox CTBoxNotVisGroup;
	CGroupBox ColorsGroup;
	CSlider CTColorVisR;
	CSlider CTColorVisG;
	CSlider CTColorVisB;
	CSlider TChams;
	CSlider CTChams;
	CSlider CTChamsR;
	CSlider CTChamsG;
	CSlider CTChamsB;
	CSlider ColorMenuAccentR;
	CSlider ColorMenuAccentG;
	CSlider ColorMenuAccentB;

	CSlider CTColorNoVisR;
	CSlider CTColorNoVisG;
	CSlider CTColorNoVisB;
	CGroupBox TVisibleBoxGroup;
	CGroupBox Tboxnotvisiblegroup;
	CGroupBox GlowGroup;
	CLabel ColorSpacer;

	CSlider TColorVisR;
	CSlider TColorVisG;
	CSlider TColorVisB;

	CSlider TColorNoVisR;
	CSlider TColorNoVisG;
	CSlider TColorNoVisB;

	CSlider GlowR;
	CSlider GlowG;
	CSlider GlowB;
	CGroupBox CTCHamsGroup;
	CGroupBox TChamsGroup;
	CGroupBox MenuColorGroup;
	CComboBox ConfigBox;
	CGroupBox ConfigGroup;
	CGroupBox Settings;
	CButton SettingsSave;
	CButton SettingsLoad;
};

class CSettingsTab : public CTab
{
public:
	void Setup();

	CGroupBox wpnhitchanceGroup;
	CCheckBox WeaponCheck;
	CSlider  scoutChance;
	CSlider  AWPChance;
	CSlider  AutoChance;
	CSlider  RifleChance;
	CSlider  MPChance;
	CSlider  PistolChance;
	CSlider  R8Chance;

	CCheckBox MinDMG;

	CGroupBox wpnmindmgGroup;
	CSlider  scoutmindmg;
	CSlider  AWPmindmg;
	CSlider  Automindmg;
	CSlider  Riflemindmg;
	CSlider  MPmindmg;
	CSlider  Pistolmindmg;
	CSlider  R8mindmg;

	CKeyBind minimalwalk;
	CKeyBind fake_crouch;
	CKeyBind AimbotBaimOnKey;
	CKeyBind ThirdPersonKeyBind;

	//--------------------------//
	CGroupBox knifegroup;
	CGroupBox snipergroup;

	//--------------------------//
	CComboBox t_knife_index;
	CComboBox ct_knife_index;

	//--------------------------//
	CSlider t_knife_wear;
	CSlider t_sniperSCAR_wear;
	CSlider t_sniperAWP_wear;

	//--------------------------//
	CComboBox t_knife_skin_id;
	CComboBox t_sniperAWP_skin_id;
	CComboBox t_sniperSCAR_skin_id;
};

class CMiscTab : public CTab
{
public:
	void Setup();

	CGroupBox OtherGroup;
	CCheckBox OtherAutoJump;
	CCheckBox fake_crouch;
	CCheckBox watermark;
	CCheckBox OtherEdgeJump;
	CComboBox OtherAutoStrafe;
	CCheckBox OtherSafeMode;
	CCheckBox ChatSpam;
	CKeyBind minimalwalk;
	CKeyBind OtherCircleButton;
	CCheckBox OtherCircleStrafe;
	CKeyBind OtherCircle;
	CTextField CustomClantag;
	CCheckBox CheatsByPass;
	CCheckBox AutoAccept;
	CCheckBox BuyBotKevlar;
	CCheckBox BuyBotDefuser;
	CComboBox BuyBotGrenades;
	CSlider CircleAmount;
	CKeyBind FakeWalk;
	CComboBox FakeLagTyp;
	CCheckBox OtherTeamChat;
	CSlider ClanTagSpeed;
	CSlider	  OtherChatDelay;
	CComboBox NameChanger;
	CCheckBox AutoPistol;
	CCheckBox ClanTag;
	CCheckBox TrashTalk;
	CKeyBind  OtherAirStuck;
	CCheckBox OtherSpectators;
	CCheckBox OtherThirdperson;
	CComboBox ThirdpersonAngle;
	CCheckBox OtherThirdpersonFake;
	CComboBox OtherClantag;
	CTextField customclan;
	CCheckBox EnableBuyBot;
	CComboBox BuyBot;
	CGroupBox BuyBotGroup;
	CSlider WalkBotDistance;
	CSlider WalkBotEdge;
	CCheckBox WalkBot;
	CGroupBox WalkBotGroup;
	CGroupBox FakeLagGroup;
	CCheckBox FakeLagEnable;
	CSlider   FakeLagChoke;
	CComboBox KnifeModel;
	CCheckBox SkinEnable;
	CTextField KnifeSkin;
	CGroupBox KnifeGroup;
	CButton KnifeApply;
};

class aristoispejawindow : public CWindow
{
public:
	void Setup();

	CRageBotTab RageBotTab;
	CLegitBotTab LegitBotTab;
	CVisualTab VisualsTab;
	CMiscTab MiscTab;
	CSettingsTab SettingsTab;
	CColorTab ColorsTab;

	CButton SaveButton1;
	CButton LoadButton1;
	CButton SaveButton;
	CButton LoadButton;
	CButton forceupdate;
	CButton UnloadButton;
	CButton PanicButton;
	CButton ThirdButton;
	CButton ByPass;
	CButton FirstButton;
	CComboBox ConfigBox;
};

namespace Menu
{
	void SetupMenu();
	void DoUIFrame();

	extern aristoispejawindow Window;
};