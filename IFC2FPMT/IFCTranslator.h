#pragma once
#include"FpmtWriter.h"
#include<map>
#include<set>
#include<vector>
#include<array>
#include <list>
#include<Eigen/Dense>
#include<Eigen/Cholesky>

enum  buildingType { FrameWork, ShearWall };

struct Beam
{
	int mNodeNum1;
	int mNodeNum2;
	int mSectNum;
	int mMatNum;
	std::array<double,3> mNode1;
	std::array<double,3> mNode2;
};

struct Wall
{
	int mSectNum;
	int mMatNum;
	std::array<double,3> mNode1;
	std::array<double,3> mNode2;
	//std::array<double,3> mNode3;
	//std::array<double,3> mNode4;
};

struct Shell
{
	std::array<double,3> mNode1;
	std::array<double,3> mNode2;
	std::array<double,3> mNode3;
	int mNodeNum1;
	int mNodeNum2;
	int mNodeNum3;
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

class IFCTranslator
{
public:
	IFCTranslator() {};
	~IFCTranslator();
	void setOutputpath(const CString& opath);
	void test1();
protected:
	//void translateOldVersion(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName);
	void translateNewVersion(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName,buildingType bt=FrameWork);

	void parseSite(const long long& siteInstance);
	void parseBuilding(const long long& buildingInstance, Eigen::Matrix4d relativeTMatrix);
	void parseBuildingStorey(const long long& instance, Eigen::Matrix4d relativeTmatrix);
	void parseElement(const long long& instance, Eigen::Matrix4d relativeTmatrix, bool isWall);
	void parseItems(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix, bool isWall);
	void parseIFCCSGSOLID(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix);
	void parseIFCFACEBASEDSURFACEMODEL(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix);
	void parseIFCFACETEDBREP(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix);
	void parseCfsFaces(long long* cfsfacesAggr, int matno, Eigen::Matrix4d relativeTmatrix);
	void parseIFCEXTRUDEDAREASOLID(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix, bool isWall);
	void parseIFCMAPPEDITEM(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix, bool isWall);
	int recordMatTable(std::wstring matname);
	int recordSectTable(std::vector<double> vsect);
	int recordNodeTable(std::array<double,3> vnode,bool isConsiderThick=false);
	bool findNodeTable(std::array<double,3> vnode);
	int recordNodeTableByVector(Eigen::Vector4d v);
	//void recordBeam(int n1, int n2, int matno, int sectno);
	void recordBeamByVector(Eigen::Vector4d node1, Eigen::Vector4d node2, int matno, int sectno);
	void recordShell(std::array<double,3> node1, std::array<double,3> node2,std::array<double,3> node3, int matno, int sectno);
	void recordShellsByWalls();
	void recordShellByWall(Wall wall);
	void recordShellByTwoNodes(std::array<double,3> node1, std::array<double,3> node2,int matno,int sectno);
	

	void recordWall(std::array<double,3> node1, std::array<double,3> node2, /*std::array<double,3> node3, std::array<double,3> node4,*/ int matno, int sectno);
	void recordWallByVector(Eigen::Vector4d node1, Eigen::Vector4d node2,/* Eigen::Vector4d node3, Eigen::Vector4d node4,*/ int matno, int sectno);
    /*std::vector<std::array<double,3>> getWallCrossLine(Wall w1, Wall w2);*/
	void splitWalls();
	
	void recordSolidElement(int n1, int n2, int n3, int n4, int matno);
	void record5SolidElement(int no1, int no2, int no3, int no4, int no5, int no6, int no7, int no8, int matno);
	void recordBeamNodes();
	void recordShellNodes();
	void resetMappedAttribute();
	
	
	//int recordSectNameTable(std::wstring sectname);
	std::vector<long long> getInstancesByAggr(long long* aggr);
	std::vector<long long> getInstancesByInstance(long long instance, char* name);
	std::array<double,3> getCoordinate(const long long& instance);
	std::wstring getMatName(const long long& elemInstance);
	std::array<double,3> getCoordByCoordAggr(long long*  coordAggr);
	std::array<double, 3> getDirectByDirectInstance(const long long& directInstance);
	Eigen::Matrix4d getTMatrixByInstance(const long long& instance);
	Eigen::Matrix4d getTMatrixByIfcAxis2Placement3D(const long long& instance);
	Eigen::Matrix4d getTMatrixByIfcAxis2Placement2D(const long long& instance);
	Eigen::Matrix4d getTMatrixByPositionInstance(const long long& instance);
	bool getLineCrossPoint(std::array<double,3> l1n1,std::array<double,3> l1n2, std::array<double,3> l2n1,std::array<double,3> l2n2,std::array<double,3>& crossPoint);
	void splitBeams();
	//输出FPMT文件
	void writeFPMT();

private:
	std::map<int, double> mElevations;
	int mStoreyNum = 0;
	FpmtWriter mFPMTWriter;
	wchar_t outputpath_[512];
	std::map<std::vector<double>, int> mSectTable;
	//std::map<std::wstring, int> mSectNameTable;
	std::map<std::wstring, int> mMatTable;
	//std::set<std::array<double,3>> mCrossPoints;
	std::map<int, std::array<double,3>> mNodeTable;
	//std::map<std::vector<int>, int> mNodeTable;
	std::map<int, std::vector<Beam>> mBeams;
	std::map<int, std::vector<Beam>> mColumns;
	std::vector<Wall> mWalls;
	std::vector<Solid> mSolids;
	std::vector<Shell> mShells;
	//用于梁单元的截面
	double mMappedXscale = 1;
	double mMappedYscale = 1;
	double mMappedAngle = 0;
	
	buildingType mBuilidngType = FrameWork;
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

