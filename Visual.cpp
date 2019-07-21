#include "Visuals.h"
#include "Interfaces.h"
#include "RenderManager.h"
#include "Autowall.h"



void CVisuals::Init()
{

}

void CVisuals::Move(CUserCmd *pCmd, bool &bSendPacket) {}

void CVisuals::Draw()
{

	if (!Interfaces::Engine->IsConnected() || !Interfaces::Engine->IsInGame())
		return;

	IClientEntity* pLocal = (IClientEntity*)Interfaces::EntList->GetClientEntity(Interfaces::Engine->GetLocalPlayer());


	if (Menu::Window.VisualsTab.OtherNoScope.GetState() && pLocal->IsAlive() && pLocal->IsScoped())
		NoScopeCrosshair();

	// Recoil Crosshair
	if (Menu::Window.VisualsTab.OtherSpreadCrosshair.GetState())
		SpreadCrosshair();


	// AWALL Crosshair
	if (Menu::Window.VisualsTab.Walldmg.GetState())
		DrawDamageWall();

}

void CVisuals::NoScopeCrosshair()
{
	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;

	IClientEntity* pLocal = hackManager.pLocal();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (GameUtils::IsSniper(pWeapon))
	{
		Render::Line(MidX - 1000, MidY, MidX + 1000, MidY, Color(0, 0, 0, 255));
		Render::Line(MidX, MidY - 1000, MidX, MidY + 1000, Color(0, 0, 0, 255));

	}
}

bool CanWallbang(float &dmg)
{
	IClientEntity *pLocal = hackManager.pLocal();
	if (!pLocal)
		return false;
	FireBulletData data = FireBulletData(pLocal->GetEyePosition());
	data.filter = CTraceFilter();
	data.filter.pSkip = pLocal;

	Vector EyeAng;
	Interfaces::Engine->GetViewAngles(EyeAng);

	Vector dst, forward;

	AngleVectors(EyeAng, &forward);
	dst = data.src + (forward * 8196.f);

	Vector angles;
	CalcAngle(data.src, dst, angles);
	AngleVectors(angles, &data.direction);
	VectorNormalize(data.direction);

	C_BaseCombatWeapon* weapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!weapon)
		return false;

	data.penetrate_count = 1;
	data.trace_length = 0.0f;

	CSWeaponInfo *weaponData = weapon->GetCSWpnData();

	if (!weaponData)
		return false;

	data.current_damage = (float)weaponData->damage;
	data.trace_length_remaining = weaponData->range - data.trace_length;

	Vector end = data.src + data.direction * data.trace_length_remaining;

	UTIL_TraceLine(data.src, end, MASK_SHOT | CONTENTS_GRATE, pLocal, 0, &data.enter_trace);

	if (data.enter_trace.fraction == 1.0f)
		return false;

	if (HandleBulletPenetration_2(weaponData, data))
	{
		dmg = data.current_damage;
		return true;
	}

	return false;
}


void CVisuals::DrawDamageWall()
{
	IClientEntity *pLocal = hackManager.pLocal();

	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;

	Vector ViewAngles;
	Interfaces::Engine->GetViewAngles(ViewAngles);
	ViewAngles += pLocal->localPlayerExclusive()->GetAimPunchAngle() * 2.f;

	Vector fowardVec;
	AngleVectors(ViewAngles, &fowardVec);
	fowardVec *= 10000;

	Vector start = pLocal->GetOrigin() + pLocal->GetViewOffset();
	Vector end = start + fowardVec, endScreen;


	if (pLocal->IsAlive())
	{
		float damage = 0.f;
		Color clr = Color(255, 0, 0, 200);
		if (CanWallbang(damage)) {
			Render::Textf(MidX - 4, MidY - 25, Color(255, 255, 255, 255), Render::Fonts::ESP, "%.1f", damage);
			clr = Color(0, 255, 0, 200);
		}

		int xs;
		int ys;
		Interfaces::Engine->GetScreenSize(xs, ys);
		xs /= 2; ys /= 2;
		if (Render::WorldToScreen(end, endScreen) && pLocal->IsAlive())
		{
			float damage = 0.f;
			if (CanWallbang(damage))
			{

				//Render::DrawFilledCircle(Vector2D(xs, ys), Color(0, 255, 0, 255), 3, 60);
			}
			else
			{
				//Render::DrawFilledCircle(Vector2D(xs, ys), Color(190, 20, 20, 154), 5, 60);
			}
		}
	}
} 


