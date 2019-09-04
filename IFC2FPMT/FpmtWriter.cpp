#include "stdafx.h"
#include "FpmtWriter.h"
#include <fstream>
#include <sstream>

void FpmtWriter::addLEMat(int id, double density, double E, double v, double zeta_a, double zeta_b)
{
	std::ostringstream oss;
	oss << id << "," << "mat_" << id << "," << "LE," << density << "," << E << "," << v << "," << zeta_a << "," << zeta_b;
	mListMat.push_back(oss.str());
}
void FpmtWriter::addEPPMat(int id, double E, double density, double v, double sigcr, double G)
{
	std::ostringstream oss;
	if (G > 0)
		oss << id << "," << "mat_" << id << "," << "EPP," << E << "," << G << "," << density << "," << v << sigcr;
	else
		oss << id << "," << "mat_" << id << "," << "EPP," << E << "," << "," << density << "," << v << sigcr;
	mListMat.push_back(oss.str());
}
void FpmtWriter::addBKINMat(int id, double E, double density, double v, double sigcr, double et, double G)
{
	std::ostringstream oss;
	if (G > 0)
		oss << id << "," << "mat_" << id << "," << "BKIN," << E << "," << G << "," << density << "," << v << sigcr << "," << et;
	else
		oss << id << "," << "mat_" << id << "," << "BKIN," << E << "," << "," << density << "," << v << sigcr << "," << et;
	mListMat.push_back(oss.str());
}
void FpmtWriter::addBISOMat(int id, double E, double density, double v, double sigcr, double et, double G)
{
	std::ostringstream oss;
	if (G > 0)
		oss << id << "," << "mat_" << id << "," << "BISO," << E << "," << G << "," << density << "," << v << sigcr << "," << et;
	else
		oss << id << "," << "mat_" << id << "," << "BISO," << E << "," << "," << density << "," << v << sigcr << "," << et;
	mListMat.push_back(oss.str());
}
void FpmtWriter::addCAHBMat(int id, double E, double density, double v, double sigcr, double et, double beta, double G)
{
	std::ostringstream oss;
	if (G > 0)
		oss << id << "," << "mat_" << id << "," << "CAHB," << E << "," << G << "," << density << "," << v << "," << sigcr << "," << et << beta;
	else
		oss << id << "," << "mat_" << id << "," << "CAHB," << E << "," << "," << density << "," << v << "," << sigcr << "," << et << beta;
	mListMat.push_back(oss.str());
}
void FpmtWriter::addTubeSect(int id, double d, double t)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "TUBE," << d << "," << t;
	mListSect.push_back(oss.str());
}
void FpmtWriter::addRectSect(int id, double w, double h, double eular)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "RECT," << w << "," << h << "," << eular;
	mListSect.push_back(oss.str());
}
void FpmtWriter::addRoundSect(int id, double d)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "ROUND," << d;
	mListSect.push_back(oss.str());
}
void FpmtWriter::addCustomSect(int id, double A, double ly, double lz, double lyz)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "CUSTOM," << A << "," << ly << "," << lz << "," << lyz;
	mListSect.push_back(oss.str());
}
void FpmtWriter::addThickSect(int id, double T)
{
	std::ostringstream oss;
	oss << id << "," << "sect_" << id << "," << "THICK," << T;
	mListSect.push_back(oss.str());
}
void FpmtWriter::addNode(int id, double x, double y, double z)
{
	std::ostringstream oss;
	oss << id << "," << x << "," << y << "," << z;
	mListNode.push_back(oss.str());
}
void FpmtWriter::addTrussElem(int id, int nn1, int nn2, int realno, int sectno, int matno)
{
	std::ostringstream oss;
	oss << id << "," << "TRUSS," << nn1 << "," << nn2 << "," << realno << "," << sectno << "," << matno;
	mListElem.push_back(oss.str());
}
void FpmtWriter::addBeamElem(int id, int nn1, int nn2, int sectno, int matno)
{
	std::ostringstream oss;
	oss << id << "," << "BEAM," << nn1 << "," << nn2 << "," << 0 << "," << sectno << "," << matno;
	mListElem.push_back(oss.str());
}
void FpmtWriter::writeFpmt(wchar_t* filename)
{
	std::ofstream outfile(filename);
	outfile << "/MATERIAL/" << std::endl;
	for (auto itr : mListMat)
		outfile << itr << std::endl;
	outfile << "##" << std::endl << std::endl;

	outfile << "/SECTION/" << std::endl;
	for (auto itr : mListSect)
		outfile << itr << std::endl;
	outfile << "##" << std::endl << std::endl;

	outfile << "/REAL/" << std::endl;
	for (auto itr : mListReal)
		outfile << itr << std::endl;
	outfile << "##" << std::endl << std::endl;

	outfile << "/NODE/" << std::endl;
	for (auto itr : mListNode)
		outfile << itr << std::endl;
	outfile << "##" << std::endl << std::endl;

	outfile << "/ELEM/" << std::endl;
	for (auto itr : mListElem)
		outfile << itr << std::endl;
	outfile << "##" << std::endl << std::endl;
}
FpmtWriter::FpmtWriter()
{
}


FpmtWriter::~FpmtWriter()
{
}
