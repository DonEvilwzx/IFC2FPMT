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

	// ��������¼����ϱ������佨�����������б�ţ����ر��ֵ��
	int recordMatTable(std::wstring matname);

	// ��������¼�����������佨�����������б�ţ����ر��ֵ��
	//int recordSectTable(std::vector<int> vsect);


	int recordSectTable(std::wstring sectname);
	//���ڵ�����¼��ڵ�������佨�����������б�ţ����ر��ֵ
	int recordNodeTable(std::vector<int> vnode);

	//¼������Ԫ��
	void recordBeamElement(int n1,int n2,int matno,int sectno);

	//¼��ʵ�嵥Ԫ��
	void recordSolidElement(int n1, int n2, int n3, int n4, int matno);

	//ֱ������8����¼��5��ʵ�嵥Ԫ
	void record5SolidElement(int no1,int no2,int no3,int no4,int no5,int no6,int no7,int no8, int matno);

	// ��������
	void parseElement(const long long& instance, std::vector<int> relativeCoord,bool isBeam);


	void parseItems(long long* itemsAggr, int matno, std::vector<int> relativeCoord, std::vector<double> direct);


	//void parseSweptSolid(const long long& itemInstance, int matno, std::vector<int> relativeCoord,std::vector<double> direct);
	// ����¥��
	void parseBuildingStorey(const long long& instance, std::vector<int> relativeCoord);


	//��ȡ����һ���˵����ά���ꡣ
	std::vector<int> getCoordinate(const long long& instance);

	//��ȡ�����ķ�������
	std::vector<double> getDirect(const long long& instance);
	//���FPMT�ļ�
	void writeFPMT();

	//��ȡ������
	std::wstring getMatName(const long long& elemInstance);

	//��Location�����ȡ����
	std::vector<int> getCoordFromLocation(const long long& locInstance);

	//��CoordAggr��ȡ����
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