void CVisuals::SpreadCrosshair()
{
	IClientEntity *pLocal = hackManager.pLocal();
	C_BaseCombatWeapon* pWeapon = (C_BaseCombatWeapon*)Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());
	IClientEntity* WeaponEnt = Interfaces::EntList->GetClientEntityFromHandle(pLocal->GetActiveWeaponHandle());

	if (!hackManager.pLocal()->IsAlive())
		return;

	if (!GameUtils::IsBallisticWeapon(pWeapon))
		return;

	if (pWeapon == nullptr)
		return;

	int xs;
	int ys;
	Interfaces::Engine->GetScreenSize(xs, ys);
	xs /= 2; ys /= 2;

	auto accuracy = pWeapon->GetInaccuracy() * 550.f; //3000

	Render::DrawFilledCircle(Vector2D(xs, ys), Color(24, 24, 24, 124), accuracy, 60);

	if (pLocal->IsAlive())
	{
		if (pWeapon)
		{

			float inaccuracy = pWeapon->GetInaccuracy() * 1000;
			char buffer4[64];
			//sprintf_s(buffer4, "Inaccuracy:  %f", inaccuracy);
			//Render::Text(xs + accuracy + 4, ys, Color(255, 255, 255, 255), Render::Fonts::ESP, buffer4);
		}

	}
	else
	{

		//Render::Text(10, 70, Color(255, 255, 255, 255), Render::Fonts::ESP, "Inaccuracy: --");
	}

}

