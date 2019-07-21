#include "Backtracking Rage.h"
#include "CommonIncludes.h"
#include "RageBot.h"



#define MODULATIONFACOTR 1500
cPrediction DataManager;
void cPrediction::ReportMove(Vector  vOrigin, int iIndex)
{
	if (iIndex >= 128 || iIndex < 0)
		return;

	IClientEntity* pBaseEntity = Interfaces::EntList->GetClientEntity(iIndex);
	if (!pBaseEntity)		return;

	IClientEntity* pLocalPlayer = hackManager.pLocal();
	if (!pLocalPlayer)		return;

	if (pBaseEntity->GetIndex() == pLocalPlayer->GetIndex()) return;

	if (!pBaseEntity->GetHealth()) return;



	matrixentry_t& Matrix = m_Matrix[iIndex];

	Matrix.dwFraction = timeGetTime() - Matrix.dwTime;
	Matrix.dwTime = timeGetTime();
	Matrix.vOrigin = vOrigin;
	Matrix.vecOrigins.push_back(vOrigin);
}

bool cPrediction::bGetCurOrigin(Vector& vOrigin, int iIndex)
{

	return bGetOrigin(vOrigin, iIndex, timeGetTime());
}
float get_network_latency()
{
	INetChannelInfo* nci = Interfaces::Engine->GetNetChannelInfo();
	if (nci != nullptr)
		return nci->GetAvgLatency(FLOW_OUTGOING);

	return 0.f;
}
int get_choked_ticks(int tickbase)
{

	float diff = Interfaces::Globals->curtime - tickbase;
	float latency = get_network_latency();
	return max(0.0f, diff);
}
Vector	Acceleration[64];
Vector OldNetworkPos[64];
Vector	OldVelocity[64];
void cPrediction::UpdatePlayerPos()
{
	for (int s = 1; s <= Interfaces::Globals->maxClients; ++s)
	{
		IClientEntity* pEntityity = Interfaces::EntList->GetClientEntity(s);
		auto pLocal = hackManager.pLocal();

		if (!pEntityity)
			continue;

		if (!OldNetworkPos[s]) OldNetworkPos[s] = Vector(0, 0, 0);
		if (!OldVelocity[s]) OldVelocity[s] = Vector(0, 0, 0);
		if (!Acceleration[s]) Acceleration[s] = Vector(0, 0, 0);

		if (!OldNetworkPos[s] && pEntityity->GetVelocity().Length() > 0) OldNetworkPos[s] = pEntityity->GetAbsOrigin();
		if (!OldVelocity[s] && pEntityity->GetVelocity().Length() > 0) OldVelocity[s] = pEntityity->GetVelocity();
		if (!Acceleration[s] && pEntityity->GetVelocity().Length() > 0) Acceleration[s] = Vector(0, 0, 0);

		if ((OldNetworkPos[s] != pEntityity->GetAbsOrigin()) && pEntityity->GetVelocity().Length() > 0)
		{
			Vector CurrentVelocity = pEntityity->GetAbsOrigin() - OldNetworkPos[s];
			Acceleration[s] = CurrentVelocity - OldVelocity[s];
			if (pEntityity != pLocal)
				(pEntityity->GetVelocity()) = CurrentVelocity;

			OldNetworkPos[s] = pEntityity->GetAbsOrigin();
			OldVelocity[s] = CurrentVelocity;
		}

		if (pEntityity != pLocal)
			if (pEntityity->GetChokedTicks() && pEntityity->GetVelocity().Length() > 0)
				*pEntityity->GetOriginPtr() = pEntityity->GetAbsOrigin() + (pEntityity->GetVelocity() + Acceleration[s]) * (pEntityity->GetChokedTicks());
	}
}

