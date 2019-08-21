#include "stdafx.h"
#include "Translator.h"
#include <string>
#include <set>
#include <fstream>
#include "ifcengine/include/engine.h"
#include "ifcengine/include/ifcengine.h"

//STRUCT_ENTITY	* new_STRUCT_ENTITY(__int32 ifcEntity)
//{
//	STRUCT_ENTITY	* entity = new STRUCT_ENTITY;
//
//	entity->ifcEntity = ifcEntity;
//
//	entity->attributeCnt = 0;
//	entity->instanceCnt = 0;
//
//	entity->parent = 0;
//	entity->child = 0;
//	entity->next = 0;
//
//	return	entity;
//}
Translator::Translator()
{
}
Translator::~Translator()
{
}
std::vector<double> roundv(std::vector<double> v)
{
	std::vector<double> rev;
	for (auto itr : v)
		rev.push_back(round(itr));
	return rev;
}
void Translator::translate(char* ifcFileName, char* ifcSchemaName)
{ 
	int model = sdaiOpenModelBN(0, ifcFileName, ifcSchemaName);					
	int* columnAggr= sdaiGetEntityExtentBN(model, "IFCCOLUMN");
	int* beamAggr = sdaiGetEntityExtentBN(model, "IFCBEAM");
	int columnCount = sdaiGetMemberCount(columnAggr);
	int beamCount = sdaiGetMemberCount(beamAggr);
	std::map<std::vector<double>,int> vsect;
	std::map<std::vector<double>,int> vnode;
	for (int i = 0; i < beamCount; i++)
	{
		int beamInstance;
		engiGetAggrElement(beamAggr, i, sdaiINSTANCE, &beamInstance);
		
		std::vector<double> sect=getBeamRectSect(beamInstance);
		if (vsect.find(sect) == vsect.end())
		{
			int n = vsect.size();
			vsect[sect] = n + 1;
			fpmtwriter_.addRectSect(n+1, sect[0]/1000.0,sect[1]/1000.0);
		}
		
		std::vector<double> coord = getBeamCoordinates(beamInstance);
		std::vector<double> node1 = { coord[0],coord[1],coord[2] };
		std::vector<double> node2 = { coord[3],coord[4],coord[5] };
		int no1, no2;
		if (vnode.find(node1) == vnode.end())
		{
			int n = vnode.size();
			vnode[node1] = n + 1;
			fpmtwriter_.addNode(n + 1, coord[0]/1000.0, coord[1]/1000.0, coord[2]/1000.0);
			no1 = n + 1;
		}
		else
			no1 = vnode[node1];

		if (vnode.find(node2) == vnode.end())
		{
			int n = vnode.size();
			vnode[node2] = n + 1;
			fpmtwriter_.addNode(n + 1, coord[3]/1000.0, coord[4]/1000.0, coord[5]/1000.0);
			no2 = n + 1;
		}
		else
			no2 = vnode[node2];
		fpmtwriter_.addBeamElem(i + 1,no1,no2,vsect[sect],1 );
	}

	for (int i = 0; i < columnCount; i++)
	{
		int columnInstance;
		engiGetAggrElement(columnAggr, i, sdaiINSTANCE, &columnInstance); 
		std::vector<double> sect = getColumnRectSect(columnInstance);
		if (vsect.find(sect) == vsect.end())
		{
			int n = vsect.size();
			vsect[sect] = n + 1;
			fpmtwriter_.addRectSect(n + 1, sect[0]/1000.0, sect[1]/1000.0);
		}

		std::vector<double> coord = getColumnCoordinates(columnInstance);
		std::vector<double> node1 = { coord[0],coord[1],coord[2] };
		std::vector<double> node2 = { coord[3],coord[4],coord[5] };
		int no1, no2;
		if (vnode.find(node1) == vnode.end())
		{
			int n = vnode.size();
			vnode[node1] = n + 1;
			fpmtwriter_.addNode(n + 1, coord[0]/1000.0, coord[1]/1000.0, coord[2]/1000.0);
			no1 = n + 1;
		}
		else
			no1 = vnode[node1];

		if (vnode.find(node2) == vnode.end())
		{
			int n = vnode.size();
			vnode[node2] = n + 1;
			fpmtwriter_.addNode(n + 1, coord[3] / 1000.0, coord[4] / 1000.0, coord[5]/1000.0);
			no2 = n + 1;
		}
		else
			no2 = vnode[node2];
		fpmtwriter_.addBeamElem(i+beamCount+ 1, no1, no2, vsect[sect], 1);
	}
	
}

