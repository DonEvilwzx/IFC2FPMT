#pragma once
#include<list>
#include<sstream>
#include<string>

class FpmtWriter
{
public:
	void addTubeSect(int id, double d, double t);
	void addRectSect(int id, double w, double h, double eular = 0);
	void addRoundSect(int id, double d);
	void addCustomSect(int id, double A, double ly, double lz, double lyz);
	void addThickSect(int id, double T);
	void addLEMat(int id, double density, double E, double v, double zeta_a = 0, double zeta_b = 0);
	void addEPPMat(int id, double E, double density, double v, double sigcr, double G = 0);
	void addBKINMat(int id, double E, double density, double v, double sigcr, double et, double G = 0);
	void addBISOMat(int id, double E, double density, double v, double sigcr, double et, double G = 0);
	void addCAHBMat(int id, double E, double density, double v, double sigcr, double et, double beta, double G = 0);
	void addNode(int id, double x, double y, double z);
	void addTrussElem(int id, int nn1, int nn2, int realno, int sectno, int matno);
	void addBeamElem(int id, int nn1, int nn2, int sectno, int matno);
	void writeFpmt(wchar_t* filename);
	FpmtWriter();
	~FpmtWriter();
	//荷载、约束、加速度未添加
private:
	std::list <std::string> mListMat;
	std::list <std::string> mListSect;
	std::list<std::string> mListReal;
	std::list<std::string> mListNode;
	std::list<std::string> mListElem;
};