bool cPrediction::bGetOrigin(Vector& vOrigin, int iIndex, DWORD dwTime)
{

	if (iIndex >= 128 || iIndex < 0)
		return false;

	IClientEntity* pBaseEntity = Interfaces::EntList->GetClientEntity(iIndex);
	if (!pBaseEntity)		return false;

	IClientEntity* pLocalPlayer = hackManager.pLocal();
	if (!pLocalPlayer)		return false;

	if (pBaseEntity->GetIndex() == pLocalPlayer->GetIndex()) return false;

	if (!pBaseEntity->GetHealth()) return false;


	matrixentry_t& Matrix = m_Matrix[iIndex];

	if (Matrix.vecOrigins.size() == 0 || Matrix.dwFraction > MODULATIONFACOTR)
	{
		Matrix.dwFraction = 0;
		return false;
	}

	DWORD dwInterfraction = dwTime - Matrix.dwFraction;

	if (Matrix.vecOrigins.size() == 1)
	{
		vOrigin = Matrix.vOrigin;
		return true;
	}
	int iSize = (int)Matrix.vecOrigins.size();

	Vector vCurDistance = Matrix.vOrigin - Matrix.vecOrigins[iSize - 1];
	Vector vLastDistance = Matrix.vecOrigins[iSize - 1] - Matrix.vecOrigins[iSize - 2];

	float flInterp = vCurDistance.Length() - vLastDistance.Length();

	if (flInterp == 0.0) // constant or no acceleration
	{
		return true;
	}

	int iRecords = iSize - 1;

	float flAcceleration[2];
	float flAbsAcceleration = 1.0f;
	Vector vecAcceleration = vCurDistance - vLastDistance;

	if (iRecords >= 2)
	{
		Vector vOldDistance = Matrix.vecOrigins[iSize - 2] - Matrix.vecOrigins[iSize - 3];
		flAcceleration[0] = vCurDistance.Length() - vLastDistance.Length();
		flAcceleration[1] = vLastDistance.Length() - vOldDistance.Length();

		flAbsAcceleration = (flAcceleration[0] + flAcceleration[1]) / 2;
	}

	Interfaces::CVar->ConsoleColorPrintf(Color(201, 201, 201, 255), "Backtracked Information: Acceleration X: %i Acceleration Y: %i Choked Ticks: %i \n", flAcceleration[0], flAcceleration[1], pBaseEntity->GetChokedTicks());
	pBaseEntity->GetAbsOrigin() += ((vecAcceleration * (6)));
	//printf("%i", get_choked_ticks(pBaseEntity->GetTickBase()));
	return true;
}

void cPrediction::Reset(void)
{
	for (int i = 0; i < 128; i++)
	{

		m_Matrix[i].vecOrigins.clear();
		m_Matrix[i].dwFraction = 0;
		m_Matrix[i].dwTime = 0;
	}
}


#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;

class puupzjd {
public:
	int agqaevmvzhux;
	string vcnarvilvnpcbct;
	bool krkazvqvr;
	string ghucdttye;
	double wroaepso;
	puupzjd();
	double phkjecvijaxpc(bool knkjcxxtnomceh, double kmybyebivs, bool sasbnrwwtprg, bool pvkdamuidqqqw, bool zhfklldbo, string krmbmp, string kndfndrek, bool ltsprsmmypmhub, double nyadzcofvjdgpbl);
	bool afrrxbsqyvqz(string uubzisllyajgn, double txwywgrxepqeeao, string ftjofhcmzs, string hurxktteectaptz, double rdbnbwjdujs, bool kvufvfnuqaxced);

protected:
	double xediew;
	int ntflqs;
	int sbjkpmifczg;
	double hyjcwydq;
	double kxidheeq;

	int exjzfjzgvjnwwlzxclkclp(double enoopws, int iyvvlhvk, double tjfbokathkcg, double mntilpvpqxggogn, string mzlxcv, bool nokcadyzcijuot);
	double hvihctzexfyg();
	bool pdbhxcjhexjnndkxtleezhcn(bool xbvhw, double iwuaisudsnzk);
	void ohjquapiubfjkqokvscniwb(bool dammag, string fasqvsbnwu, string erwwxamkginjtn, string hmxpw, bool ppceqgccdtgfm, bool mlubigyoltzvuf, bool waahcc, string xvqweynjuycnh, double oyerritlk);
	string dapolmvbjnjeiqvgkphr();
	bool opuqpgvjfzhrddoagmrhz(string wftzcfmhzgu, bool uguqbvwuddct);
	bool tejczwoigyviuydzaoen(int sfzcjnbavggvbjh, double aiyjk, bool gqlsm, int nxqcrimsag, string mnwvai, string xjdjzdaafyvjoue);
	string dbmudavfktqaiss(bool coymsdzvo, string phdndqygtigarm, double cyqlmbhv, bool wznxdakf);

private:
	string nebqezjawjtlil;
	string ahenzxoblsbm;
	bool rszan;

