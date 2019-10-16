#pragma once
#include"FpmtWriter.h"
#include<map>
#include<set>
#include<vector>
#include <list>

struct Element
{
	std::vector<double> mNode1;
	std::vector<double> mNode2;
	int mSectNum;
	int mMatNum;
	std::vector<std::pair<double,double>> mElemCrossPoints;
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
	FpmtWriter mFPMTWriter;
	wchar_t outputpath_[512];
	
	std::map<std::wstring, int>  mSectTable;
	std::map<std::wstring, int> mMatTable;
	std::set<std::vector<double>> mCrossPoints;
	std::map<std::vector<double>, int> mNodeTable;
	std::vector<Element> mElems;
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
	std::vector<Element> getBeams(const long long& ElemInstance);
	//void splitByNode(int splitnodeNum, int splitelemNum, Element splitElem);
	std::vector<double> getCrossPoint(Element elem1, Element elem2);
	void splitBeam(Element& e, std::vector<Element> v);
	void splitBeams(std::vector<Element>& v);
	void setBeams(const std::vector <Element>& velem);

	//double getColumenLength(const long long& elemInstance);
	//double getH(const long long& elemInstance);
	//void setElem2Fpmt(long long** aggr);
};


/*
	交叉梁分割算法思路

	struct Element
	{
		std::vector<double> node1;
		std::vector<double> node2;
		int matno;
		int sectno;
	}

	set<Element> mXBeamSet;
	set<Element> mYBeamSet;
	set<Element> mColumnSet;

	set<Element> mElement;
	set<double> mXAxisSet;
	set<double> mYAxisSet;

	遍历每一个楼层:
		遍历每一个单元	
			if(梁单元)
			{
				getSect();
				getMat();

				coord= getCoord();
				node1=..
				node2=...
				if(node1[0]==node2[0])  //水平
				{
					mXAxisSet.insert(node1[0]);
					mXBeamSet.insert(Element)
				}
				else if(node1[1]==mode2[1])		//垂直
				{
					mYAxisSet.insert(node1[1]);
					mYBeamSet.insert(Element)
				}
				else
				{
					AfxMessage("暂不支持非框架结构！")
					exit(0);
				}
			}
			else if(柱单元）
			{
				...
			}
		
		for(auto beam:mXBeamSet)
		{
			for(auto itr:mYAxis)
				splitXBeam(beam,);
		}

		for(auto beam:mYBeamSet)
		{
			splitYBeam(beam);
		}

		void splitXBeam(beam)
		{
			
		}
*/