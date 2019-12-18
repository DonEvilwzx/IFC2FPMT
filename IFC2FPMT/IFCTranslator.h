#pragma once
#include"FpmtWriter.h"
#include<map>
#include<set>
#include<vector>
#include <list>

struct BeamElement
{
	std::vector<double> mNode1;
	std::vector<double> mNode2;
	int mSectNum;
	int mMatNum;
	std::vector<std::pair<double, double>> mElemCrossPoints;
};

struct SolidElement
{
	int mNode1;
	int mNode2;
	int mNode3;
	int mNode4;
	int mMatNum;
};

class IFCTranslator
{
public:
	IFCTranslator() {};
	~IFCTranslator();
	void setOutputpath(const CString& opath);
	void test1();
protected:
	void translateOldVersion(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName);
	void translateNewVersion(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName);

	// 将材料名录入材料表，并对其进行编号，返回编号值。
	int numberMatTable(std::wstring matname);

	// 将截面名录入截面表，并对其进行编号，返回编号值。
	int numberSectTable(std::vector<double> vsect);

	//将节点坐标录入节点表，并对其进行编号，返回编号值
	int numberNodeTable(std::vector<double> vnode);
	// 解析构件
	void parseElement(const long long& instance, std::vector<double> relativeCoord);

	// 解析楼层
	void parseBuildingStorey(const long long& instance, std::vector<double> relativeCoord);


	//获取构件一个端点的三维坐标。
	std::vector<double> getCoordinate(const long long & instance);

	//输出FPMT文件
	void writeFPMT();

	//从Location句柄获取坐标
	std::vector<double> getCoordFromLocation(const long long& locInstance);
private:
	FpmtWriter mFPMTWriter;
	wchar_t outputpath_[512];
	std::map<std::vector<double>, int> mSectTable;
	std::map<std::wstring, int> mSectNameTable;
	std::map<std::wstring, int> mMatTable;
	std::set<std::vector<double>> mCrossPoints;
	std::map<std::vector<double>, int> mNodeTable;
	std::vector<BeamElement> mBeamElems;
	std::vector<SolidElement> mSolidElems;
	std::vector<double> getBeamRectSect(const long long& elemInstance);
	std::vector<double> getColumnRectSect(const long long& elemInstance);
	std::wstring getSectName(const long long& elemInstance);
	std::vector<double> getBeamCoordinates(const long long& elemInstance);
	std::vector<double> getColumnCoordinates(const long long& elemInstance);
	double getBeamCalcuLength(const long long& elemInstance);
	double getBeamRealLength(const long long& elemInstance);
	std::wstring getMatName(const long long& elemInstance);
	std::vector<BeamElement> getBeams(const long long& ElemInstance);
	std::vector<double> getCrossPoint(BeamElement elem1, BeamElement elem2);
	void splitBeam(BeamElement& e, std::vector<BeamElement> v);
	void splitBeams(std::vector<BeamElement>& v);
	void setBeams(const std::vector <BeamElement>& velem);

};