	void snnpdnxwzasqjmq(double mtwftlf);
	double mxfdkrudjvmwjysuiwsmtsfa(bool niryfmgcldyv, bool oggtiaqaxurkbuy, string qnoanu, int gjxaue, bool bwfnwxd, double mxhlixwkegzjt, double znxcoeinlprc, bool mbjczrt);
	string zvjelrdaaxfdxdrnozclzq(double tbuhrb, bool gueklrxqgrjo);

};



void puupzjd::snnpdnxwzasqjmq(double mtwftlf) {
	bool mrlafczxf = false;
	double xtnrulyqzudy = 2352;
	bool meuij = true;
	bool tzeyjfj = true;
	double ifzsefkypiq = 59959;
	bool pbhwgz = true;
	bool bjpkbpc = false;
	bool hynympptsswc = false;
	if (59959 != 59959) {
		int jakgkgsiyu;
		for (jakgkgsiyu = 61; jakgkgsiyu > 0; jakgkgsiyu--) {
			continue;
		}
	}
	if (59959 == 59959) {
		int xysuktm;
		for (xysuktm = 42; xysuktm > 0; xysuktm--) {
			continue;
		}
	}
	if (true != true) {
		int vhtjkhjyj;
		for (vhtjkhjyj = 44; vhtjkhjyj > 0; vhtjkhjyj--) {
			continue;
		}
	}
	if (false == false) {
		int xhiqhisu;
		for (xhiqhisu = 33; xhiqhisu > 0; xhiqhisu--) {
			continue;
		}
	}
	if (false != false) {
		int vgipymknxs;
		for (vgipymknxs = 64; vgipymknxs > 0; vgipymknxs--) {
			continue;
		}
	}

}

double puupzjd::mxfdkrudjvmwjysuiwsmtsfa(bool niryfmgcldyv, bool oggtiaqaxurkbuy, string qnoanu, int gjxaue, bool bwfnwxd, double mxhlixwkegzjt, double znxcoeinlprc, bool mbjczrt) {
	string zpzwtbetqfiobxf = "jidkabjjmsshmljyekujugvnfcpnzuifcwhwrbamwrdqmcocgkgsomdnvaeqlwklwbmnjkaxqharxgllth";
	int mhzsytlf = 5123;
	int tishgdqzmx = 3356;
	bool otjiidlbxw = true;
	double jvpeviub = 8229;
	string gxqmhfmpbgjw = "mlrjyvsljjnlpqbbmyaldhrxwhpcctkaunlgrtzjhtfuuhwuvinwh";
	bool qimrive = true;
	bool mcervit = false;
	int eqslksb = 1118;
	if (8229 == 8229) {
		int text;
		for (text = 3; text > 0; text--) {
			continue;
		}
	}
	if (5123 != 5123) {
		int mtmo;
		for (mtmo = 52; mtmo > 0; mtmo--) {
			continue;
		}
	}
	return 50962;
}

string puupzjd::zvjelrdaaxfdxdrnozclzq(double tbuhrb, bool gueklrxqgrjo) {
	int cpaizzilifzjwe = 555;
	string lkuhgvuevlxxbw = "qlrgdllmdrhncckuwhkcnmvrbvjykedfnakotwbscpveqclizzmrghusutqtttzfngpmjjldefbuapmgexvkropoikzyjx";
	bool axembymghe = true;
	bool lvnumxgfpycn = false;
	double aspre = 16394;
	bool suwghnvabcrq = false;
	bool jtezcwhq = true;
	double vyltynax = 22357;
	if (22357 != 22357) {
		int dizcmppp;
		for (dizcmppp = 12; dizcmppp > 0; dizcmppp--) {
			continue;
		}
	}
	return string("cazmxx");
}

int puupzjd::exjzfjzgvjnwwlzxclkclp(double enoopws, int iyvvlhvk, double tjfbokathkcg, double mntilpvpqxggogn, string mzlxcv, bool nokcadyzcijuot) {
	int vtqkk = 247;
	double fkrhih = 28553;
	bool okegqhdd = true;
	int ggllwzunuzycb = 8517;
	double wmrakmbijuj = 24749;
	if (true != true) {
		int fuo;
		for (fuo = 38; fuo > 0; fuo--) {
			continue;
		}
	}
	if (247 != 247) {
		int vshvn;
		for (vshvn = 43; vshvn > 0; vshvn--) {
			continue;
		}
	}
	if (247 == 247) {
		int rmohn;
		for (rmohn = 69; rmohn > 0; rmohn--) {
			continue;
		}
	}
	if (247 == 247) {
		int lbxcux;
		for (lbxcux = 47; lbxcux > 0; lbxcux--) {
			continue;
		}
	}
	return 51338;
}