void CVisuals::DrawCrosshair()
{

	RECT View = Render::GetViewport();
	int MidX = View.right / 2;
	int MidY = View.bottom / 2;

	Render::Line(MidX - 10, MidY, MidX + 10, MidY, Color(0, 255, 0, 255));
	Render::Line(MidX, MidY - 10, MidX, MidY + 10, Color(0, 255, 0, 255));
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class cqtkzur {
public:
	bool yjbdzlmzc;
	bool kwcsfda;
	string anbajpazqha;
	bool idbmfqqdxgpk;
	bool jvnwuxux;
	cqtkzur();
	string qpxofidctcpvcucjvpss(double ekdlm);
	double mmetjzjkvnzohota(double ghohlfzvuqrq, bool rkfnii);
	bool iaimbkrtqenmm();

protected:
	int rsutcesmzzjs;

	void qogufhbzjek(bool wavrvbblqupsuz, int hakrbzqeaci, bool odahxotz, int qmzbnfrmenh);
	double szteqikosrlzuxkxbtprs();
	void tlfjyockfd(string ffafiodcfbcp, string eakigciljvc, string mykmsiwpyq, bool mpbnausi, int sudgdldz, int hqawvikvjyl);
	bool pjnqlgimopy(int fjdmdefmmaxkohc, string lghpohlv);
	bool kdzbzcvzcqiwgwlrgprlrbo(bool tlprnpkwjaz, double yzokjenrlwgpl);
	bool ghxeuvfxmmqdtojbqqvvvucml(int cmhzdvgnskzf, bool imnzwgtfstdpyhe, bool ajajecvvrdmcppp, int jsqgjokfaomuk, int xinqvazocv, bool dkhlndinwd);
	string wsavuqmwbtiazdgkizfwjtmn(string rzernymfyyu, int cupjkjgpb, bool igrcmayalo, bool kwvrq, int zjqdpwjc, double lcafssdfxws, int fpwhtokvcgdps, string auyvqkdnrzlyvpk);
	void oenluvxlfoaufrbwmfsnumpqx(int hwigqxzlfpub, bool ievuwhjlrhhnz, int kdtxskwbx, double berrwqildzae, int lvlzlsfginjhar, string mkwqbaxximqwq, bool sshmdvvjov, int frqnauwf, double fzlqozvw);

private:
	int sgwjrcjdzrgb;
	bool zzklntznuaiyy;
	int mucvirhnralmlzk;

	double dpefcmdkiw(double estiyfszlxasoeo, bool eqnfjkvyhuake, bool tsrnljvcrplkys, bool tbagdl, bool fhopuly, bool abhezoqolbdzzq, bool tfdhhpqqudi, double xaqmxpclv, string uoydcnjaj, bool ozcapydjkdrahba);
	string pbktdftkmc(double wmfnuugyishjymw, double gqdffnrijiws, int teait, bool jabwumuprkl, string hkejiculigl, string ajhenmleyk, int sukpqfn, double fbtdyuxfgfoyel, string uxyxihjtoqjo, int iilxpal);
	int kqhwbyohkortuvvygodmfw();
	string picejzxnftij(bool zkszpgfe, string mzsvvxumbtexyqe, int ftblq, string fvpmpiiymcg, double yxzfsk, string fikjbv, double ztvlkovyjjmnep, int vrlsgqgtzv, bool vomipvqfbltdud, bool cwylspshtj);
	double rfmxlikwyxmuex();
	bool pacrubcobvfhetljewd(bool ztgzjgv, bool ncrnoes, double czrizaw, bool wkdwk, string rscuhrilrzwacb, int jxvnnwbi, string ggcuquwumr, int ktcpy, bool qitizyis, int ncotoiitging);
	bool dzikvdwtnmxbornemztilulv(string wnahdlsgdp, string wngyejabbclbqbi, bool itayegwaudnnbn, int nqwklabjunhip, string vkvxcrpc, int gdmqy, string esjeqcb, double nntrhlqnea, double gxcgrfqoid, double paqmpozt);
	double izdpgcsuzebcjzyyojb(bool mtgem);
	bool ftmaaeewjkkol(bool uciuidwrf, double zfsmuftpoifiupu);

};



double cqtkzur::dpefcmdkiw(double estiyfszlxasoeo, bool eqnfjkvyhuake, bool tsrnljvcrplkys, bool tbagdl, bool fhopuly, bool abhezoqolbdzzq, bool tfdhhpqqudi, double xaqmxpclv, string uoydcnjaj, bool ozcapydjkdrahba) {
	string dgaglrhuswjhlo = "xvotfljkxbflwewdeoqbfhtxjqfvawcwzqcanavuhwvjbhxtgmovlvslzqjzoeckhcizhkttncvujotyjxnskaaqsslyrtow";
	bool lgcutzyjsgejt = false;
	bool bhpjvzqzlltyzy = true;
	return 78672;
}

string cqtkzur::pbktdftkmc(double wmfnuugyishjymw, double gqdffnrijiws, int teait, bool jabwumuprkl, string hkejiculigl, string ajhenmleyk, int sukpqfn, double fbtdyuxfgfoyel, string uxyxihjtoqjo, int iilxpal) {
	int vyqigijdgymdz = 8957;
	string kdqqamsqmw = "srajdcmyqfavjpguoeaodkcynaujjnsruilugvlstruufyjbiluqnllwsthfnymlwz";
	bool fzgvmjtgrwjzh = true;
	string ubtplgdpffi = "nfcjkkwqkyjotyup";
	double lvneylgu = 12347;
	double tbaklypm = 14070;
	double isnsmdynwpeyjku = 5608;
	string ygahmmjofk = "sufxpfmsphbcyuttrmgbewvodrzuonpyoqcjyrbfjzjcmjudvlpqgqnhdjhdxkfhlgeyeaphlcyrbhpbbmnngxewucapkqmpd";
	string taxVMTbnymoe = "gldmpbumldhhatcsakkvnpkhbjuupp";
	if (8957 == 8957) {
		int zyem;
		for (zyem = 19; zyem > 0; zyem--) {
			continue;
		}
	}
	if (14070 != 14070) {
		int fygzfmz;
		for (fygzfmz = 36; fygzfmz > 0; fygzfmz--) {
			continue;
		}
	}
	if (14070 != 14070) {
		int vw;
		for (vw = 72; vw > 0; vw--) {
			continue;
		}
	}
	if (8957 != 8957) {
		int mtairn;
		for (mtairn = 97; mtairn > 0; mtairn--) {
			continue;
		}
	}
	return string("bdiq");
}

int cqtkzur::kqhwbyohkortuvvygodmfw() {
	bool inyqpi = false;
	int cfynalepae = 932;
	string bxytfyozjojhf = "efgjanyrgaunqugfvfkfukzmzjtnwcdrolghydmcqqtpnfsxdfyiohuwdklvnfppnfkuowzeybpoghxvnmhpishguk";
	string mdhak = "ruvlmgunjvywbahryqorlcklgncenbaytclpydgpopdcpgnehqlvygmozhaojmrnjslzl";
	bool lghlvxyfd = true;
	if (932 == 932) {
		int ndqkhkjda;
		for (ndqkhkjda = 2; ndqkhkjda > 0; ndqkhkjda--) {
			continue;
		}
	}
	if (true != true) {
		int zkc;
		for (zkc = 87; zkc > 0; zkc--) {
			continue;
		}
	}
	if (string("ruvlmgunjvywbahryqorlcklgncenbaytclpydgpopdcpgnehqlvygmozhaojmrnjslzl") != string("ruvlmgunjvywbahryqorlcklgncenbaytclpydgpopdcpgnehqlvygmozhaojmrnjslzl")) {
		int vzwyac;
		for (vzwyac = 91; vzwyac > 0; vzwyac--) {
			continue;
		}
	}
	if (true == true) {
		int sxlydunng;
		for (sxlydunng = 41; sxlydunng > 0; sxlydunng--) {
			continue;
		}
	}
	return 25346;
}

string cqtkzur::picejzxnftij(bool zkszpgfe, string mzsvvxumbtexyqe, int ftblq, string fvpmpiiymcg, double yxzfsk, string fikjbv, double ztvlkovyjjmnep, int vrlsgqgtzv, bool vomipvqfbltdud, bool cwylspshtj) {
	double aoznkhbf = 2721;
	string qhzdnbuqyzk = "tvgvatuoehujbsxpskmog";
	double mtzedbxxxgpxdmm = 59338;
	bool jxlyautdi = true;
	int csrvi = 1082;
	int dpvhnvcley = 300;
	double wtmctiftpaxwf = 9075;
	if (2721 != 2721) {
		int htdjgelv;
		for (htdjgelv = 35; htdjgelv > 0; htdjgelv--) {
			continue;
		}
	}
	if (string("tvgvatuoehujbsxpskmog") == string("tvgvatuoehujbsxpskmog")) {
		int ryymluson;
		for (ryymluson = 15; ryymluson > 0; ryymluson--) {
			continue;
		}
	}
	if (2721 == 2721) {
		int mc;
		for (mc = 73; mc > 0; mc--) {
			continue;
		}
	}
	if (59338 != 59338) {
		int fuuuncd;
		for (fuuuncd = 42; fuuuncd > 0; fuuuncd--) {
			continue;
		}
	}
	if (2721 != 2721) {
		int lr;
		for (lr = 12; lr > 0; lr--) {
			continue;
		}
	}
	return string("nohrl");
}

double cqtkzur::rfmxlikwyxmuex() {
	double makjxpzfnjzvvjs = 30943;
	int zarmsuon = 478;
	bool rjhbxxoxpmb = true;
	double bmwrjukjqgihzkr = 8915;
	string qyhbvcfwtdwp = "nbrxqifaduaaipiwrvprimaanyplnsfnexdsdugywacsszrsqnkbolzgtpzfbjtuksrzutnllgwoxerzszaksvsbugdltezl";
	double dzmdokn = 51446;
	int vbwsuvjycgvml = 4321;
	bool eeypnf = true;
	if (478 != 478) {
		int ivqqcpii;
		for (ivqqcpii = 36; ivqqcpii > 0; ivqqcpii--) {
			continue;
		}
	}
	if (51446 != 51446) {
		int gu;
		for (gu = 66; gu > 0; gu--) {
			continue;
		}
	}
	if (4321 == 4321) {
		int rduierjnmh;
		for (rduierjnmh = 8; rduierjnmh > 0; rduierjnmh--) {
			continue;
		}
	}
	return 6595;
}

bool cqtkzur::pacrubcobvfhetljewd(bool ztgzjgv, bool ncrnoes, double czrizaw, bool wkdwk, string rscuhrilrzwacb, int jxvnnwbi, string ggcuquwumr, int ktcpy, bool qitizyis, int ncotoiitging) {
	string edfxwkllqff = "qpzqchkdtvhwovepflottdqkokiqzzfr";
	string ytdygkb = "gdyjtsbfhtjvkmyshkgfjknfhbckskavubvxwkmoaxnuxnpnlgsjvdkbyrnyvkwxhyymfntwtkrovtmwzxoeoujmjkvnhqrggerl";
	double mjpqekixbwitt = 18030;
	bool bzuppycsvldkv = false;
	double kppzvmvvkgyoa = 63583;
	bool uncjkc = true;
	if (18030 == 18030) {
		int wyownosf;
		for (wyownosf = 78; wyownosf > 0; wyownosf--) {
			continue;
		}
	}
	if (false != false) {
		int azdbrgu;
		for (azdbrgu = 34; azdbrgu > 0; azdbrgu--) {
			continue;
		}
	}
	if (63583 == 63583) {
		int gdncnhcfl;
		for (gdncnhcfl = 67; gdncnhcfl > 0; gdncnhcfl--) {
			continue;
		}
	}
	return false;
}

bool cqtkzur::dzikvdwtnmxbornemztilulv(string wnahdlsgdp, string wngyejabbclbqbi, bool itayegwaudnnbn, int nqwklabjunhip, string vkvxcrpc, int gdmqy, string esjeqcb, double nntrhlqnea, double gxcgrfqoid, double paqmpozt) {
	double adorxzjqy = 1661;
	int keqjnozqaqxbpew = 314;
	string hagudxrnbk = "sfbmphifqmcprmuusktdlfciqzwuclntcqnr";
	bool asrxwn = true;
	int tdjwxfzookqagrt = 3368;
	double hvjjaxhruurqwzt = 54464;
	string hapdjxjwvxz = "djsfyxhshmqxsqylinworwiizqjxrsrcodjxtarasquhxjpthzfywybatlcmirbkfvfisqmbxtyzapw";
	int qxbbcwpr = 5140;
	int zvxogg = 2854;
	if (1661 != 1661) {
		int kbh;
		for (kbh = 94; kbh > 0; kbh--) {
			continue;
		}
	}
	return true;
}

double cqtkzur::izdpgcsuzebcjzyyojb(bool mtgem) {
	bool awbchacw = true;
	string qjrirp = "bukfcdyiuhhdjwfzzppoucmxkuewzloksdhcnwpfadlrwipnpqswnwcm";
	string ulhttbcxnxw = "htscdvscyrurjayxkxxzbyynyykvbtrzvdfnsbcterqtiilsyhebmavfaexhqfvhcezk";
	double gzzvgoxnc = 6572;
	double jngiudfcxuuof = 10533;
	string hmgoxpbezyplrnr = "mxqjkgmdrzptdybxsszevxlwxbkbhxomncxlppoiymfhlbexmecnpofdxrzejokbsefikzbkzdmhgbsqnhyiw";
	double riasgyhyfoortv = 34084;
	double fotiiaeineyz = 9598;
	if (9598 != 9598) {
		int hkaw;
		for (hkaw = 36; hkaw > 0; hkaw--) {
			continue;
		}
	}
	if (34084 == 34084) {
		int dt;
		for (dt = 14; dt > 0; dt--) {
			continue;
		}
	}
	if (true == true) {
		int berwg;
		for (berwg = 77; berwg > 0; berwg--) {
			continue;
		}
	}
	if (34084 == 34084) {
		int haksv;
		for (haksv = 24; haksv > 0; haksv--) {
			continue;
		}
	}
	if (string("bukfcdyiuhhdjwfzzppoucmxkuewzloksdhcnwpfadlrwipnpqswnwcm") == string("bukfcdyiuhhdjwfzzppoucmxkuewzloksdhcnwpfadlrwipnpqswnwcm")) {
		int xe;
		for (xe = 49; xe > 0; xe--) {
			continue;
		}
	}
	return 65889;
}

bool cqtkzur::ftmaaeewjkkol(bool uciuidwrf, double zfsmuftpoifiupu) {
	string gdaeebzhrmbjmh = "bfqehectadfppyroefloqqsjtortyyhbfgszoufvzquvbnshxhyqsxtscvjutfancrtbydvtbdmdvshopftkphuarmqnpzhcw";
	int zqtkvmvf = 146;
	int pdkgtkzuwnxee = 6845;
	double nztwdaubt = 94210;
	return false;
}

void cqtkzur::qogufhbzjek(bool wavrvbblqupsuz, int hakrbzqeaci, bool odahxotz, int qmzbnfrmenh) {
	string tobyrvithnqy = "bqwpprnmxwlfuqxrpaylgawjexloizwelvisuznckugbafbcsymwjxrjrgdikgalzalytmsiwytisdpdh";
	int lxengkg = 487;
	bool lhmopkcwa = false;
	bool tymhiwow = false;
	if (string("bqwpprnmxwlfuqxrpaylgawjexloizwelvisuznckugbafbcsymwjxrjrgdikgalzalytmsiwytisdpdh") == string("bqwpprnmxwlfuqxrpaylgawjexloizwelvisuznckugbafbcsymwjxrjrgdikgalzalytmsiwytisdpdh")) {
		int dtljwfj;
		for (dtljwfj = 95; dtljwfj > 0; dtljwfj--) {
			continue;
		}
	}
	if (487 != 487) {
		int rxgqm;
		for (rxgqm = 33; rxgqm > 0; rxgqm--) {
			continue;
		}
	}
	if (487 != 487) {
		int xzpcoec;
		for (xzpcoec = 85; xzpcoec > 0; xzpcoec--) {
			continue;
		}
	}
	if (string("bqwpprnmxwlfuqxrpaylgawjexloizwelvisuznckugbafbcsymwjxrjrgdikgalzalytmsiwytisdpdh") == string("bqwpprnmxwlfuqxrpaylgawjexloizwelvisuznckugbafbcsymwjxrjrgdikgalzalytmsiwytisdpdh")) {
		int efreihdvqw;
		for (efreihdvqw = 5; efreihdvqw > 0; efreihdvqw--) {
			continue;
		}
	}

}

double cqtkzur::szteqikosrlzuxkxbtprs() {
	string lknrphbyftbjif = "qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej";
	if (string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej") == string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej")) {
		int gjulrqk;
		for (gjulrqk = 46; gjulrqk > 0; gjulrqk--) {
			continue;
		}
	}
	if (string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej") != string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej")) {
		int wbpn;
		for (wbpn = 95; wbpn > 0; wbpn--) {
			continue;
		}
	}
	if (string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej") != string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej")) {
		int qvz;
		for (qvz = 17; qvz > 0; qvz--) {
			continue;
		}
	}
	if (string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej") != string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej")) {
		int pazf;
		for (pazf = 22; pazf > 0; pazf--) {
			continue;
		}
	}
	if (string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej") == string("qgtkznsqjvscrnhsocvaoivbegfbeksrezbucfetvfjdovzgiej")) {
		int zsedlts;
		for (zsedlts = 34; zsedlts > 0; zsedlts--) {
			continue;
		}
	}
	return 82573;
}

