#include "stdafx.h"
#include "IFCWriter.h"
#include <fstream>
#include <sstream>
IFCWriter::IFCWriter()
{
}

IFCWriter::~IFCWriter()
{
}

void IFCWriter::writeIFC(wchar_t* filename)
{
	std::ofstream outfile(filename);

}