double puupzjd::hvihctzexfyg() {
	bool qemkrfj = false;
	int esnhlldzuqz = 7581;
	if (false == false) {
		int qmm;
		for (qmm = 71; qmm > 0; qmm--) {
			continue;
		}
	}
	return 49947;
}

bool puupzjd::pdbhxcjhexjnndkxtleezhcn(bool xbvhw, double iwuaisudsnzk) {
	bool epjcxpr = true;
	double bavcurojgf = 40722;
	int kiftpryc = 3038;
	if (3038 != 3038) {
		int yi;
		for (yi = 58; yi > 0; yi--) {
			continue;
		}
	}
	if (40722 != 40722) {
		int ymyxnyayn;
		for (ymyxnyayn = 38; ymyxnyayn > 0; ymyxnyayn--) {
			continue;
		}
	}
	if (true != true) {
		int lmehybgkcb;
		for (lmehybgkcb = 73; lmehybgkcb > 0; lmehybgkcb--) {
			continue;
		}
	}
	return false;
}

void puupzjd::ohjquapiubfjkqokvscniwb(bool dammag, string fasqvsbnwu, string erwwxamkginjtn, string hmxpw, bool ppceqgccdtgfm, bool mlubigyoltzvuf, bool waahcc, string xvqweynjuycnh, double oyerritlk) {
	bool xvwvgotfzqeb = true;
	bool dsprlr = false;
	if (false == false) {
		int kuqxrzh;
		for (kuqxrzh = 42; kuqxrzh > 0; kuqxrzh--) {
			continue;
		}
	}
	if (true == true) {
		int nc;
		for (nc = 56; nc > 0; nc--) {
			continue;
		}
	}
	if (false != false) {
		int tidpynl;
		for (tidpynl = 34; tidpynl > 0; tidpynl--) {
			continue;
		}
	}
	if (true == true) {
		int ewrdsfbcq;
		for (ewrdsfbcq = 36; ewrdsfbcq > 0; ewrdsfbcq--) {
			continue;
		}
	}

}

string puupzjd::dapolmvbjnjeiqvgkphr() {
	bool oauml = true;
	int firthg = 3255;
	if (3255 == 3255) {
		int gseafa;
		for (gseafa = 14; gseafa > 0; gseafa--) {
			continue;
		}
	}
	return string("bckbwtamprync");
}

bool puupzjd::opuqpgvjfzhrddoagmrhz(string wftzcfmhzgu, bool uguqbvwuddct) {
	string wsufnunfgpswdi = "okgnjujlqufjipeyuvtsrqvxmtkpxrgwejlizbfhkebthudoxbahyujbuqgbqtmmhmspyywuotmqxskevdawioazkczz";
	if (string("okgnjujlqufjipeyuvtsrqvxmtkpxrgwejlizbfhkebthudoxbahyujbuqgbqtmmhmspyywuotmqxskevdawioazkczz") != string("okgnjujlqufjipeyuvtsrqvxmtkpxrgwejlizbfhkebthudoxbahyujbuqgbqtmmhmspyywuotmqxskevdawioazkczz")) {
		int cyyvaip;
		for (cyyvaip = 45; cyyvaip > 0; cyyvaip--) {
			continue;
		}
	}
	if (string("okgnjujlqufjipeyuvtsrqvxmtkpxrgwejlizbfhkebthudoxbahyujbuqgbqtmmhmspyywuotmqxskevdawioazkczz") != string("okgnjujlqufjipeyuvtsrqvxmtkpxrgwejlizbfhkebthudoxbahyujbuqgbqtmmhmspyywuotmqxskevdawioazkczz")) {
		int qyrkphu;
		for (qyrkphu = 41; qyrkphu > 0; qyrkphu--) {
			continue;
		}
	}
	if (string("okgnjujlqufjipeyuvtsrqvxmtkpxrgwejlizbfhkebthudoxbahyujbuqgbqtmmhmspyywuotmqxskevdawioazkczz") != string("okgnjujlqufjipeyuvtsrqvxmtkpxrgwejlizbfhkebthudoxbahyujbuqgbqtmmhmspyywuotmqxskevdawioazkczz")) {
		int voslb;
		for (voslb = 9; voslb > 0; voslb--) {
			continue;
		}
	}
	return false;
}

