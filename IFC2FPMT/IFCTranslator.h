#pragma once
#include"FpmtWriter.h"
#include<map>
#include<unordered_set>
struct Element
{
	int nodeNum1;
	int nodeNum2;
	int sectNum;
	int matNum;
};
class IFCTranslator
{
public:
	IFCTranslator() {};
	~IFCTranslator();
	void translate(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName);
	void setOutputpath(const CString& opath);
	void test1();
private:
	FpmtWriter fpmtwriter_;
	wchar_t outputpath_[512];
	std::map<std::wstring, int>  mSectTable;
	std::map<std::wstring, int> mMatTable;
	std::map<int, std::vector<double>> mNodeTable;
	std::map<int, Element> mElemTable;
	int elemcount_ = 0;
	std::map<std::vector<double>, int> vsect_;
	std::map<std::vector<double>, int> vmat_;
	std::map<std::vector<double>, int> vnode_;
	std::vector<double> getBeamRectSect(const long long& elemInstance);
	std::vector<double> getColumnRectSect(const long long& elemInstance);
	std::vector<double> getSect(const long long& elemInstance);
	std::wstring getSectName(const long long& elemInstance);
	std::vector<double> getBeamCoordinates(const long long& elemInstance);
	std::vector<double> getColumnCoordinates(const long long& elemInstance);
	double getBeamCalcuLength(const long long& elemInstance);
	double getBeamRealLength(const long long& elemInstance);
	std::vector<double> getMat(const long long& elemInstance);
	std::wstring getMatName(const long long& elemInstance);
	void setBeam(const long long& ElemInstance);
	void splitBeams(int elemNum,Element e);
	void splitByNode(int splitnodeNum, int splitelemNum, Element splitElem);
	std::vector<double> getCrossPoint(Element elem1, Element elem2);
	//double getColumenLength(const long long& elemInstance);
	//double getH(const long long& elemInstance);
	//void setElem2Fpmt(long long** aggr);
};