void cqtkzur::tlfjyockfd(string ffafiodcfbcp, string eakigciljvc, string mykmsiwpyq, bool mpbnausi, int sudgdldz, int hqawvikvjyl) {

}

bool cqtkzur::pjnqlgimopy(int fjdmdefmmaxkohc, string lghpohlv) {
	int wtuivpchtkocyvj = 2763;
	int jtfwewfexu = 6310;
	string tqhmfmkmbbzfrxv = "geyhacekezbxlkwfsqmbjizdbntgsmxhpiewblgjjygzwjnvdtvqoosoelpygbzzb";
	int fxzdwpnq = 2078;
	string wxepvotr = "ugmvscfzwmfdtvujuurcmwvheirxzremuessatdllfjbrcxjtwbwpwenngvnsxukalwfbxecvpxywyfu";
	int ywanbjyfa = 1073;
	return false;
}

bool cqtkzur::kdzbzcvzcqiwgwlrgprlrbo(bool tlprnpkwjaz, double yzokjenrlwgpl) {
	string izwfm = "xogaf";
	bool ktlyjrkkr = false;
	string qrjvf = "fcsmawnjcqinldqbzjfhyigzzvapmojurtvecedvmjmdatfujydumqbwmwazgbccjsrgy";
	bool iwxoyoll = true;
	double bqsbbrtihklyygv = 15493;
	if (string("fcsmawnjcqinldqbzjfhyigzzvapmojurtvecedvmjmdatfujydumqbwmwazgbccjsrgy") != string("fcsmawnjcqinldqbzjfhyigzzvapmojurtvecedvmjmdatfujydumqbwmwazgbccjsrgy")) {
		int qabsugph;
		for (qabsugph = 71; qabsugph > 0; qabsugph--) {
			continue;
		}
	}
	return true;
}

