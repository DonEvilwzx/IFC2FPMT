#pragma once
#include"FpmtWriter.h"
#include<map>
#include<set>
#include<vector>
#include <list>

struct Beam
{
	int mNode1;
	int mNode2;
	int mSectNum;
	int mMatNum;
};
struct Truss
{
	int mNode1;
	int mNode2;
	int mSectNum;
	int mMatNum;
};

struct Solid
{
	int mNode1;
	int mNode2;
	int mNode3;
	int mNode4;
	int mMatNum;
};

struct Shell
{

};
class IFCTranslator
{
public:
	IFCTranslator() {};
	~IFCTranslator();
	void setOutputpath(const CString& opath);
	void test1();
protected:
	//void translateOldVersion(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName);
	void translateNewVersion(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName);

	// 将材料名录入材料表，并对其建立索引，进行编号，返回编号值。
	int recordMatTable(std::wstring matname);

	// 将截面名录入截面表，并对其建立索引，进行编号，返回编号值。
	//int recordSectTable(std::vector<int> vsect);


	int recordSectTable(std::wstring sectname);
	//将节点坐标录入节点表，并对其建立索引，进行编号，返回编号值
	int recordNodeTable(std::vector<int> vnode);

	//录入梁单元表
	void recordBeamElement(int n1,int n2,int matno,int sectno);

	//录入实体单元表
	void recordSolidElement(int n1, int n2, int n3, int n4, int matno);

	//直接输入8个点录入5个实体单元
	void record5SolidElement(int no1,int no2,int no3,int no4,int no5,int no6,int no7,int no8, int matno);

	// 解析构件
	void parseElement(const long long& instance, std::vector<int> relativeCoord,bool isBeam);


	void parseItems(long long* itemsAggr, int matno, std::vector<int> relativeCoord, std::vector<double> direct);


	//void parseSweptSolid(const long long& itemInstance, int matno, std::vector<int> relativeCoord,std::vector<double> direct);
	// 解析楼层
	void parseBuildingStorey(const long long& instance, std::vector<int> relativeCoord);


	//获取构件一个端点的三维坐标。
	std::vector<int> getCoordinate(const long long& instance);

	//获取构件的方向向量
	std::vector<double> getDirect(const long long& instance);
	//输出FPMT文件
	void writeFPMT();

	//获取材料名
	std::wstring getMatName(const long long& elemInstance);

	//从Location句柄获取坐标
	std::vector<int> getCoordFromLocation(const long long& locInstance);

	//从CoordAggr获取坐标
	std::vector<int> getCoordFromCoordAggr( long long*  coordAggr);


private:
	FpmtWriter mFPMTWriter;
	wchar_t outputpath_[512];
	//std::map<std::vector<int>, int> mSectTable;
	std::map<std::wstring, int> mSectNameTable;
	std::map<std::wstring, int> mMatTable;
	//std::set<std::vector<double>> mCrossPoints;
	std::map<std::vector<int>, int> mNodeTable;
	std::vector<Beam> mBeamElems;
	std::vector<Solid> mSolidElems;
	
	//std::vector<double> getBeamRectSect(const long long& elemInstance);
	//std::vector<double> getColumnRectSect(const long long& elemInstance);
	//std::wstring getSectName(const long long& elemInstance);
	//std::vector<double> getBeamCoordinates(const long long& elemInstance);
	//std::vector<double> getColumnCoordinates(const long long& elemInstance);
	//double getBeamCalcuLength(const long long& elemInstance);
	//double getBeamRealLength(const long long& elemInstance);
	
	//std::vector<BeamElement> getBeams(const long long& ElemInstance);
	//std::vector<double> getCrossPoint(BeamElement elem1, BeamElement elem2);
	//void splitBeam(BeamElement& e, std::vector<BeamElement> v);
	//void splitBeams(std::vector<BeamElement>& v);
	//void setBeams(const std::vector <BeamElement>& velem);

};

