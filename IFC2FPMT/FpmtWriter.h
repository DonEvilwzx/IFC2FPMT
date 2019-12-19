#pragma once
#include<list>
#include<sstream>
#include<string>

class FpmtWriter
{
public:
	void addTubeSect(int id, double d, double t);
	void addRectSect(int id, double w = 0.6, double h = 0.6, double eular = 0);
	void addRoundSect(int id, double d);
	void addCustomSect(int id, double A, double ly, double lz, double lyz);
	void addThickSect(int id, double T);
	void addLEMat(int id, double density = 7850, double E = 2.06e11, double v = 0.3, double zeta_a = 0, double zeta_b = 0);
	void addEPPMat(int id, double E, double density, double v, double sigcr, double G = 0);
	void addBKINMat(int id, double E, double density, double v, double sigcr, double et, double G = 0);
	void addBISOMat(int id, double E, double density, double v, double sigcr, double et, double G = 0);
	void addCAHBMat(int id, double E, double density, double v, double sigcr, double et, double beta, double G = 0);
	void addNode(int id, double x, double y, double z);
	void addTrussElem(int id, int nn1, int nn2, int realno, int sectno, int matno);
	void addBeamElem(int id, int nn1, int nn2, int sectno, int matno);
	void addSolidElem(int id, int nn1, int nn2, int nn3, int nn4, int matno);
	void writeFPMT(wchar_t* filename);
	FpmtWriter();
	~FpmtWriter();
private:
	//材料字符串表
	std::list <std::string> mListMat;
	//截面字符串表
	std::list <std::string> mListSect;
	//实数字符串表
	std::list<std::string> mListReal;
	//节点字符串表
	std::list<std::string> mListNode;
	//单元字符串表
	std::list<std::string> mListElem;
};