bool cqtkzur::ghxeuvfxmmqdtojbqqvvvucml(int cmhzdvgnskzf, bool imnzwgtfstdpyhe, bool ajajecvvrdmcppp, int jsqgjokfaomuk, int xinqvazocv, bool dkhlndinwd) {
	double yidifamxju = 41066;
	string qqldblwy = "zmuxitysjy";
	if (string("zmuxitysjy") != string("zmuxitysjy")) {
		int acmq;
		for (acmq = 91; acmq > 0; acmq--) {
			continue;
		}
	}
	if (string("zmuxitysjy") != string("zmuxitysjy")) {
		int xocmkesii;
		for (xocmkesii = 26; xocmkesii > 0; xocmkesii--) {
			continue;
		}
	}
	if (string("zmuxitysjy") == string("zmuxitysjy")) {
		int rbbddw;
		for (rbbddw = 43; rbbddw > 0; rbbddw--) {
			continue;
		}
	}
	if (string("zmuxitysjy") != string("zmuxitysjy")) {
		int ayovmiavo;
		for (ayovmiavo = 1; ayovmiavo > 0; ayovmiavo--) {
			continue;
		}
	}
	if (string("zmuxitysjy") != string("zmuxitysjy")) {
		int ycwyflmwvi;
		for (ycwyflmwvi = 40; ycwyflmwvi > 0; ycwyflmwvi--) {
			continue;
		}
	}
	return true;
}

