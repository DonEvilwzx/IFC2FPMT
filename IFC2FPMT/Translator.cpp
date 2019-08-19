#include "stdafx.h"
#include "Translator.h"
#include <string>
#include <set>
#include <fstream>
#include "ifcengine/include/engine.h"
#include "ifcengine/include/ifcengine.h"

STRUCT_ENTITY	* new_STRUCT_ENTITY(__int32 ifcEntity)
{
	STRUCT_ENTITY	* entity = new STRUCT_ENTITY;

	entity->ifcEntity = ifcEntity;

	entity->attributeCnt = 0;
	entity->instanceCnt = 0;

	entity->parent = 0;
	entity->child = 0;
	entity->next = 0;

	return	entity;
}

void Translator::translate()
{ 
	char  * ifcFileName, *ifcSchemaName;
	ifcFileName = (char*)"Á½é¯èì¼Ü.ifc";
	//ifcFileName = (char*)"10²ã¿ò¼Ü.ifc";
	ifcSchemaName = (char*)"IFC2X3_TC1.exp";
	int model = sdaiOpenModelBN(0, ifcFileName, ifcSchemaName);					
	int* storeyAggr= sdaiGetEntityExtentBN(model, "IFCBUILDINGSTOREY");			
	int* columnAggr= sdaiGetEntityExtentBN(model, "IFCCOLUMN");
	int* beamAggr = sdaiGetEntityExtentBN(model, "IFCBEAM");
	int storeysCount = sdaiGetMemberCount(storeyAggr);							
	int columnCount = sdaiGetMemberCount(columnAggr);
	int beamCount = sdaiGetMemberCount(beamAggr);
	std::set<std::vector<double>> vsect;
	for (int i = 0; i < storeysCount; i++)
	{
		int storeyInstance;
		engiGetAggrElement(storeyAggr, i, sdaiINSTANCE, &storeyInstance);
		double elevation;
		sdaiGetAttrBN(storeyInstance, "Elevation", sdaiREAL, &elevation);
		int* containelemsAggr=nullptr;
		sdaiGetAttrBN(storeyInstance, "ContainsElements", sdaiAGGR, &containelemsAggr);
		int containelemsCount = sdaiGetMemberCount(containelemsAggr);
		for (int j = 0; j < containelemsCount; j++)
		{
			int containelemInstance;
			engiGetAggrElement(containelemsAggr, j, sdaiINSTANCE, &containelemInstance);
			int* relatedelemAggr = nullptr;
			sdaiGetAttrBN(containelemInstance, "RelatedElements", sdaiAGGR, &relatedelemAggr);
			int realatedelemCounts = sdaiGetMemberCount(relatedelemAggr);
			for (int k = 0; k < realatedelemCounts; k++)
			{
				int relatedelemInstance;
				engiGetAggrElement(relatedelemAggr, k, sdaiINSTANCE, &relatedelemInstance);
				floorElemInstanceTbl_[elevation].push_back(relatedelemInstance);
				std::string elemtype = engiGetInstanceClassInfo(relatedelemInstance);
				std::vector<double> sect;
				if (elemtype == "IfcBeam")
				{
					sect = getBeamRectSect(relatedelemInstance);
					double beamcallength = getBeamCalcuLength(relatedelemInstance);
					double beamrellength = getBeamRealLength(relatedelemInstance);
				}
				else if (elemtype == "IfcColumn")
					sect = getColumnRectSect(relatedelemInstance);
				if (vsect.find(sect)==vsect.end())
				{
					vsect.insert(sect);
					fpmtwriter_.addSect(vsect.size(), sect[0], sect[1]);
				}
			}
		}
	}
	int a = 10;

}