void Translator::test1()
{
	char* ifcFileName, * ifcSchemaName;
	//ifcFileName = (char*)"Á½é¯èì¼Ü.ifc";
	ifcFileName = (char*)"10²ã¿ò¼Ü.ifc";
	//ifcFileName = (char*)"°²ÖÐ´óÂ¥.ifc";
	ifcSchemaName = (char*)"IFC2X3_TC1.exp";
	translate(ifcFileName,ifcSchemaName);
	fpmtwriter_.addLEMat(1, 2.06e11, 7850, 0.3);
	fpmtwriter_.writeFpmt();
}
//int Translator::getChildIndex(int parentIndex, std::string childname)
//{
//	int instanceCnt = entities_[parentIndex]->instanceCnt;
//	int attrCnt = entities_[parentIndex]->attributeCnt;
//	for (int i = 0; i <instanceCnt;i++ )
//	{
//		if (entities_[parentIndex]->child)
//		{
//			
//		}
//	}
//	return -1;
//}
//wchar_t* Translator::getName(int index)
//{
//	wchar_t* ifcEntityName = 0;
//	engiGetEntityName(entities_[index]->ifcEntity, sdaiUNICODE, (char**)& ifcEntityName);
//	return ifcEntityName;
//}
std::vector<double> Translator::getBeamRectSect(const int& elemInstance)
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
	return roundv(sect);
}
std::vector<double> Translator::getColumnRectSect(const int& elemInstance)
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
	return roundv(sect);
}
double Translator::getBeamRealLength(const int& elemInstance)
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
double Translator::getBeamCalcuLength(const int& elemInstance)
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
double Translator::getColumenLength(const int& elemInstance)
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
	int posInstance;
	sdaiGetAttrBN(itemInstance2, "Position", sdaiINSTANCE, &posInstance);
	int locInstance;
	sdaiGetAttrBN(posInstance, "Location", sdaiINSTANCE, &locInstance);
	int* coordAggr = nullptr;
	double len;
	sdaiGetAttrBN(itemInstance2, "Depth", sdaiREAL, &len);
	return len;
}
double Translator::getH(const int& elemInstance)
{
	int opInstance;
	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	int prtInstance;
	sdaiGetAttrBN(opInstance, "PlacementRelTo", sdaiINSTANCE, &prtInstance);
	int* poAggr;
	sdaiGetAttrBN(prtInstance, "PlacesObject", sdaiAGGR, &poAggr);
	int bsInstance;
	engiGetAggrElement(poAggr, 0, sdaiINSTANCE, &bsInstance);
	double h;
	sdaiGetAttrBN(bsInstance, "Elevation", sdaiREAL, &h);
	return h;
}
std::vector<double> Translator::getBeamCoordinates(const int& elemInstance)
{
	int opInstance;
	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	int rpInstance;
	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
	int locInstance;
	sdaiGetAttrBN(rpInstance, "Location", sdaiINSTANCE, &locInstance);
	int* coordAggr;
	sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &coordAggr);
	double x0, y0, z0;
	engiGetAggrElement(coordAggr, 0, sdaiREAL, &x0);
	engiGetAggrElement(coordAggr, 1, sdaiREAL, &y0);
	engiGetAggrElement(coordAggr, 2, sdaiREAL, &z0);
	int directInstance=0;
	sdaiGetAttrBN(rpInstance, "RefDirection", sdaiINSTANCE, &directInstance);
	std::vector<double> vd = { 1,0,0 };
	if (directInstance)
	{
		int* drAggr;
		sdaiGetAttrBN(directInstance, "DirectionRatios", sdaiAGGR, &drAggr);
		engiGetAggrElement(drAggr, 0, sdaiREAL, &vd[0]);
		engiGetAggrElement(drAggr, 1, sdaiREAL, &vd[1]);
		engiGetAggrElement(drAggr, 2, sdaiREAL, &vd[2]);
	}
	double len = getBeamCalcuLength(elemInstance);
	double h = getH(elemInstance);
	return roundv({ x0,y0, h,x0 + len * vd[0],y0 + len * vd[1], h + len * vd[2] });
}