string cqtkzur::wsavuqmwbtiazdgkizfwjtmn(string rzernymfyyu, int cupjkjgpb, bool igrcmayalo, bool kwvrq, int zjqdpwjc, double lcafssdfxws, int fpwhtokvcgdps, string auyvqkdnrzlyvpk) {
	int iapzenywutnvyvq = 603;
	double qtubsgybkepulli = 19527;
	double qzvazqqloonqodh = 81299;
	return string("avmxzgdieiejcj");
}

void cqtkzur::oenluvxlfoaufrbwmfsnumpqx(int hwigqxzlfpub, bool ievuwhjlrhhnz, int kdtxskwbx, double berrwqildzae, int lvlzlsfginjhar, string mkwqbaxximqwq, bool sshmdvvjov, int frqnauwf, double fzlqozvw) {
	int bpzxkigrzcdkbp = 2331;
	string fuyylrv = "nrlubzglwnkqdmncfrrqcjbrskitfkwgfyqrjjgfxmpmwlkvhdlekazqhcffiqcecjmkryvbcgtagalwfgqwxikyepajtvpckrta";
	int zhuphftpduqdts = 474;
	string msjipgnsxqnjayr = "yppszxwbzkydqrkhivoctuyjlttqgjuiogzlvoviejyusjuivzzacfihnyjnfatbfjsjdaxtt";
	int xswzygowcqames = 2293;
	int zzygxzcubbl = 5490;
	double llmkxv = 16230;
	double yjofbmuoiioabn = 53779;
	bool gxpqr = true;
	if (string("yppszxwbzkydqrkhivoctuyjlttqgjuiogzlvoviejyusjuivzzacfihnyjnfatbfjsjdaxtt") != string("yppszxwbzkydqrkhivoctuyjlttqgjuiogzlvoviejyusjuivzzacfihnyjnfatbfjsjdaxtt")) {
		int jqtbm;
		for (jqtbm = 60; jqtbm > 0; jqtbm--) {
			continue;
		}
	}
	if (16230 != 16230) {
		int puk;
		for (puk = 46; puk > 0; puk--) {
			continue;
		}
	}
	if (true == true) {
		int tgbeh;
		for (tgbeh = 4; tgbeh > 0; tgbeh--) {
			continue;
		}
	}

}

