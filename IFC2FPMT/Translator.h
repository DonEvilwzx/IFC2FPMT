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
	void translate();				//读入信息
	void writefpmt();				//输出.fpmt文件
	void test1();
	int getIndex(std::string name);
	int getChildIndex(int parentIndex, std::string childname);
	wchar_t* getName(int index);
	void initialSTRUCT_ENTITY();
	bool matchEntityName(wchar_t* ifcEntityName, int n, std::string s);
	//void writeEntities();			
private:
	int model_;
	int entitiesCnt_;
	STRUCT_ENTITY	** entities_;
	FpmtWriter fpmtwriter_;
	std::map<double,std::vector<int>> floorElemInstanceTbl_;	//每层单元句柄集
	std::unordered_set<std::string> matnameTbl_;				//材料表	
	std::vector<double> getCoordinates(int elemInstance);
	//double* getSect(int elemInstacne,bool isbeam);
	std::pair<double,double> getSect(int elemInstacne, bool isbeam);
};