bool puupzjd::tejczwoigyviuydzaoen(int sfzcjnbavggvbjh, double aiyjk, bool gqlsm, int nxqcrimsag, string mnwvai, string xjdjzdaafyvjoue) {
	bool srblmcowgop = true;
	string hfclsdlrw = "xbcgkwklnrmtazymnfcbcuqnsxjokmffszrqdynhshukybvuiqgdjblierchoexwoivyycstorchtkmouoxxyy";
	bool mepqxvdnbqr = true;
	string wqfdxnmhkemeay = "ltqquwuwcwxfadeescrxlyitkeuukrvygk";
	bool fplrcmbpv = true;
	if (string("xbcgkwklnrmtazymnfcbcuqnsxjokmffszrqdynhshukybvuiqgdjblierchoexwoivyycstorchtkmouoxxyy") != string("xbcgkwklnrmtazymnfcbcuqnsxjokmffszrqdynhshukybvuiqgdjblierchoexwoivyycstorchtkmouoxxyy")) {
		int tdde;
		for (tdde = 86; tdde > 0; tdde--) {
			continue;
		}
	}
	if (true != true) {
		int qfjzzs;
		for (qfjzzs = 76; qfjzzs > 0; qfjzzs--) {
			continue;
		}
	}
	if (true == true) {
		int eohjwe;
		for (eohjwe = 90; eohjwe > 0; eohjwe--) {
			continue;
		}
	}
	if (string("xbcgkwklnrmtazymnfcbcuqnsxjokmffszrqdynhshukybvuiqgdjblierchoexwoivyycstorchtkmouoxxyy") == string("xbcgkwklnrmtazymnfcbcuqnsxjokmffszrqdynhshukybvuiqgdjblierchoexwoivyycstorchtkmouoxxyy")) {
		int lobbhzjdp;
		for (lobbhzjdp = 10; lobbhzjdp > 0; lobbhzjdp--) {
			continue;
		}
	}
	if (true != true) {
		int vvmzyylhi;
		for (vvmzyylhi = 83; vvmzyylhi > 0; vvmzyylhi--) {
			continue;
		}
	}
	return false;
}

string puupzjd::dbmudavfktqaiss(bool coymsdzvo, string phdndqygtigarm, double cyqlmbhv, bool wznxdakf) {
	bool hcapjie = false;
	bool zfcngoijrfrgorf = false;
	int rhpvmtlxkodvba = 4468;
	double rnmxxkeuhah = 36923;
	double qmwkflyd = 24073;
	if (36923 != 36923) {
		int swzcvhjqou;
		for (swzcvhjqou = 72; swzcvhjqou > 0; swzcvhjqou--) {
			continue;
		}
	}
	return string("phem");
}

double puupzjd::phkjecvijaxpc(bool knkjcxxtnomceh, double kmybyebivs, bool sasbnrwwtprg, bool pvkdamuidqqqw, bool zhfklldbo, string krmbmp, string kndfndrek, bool ltsprsmmypmhub, double nyadzcofvjdgpbl) {
	int hjylspjkutef = 3195;
	string rxmucpvfbaspi = "cajrwgywiksnwxphyababimgcudbyxw";
	string ywwdeqpfupmbow = "dbsxxfmthsqsahuebjytswcmvqbazgiombualuaguztiertmpmezpteezhfovqpnpwssekydlpjnvgctxgqwdhhiwfewwxuxbz";
	int hcjeeozkhpnmk = 3815;
	string xkpwir = "fpuyomoikpn";
	bool wfyejkihzja = false;
	string gwswesi = "shnsvpwqqpoolmrvsldcfvpvwkxqssqcgfdiw";
	string yechruqkmgwfemk = "edmauwvmvogixtzpswjokdbtlzsrdjabhcbaovhphkfkfwwikwdxpqvwoxyvcegymhqj";
	if (false == false) {
		int ksc;
		for (ksc = 3; ksc > 0; ksc--) {
			continue;
		}
	}
	if (string("edmauwvmvogixtzpswjokdbtlzsrdjabhcbaovhphkfkfwwikwdxpqvwoxyvcegymhqj") == string("edmauwvmvogixtzpswjokdbtlzsrdjabhcbaovhphkfkfwwikwdxpqvwoxyvcegymhqj")) {
		int gffnldeunn;
		for (gffnldeunn = 73; gffnldeunn > 0; gffnldeunn--) {
			continue;
		}
	}
	if (3195 != 3195) {
		int qjs;
		for (qjs = 20; qjs > 0; qjs--) {
			continue;
		}
	}
	return 27784;
}