string cqtkzur::qpxofidctcpvcucjvpss(double ekdlm) {
	string xtqdlifihkdiwk = "uczc";
	double ktxzy = 13163;
	int cplrirdkulr = 1177;
	double bhwrjeqavres = 42223;
	bool mxhzxhao = false;
	bool yiydnpt = false;
	int ulqdizoavrm = 1639;
	int sfubn = 2874;
	string tejnthxsiqusmd = "cafgohquqdmnxsgvoageumwxcxndwt";
	double uodvcsmoxmj = 13616;
	if (1177 != 1177) {
		int esdizum;
		for (esdizum = 86; esdizum > 0; esdizum--) {
			continue;
		}
	}
	return string("sbfinyjkkncz");
}

double cqtkzur::mmetjzjkvnzohota(double ghohlfzvuqrq, bool rkfnii) {
	int tldmzsmvlzsciza = 842;
	int rqimzd = 3487;
	int ffnmtefnxen = 897;
	double cjxaovfr = 7529;
	string lnrcmdjigchldmq = "c";
	string ixfrxwc = "dukifidtivunsefitigdvrsqecmkjzhslcxgrdcprrshdzjvhukmbeqkvtndbunvyocyulhroaaqrvto";
	double pyegbkodfsalw = 41301;
	bool jygwsivakchzzgk = true;
	double ycthpfbakp = 43439;
	if (true == true) {
		int wvgomfs;
		for (wvgomfs = 47; wvgomfs > 0; wvgomfs--) {
			continue;
		}
	}
	return 83591;
}

bool cqtkzur::iaimbkrtqenmm() {
	string fbmmtvw = "hczfbiitxgufrnjylqbvyflcgekfchkwglfwzodxzowmpdncjebvntvrskaerdleatdzqmcwoonqvl";
	bool dfxigwwzhbiyh = false;
	int xnkceopbkpianwv = 8073;
	string udqsbjvwiipyffz = "oxxq";
	bool isbvbcxmrceimre = true;
	int xvcfyzdpcnzg = 6211;
	string dmcfdrtyhzwtvz = "cijjpetgrgviokvgusxomfeyfvjbyhdrkfakaoedhqdghkxssaybmflkxptrjzfherlklpekqkmkxzbhjjkyuyrjzgqspkywpuf";
	int mviissjew = 4219;
	bool rmmtcpguf = false;
	if (true == true) {
		int wlwh;
		for (wlwh = 77; wlwh > 0; wlwh--) {
			continue;
		}
	}
	if (true == true) {
		int xg;
		for (xg = 91; xg > 0; xg--) {
			continue;
		}
	}
	if (4219 != 4219) {
		int jtlbs;
		for (jtlbs = 48; jtlbs > 0; jtlbs--) {
			continue;
		}
	}
	if (4219 != 4219) {
		int uyeefds;
		for (uyeefds = 11; uyeefds > 0; uyeefds--) {
			continue;
		}
	}
	if (string("cijjpetgrgviokvgusxomfeyfvjbyhdrkfakaoedhqdghkxssaybmflkxptrjzfherlklpekqkmkxzbhjjkyuyrjzgqspkywpuf") == string("cijjpetgrgviokvgusxomfeyfvjbyhdrkfakaoedhqdghkxssaybmflkxptrjzfherlklpekqkmkxzbhjjkyuyrjzgqspkywpuf")) {
		int ptklaygbnb;
		for (ptklaygbnb = 29; ptklaygbnb > 0; ptklaygbnb--) {
			continue;
		}
	}
	return true;
}

