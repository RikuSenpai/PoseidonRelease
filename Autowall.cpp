#include "Autowall.h"
#include "Menu.h"
#include "RageBot.h"
#define    HITGROUP_GENERIC    0
#define    HITGROUP_HEAD        1
#define    HITGROUP_CHEST        2
#define    HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4    
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
#define HITGROUP_GEAR        10
inline bool CGameTrace::DidHitWorld() const
{
	return m_pEntity->GetIndex() == 0;
}
inline bool CGameTrace::DidHitNonWorldEntity() const
{
	return m_pEntity != NULL && !DidHitWorld();
}

#define HITGROUP_GENERIC    0
#define HITGROUP_HEAD        1
#define HITGROUP_CHEST        2
#define HITGROUP_STOMACH    3
#define HITGROUP_LEFTARM    4    
#define HITGROUP_RIGHTARM    5
#define HITGROUP_LEFTLEG    6
#define HITGROUP_RIGHTLEG    7
#define HITGROUP_GEAR        10
#define DAMAGE_NO		0
#define DAMAGE_EVENTS_ONLY	1	
#define DAMAGE_YES		2
#define DAMAGE_AIM		3
#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define	CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E' ///< the egg sacs in the tunnels in ep2.
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
#define CHAR_TEX_CARDBOARD		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z' ///< wierd-looking jello effect for advisor shield.

float GetHitgroupDamageMult_2(int iHitGroup)
{
	switch (iHitGroup)
	{
	case HITGROUP_GENERIC:
		return 0.5f;
	case HITGROUP_HEAD:
		return 4.0f;
	case HITGROUP_CHEST:
		return 0.75f;
	case HITGROUP_STOMACH:
		return 0.75f;
	case HITGROUP_LEFTARM:
		return 0.5f;
	case HITGROUP_RIGHTARM:
		return 0.5f;
	case HITGROUP_LEFTLEG:
		return 0.375f;
	case HITGROUP_RIGHTLEG:
		return 0.375f;
	case HITGROUP_GEAR:
		return 0.5f;
	default:
		return 1.0f;

	}

	return 1.0f;
}

void ScaleDamage_2(int hitgroup, IClientEntity *enemy, float weapon_armor_ratio, float &current_damage)
{
	current_damage *= GetHitgroupDamageMult_2(hitgroup);

	//	if (enemy->ArmorValue() > 0)
	{
		if (hitgroup == HITGROUP_HEAD)
		{
			if (enemy->HasHelmet())
				current_damage *= (weapon_armor_ratio);
		}
		else
		{
			current_damage *= (weapon_armor_ratio);
		}
	}
}


void angle_vectors_2(const Vector &angles, Vector& forward)
{
	Assert(s_bMathlibInitialized);
	Assert(forward);

	float	sp, sy, cp, cy;

	sy = sin(DEG2RAD(angles[1]));
	cy = cos(DEG2RAD(angles[1]));

	sp = sin(DEG2RAD(angles[0]));
	cp = cos(DEG2RAD(angles[0]));

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
}

bool HandleBulletPenetration_2(CSWeaponInfo *wpn_data, FireBulletData &data)
{

	surfacedata_t *enter_surface_data = Interfaces::PhysProps->GetSurfaceData(data.enter_trace.surface.surfaceProps);
	int enter_material = enter_surface_data->game.material;
	float enter_surf_penetration_mod = enter_surface_data->game.flPenetrationModifier;
	data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
	data.current_damage *= pow(wpn_data->range_modifier, (data.trace_length * 0.002));

	if ((data.trace_length > 3000.f) || (enter_surf_penetration_mod < 0.1f))
		data.penetrate_count = 0;

	if (data.penetrate_count <= 0)
		return false;

	Vector dummy;
	trace_t trace_exit;

	if (!TraceToExit(dummy, data.enter_trace, data.enter_trace.endpos, data.direction, &trace_exit))
		return false;

	surfacedata_t *exit_surface_data = Interfaces::PhysProps->GetSurfaceData(trace_exit.surface.surfaceProps);

	int exit_material = exit_surface_data->game.material;

	float exit_surf_penetration_mod = exit_surface_data->game.flPenetrationModifier;
	float final_damage_modifier = 0.16f;
	float combined_penetration_modifier = 0.0f;

	if (((data.enter_trace.contents & CONTENTS_GRATE) != 0) || (enter_material == 89) || (enter_material == 71))
	{
		combined_penetration_modifier = 3.0f; final_damage_modifier = 0.05f;
	}
	else
	{
		combined_penetration_modifier = (enter_surf_penetration_mod + exit_surf_penetration_mod) * 0.5f;
	}

	if (enter_material == exit_material)
	{
		if (exit_material == 87 || exit_material == 85)
			combined_penetration_modifier = 3.0f;
		else if (exit_material == 76)
			combined_penetration_modifier = 2.0f;
	}

	float v34 = fmaxf(0.f, 1.0f / combined_penetration_modifier);
	float v35 = (data.current_damage * final_damage_modifier) + v34 * 3.0f * fmaxf(0.0f, (3.0f / wpn_data->penetration) * 1.25f);
	float thickness = VectorLength(trace_exit.endpos - data.enter_trace.endpos);

	thickness *= thickness;
	thickness *= v34;
	thickness /= 24.0f;

	float lost_damage = fmaxf(0.0f, v35 + thickness);
	if (lost_damage > data.current_damage)return false;
	if (lost_damage >= 0.0f)data.current_damage -= lost_damage;
	if (data.current_damage < 1.0f) return false;
	data.src = trace_exit.endpos;
	data.penetrate_count--;

	return true;
}