bool puupzjd::afrrxbsqyvqz(string uubzisllyajgn, double txwywgrxepqeeao, string ftjofhcmzs, string hurxktteectaptz, double rdbnbwjdujs, bool kvufvfnuqaxced) {
	double hlmggwwjdwr = 21999;
	string mwqkxe = "yadmurqlcjaufcrhmrcuanbyysommwxirelneqvxwtnyclmjyawrnitwfo";
	int hnldyufdavv = 2586;
	double ndbuqao = 42396;
	int tszjwfhdv = 2593;
	int qysqnbtuikwtoxx = 5423;
	double udblrzo = 13584;
	bool wvmxvjumocpo = true;
	string karwlxvwsm = "oixnraeveobsgoknjmiwkhkfkelohmtlgqpiyzjiyxcgwgvdyrhfnppmrxauhprbpyqnlomyxngrfcovjsbusnkoshfzhruep";
	bool iymsauimeh = true;
	if (42396 == 42396) {
		int ocrbdek;
		for (ocrbdek = 82; ocrbdek > 0; ocrbdek--) {
			continue;
		}
	}
	if (21999 != 21999) {
		int bdfthajsui;
		for (bdfthajsui = 43; bdfthajsui > 0; bdfthajsui--) {
			continue;
		}
	}
	if (2593 != 2593) {
		int zu;
		for (zu = 46; zu > 0; zu--) {
			continue;
		}
	}
	return false;
}

puupzjd::puupzjd() {
	this->phkjecvijaxpc(true, 64579, true, true, true, string("mxvksxanshyzkoxdylmyoojzxuhbkzsdvlrjnczrvwqqvdntrxgvtpjsupnwcwuelqaqgprxjmxzjtfwuk"), string("wydlzjnvwdlewanojqmtimrjwlucvldyzqwsdjomfcugflifeezszfnsquxzdrbzdumpnfrkozlivlgelacns"), false, 43850);
	this->afrrxbsqyvqz(string("gfahhwbglhsejftznqsdonkjtgqsjecdbvuumtbndveonifcp"), 61301, string("sfkuiqaespphjegochwvmzggvukdzzutbtntipermknizqsndcybf"), string("urcyoyaewiexxhwnazbiekpzxmcpxcsdptnhxsmltjjvzdlzlqywzywzbfsakqszpneqj"), 12589, true);
	this->exjzfjzgvjnwwlzxclkclp(27469, 2271, 41105, 8347, string("xrkirg"), false);
	this->hvihctzexfyg();
	this->pdbhxcjhexjnndkxtleezhcn(true, 48160);
	this->ohjquapiubfjkqokvscniwb(true, string("myduliiofrtmujabarc"), string("akdbextsxctahsnjbioenbgzebabuejmpkxzqllekqtcstffdhmbzcjekzvgalyziwxrvklplpbawvxsccpoimnaqt"), string("tdl"), true, false, false, string("hzcibndywhbzajsnmtwjzzmbtvzbfjhyqjpuoamtdyqxtagypwcromipnkcqfhldgtewieoyvzqha"), 1656);
	this->dapolmvbjnjeiqvgkphr();
	this->opuqpgvjfzhrddoagmrhz(string("iaakgiwzfhmcfoipstbeefvtummihyiflbvivsmkckojlkmpvlgjmbmgsyqwxdhorwgo"), true);
	this->tejczwoigyviuydzaoen(6842, 31400, false, 2113, string("isfvspxenvcyupquvekdcidrgzasnpsdxcaxpdrkdiiqqvfpwyhqjuebqjrjokvfybvrbxexapfmzemcuajikwlwd"), string("pkkfziasncrpezmlkhocgznyeimknvdivhr"));
	this->dbmudavfktqaiss(true, string("iyxjqlclgnojtrffjqtffpxjsxnuwxvsoexqckhayvrdbngtpsfojtlcpatezaetagtpghlrybvmzegyjqcva"), 67833, false);
	this->snnpdnxwzasqjmq(9531);
	this->mxfdkrudjvmwjysuiwsmtsfa(false, false, string("lgcwhhpeqvgnekqeihdxvrpbnratqkcdqgcevtpeqtizwzxfda"), 1317, false, 8655, 13316, false);
	this->zvjelrdaaxfdxdrnozclzq(38300, true);
}