void Translator::test1()
{
	translate();
}
int Translator::getChildIndex(int parentIndex, std::string childname)
{
	int instanceCnt = entities_[parentIndex]->instanceCnt;
	int attrCnt = entities_[parentIndex]->attributeCnt;
	for (int i = 0; i <instanceCnt;i++ )
	{
		if (entities_[parentIndex]->child)
		{
			
		}
	}
	return -1;
}
wchar_t* Translator::getName(int index)
{
	wchar_t* ifcEntityName = 0;
	engiGetEntityName(entities_[index]->ifcEntity, sdaiUNICODE, (char**)& ifcEntityName);
	return ifcEntityName;
}
std::vector<double> Translator::getBeamRectSect(int elemInstance)
{
	int repreInstance;
	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
	int* repreAggr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	int shaperepInstance;
	engiGetAggrElement(repreAggr, 1, sdaiINSTANCE, &shaperepInstance);
	int* itemsAggr;
	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
	int itemInstance;
	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
	std::vector<double> sect(2);
	int areaInstance;
	sdaiGetAttrBN(itemInstance, "SweptArea", sdaiINSTANCE, &areaInstance);
	sdaiGetAttrBN(areaInstance, "YDim", sdaiREAL, &sect[0]);
	sdaiGetAttrBN(areaInstance, "XDim", sdaiREAL, &sect[1]);
	return sect;
}
std::vector<double> Translator::getColumnRectSect(int elemInstance)
{
	int repreInstance;
	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
	int* repreAggr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	int shaperepInstance;
	engiGetAggrElement(repreAggr, 0, sdaiINSTANCE, &shaperepInstance);
	int* itemsAggr = nullptr;
	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
	int itemInstance;
	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
	std::vector<double> sect(2);
	int msInstance;
	sdaiGetAttrBN(itemInstance, "MappingSource", sdaiINSTANCE, &msInstance);
	int mpInstance;
	sdaiGetAttrBN(msInstance, "MappedRepresentation", sdaiINSTANCE, &mpInstance);
	int* itemsAggr2 = nullptr;
	sdaiGetAttrBN(mpInstance, "Items", sdaiAGGR, &itemsAggr2);
	int itemInstance2;
	engiGetAggrElement(itemsAggr2, 0, sdaiINSTANCE, &itemInstance2);
	int areaInstance;
	sdaiGetAttrBN(itemInstance2, "SweptArea", sdaiINSTANCE, &areaInstance);
	sdaiGetAttrBN(areaInstance, "YDim", sdaiREAL, &sect[0]);
	sdaiGetAttrBN(areaInstance, "XDim", sdaiREAL, &sect[1]);
	return sect;
}
double Translator::getBeamRealLength(int elemInstance)
{
	int repreInstance;
	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
	int* repreAggr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	int shaperepInstance;
	engiGetAggrElement(repreAggr, 1, sdaiINSTANCE, &shaperepInstance);
	int* itemsAggr = nullptr;
	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
	int itemInstance;
	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
	double elementlength = 0;
	sdaiGetAttrBN(itemInstance, "Depth", sdaiREAL, &elementlength);
	return elementlength;
}
double Translator::getBeamCalcuLength(int elemInstance)
{
	int repreInstance;
	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
	int* repreAggr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	int shaperepInstance;
	engiGetAggrElement(repreAggr, 0, sdaiINSTANCE, &shaperepInstance);
	int* itemsAggr = nullptr;
	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
	int itemInstance;
	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
	int* pointAggr;
	sdaiGetAttrBN(itemInstance, "Points", sdaiAGGR, &pointAggr);
	int point0Instance,point1Instance;
	engiGetAggrElement(pointAggr, 0, sdaiINSTANCE, &point0Instance);
	engiGetAggrElement(pointAggr, 1, sdaiINSTANCE, &point1Instance);
	int* coordinates0Aggr;
	int* coordinates1Aggr;
	sdaiGetAttrBN(point0Instance, "Coordinates", sdaiAGGR, &coordinates0Aggr);
	sdaiGetAttrBN(point1Instance, "Coordinates", sdaiAGGR, &coordinates1Aggr);
	double x0, y0, x1, y1;
	engiGetAggrElement(coordinates0Aggr, 0, sdaiREAL, &x0);
	engiGetAggrElement(coordinates0Aggr, 1, sdaiREAL, &y0);
	engiGetAggrElement(coordinates1Aggr, 0, sdaiREAL, &x1);
	engiGetAggrElement(coordinates1Aggr, 1, sdaiREAL, &y1);
	return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
}


int Translator::getIndex(std::string name)
{
	for (int k = 0; k < entitiesCnt_; k++)
	{
		wchar_t* ifcEntityName = getName(k);
		size_t	size = 0;
		if (ifcEntityName) {
			size += wcslen(ifcEntityName);
		}
		else {
			ASSERT(false);
		}
		if (matchEntityName(ifcEntityName, size,name))
		{
			return k;
		}
	}
	return -1;
}
void Translator::initialSTRUCT_ENTITY()
{
	char  * ifcFileName, *ifcSchemaName;
	ifcFileName = (char*)"Á½é¯èì¼Ü.ifc";
	//ifcFileName = (char*)"10²ã¿ò¼Ü.ifc";
	ifcSchemaName = (char*)"IFC2X3_TC1.exp";
	int model_ = sdaiOpenModelBN(0, ifcFileName, ifcSchemaName);
	if (model_)
	{
		entitiesCnt_ = engiGetEntityCount(model_);
		entities_ = (STRUCT_ENTITY **) new size_t[entitiesCnt_];
		int i = 0;

		while (i < entitiesCnt_) {
			entities_[i] = new_STRUCT_ENTITY(engiGetEntityElement(model_, i));
			entities_[i]->attributeCnt = engiGetEntityNoArguments(entities_[i]->ifcEntity);
			entities_[i]->instanceCnt = sdaiGetMemberCount(sdaiGetEntityExtent(model_, entities_[i]->ifcEntity));
			i++;
		}

		while (i) {
			i--;
			int ifcParentEntity;
			ifcParentEntity = engiGetEntityParent(entities_[i]->ifcEntity);
			if (ifcParentEntity) {
				int	j = 0;
				while (j < entitiesCnt_) {
					if (ifcParentEntity == entities_[j]->ifcEntity) {
						if (entities_[j]->child) {
							entities_[i]->next = entities_[j]->child;
						}
						entities_[j]->child = entities_[i];
						entities_[i]->parent = entities_[j];
					}
					j++;
				}
			}
		}
	}
}
bool Translator::matchEntityName(wchar_t* ifcEntityName,int n,std::string s)
{
	if (s.size() != n)return false;
	for (int i = 0; i < n; i++)
	{
		if (ifcEntityName[i] != s[i])
			return false;
	}
	return true;
}

void Translator::writefpmt()
{

}
Translator::Translator()
{
}


Translator::~Translator()
{
}
