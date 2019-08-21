#pragma once
#include<vector>
#include<list>
#include<sstream>
#include<string>
class FpmtWriter
{
public:
	void writeFpmt();
	bool emptySect();
	void addTubeSect(int id,double d,double t);
	void addRectSect(int id,double w,double h,double eular=0);
	void addRoundSect(int id,  double d);
	void addCustomSect(int id, double A ,double ly, double lz, double lyz);
	void addThickSect(int id,double T);
	void addLEMat(int id, double E,double density,double v, double G=0);
	void addEPPMat(int id,  double E, double density, double v,double sigcr, double G = 0);
	void addBKINMat(int id,  double E, double density, double v,double sigcr,double et, double G = 0);
	void addBISOMat(int id,  double E, double density, double v, double sigcr, double et, double G = 0);
	void addCAHBMat(int id,   double E, double density, double v, double sigcr, double et, double beta, double G = 0);
	void addNode(int id, double x, double y, double z);
	void addTrussElem(int id, int nn1,int nn2, int realno,int sectno, int matno);
	void addBeamElem(int id, int nn1, int nn2, int sectno, int matno);
	FpmtWriter();
	~FpmtWriter();
private:
	std::list <std::string> smaterial_;
	std::list <std::string> ssect_;
	std::list<std::string> sreal_;
	std::list<std::string> snode_;
	std::list<std::string> selem_;
};

