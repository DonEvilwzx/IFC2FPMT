#pragma once
#include"FpmtWriter.h"
#include<map>
#include<unordered_set>
class IFCTranslator
{
public:
	IFCTranslator();
	~IFCTranslator();
	void translate(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName);
	void setOutputpath(const CString& opath);
	void test1();
private:
	FpmtWriter fpmtwriter_;
	wchar_t outputpath_[512];
	std::vector<double> getBeamRectSect(const long long& elemInstance);
	std::vector<double> getColumnRectSect(const long long& elemInstance);
	std::vector<double> getBeamCoordinates(const long long& elemInstance);
	std::vector<double> getColumnCoordinates(const long long& elemInstance);
	double getBeamCalcuLength(const long long& elemInstance);
	double getBeamRealLength(const long long& elemInstance);
	double getColumenLength(const long long& elemInstance);
	double getH(const long long& elemInstance);
};
