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

	// ��������¼����ϱ���������б�ţ����ر��ֵ��
	int numberMatTable(std::wstring matname);

	// ��������¼��������������б�ţ����ر��ֵ��
	int numberSectTable(std::vector<double> vsect);

	//���ڵ�����¼��ڵ����������б�ţ����ر��ֵ
	int numberNodeTable(std::vector<double> vnode);
	// ��������
	void parseElement(const long long& instance, std::vector<double> relativeCoord);

	// ����¥��
	void parseBuildingStorey(const long long& instance, std::vector<double> relativeCoord);


	//��ȡ����һ���˵����ά���ꡣ
	std::vector<double> getCoordinate(const long long & instance);

	//���FPMT�ļ�
	void writeFPMT();

	//��Location�����ȡ����
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

