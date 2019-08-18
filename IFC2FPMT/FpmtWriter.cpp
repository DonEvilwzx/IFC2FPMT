#include "stdafx.h"
#include "FpmtWriter.h"
#include <fstream>
#include<sstream>
void FpmtWriter::addNode(int id, double x, double y, double z)
{
	Node node(id, x, y, z);
	vnode_.push_back(node);
}
void FpmtWriter::addElem(int id, int type, std::vector<int> vnodenum, int realno, int sectno, int matno)
{
	Element elem(id, type, vnodenum, realno, sectno, matno);
	velem_.push_back(elem);
}
void FpmtWriter::addSect(int id,int b,int h)
{
	std::ostringstream oss;
	oss << id << "," << "sect_rect" << id << "," << "RECT," << b << "," << h << "," << "0";
	ssect_.insert(oss.str());
}
bool FpmtWriter::emptySect()
{
	return ssect_.empty();
}
void FpmtWriter::writeFpmt()
{
	std::ofstream outfile("output.fpmt");
	outfile << "/MATERIAL/" << std::endl;
	for (auto itr : smaterial_)
		outfile << itr << std::endl;
	outfile << "##" << std::endl;
	
	outfile << "/SECTION/" << std::endl;
	for (auto itr : ssect_)
		outfile << itr << std::endl;
	outfile << "##" << std::endl;
	
	outfile << "/REAL/" << std::endl;
	for (auto itr : sreal_)
		outfile << itr << std::endl;
	outfile << "##" << std::endl;

	outfile << "/NODE/" << std::endl;
	for (auto itr : vnode_)
		outfile << itr.getOutputString();
	outfile << "##" << std::endl;

	outfile << "/ELEM" << std::endl;
	for (auto itr : velem_)
		outfile << itr.getOutputString();
	outfile << "##" << std::endl;
}
FpmtWriter::FpmtWriter()
{
}


FpmtWriter::~FpmtWriter()
{
}
