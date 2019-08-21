#include "stdafx.h"
#include "FpmtWriter.h"
#include <fstream>
#include<sstream>

bool FpmtWriter::emptySect()
{
	return ssect_.empty();
}
void FpmtWriter::addLEMat(int id,  double E,double density,double v, double G)
{
	std::ostringstream oss;
	if(G>0)
		oss << id << "," << "mat_" << id << "," << "LE,"<<E<<","<<G<<","<<density<<","<<v;
	else
		oss << id << "," << "mat_" << id << "," << "LE," << E << "," <<  "," << density << "," << v;
	smaterial_.push_back(oss.str());
}
void FpmtWriter::addEPPMat(int id,   double E,  double density, double v, double sigcr, double G )
{
	std::ostringstream oss;
	if (G > 0)
		oss << id << "," << "mat_" << id << "," << "EPP," << E << "," << G << "," << density << "," << v<<sigcr;
	else
		oss << id << "," << "mat_" << id << "," << "EPP," << E << "," << "," << density << "," << v<<sigcr;
	smaterial_.push_back(oss.str());
}
void FpmtWriter::addBKINMat(int id,   double E,  double density, double v, double sigcr, double et, double G )
{
	std::ostringstream oss;
	if (G > 0)
		oss << id << "," << "mat_" << id << "," << "BKIN," << E << "," << G << "," << density << "," << v << sigcr << "," << et;
	else
		oss << id << "," << "mat_" << id << "," << "BKIN," << E << "," << "," << density << "," << v << sigcr << "," << et;
	smaterial_.push_back(oss.str());
}
void FpmtWriter::addBISOMat(int id,  double E,  double density, double v, double sigcr, double et, double G )
{
	std::ostringstream oss;
	if (G > 0)
		oss << id << "," << "mat_" << id << "," << "BISO," << E << "," << G << "," << density << "," << v << sigcr << "," << et;
	else
		oss << id << "," << "mat_" << id << "," << "BISO," << E << "," << "," << density << "," << v << sigcr << "," << et;
	smaterial_.push_back(oss.str());
}
void FpmtWriter::addCAHBMat(int id,  double E,  double density, double v, double sigcr, double et, double beta, double G )
{
	std::ostringstream oss;
	if(G>0)
		oss << id << "," << "mat_" << id << "," << "CAHB," << E << "," << G << "," << density << "," << v << "," << sigcr << "," << et<<beta;
	else
		oss << id << "," << "mat_" << id << "," << "CAHB," << E << "," << "," << density << "," << v << "," << sigcr << "," << et << beta;
	smaterial_.push_back(oss.str());
}
void FpmtWriter::addTubeSect(int id, double d, double t)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "TUBE," << d << "," << t;
	ssect_.push_back(oss.str());
}
void FpmtWriter::addRectSect(int id, double w, double h, double eular)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "RECT," << w << "," << h << "," << eular;
	ssect_.push_back(oss.str());
}
void FpmtWriter::addRoundSect(int id, double d)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "ROUND," <<d;
	ssect_.push_back(oss.str());
}
void FpmtWriter::addCustomSect(int id, double A,double ly,double lz,double lyz)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "CUSTOM," << A << "," << ly << "," << lz<<","<<lyz;
	ssect_.push_back(oss.str());
}
void FpmtWriter::addThickSect(int id, double T)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "THICK," << T;
	ssect_.push_back(oss.str());
}
void FpmtWriter::addNode(int id, double x, double y, double z)
{
	std::ostringstream oss;
	oss << id << "," << x << "," << y << ","<<z;
	snode_.push_back(oss.str());
}
void FpmtWriter::addTrussElem(int id,  int nn1,int nn2,int realno, int sectno, int matno)
{
	std::ostringstream oss;
	oss << id << "," <<"TRUSS,"<< nn1 << "," << nn2 << "," << realno<<","<<sectno<<","<<matno;
	selem_.push_back(oss.str());
}
void FpmtWriter::addBeamElem(int id, int nn1, int nn2, int sectno, int matno)
{
	std::ostringstream oss;
	oss << id << "," <<"BEAM,"<< nn1 << "," << nn2 << "," << 0 << "," << sectno << "," << matno;
	selem_.push_back(oss.str());
}
void FpmtWriter::writeFpmt()
{
	std::ofstream outfile("output.fpmt");
	outfile << "/MATERIAL/" << std::endl;
	for (auto itr : smaterial_)
		outfile << itr << std::endl;
	outfile << "##" << std::endl<<std::endl;

	outfile << "/SECTION/" << std::endl;
	for (auto itr : ssect_)
		outfile << itr << std::endl;
	outfile << "##" << std::endl<<std::endl;
	
	outfile << "/REAL/" << std::endl;
	for (auto itr : sreal_)
		outfile << itr << std::endl;
	outfile << "##" << std::endl<<std::endl;

	outfile << "/NODE/" << std::endl;
	for (auto itr : snode_)
		outfile << itr<<std::endl;
	outfile << "##" << std::endl<<std::endl;

	outfile << "/ELEM/" << std::endl;
	for (auto itr : selem_)
		outfile << itr<<std::endl;
	outfile << "##" << std::endl<<std::endl;
}
FpmtWriter::FpmtWriter()
{
}


FpmtWriter::~FpmtWriter()
{
}
