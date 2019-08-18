#include "stdafx.h"
#include "Translator.h"
#include <string>
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
	std::vector<std::pair<double, double>> vsect;
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
				std::vector<double> coordinates = getCoordinates(relatedelemInstance);
				std::pair<double,double> sect = getSect(relatedelemInstance,elemtype=="IfcBeam");
				bool hassect = false;
				for (int i = 0; i < vsect.size(); i++)
				{
					if (vsect[i]==sect)
					{
						hassect = true;
						break;
					}
				}
				if (!hassect) {
					vsect.push_back({ sect.first, sect.second });
					fpmtwriter_.addSect(vsect.size(), sect.first, sect.second);
				} 
			}
		}
	}
}

void Translator::test1()
{
	initialSTRUCT_ENTITY();
	int buildingstoreyIndex = getIndex("IfcBuildingStorey");
	int storeysCnt = entities_[buildingstoreyIndex]->instanceCnt;
	wchar_t* s = getName(buildingstoreyIndex);
	std::string ss = "test";

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
std::pair<double,double> Translator::getSect(int elemInstance, bool isbeam)
{
	int repreInstance;
	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
	int* repreAggr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	int tempInstance;
	if (isbeam)
		engiGetAggrElement(repreAggr, 1, sdaiINSTANCE, &tempInstance);
	else
		engiGetAggrElement(repreAggr, 0, sdaiINSTANCE, &tempInstance);
	int* itemsAggr = nullptr;
	sdaiGetAttrBN(tempInstance, "Items", sdaiAGGR, &itemsAggr);
	int itemInstance;
	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
	std::pair<double,double> sect;
	if (isbeam)
	{
		int areaInstance;
		sdaiGetAttrBN(itemInstance, "SweptArea", sdaiINSTANCE, &areaInstance);
		sdaiGetAttrBN(areaInstance, "YDim", sdaiREAL, &sect.first);
		sdaiGetAttrBN(areaInstance, "XDim", sdaiREAL, &sect.second);
	}
	else
	{
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
		sdaiGetAttrBN(areaInstance, "YDim", sdaiREAL, &sect.first);
		sdaiGetAttrBN(areaInstance, "XDim", sdaiREAL, &sect.second);
	}
	sect.first = round(sect.first);
	sect.second = round(sect.second);
	return sect;
}

std::vector<double> Translator::getCoordinates(int elemInstance)
{
	int opInstance;
	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	int rpInstance;
	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
	int locInstance;
	sdaiGetAttrBN(rpInstance, "Location", sdaiINSTANCE, &locInstance);
	int* coordAggr;
	sdaiGetAttrBN(locInstance, "Coordinates",sdaiAGGR,&coordAggr);
	int dims = sdaiGetMemberCount(coordAggr);
	std::vector<double> coordinates(3);
	for (int i = 0; i < dims; i++)
	{
		engiGetAggrElement(coordAggr, i, sdaiREAL,&coordinates[i]);
	}
	return coordinates;
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