std::vector<double> Translator::getColumnCoordinates(const int& elemInstance)
{
	int opInstance;
	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	int rpInstance;
	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
	int locInstance;
	sdaiGetAttrBN(rpInstance, "Location", sdaiINSTANCE, &locInstance);
	int* coordAggr;
	sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &coordAggr);
	double x0, y0, z0;
	engiGetAggrElement(coordAggr, 0, sdaiREAL, &x0);
	engiGetAggrElement(coordAggr, 1, sdaiREAL, &y0);
	engiGetAggrElement(coordAggr, 2, sdaiREAL, &z0);
	double len=getColumenLength(elemInstance);
	return roundv({ x0,y0,abs(z0),x0,y0,abs(z0) + len });
}
//int Translator::getIndex(std::string name)
//{
//	for (int k = 0; k < entitiesCnt_; k++)
//	{
//		wchar_t* ifcEntityName = getName(k);
//		size_t	size = 0;
//		if (ifcEntityName) {
//			size += wcslen(ifcEntityName);
//		}
//		else {
//			ASSERT(false);
//		}
//		if (matchEntityName(ifcEntityName, size,name))
//		{
//			return k;
//		}
//	}
//	return -1;
//}
//void Translator::initialSTRUCT_ENTITY()
//{
//	char  * ifcFileName, *ifcSchemaName;
//	ifcFileName = (char*)"Á½é¯èì¼Ü.ifc";
//	//ifcFileName = (char*)"10²ã¿ò¼Ü.ifc";
//	ifcSchemaName = (char*)"IFC2X3_TC1.exp";
//	int model_ = sdaiOpenModelBN(0, ifcFileName, ifcSchemaName);
//	if (model_)
//	{
//		entitiesCnt_ = engiGetEntityCount(model_);
//		entities_ = (STRUCT_ENTITY **) new size_t[entitiesCnt_];
//		int i = 0;
//
//		while (i < entitiesCnt_) {
//			entities_[i] = new_STRUCT_ENTITY(engiGetEntityElement(model_, i));
//			entities_[i]->attributeCnt = engiGetEntityNoArguments(entities_[i]->ifcEntity);
//			entities_[i]->instanceCnt = sdaiGetMemberCount(sdaiGetEntityExtent(model_, entities_[i]->ifcEntity));
//			i++;
//		}
//
//		while (i) {
//			i--;
//			int ifcParentEntity;
//			ifcParentEntity = engiGetEntityParent(entities_[i]->ifcEntity);
//			if (ifcParentEntity) {
//				int	j = 0;
//				while (j < entitiesCnt_) {
//					if (ifcParentEntity == entities_[j]->ifcEntity) {
//						if (entities_[j]->child) {
//							entities_[i]->next = entities_[j]->child;
//						}
//						entities_[j]->child = entities_[i];
//						entities_[i]->parent = entities_[j];
//					}
//					j++;
//				}
//			}
//		}
//	}
//}
//bool Translator::matchEntityName(wchar_t* ifcEntityName,int n,std::string s)
//{
//	if (s.size() != n)return false;
//	for (int i = 0; i < n; i++)
//	{
//		if (ifcEntityName[i] != s[i])
//			return false;
//	}
//	return true;
//}