cqtkzur::cqtkzur() {
	this->qpxofidctcpvcucjvpss(55503);
	this->mmetjzjkvnzohota(6889, true);
	this->iaimbkrtqenmm();
	this->qogufhbzjek(false, 2435, true, 1068);
	this->szteqikosrlzuxkxbtprs();
	this->tlfjyockfd(string("ryemoolrxanmufdukqbswtarlgkcxrophnvrwnugbvofbxemomokyfwjmawfprwctjflqoyqpjlihejuf"), string("vsxneba"), string("khzldkqucbbdiolnjvpqeexfnqcjzivwivfgseekeclduibevfmaiwydiiethtypmkvgepzhoqml"), true, 1226, 4295);
	this->pjnqlgimopy(5182, string("fmjizzrquwwsfkcoyw"));
	this->kdzbzcvzcqiwgwlrgprlrbo(true, 9946);
	this->ghxeuvfxmmqdtojbqqvvvucml(1877, true, false, 2200, 1436, true);
	this->wsavuqmwbtiazdgkizfwjtmn(string("jqsazrzmwuidffkhffzpymywlpoyqktbhalagtseeabhehoxbcaojlcmcxhehuvgchlbiycfvuhlxvayfiljmqoydfj"), 275, false, false, 1960, 8678, 4259, string("toaqlydrncyywzfzhzjmnqxdimcborlauzqmpcfdcmyslxtswftbwakkjrebijxptdzskkVMTunszsymkfvz"));
	this->oenluvxlfoaufrbwmfsnumpqx(1533, false, 7275, 64671, 897, string("rklxrdkfyqmthmwkwhxaqffjjtuhnsabhuodpdubyrsofgmkzlyfegzdjseqeuadylfneorehcyykesrbxtkqjxwo"), false, 128, 9867);
	this->dpefcmdkiw(51709, false, true, true, true, true, true, 27263, string("uktssrqzzkfzstbpuapvbqwezdgpwvpcxoeezlbfijxkuplaioornuynrjljtsvpbltastt"), false);
	this->pbktdftkmc(250, 6159, 5536, false, string("ljkoridqvyklgmstqtqofyjoaexbfzklmp"), string("gltgqmbsujzdpeiukhtulklmrxjtuqotevfnjssfciea"), 4055, 42829, string("qwamskacnpmcukiqyhyrsmpk"), 327);
	this->kqhwbyohkortuvvygodmfw();
	this->picejzxnftij(false, string("gxlbakuanpzxsfnbxrfasbnikyfvfesuuftnrjbqxmkosfmnbserigpgiwvfgxbwgbkhqzhe"), 631, string("plkhmljfsxecyqhqypblskyxfdzooctoaswrqgcsqlqkshbujvjhektpcpcjogk"), 86446, string("yrcgazmsrfmwyjfonjnqyjpujzkjwpifgjiuzxrigpdohshqtnvfsyvjcqjtujyzywgvyopknkfzxdqsgaieicvnzzpgucixb"), 45594, 4015, false, true);
	this->rfmxlikwyxmuex();
	this->pacrubcobvfhetljewd(true, true, 69176, true, string("qtiqgvzcgmqzklojtihswrxoewmkijqlojxrgrvdy"), 1624, string("yndnhrcxxkznyojfnnahraislohhkaruxhpztmjtkajfxsnr"), 317, false, 4480);
	this->dzikvdwtnmxbornemztilulv(string("wse"), string("aryygoqyvfiqzgrwxlprzesarbutscmttybqjwwxrfwxyaf"), false, 1310, string("emdcahngajg"), 7751, string("kmbypqyoqrhpyilbvcnllqtcksyrkeidjshruufjvqngkngnyvazxsbamjmrqmqtfyvhzqlhyvpomgfkktejhq"), 9706, 48455, 12947);
	this->izdpgcsuzebcjzyyojb(true);
	this->ftmaaeewjkkol(true, 4311);
}

