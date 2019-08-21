#pragma once
#include"FpmtWriter.h"
#include<map>
#include<unordered_set>

struct	STRUCT_ENTITY
{
#ifdef WIN64
__int64				ifcEntity;
#else
__int32				ifcEntity;
#endif

#ifdef WIN64
__int64				attributeCnt;
__int64				instanceCnt;
#else
__int32				attributeCnt;
__int32				instanceCnt;
#endif

STRUCT_ENTITY		* parent;
STRUCT_ENTITY		* child;
STRUCT_ENTITY		* next;
};

class Translator
{
public:
	Translator();
	~Translator();
	void translate(char* ifcFileName,char* ifcSchemaName);				//读入信息
	void test1();
	//int getIndex(std::string name);
	//wchar_t* getName(int index);
	//void initialSTRUCT_ENTITY();
	//bool matchEntityName(wchar_t* ifcEntityName, int n, std::string s);
	//void writeEntities();			
private:
	//int model_=0;
	//int entitiesCnt_;
	//STRUCT_ENTITY	** entities_;
	FpmtWriter fpmtwriter_;
	std::map<double,std::vector<int>> floorElemInstanceTbl_;	//每层单元句柄集
	std::unordered_set<std::string> matnameTbl_;				//材料表	
	std::vector<double> getBeamRectSect(const int& elemInstance);
	std::vector<double> getColumnRectSect(const int& elemInstance);
	std::vector<double> getBeamCoordinates(const int& elemInstance);
	std::vector<double> getColumnCoordinates(const int& elemInstance);
	double getBeamCalcuLength(const int& elemInstance);
	double getBeamRealLength(const int& elemInstance);
	double getColumenLength(const int& elemInstance);
	double getH(const int& elemInstance);
};