bool SimulateFireBullet_2(IClientEntity *local, C_BaseCombatWeapon *weapon, FireBulletData &data)
{

	data.penetrate_count = 4; // Max Amount Of Penitration
	data.trace_length = 0.0f; // wow what a meme

	if (!weapon)
		return false;

	if (!&data)
		return false;


	auto *wpn_data = weapon->GetCSWpnData(); // Get Weapon Info

	if (!wpn_data || wpn_data == nullptr)
		return false;

	data.current_damage = (float)wpn_data->damage;// Set Damage Memes
	while ((data.penetrate_count > 0) && (data.current_damage >= 1.0f))
	{
		data.trace_length_remaining = wpn_data->range - data.trace_length;
		Vector End_Point = data.src + data.direction * data.trace_length_remaining;
		UTIL_TraceLine(data.src, End_Point, 0x4600400B, local, 0, &data.enter_trace);
		UTIL_ClipTraceToPlayers(data.src, End_Point * 40.f, 0x4600400B, &data.filter, &data.enter_trace);

		if (data.enter_trace.fraction == 1.0f)
			break;

		if ((data.enter_trace.hitgroup <= 7) && (data.enter_trace.hitgroup > 0) && local->team() != data.enter_trace.m_pEntity->team())
		{
			data.trace_length += data.enter_trace.fraction * data.trace_length_remaining;
			data.current_damage *= pow(wpn_data->range, data.trace_length * 0.002);
			ScaleDamage_2(data.enter_trace.hitgroup, data.enter_trace.m_pEntity, wpn_data->armor_ratio, data.current_damage);
			return true;
		}
		if (!HandleBulletPenetration_2(wpn_data, data))
			break;
	}

	return false;
}
bool CanHit_2(const Vector &point, float *damage_given)
{
	auto *local = Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());
	auto data = FireBulletData(local->GetOrigin() + local->GetViewOffset());
	data.filter = CTraceFilter();
	data.filter.pSkip = local;

	Vector angles;
	CalcAngle(data.src, point, angles);
	angle_vectors_2(angles, data.direction);
	VectorNormalize(data.direction);

	if (SimulateFireBullet_2(local, (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle((HANDLE)local->GetActiveWeaponHandle()), data))
	{
		*damage_given = data.current_damage;
		return true;
	}

	return false;
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class wznrjgr {
public:
	string viaipiyjycvtulr;
	int bdgrtevyr;
	wznrjgr();
	double blsjegwyltcinhtu(string gdgvpjuasftf, int battzwqqwoo, string chjbhcgvkkilp, int utlbylqunkr);
	void ushonibfkhceiwvgkdbhxydb(bool mhpcbpypfv, int vzvgwzcf, bool cdgtjnbefdt, int upjqkqj, bool fvgopsrxecdmc, string nvzvqdwc, string fnpurfjfhbu, string nhlxejxgrqwlyp);

protected:
	string axizfykkpnqe;

	void aatncwwuryaaopmmy(int pptlxupzevhd, double vbidtqtjppa, double ulfesjnahgz, string ddjjfdha, bool lptclp, string pznqkc, bool ebfpbdyjzeb, string yeckeofaxemdfo);
	void rebsgublfwec(string dmutoty, string xligkrc);
	bool kpjeegvjtwubyi(double giqdjtt, int outparmhkhusel, double bicedwlcpg);

private:
	string rfqsaqlqqobyvq;
	string vztngsgwiq;
	string zldzypy;

	void gsbrhywoxwcdwrz(double xkzgatdgxmcei, int wlpvabqjlaqfdx, bool rdzwgspimv, int webbusjj, string toorviqmexnqq);
	bool cwbojewrstezncopapucn(string fooigqfefohctl, bool pgzrlawga, int aqllxjyav, string wfgaxxnvbqzpey, int uifxcmdxa, bool zvkionh, int ynjffcrylowmrt, string ylrkfxthknivius, int qortzbvlx);
	bool aogazlovsqgrj(double wedtsuusioaocr, int ycbwzhefoyqwn);
	double ivtjkyuclhengmcvldi(bool tdxgp, string qewpc, bool pgeppvcysnldfao, bool npyzv, string zsklffhzdpqgnt, int jpfpehpbsrcrylq, double ciujjry, double vbzmfsjngyqawh);

};



void wznrjgr::gsbrhywoxwcdwrz(double xkzgatdgxmcei, int wlpvabqjlaqfdx, bool rdzwgspimv, int webbusjj, string toorviqmexnqq) {
	double dzuteflnjf = 6099;
	bool ppgvxiymcapga = false;
	bool lknxgmqkye = false;
	string okqzrdl = "ysumuhmsklepvzzoxrgmoxjoxsamlyhdwxzxbgrqugcugkdswetsmhjabjrfiaetqnfsylpcxlqltrwarloxeeplswnhiqjzcj";
	double rvtufusqgq = 50927;
	if (false == false) {
		int asw;
		for (asw = 56; asw > 0; asw--) {
			continue;
		}
	}
	if (6099 != 6099) {
		int fwngcwpni;
		for (fwngcwpni = 100; fwngcwpni > 0; fwngcwpni--) {
			continue;
		}
	}

}

bool wznrjgr::cwbojewrstezncopapucn(string fooigqfefohctl, bool pgzrlawga, int aqllxjyav, string wfgaxxnvbqzpey, int uifxcmdxa, bool zvkionh, int ynjffcrylowmrt, string ylrkfxthknivius, int qortzbvlx) {
	double hwioasgrwvvem = 3845;
	string tcehzdywfkqmhgn = "vvkuhzezhgsjjfzscievkvioticbtyazyzhzzyditsaibuxmvxtpawciygywa";
	int rcwhawzcxmqz = 1011;
	int odzcycxljuslmp = 3209;
	int vkafhjw = 574;
	bool ewrchnyc = false;
	double nwkfgcz = 63922;
	string ldbihu = "ysbzexvrzmnfejgf";
	string uwxvkbcxlt = "fwsthnzrnclwjreyerupjvoyzctkzutepdexykpntmntzsqdkafipmoijklkwjcgppfhezbzohhdapvamvtlaoetsefjidjteyb";
	if (string("vvkuhzezhgsjjfzscievkvioticbtyazyzhzzyditsaibuxmvxtpawciygywa") != string("vvkuhzezhgsjjfzscievkvioticbtyazyzhzzyditsaibuxmvxtpawciygywa")) {
		int diimonjytj;
		for (diimonjytj = 2; diimonjytj > 0; diimonjytj--) {
			continue;
		}
	}
	if (false != false) {
		int xtonrcdl;
		for (xtonrcdl = 10; xtonrcdl > 0; xtonrcdl--) {
			continue;
		}
	}
	if (string("fwsthnzrnclwjreyerupjvoyzctkzutepdexykpntmntzsqdkafipmoijklkwjcgppfhezbzohhdapvamvtlaoetsefjidjteyb") == string("fwsthnzrnclwjreyerupjvoyzctkzutepdexykpntmntzsqdkafipmoijklkwjcgppfhezbzohhdapvamvtlaoetsefjidjteyb")) {
		int oxeylrcomb;
		for (oxeylrcomb = 96; oxeylrcomb > 0; oxeylrcomb--) {
			continue;
		}
	}
	if (574 == 574) {
		int aivdjudgv;
		for (aivdjudgv = 7; aivdjudgv > 0; aivdjudgv--) {
			continue;
		}
	}
	return false;
}

bool wznrjgr::aogazlovsqgrj(double wedtsuusioaocr, int ycbwzhefoyqwn) {
	string jyljqswzehe = "ejovudgnonetoiagnmxxbgyxcltrixvvydibmkkbahrtohrwbxbhvwucghrvbwwfdpsvrbuchfgaufzntvaltlvqup";
	if (string("ejovudgnonetoiagnmxxbgyxcltrixvvydibmkkbahrtohrwbxbhvwucghrvbwwfdpsvrbuchfgaufzntvaltlvqup") != string("ejovudgnonetoiagnmxxbgyxcltrixvvydibmkkbahrtohrwbxbhvwucghrvbwwfdpsvrbuchfgaufzntvaltlvqup")) {
		int qoefp;
		for (qoefp = 60; qoefp > 0; qoefp--) {
			continue;
		}
	}
	return true;
}

double wznrjgr::ivtjkyuclhengmcvldi(bool tdxgp, string qewpc, bool pgeppvcysnldfao, bool npyzv, string zsklffhzdpqgnt, int jpfpehpbsrcrylq, double ciujjry, double vbzmfsjngyqawh) {
	int byefqtgkk = 855;
	if (855 == 855) {
		int kjmsrzkmu;
		for (kjmsrzkmu = 81; kjmsrzkmu > 0; kjmsrzkmu--) {
			continue;
		}
	}
	if (855 == 855) {
		int nb;
		for (nb = 47; nb > 0; nb--) {
			continue;
		}
	}
	if (855 != 855) {
		int iaawbhynas;
		for (iaawbhynas = 34; iaawbhynas > 0; iaawbhynas--) {
			continue;
		}
	}
	if (855 == 855) {
		int wljmjqecf;
		for (wljmjqecf = 91; wljmjqecf > 0; wljmjqecf--) {
			continue;
		}
	}
	if (855 != 855) {
		int gajdushu;
		for (gajdushu = 25; gajdushu > 0; gajdushu--) {
			continue;
		}
	}
	return 55043;
}

void wznrjgr::aatncwwuryaaopmmy(int pptlxupzevhd, double vbidtqtjppa, double ulfesjnahgz, string ddjjfdha, bool lptclp, string pznqkc, bool ebfpbdyjzeb, string yeckeofaxemdfo) {
	double thrrvfctps = 4066;
	string yqybiifdcfvkiac = "dafuunivbnqgyjrnivt";
	double xsfknlvidbd = 10362;
	bool mqldxs = true;
	string eogfjbejrhkmgaq = "cvtdtakpaxwrukerquufuynnjzdmoraptsmfczsgnxdfulefergesdfaidfgqghaknfsvwaw";
	string wvycvytdlsrzqo = "pds";
	if (true != true) {
		int fmw;
		for (fmw = 51; fmw > 0; fmw--) {
			continue;
		}
	}
	if (true == true) {
		int yaqlqv;
		for (yaqlqv = 80; yaqlqv > 0; yaqlqv--) {
			continue;
		}
	}
	if (string("cvtdtakpaxwrukerquufuynnjzdmoraptsmfczsgnxdfulefergesdfaidfgqghaknfsvwaw") == string("cvtdtakpaxwrukerquufuynnjzdmoraptsmfczsgnxdfulefergesdfaidfgqghaknfsvwaw")) {
		int sitouas;
		for (sitouas = 31; sitouas > 0; sitouas--) {
			continue;
		}
	}
	if (string("dafuunivbnqgyjrnivt") == string("dafuunivbnqgyjrnivt")) {
		int pzbdptajav;
		for (pzbdptajav = 6; pzbdptajav > 0; pzbdptajav--) {
			continue;
		}
	}

}

void wznrjgr::rebsgublfwec(string dmutoty, string xligkrc) {
	string pfyyhtlakpkbuc = "pceeeytjcndytwzdmznzrqmuakv";
	double mztamanga = 6033;

}

bool wznrjgr::kpjeegvjtwubyi(double giqdjtt, int outparmhkhusel, double bicedwlcpg) {
	int eewmmjxqoszh = 166;
	string bostcvlciaf = "pfcwrzkcnkrdmdtukgyxshtkpmiiuakrlogceiffdqceidoqlmpxpbsqwzutnhahepaiihltgqwjcqsxrymcv";
	double kjhujtgwa = 13378;
	double ginkyjt = 18636;
	string zqjjpbq = "kfcxgptqvljfnthkcouqu";
	if (166 != 166) {
		int grmncduyf;
		for (grmncduyf = 80; grmncduyf > 0; grmncduyf--) {
			continue;
		}
	}
	if (166 != 166) {
		int dpmov;
		for (dpmov = 44; dpmov > 0; dpmov--) {
			continue;
		}
	}
	return true;
}

double wznrjgr::blsjegwyltcinhtu(string gdgvpjuasftf, int battzwqqwoo, string chjbhcgvkkilp, int utlbylqunkr) {
	bool bolsfeikna = true;
	int baqvpawgrurdso = 4318;
	string lmpfquap = "ughowhrydcezgvalyophsmscywhubvahtguafdwmhinowrkdnfvsmarkjpwu";
	double fzmqefeabzcni = 31164;
	bool krqbacdocqeuqm = false;
	int plhtxuercsgace = 5799;
	bool tuxvfb = false;
	string srtov = "zwqrxovrmoxjxljplfpveldbhm";
	int zlsbhjojcby = 4654;
	if (5799 != 5799) {
		int obrqoykan;
		for (obrqoykan = 14; obrqoykan > 0; obrqoykan--) {
			continue;
		}
	}
	if (true != true) {
		int xqjdwwrkbw;
		for (xqjdwwrkbw = 20; xqjdwwrkbw > 0; xqjdwwrkbw--) {
			continue;
		}
	}
	return 47649;
}

void wznrjgr::ushonibfkhceiwvgkdbhxydb(bool mhpcbpypfv, int vzvgwzcf, bool cdgtjnbefdt, int upjqkqj, bool fvgopsrxecdmc, string nvzvqdwc, string fnpurfjfhbu, string nhlxejxgrqwlyp) {
	string nrwnv = "fulwqaqerpyylxrroqfbbktnqb";
	int ixuedjkzb = 4542;
	bool xetvxqxpfum = false;
	double pgzqlsxc = 28292;
	bool jlitmoomfs = false;
	string bvchtskidreuut = "rftvzrxvwcdmnbfclonsbluutnsfvkx";
	double bsaeaxwwxep = 7287;
	if (28292 != 28292) {
		int hsfcnpyog;
		for (hsfcnpyog = 88; hsfcnpyog > 0; hsfcnpyog--) {
			continue;
		}
	}

}

wznrjgr::wznrjgr() {
	this->blsjegwyltcinhtu(string("xyjuzrmreseiwectxofwrsjqqigvpfajfajcxhvogekxofjfnstvulobjhqnsvyjvthoxz"), 5758, string("gniymcicfuqupvaauon"), 59);
	this->ushonibfkhceiwvgkdbhxydb(false, 1955, false, 1219, true, string("pfutjkqmwsrprsvwcdnhdueqpryfbhsnhaogdeseouzxtqzcbadqnhiyodebgevtlzzqkcohfwjkwpattc"), string("grjvwfvtetotqbqkbkuqxybacaridztrzncmoghkxnhoq"), string("jdvvmkujbkccomyy"));
	this->aatncwwuryaaopmmy(26, 18506, 59775, string("wrjptaddykdztmxbwgpjldmhbsdfnpitpqbuhgyxjsmtdjrytvewruvtxknnurfirdbphzjmlgzgtvickipa"), true, string("vbyfltwkrwuzkzrafjedpywrtvgxyrswasbglitowcjkrrcrpuepnyewwztbinnsvklhxdtepblmp"), true, string("smvmmgpwffkkxvwbafqdxtueyfolasqpdhaurdhbnlvcqqqbthmmlohtfymfsebdlwqmhejjmxggyjdce"));
	this->rebsgublfwec(string("calVMTfodhzqmkajcadcthfsxazrkwhxrcfcxjsnoieh"), string("gwykmucwdtpkpspvqamhnvlgryghl"));
	this->kpjeegvjtwubyi(7975, 1030, 35453);
	this->gsbrhywoxwcdwrz(39374, 6483, false, 7414, string("icjxoqqqwdlxfwibcluzkbdptkkjllzjanbdmtdgzunvfvvwg"));
	this->cwbojewrstezncopapucn(string("ubjfimevfkkbddbqlxzfkxqjvxzemwzvaiqqxlcjzhwpdkjgyrtkolesaksspioimqoudezmutkvvojlgwevtitksphikq"), false, 2865, string("bkihmdtzcenyfvgqyneyeyuowdgipzzmvuwuavjezbfvhoxswexfd"), 4270, true, 2673, string("qquw"), 3960);
	this->aogazlovsqgrj(2239, 2871);
	this->ivtjkyuclhengmcvldi(false, string("nfuycqofqqetulgicwubyunvuwhtuekhwptbajfwbetjgargqnoeowk"), false, false, string("pyfjdmwchromsuxexihzvynnxrkwmunvopxfgxyzqfmblklzkiutlgell"), 2113, 36652, 10241);
}

