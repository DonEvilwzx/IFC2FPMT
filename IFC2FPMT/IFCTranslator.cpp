#include "stdafx.h"
#include "IFCTranslator.h"
#include <string>
#include <set>
#include <fstream>
#include<algorithm>
#include "ifcengine/include/engine.h"
#include "ifcengine/include/ifcengine.h"

using namespace std;
const double ERRORTHICK = 800;
const double ERRORDOUBLE = 1;


IFCTranslator::~IFCTranslator()
{
}

std::vector<double> roundv(std::vector<double> v)
{
	std::vector<double> rev;
	for (auto itr : v)
		rev.push_back(round(itr));
	return rev;
}
bool equalDouble(std::vector<double> v1, std::vector<double> v2)
{
	int n = v2.size();
	for (int i = 0; i < n; i++)
	{
		if (v1[i] - v2[i] > ERRORDOUBLE || v1[i] - v2[i] < -ERRORDOUBLE)return false;
	}
	return true;
}

bool equalThick(std::vector<double> v1, std::vector<double> v2)
{
	int n = v2.size();
	for (int i = 0; i < n; i++)
	{
		if (v1[i] - v2[i] > ERRORTHICK || v1[i] - v2[i] < -ERRORTHICK)return false;
	}
	return true;

}

vector<int> addVector(vector<int> v1, vector<int> v2)
{
	vector<int> v(v1.size());
	for (int i = 0; i < v1.size(); i++)
	{
		v[i] = v1[i] + v2[i];
	}
	return v;
}
//
//std::vector<double> IFCTranslator::getCrossPoint(BeamElement elem1, BeamElement elem2)
//{
//	std::vector<double> e1n1 = elem1.mNode1;
//	std::vector<double> e1n2 = elem1.mNode2;
//	std::vector<double> e2n1 = elem2.mNode1;
//	std::vector<double> e2n2 = elem2.mNode2;
//	if (abs(e1n1[0] - e1n2[0]) < ERRORDOUBLE)
//	{
//		double x = e1n1[0];
//		double y = e2n1[1];
//		if (abs(e2n1[0] - e2n2[0]) < ERRORDOUBLE) return{}; //平行
//		if (abs(x - e2n1[0]) > ERRORTHICK && abs(x - e2n2[0]) > ERRORTHICK && (x - e2n1[0]) * (x - e2n2[0]) > 0) return {};  //交点在e2延长线
//		if (
//			(y - e1n1[1]) * (y - e1n2[1]) < 0
//			)
//		{
//			mCrossPoints.insert({ x,y,e1n1[2] });
//			return { x,y };
//		}
//		else if (abs(y - e1n1[1]) < ERRORTHICK || abs(y - e1n2[1]) < ERRORTHICK)
//		{
//			mCrossPoints.insert({ x,y,e1n1[2] });
//			return { x,y };
//		}
//		else
//			return {};
//	}
//	else
//	{
//		double x = e2n1[0];
//		double y = e1n1[1];
//		if (abs(e2n1[1] - e2n2[1]) < ERRORDOUBLE) return{}; //平行
//		if (abs(y - e2n1[1]) > ERRORTHICK && abs(y - e2n2[1]) > ERRORTHICK && (y - e2n1[1]) * (y - e2n2[1]) > 0) return {};  //交点在e2延长线
//		if (
//			(x - e1n1[0]) * (x - e1n2[0]) < 0
//			)
//		{
//			mCrossPoints.insert({ x,y,e1n1[2] });
//			return { x,y };
//		}
//		else if (abs(x - e1n1[0]) < ERRORTHICK || abs(x - e1n2[0]) < ERRORTHICK)
//		{
//			mCrossPoints.insert({ x,y,e1n1[2] });
//			return { x,y };
//		}
//		else
//			return {};
//	}
//}

void IFCTranslator::test1()
{
	setOutputpath(_T("output.fpmt"));
	//std::basic_string<wchar_t> ifcSchemaName_IFC2x3 = L"IFC2X3_TC1.exp";
	//CString m_path = _T("10层框架.ifc");
	//CString m_path = _T("安中大楼.ifc");
	//CString m_path = _T("教学楼项目.ifc");
	//CString m_path = _T("15#梁柱.0001.ifc");
	//CString m_path = _T("小高层商住楼.0001.ifc");
	//CString m_path = _T("03IDC.ifc");
	//translate(m_path, ifcSchemaName_IFC2x3);

	std::basic_string<wchar_t> ifcSchemaName_IFC4_2 = L"IFC4_ADD2.exp";
	//CString m_path = _T("CSGprimitive.ifc");
	//CString m_path = _T("ExtrudedSolid.ifc");
	//CString m_path = _T("SurfaceModel.ifc");
	//CString m_path = _T("BrepModel.ifc");
	CString m_path = _T("MappedModel.ifc");
	translateNewVersion(m_path, ifcSchemaName_IFC4_2);
}

int IFCTranslator::recordMatTable(std::wstring matname)
{
	int matno;
	if (mMatTable.find(matname) == mMatTable.end())
	{
		int n = mMatTable.size();
		matno = n + 1;
		mMatTable[matname] = matno;
	}
	else
		matno = mMatTable[matname];
	return matno;
}


//int IFCTranslator::recordSectTable(std::vector<int> vsect)
//{
//	int sectno;
//	if (mSectTable.find(vsect) == mSectTable.end())
//	{
//		int n = mSectTable.size()+mSectNameTable.size();
//		sectno = n + 1;
//		mSectTable[vsect] = sectno;
//	}
//	else
//		sectno = mSectTable[vsect];
//	return sectno;
//}

int IFCTranslator::recordSectTable(std::wstring sectname)
{
	int sectno;
	if (mSectNameTable.find(sectname) == mSectNameTable.end())
	{
		int n = mSectNameTable.size();
		sectno = n + 1;
		mSectNameTable[sectname] = sectno;
	}
	else
		sectno = mSectNameTable[sectname];
	return sectno;
}
int IFCTranslator::recordNodeTable(std::vector<int> vnode)
{
	int nodeno;
	if (mNodeTable.find(vnode) == mNodeTable.end())
	{
		int n = mNodeTable.size();
		nodeno = n + 1;
		mNodeTable[vnode] = nodeno;
	}
	else
		nodeno = mNodeTable[vnode];
	return nodeno;
}
void IFCTranslator::parseBuildingStorey(const long long& instance, std::vector<int> relativeCoord)
{

}

void IFCTranslator::recordSolidElement(int n1, int n2, int n3,int n4,int matno)
{
	Solid elem;
	elem.mMatNum = matno;
	elem.mNode1 = n1;
	elem.mNode2 = n2;
	elem.mNode3 = n3;
	elem.mNode4 = n4;
	mSolidElems.push_back(elem);
}
void IFCTranslator::recordBeamElement(int n1, int n2, int matno, int sectno)
{
	Beam elem;
	elem.mMatNum = matno;

	elem.mSectNum = sectno;
	elem.mNode1 = n1;
	elem.mNode2 = n2;
	mBeamElems.push_back(elem);
}

void IFCTranslator::record5SolidElement(int no1, int no2, int no3, int no4, int no5, int no6, int no7, int no8, int matno)
{
	//将长方体分割成5个四面体单元
	recordSolidElement(no1, no2, no3, no5, matno);
	recordSolidElement(no2, no3, no4, no8, matno);
	recordSolidElement(no5, no6, no8, no2, matno);
	recordSolidElement(no5, no7, no8, no3, matno);
	recordSolidElement(no2, no3, no5, no8, matno);
}

void IFCTranslator::parseItems(long long* itemsAggr, int matno, std::vector<int> relativeCoord, std::vector<double> direct)
{
	int itemcnt = sdaiGetMemberCount(itemsAggr);
	for (int i = 0; i < itemcnt; i++)
	{
		long long itemInstance;
		engiGetAggrElement(itemsAggr, i, sdaiINSTANCE, &itemInstance);
		std::string  itemName = engiGetInstanceClassInfoUC(itemInstance);
		//当实体为IfcCSGSolid实体时
		if (itemName == "IFCCSGSOLID")
		{
			long long treeRootInstance;
			sdaiGetAttrBN(itemInstance, "TreeRootExpression", sdaiINSTANCE, &treeRootInstance);
			double xlength, ylength, zlength;
			sdaiGetAttrBN(treeRootInstance, "XLength", sdaiREAL, &xlength);
			sdaiGetAttrBN(treeRootInstance, "YLength", sdaiREAL, &ylength);
			sdaiGetAttrBN(treeRootInstance, "ZLength", sdaiREAL, &zlength);
			long long positionInstance;
			sdaiGetAttrBN(treeRootInstance, "Position", sdaiINSTANCE, &positionInstance);
			vector<int> LocalCoord = getCoordFromLocation(positionInstance);
			vector<int> coord = addVector(relativeCoord, LocalCoord);
			int x1 = coord[0];
			int y1 = coord[1];
			int z1 = coord[2];

			int xlengthInt = (int)xlength;
			int ylengthInt = (int)ylength;
			int zlengthInt = (int)zlength;
			//长方体共8个顶点，最少可分割为5个四面体单元
			vector<int> node1 = { x1,y1,z1 };
			vector<int> node2 = { x1 + xlengthInt,y1,z1 };
			vector<int> node3 = { x1,y1 + ylengthInt,z1 };
			vector<int> node4 = { x1 + xlengthInt,y1 + ylengthInt,z1 };

			vector<int> node5 = { x1,y1,z1 + zlengthInt };
			vector<int> node6 = { x1 + xlengthInt,y1,z1 + zlengthInt };
			vector<int> node7 = { x1,y1 + ylengthInt,z1 + zlengthInt };
			vector<int> node8 = { x1 + xlengthInt,y1 + ylengthInt,z1 + zlengthInt };

			//将节点录入到节点表格
			int no1 = recordNodeTable(node1);
			int no2 = recordNodeTable(node2);
			int no3 = recordNodeTable(node3);
			int no4 = recordNodeTable(node4);
			int no5 = recordNodeTable(node5);
			int no6 = recordNodeTable(node6);
			int no7 = recordNodeTable(node7);
			int no8 = recordNodeTable(node8);

			record5SolidElement(no1, no2, no3, no4, no5, no6, no7, no8, matno);
		}
		//当实体为ExtrudedSolid实体
		else if (itemName == "IFCEXTRUDEDAREASOLID")
		{
			double depth = 0;
			sdaiGetAttrBN(itemInstance, "Depth", sdaiREAL, &depth);
			long long sweptAreaInstance;
			sdaiGetAttrBN(itemInstance, "SweptArea", sdaiINSTANCE, &sweptAreaInstance);


			wchar_t* sectname = 0;
			sdaiGetAttrBN(sweptAreaInstance, "ProfileName", sdaiUNICODE, &sectname);
			int sectno = recordSectTable(sectname);

			long long positionInstance;
			sdaiGetAttrBN(sweptAreaInstance, "Position", sdaiINSTANCE, &positionInstance);
			vector<int> LocalCoord = getCoordFromLocation(positionInstance);
			vector<int> node1 = addVector(relativeCoord, LocalCoord);
			vector<int> node2 = addVector(node1, { (int)(depth * direct[0]),(int)(depth * direct[1]),(int)(depth * direct[2]) });
			int no1 = recordNodeTable(node1);
			int no2 = recordNodeTable(node2);
			recordBeamElement(no1, no2, matno, sectno);
			//std::string  areaName = engiGetInstanceClassInfoUC(sweptAreaInstance);

			//if (areaName == "IFCRECTANGLEPROFILEDEF")
			//{
			//	long long positionInstance;
			//	sdaiGetAttrBN(sweptAreaInstance, "Position", sdaiINSTANCE, &positionInstance);
			//	vector<int> LocalCoord = getCoordFromLocation(positionInstance);
			//	vector<int> coord = addVector(relativeCoord, LocalCoord);
			//	int x1 = coord[0];
			//	int y1 = coord[1];
			//	int z1 = coord[2];


			//	double xdim = 0, ydim = 0;
			//	sdaiGetAttrBN(sweptAreaInstance, "XDim", sdaiREAL, &xdim);
			//	sdaiGetAttrBN(sweptAreaInstance, "YDim", sdaiREAL, &ydim);


			//	vector<int> node1 = { x1,y1,z1 };
			//	vector<int> node2 = { x1 + (int)(depth * direct[0]),y1 + (int)(depth * direct[1]),z1 + (int)(depth * direct[2]) };

			//	int no1 = recordNodeTable(node1);
			//	int no2 = recordNodeTable(node2);
			//	int sectno = recordSectTable({ (int)xdim,(int)ydim });

			//	recordBeamElement(no1, no2, matno, sectno);
			//}
		}
		//当实体为SurfceModel实体
		else if (itemName == "IFCFACEBASEDSURFACEMODEL")
		{
			long long* fbsmFacesAggr;
			sdaiGetAttrBN(itemInstance, "FbsmFaces", sdaiAGGR, &fbsmFacesAggr);
			int connectfacecnt = sdaiGetMemberCount(fbsmFacesAggr);
			if (connectfacecnt == 1)
			{
				long long fbsmFaceInstance;
				engiGetAggrElement(fbsmFacesAggr, 0, sdaiINSTANCE, &fbsmFaceInstance);
				long long* cfsfacesAggr;
				sdaiGetAttrBN(fbsmFaceInstance, "CfsFaces", sdaiAGGR, &cfsfacesAggr);
				int facecnt = sdaiGetMemberCount(cfsfacesAggr);
				//六面体的处理
				if (facecnt == 6)
				{
					set<vector<int>> points;
					for (int i = 0; i < facecnt; i++)
					{
						long long faceInstance;
						engiGetAggrElement(cfsfacesAggr, i, sdaiINSTANCE, &faceInstance);
						long long* boundsAggr;
						sdaiGetAttrBN(faceInstance, "Bounds", sdaiAGGR, &boundsAggr);
						int boundscnt = sdaiGetMemberCount(boundsAggr);
						for (int i = 0; i < boundscnt; i++)
						{
							long long boundsInstance;
							engiGetAggrElement(boundsAggr, i, sdaiINSTANCE, &boundsInstance);
							long long boundInstance;
							sdaiGetAttrBN(boundsInstance, "Bound", sdaiINSTANCE, &boundInstance);
							long long* polygonAggr;
							sdaiGetAttrBN(boundInstance, "Polygon", sdaiAGGR, &polygonAggr);
							int polygoncnt = sdaiGetMemberCount(polygonAggr);
							for (int i = 0; i < polygoncnt; i++)
							{
								long long polygonInstance;
								engiGetAggrElement(polygonAggr, i, sdaiINSTANCE, &polygonInstance);
								long long* coordAggr;
								sdaiGetAttrBN(polygonInstance, "Coordinates", sdaiAGGR, &coordAggr);
								vector<int> localCoord = getCoordFromCoordAggr(coordAggr);
								points.insert(addVector(relativeCoord,localCoord));
							}
						}
					}
					vector <int>vnodes;
					for (auto itr : points)
						vnodes.push_back(recordNodeTable(itr));
					record5SolidElement(vnodes[0], vnodes[1], vnodes[2], vnodes[3], vnodes[4],
						vnodes[5], vnodes[6], vnodes[7], matno);

				}
			}
		}
		else if (itemName == "IFCFACETEDBREP")
		{
			long long outerInstacne;
			sdaiGetAttrBN(itemInstance, "Outer", sdaiINSTANCE, &outerInstacne);
			long long* cfsfacesAggr;
			sdaiGetAttrBN(outerInstacne, "CfsFaces", sdaiAGGR, &cfsfacesAggr);
			int facecnt = sdaiGetMemberCount(cfsfacesAggr);
			//六面体的处理
			if (facecnt == 6)
			{
				set<vector<int>> points;
				for (int i = 0; i < facecnt; i++)
				{
					long long faceInstance;
					engiGetAggrElement(cfsfacesAggr, i, sdaiINSTANCE, &faceInstance);
					long long* boundsAggr;
					sdaiGetAttrBN(faceInstance, "Bounds", sdaiAGGR, &boundsAggr);
					int boundscnt = sdaiGetMemberCount(boundsAggr);
					for (int i = 0; i < boundscnt; i++)
					{
						long long boundsInstance;
						engiGetAggrElement(boundsAggr, i, sdaiINSTANCE, &boundsInstance);
						long long boundInstance;
						sdaiGetAttrBN(boundsInstance, "Bound", sdaiINSTANCE, &boundInstance);
						long long* polygonAggr;
						sdaiGetAttrBN(boundInstance, "Polygon", sdaiAGGR, &polygonAggr);
						int polygoncnt = sdaiGetMemberCount(polygonAggr);
						for (int i = 0; i < polygoncnt; i++)
						{
							long long polygonInstance;
							engiGetAggrElement(polygonAggr, i, sdaiINSTANCE, &polygonInstance);
							long long* coordAggr;
							sdaiGetAttrBN(polygonInstance, "Coordinates", sdaiAGGR, &coordAggr);
							vector<int> localCoord = getCoordFromCoordAggr(coordAggr);
							points.insert(addVector(relativeCoord,localCoord));
						}
					}
				}
				vector <int>vnodes;
				for (auto itr : points)
					vnodes.push_back(recordNodeTable(itr));
				record5SolidElement(vnodes[0], vnodes[1], vnodes[2], vnodes[3], vnodes[4],
					vnodes[5], vnodes[6], vnodes[7], matno);
			}
		}
		else if (itemName == "IFCMAPPEDITEM")
		{
			long long mappingSourceInstance;
			sdaiGetAttrBN(itemInstance, "MappingSource", sdaiINSTANCE, &mappingSourceInstance);
			long long mappedRepreInstance;
			sdaiGetAttrBN(mappingSourceInstance, "MappedRepresentation", sdaiINSTANCE, &mappedRepreInstance);
			long long* itemsAggr;
			sdaiGetAttrBN(mappedRepreInstance, "Items", sdaiAGGR, &itemsAggr);
			parseItems(itemsAggr,matno,relativeCoord,direct);
		}
	}
}

void IFCTranslator::parseElement(const long long& instance, std::vector<int> relativeCoord,bool isBeam)
{
	vector<double> direct = { 0,0,1 };
	if (isBeam)
		direct = { 1,0,0 };
	//获取构件端点坐标
	vector<int> localCoord = getCoordinate(instance);
	
	//获取坐标方向向量
	vector<double> temp = getDirect(instance);
	if (temp.size() > 0)
		direct = temp;
	//获取材料名并录入到材料表
	wstring matname = getMatName(instance);
	int matno=recordMatTable(matname);
	long long repreInstance;
	sdaiGetAttrBN(instance, "Representation", sdaiINSTANCE, &repreInstance);
	long long* repreAggr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	long long shaperepInstance;
	engiGetAggrElement(repreAggr, 0, sdaiINSTANCE, &shaperepInstance);
	long long* itemsAggr = nullptr;
	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
	parseItems(itemsAggr, matno, addVector(relativeCoord,localCoord), direct);
}

//void IFCTranslator::parseSweptSolid(const long long& itemInstance, int matno, std::vector<int> relativeCoord,std::vector<double> direct)
//{
//	double depth = 0;
//	sdaiGetAttrBN(itemInstance, "Depth", sdaiREAL, &depth);
//	long long sweptAreaInstance;
//	sdaiGetAttrBN(itemInstance, "SweptArea", sdaiINSTANCE, &sweptAreaInstance);
//
//	std::string  areaName = engiGetInstanceClassInfoUC(sweptAreaInstance);
//	if (areaName == "IFCRECTANGLEPROFILEDEF")
//	{
//		long long positionInstance;
//		sdaiGetAttrBN(sweptAreaInstance, "Position", sdaiINSTANCE, &positionInstance);
//		vector<int> AreaCoord = getCoordFromLocation(positionInstance);
//
//		int x1 = relativeCoord[0]  + AreaCoord[0];
//		int y1 = relativeCoord[1]  + AreaCoord[1];
//		int z1 = relativeCoord[2]  + AreaCoord[2];
//
//
//		double xdim = 0, ydim = 0;
//		sdaiGetAttrBN(sweptAreaInstance, "XDim", sdaiREAL, &xdim);
//		sdaiGetAttrBN(sweptAreaInstance, "YDim", sdaiREAL, &ydim);
//
//
//		vector<int> node1 = { x1,y1,z1 };
//		vector<int> node2 = { x1 + (int)(depth * direct[0]),y1 + (int)(depth * direct[1]),z1 + (int)(depth * direct[2]) };
//
//		int no1 = recordNodeTable(node1);
//		int no2 = recordNodeTable(node2);
//		int sectno = recordSectTable({ (int)xdim,(int)ydim });
//
//		recordBeamElement(no1, no2, matno, sectno);
//	}
//}
std::vector<int> IFCTranslator::getCoordFromCoordAggr( long long*  coordAggr)
{
	double x0 = 0, y0 = 0, z0 = 0;
	engiGetAggrElement(coordAggr, 0, sdaiREAL, &x0);
	engiGetAggrElement(coordAggr, 1, sdaiREAL, &y0);
	engiGetAggrElement(coordAggr, 2, sdaiREAL, &z0);
	//double len = getColumenLength(elemInstance);
	return { (int)x0,(int)y0 ,(int)z0 };
}

std::vector<int> IFCTranslator::getCoordFromLocation(const long long& instance)
{
	if (instance < 0)return { 0,0,0 };
	long long locInstance;
	sdaiGetAttrBN(instance, "Location", sdaiINSTANCE, &locInstance);
	long long* coordAggr = nullptr;
	sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &coordAggr);
	return getCoordFromCoordAggr(coordAggr);
}


std::vector<double>  IFCTranslator::getDirect(const long long& instance)
{
	long long opInstance;
	sdaiGetAttrBN(instance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	long long rpInstance;
	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
	long long directInstance = 0;
	sdaiGetAttrBN(rpInstance, "RefDirection", sdaiINSTANCE, &directInstance);
	if (directInstance)
	{
		double x, y, z;
		long long* drAggr;
		sdaiGetAttrBN(directInstance, "DirectionRatios", sdaiAGGR, &drAggr);
		engiGetAggrElement(drAggr, 0, sdaiREAL, &x);
		engiGetAggrElement(drAggr, 1, sdaiREAL, &y);
		engiGetAggrElement(drAggr, 2, sdaiREAL, &z);
		return { x,y,z };
	}
	else
		return {};
}
std::vector<int> IFCTranslator::getCoordinate(const long long & instance)
{
	long long opInstance;
	sdaiGetAttrBN(instance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	long long rpInstance;
	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
	long long locInstance;
	return getCoordFromLocation(locInstance);
}

void IFCTranslator::translateNewVersion(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName)
{
	wchar_t fileName[512];
	memcpy(fileName, ifcFileName, ifcFileName.GetLength() * sizeof(wchar_t));
	fileName[ifcFileName.GetLength()] = 0;
	long long model = 0;
	model = sdaiOpenModelBNUnicode(0, (char*)fileName, (char*)ifcSchemaName.c_str());
	long long* buildingAggr = sdaiGetEntityExtentBN(model, "IFCBUILDING");
	int buildingcnt = sdaiGetMemberCount(buildingAggr);
	for (int i = 0; i < buildingcnt; i++)
	{
		long long buidingInstance;
		engiGetAggrElement(buildingAggr, i, sdaiINSTANCE, &buidingInstance);
		vector<int> coord = getCoordinate(buidingInstance);
		long long* containElemAggr;
		sdaiGetAttrBN(buidingInstance, "ContainsElements", sdaiAGGR, &containElemAggr);
		long long containElemInstance;
		engiGetAggrElement(containElemAggr, 0, sdaiINSTANCE, &containElemInstance);
		long long* relatedElemAggr;
		sdaiGetAttrBN(containElemInstance, "RelatedElements", sdaiAGGR, &relatedElemAggr);
		int count = sdaiGetMemberCount(relatedElemAggr);
		for (int i = 0; i < count; i++)
		{
			long long instance;
			engiGetAggrElement(relatedElemAggr, i, sdaiINSTANCE, &instance);
			string elemtype = engiGetInstanceClassInfoUC(instance);
			if (elemtype == "IFCBUILDINGSTOREY")
				parseBuildingStorey(instance, coord);
			else if (elemtype == "IFCBUILDINGELEMENTPROXY")
				parseElement(instance, coord,false);
		}
	}
	writeFPMT();

}

void IFCTranslator::writeFPMT()
{
	//建立新表，使键值为编号，属性值对应材料、截面、坐标参数，以使得编号值能够得到排序
	map<int, std::vector<int>> nodeTable;
	map<int, std::wstring> sectTable;
	map<int, std::wstring> matTable;
	for (auto itr : mNodeTable)
		nodeTable[itr.second] = itr.first;
	for (auto itr : mMatTable)
		matTable[itr.second] = itr.first;
	for (auto itr : mSectNameTable)
		sectTable[itr.second] = itr.first;

	for (auto itr : matTable)
		mFPMTWriter.addLEMat(itr.first);
	for (auto itr : sectTable)
	{
		mFPMTWriter.addRectSect(itr.first);
	}
	for (auto itr : nodeTable)
		mFPMTWriter.addNode(itr.first, itr.second[0] / 1000.0, itr.second[1] / 1000.0, itr.second[2] / 1000.0);
	int elemno = 1;
	for (auto itr : mBeamElems)
		mFPMTWriter.addBeamElem(elemno++, itr.mNode1, itr.mNode2, itr.mSectNum, itr.mMatNum);

	for (auto itr : mSolidElems)
		mFPMTWriter.addSolidElem(elemno++, itr.mNode1, itr.mNode2, itr.mNode3, itr.mNode4, itr.mMatNum);
	mFPMTWriter.writeFPMT(outputpath_);
}


//void IFCTranslator::translateOldVersion(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName)
//{
//	wchar_t fileName[512];
//	memcpy(fileName, ifcFileName, ifcFileName.GetLength() * sizeof(wchar_t));
//	fileName[ifcFileName.GetLength()] = 0;
//	long long model = 0;
//	model = sdaiOpenModelBNUnicode(0, (char*)fileName, (char*)ifcSchemaName.c_str());
//	long long* buildingstoreyAggr = sdaiGetEntityExtentBN(model, "IFCBUILDINGSTOREY");
//	long long* beamAggr = sdaiGetEntityExtentBN(model, "IFCBEAM");
//	long long* columnAggr = sdaiGetEntityExtentBN(model, "IFCCOLUMN");
//	int beamcnt = sdaiGetMemberCount(beamAggr);
//	int cocnt = sdaiGetMemberCount(columnAggr);
//	int bscnt = sdaiGetMemberCount(buildingstoreyAggr);
//
//	//梁
//	for (int i = 0; i < bscnt; i++)
//	{
//		long long bsInstance;
//		engiGetAggrElement(buildingstoreyAggr, i, sdaiINSTANCE, &bsInstance);
//		std::vector<BeamElement> vbeam = getBeams(bsInstance);
//		setBeams(vbeam);
//	}
//
//	//根据梁单元对节点编号
//	for (auto elem : mBeamElems)
//	{
//		std::vector<double> node1 = elem.mNode1;
//		if (mNodeTable.find(node1) == mNodeTable.end())
//		{
//			int n = mNodeTable.size() + 1;
//			mNodeTable.insert({ node1,n });
//		}
//
//		std::vector<double> node2 = elem.mNode2;
//		if (mNodeTable.find(node2) == mNodeTable.end())
//		{
//			int n = mNodeTable.size() + 1;
//			mNodeTable.insert({ node2,n });
//		}
//	}
//
//
//	//柱
//	for (int i = 0; i < bscnt - 1; i++)
//	{
//		long long bsInstance1;
//		long long bsInstance2;
//		engiGetAggrElement(buildingstoreyAggr, i, sdaiINSTANCE, &bsInstance1);
//		engiGetAggrElement(buildingstoreyAggr, i + 1, sdaiINSTANCE, &bsInstance2);
//		double elevation1, elevation2;
//		sdaiGetAttrBN(bsInstance1, "Elevation", sdaiREAL, &elevation1);
//		sdaiGetAttrBN(bsInstance2, "Elevation", sdaiREAL, &elevation2);
//		long long* containElemAggr;
//		sdaiGetAttrBN(bsInstance1, "ContainsElements", sdaiAGGR, &containElemAggr);
//		long long containElemInstance;
//		engiGetAggrElement(containElemAggr, 0, sdaiINSTANCE, &containElemInstance);
//		long long* relatedElemAggr;
//		sdaiGetAttrBN(containElemInstance, "RelatedElements", sdaiAGGR, &relatedElemAggr);
//		int membercount = sdaiGetMemberCount(relatedElemAggr);
//
//		for (int j = 0; j < membercount; j++)
//		{
//			long long instance;
//			engiGetAggrElement(relatedElemAggr, j, sdaiINSTANCE, &instance);
//			std::string elemtype = engiGetInstanceClassInfoUC(instance);
//			if (elemtype != "IFCCOLUMN")continue;
//			std::wstring matname;
//			matname = getMatName(instance);
//			int matno;
//			if (mMatTable.find(matname) == mMatTable.end())
//			{
//				int n = mMatTable.size();
//				matno = n + 1;
//				mMatTable[matname] = matno;
//			}
//			else
//				matno = mMatTable[matname];
//
//			std::wstring sectname;
//			sectname = getSectName(instance);
//			int sectno;
//			if (mSectNameTable.find(sectname) == mSectNameTable.end())
//			{
//				int n = mSectNameTable.size();
//				sectno = n + 1;
//				mSectNameTable[sectname] = sectno;
//			}
//			else
//				sectno = mSectNameTable[sectname];
//
//			std::vector<double> coord;
//			coord = getColumnCoordinates(instance);
//			coord[2] += round(elevation2);
//			std::vector<double> node1, node2;
//			//for (auto itr : vnode_)
//			//{
//			//	if (equalThick(itr.first, coord))
//			//	{
//			//		node2 = itr.first;
//			//		node1 = node2;
//			//		node1[2] = round(elevation1);
//			//		break;
//			//	}
//			//}
//			//
//
//			for (auto itr : mCrossPoints)
//			{
//				if (equalThick(itr, coord))
//				{
//					node2 = itr;
//					node1 = node2;
//					node1[2] = round(elevation1);
//					break;
//				}
//			}
//
//			//当楼层为第0层时，添加底层节点
//			if ((i == 0 || i == 1) && node1.size() > 0)
//			{
//				if (mNodeTable.find(node1) == mNodeTable.end())
//				{
//					int n = mNodeTable.size() + 1;
//					mNodeTable[node1] = n;
//				}
//			}
//			if (node1.size() > 0)
//			{
//				BeamElement elem;
//				elem.mMatNum = matno;
//				elem.mSectNum = sectno;
//				elem.mNode1 = node1;
//				elem.mNode2 = node2;
//				mBeamElems.push_back(elem);
//			}
//		}
//	}
//
//
//
//}


//
//std::vector<BeamElement> IFCTranslator::getBeams(const long long& bsInstance)
//{
//	double elevation;
//	sdaiGetAttrBN(bsInstance, "Elevation", sdaiREAL, &elevation);
//	long long* containElemAggr;
//	sdaiGetAttrBN(bsInstance, "ContainsElements", sdaiAGGR, &containElemAggr);
//	long long containElemInstance;
//	engiGetAggrElement(containElemAggr, 0, sdaiINSTANCE, &containElemInstance);
//	long long* relatedElemAggr;
//	sdaiGetAttrBN(containElemInstance, "RelatedElements", sdaiAGGR, &relatedElemAggr);
//	int count = sdaiGetMemberCount(relatedElemAggr);
//	std::vector<BeamElement> rev;
//	for (int i = 0; i < count; i++)
//	{
//		long long instance;
//		engiGetAggrElement(relatedElemAggr, i, sdaiINSTANCE, &instance);
//		std::string elemtype = engiGetInstanceClassInfoUC(instance);
//		if (elemtype != "IFCBEAM")continue;
//		std::wstring matname;
//		matname = getMatName(instance);
//		int matno;
//		if (mMatTable.find(matname) == mMatTable.end())
//		{
//			int n = mMatTable.size();
//			matno = n + 1;
//			mMatTable[matname] = matno;
//		}
//		else
//			matno = mMatTable[matname];
//
//		std::wstring sectname;
//		sectname = getSectName(instance);
//		int sectno;
//		if (mSectNameTable.find(sectname) == mSectNameTable.end())
//		{
//			int n = mSectNameTable.size();
//			sectno = n + 1;
//			mSectNameTable[sectname] = sectno;
//		}
//		else
//			sectno = mSectNameTable[sectname];
//
//		std::vector<double> coord;
//		coord = getBeamCoordinates(instance);
//		coord[2] += round(elevation);
//		coord[5] += round(elevation);
//
//		std::vector<double> node1 = { coord[0],coord[1],coord[2] };
//		std::vector<double> node2 = { coord[3],coord[4],coord[5] };
//
//		BeamElement elem;
//		elem.mMatNum = matno;
//		elem.mSectNum = sectno;
//		elem.mNode1 = node1;
//		elem.mNode2 = node2;
//		rev.push_back(elem);
//	}
//	splitBeams(rev);
//	return rev;
//}
//

//
//std::vector<double> IFCTranslator::getBeamRectSect(const long long& elemInstance)
//{
//	long long repreInstance = 0;
//	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
//	long long* repreAggr = nullptr;
//	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
//	long long shaperepInstance;
//	engiGetAggrElement(repreAggr, 1, sdaiINSTANCE, &shaperepInstance);
//	long long* itemsAggr;
//	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
//	long long itemInstance;
//	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
//	std::vector<double> sect(2);
//	long long areaInstance;
//	sdaiGetAttrBN(itemInstance, "SweptArea", sdaiINSTANCE, &areaInstance);
//	sdaiGetAttrBN(areaInstance, "YDim", sdaiREAL, &sect[0]);
//	sdaiGetAttrBN(areaInstance, "XDim", sdaiREAL, &sect[1]);
//	return roundv(sect);
//}
//
//std::vector<double> IFCTranslator::getColumnRectSect(const long long& elemInstance)
//{
//	long long repreInstance;
//	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
//	long long* repreAggr;
//	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
//	long long shaperepInstance;
//	engiGetAggrElement(repreAggr, 0, sdaiINSTANCE, &shaperepInstance);
//	long long* itemsAggr = nullptr;
//	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
//	long long itemInstance;
//	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
//	std::vector<double> sect = { 200,200 };
//	long long msInstance;
//	sdaiGetAttrBN(itemInstance, "MappingSource", sdaiINSTANCE, &msInstance);
//	long long mpInstance;
//	sdaiGetAttrBN(msInstance, "MappedRepresentation", sdaiINSTANCE, &mpInstance);
//	long long* itemsAggr2 = nullptr;
//	sdaiGetAttrBN(mpInstance, "Items", sdaiAGGR, &itemsAggr2);
//	long long itemInstance2;
//	engiGetAggrElement(itemsAggr2, 0, sdaiINSTANCE, &itemInstance2);
//	long long areaInstance;
//	sdaiGetAttrBN(itemInstance2, "SweptArea", sdaiINSTANCE, &areaInstance);
//	if (areaInstance < 0) return {};
//	sdaiGetAttrBN(areaInstance, "YDim", sdaiREAL, &sect[0]);
//	sdaiGetAttrBN(areaInstance, "XDim", sdaiREAL, &sect[1]);
//	return roundv(sect);
//}
//
//void IFCTranslator::setElem2Fpmt(long long** aggr)
//{
//	int count = sdaiGetMemberCount(*aggr);
//	for (int i = 0; i < count; i++)
//	{
//		long long instance;
//		engiGetAggrElement(*aggr, i, sdaiINSTANCE, &instance);
//		std::string elemtype = engiGetInstanceClassInfoUC(instance);
//		if (elemtype != "IFCCOLUMN" || elemtype != "IFCBEAM")continue;
//		std::vector<double> sect;
//		if (elemtype == "IFCBEAM")
//			sect = getBeamRectSect(instance);
//		else if (elemtype == "IFCCOLUMN")
//		{
//			sect = getColumnRectSect(instance);
//			if (sect.size() == 0)
//				sect = getSect(instance);
//		}
//		//sect = getSect(instance);
//		if (vsect_.find(sect) == vsect_.end())
//		{
//			int n = vsect_.size();
//			vsect_[sect] = n + 1;
//			fpmtwriter_.addRectSect(n + 1, sect[0] / 1000.0, sect[1] / 1000.0);
//		}
//		std::vector<double> mat;
//		mat = getMat(instance);
//		bool isFind = false;
//		for (auto itr : vmat_)
//		{
//			if (itr.first[0] == mat[0] && itr.first[1] == mat[1] && itr.first[2] - mat[2] < 10e-6)
//			{
//				isFind = true;
//				break;
//			}
//		}
//		if (!isFind)
//		{
//			int n = vmat_.size();
//			vmat_[mat] = n + 1;
//			fpmtwriter_.addLEMat(n + 1, mat[0], mat[1], mat[2]);
//		}
//		std::vector<double> coord;
//		if (elemtype == "IFCBEAM")
//			coord = getBeamCoordinates(instance);
//		else if (elemtype == "IFCCOLUMN")
//			coord = getColumnCoordinates(instance);
//		std::vector<double> node1 = { coord[0],coord[1],coord[2] };
//		std::vector<double> node2 = { coord[3],coord[4],coord[5] };
//		int no1, no2;
//		if (vnode_.find(node1) == vnode_.end())
//		{
//			int n = vnode_.size();
//			vnode_[node1] = n + 1;
//			fpmtwriter_.addNode(n + 1, coord[0] / 1000.0, coord[1] / 1000.0, coord[2] / 1000.0);
//			no1 = n + 1;
//		}
//		else
//			no1 = vnode_[node1];
//
//		if (vnode_.find(node2) == vnode_.end())
//		{
//			int n = vnode_.size();
//			vnode_[node2] = n + 1;
//			fpmtwriter_.addNode(n + 1, coord[3] / 1000.0, coord[4] / 1000.0, coord[5] / 1000.0);
//			no2 = n + 1;
//		}
//		else
//			no2 = vnode_[node2];
//		fpmtwriter_.addBeamElem(++elemcount_, no1, no2, vsect_[sect], 1);
//	}
//}

//double IFCTranslator::getBeamRealLength(const long long& elemInstance)
//{
//	long long repreInstance;
//	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
//	long long* repreAggr;
//	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
//	long long shaperepInstance;
//	engiGetAggrElement(repreAggr, 1, sdaiINSTANCE, &shaperepInstance);
//	long long* itemsAggr = nullptr;
//	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
//	long long itemInstance;
//	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
//	double elementlength = 0;
//	sdaiGetAttrBN(itemInstance, "Depth", sdaiREAL, &elementlength);
//	return elementlength;
//}
////double IFCTranslator::getBeamCalcuLength(const long long& elemInstance)
//{
//	long long repreInstance;
//	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
//	long long* repreAggr;
//	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
//	long long shaperepInstance;
//	engiGetAggrElement(repreAggr, 0, sdaiINSTANCE, &shaperepInstance);
//	long long* itemsAggr = nullptr;
//	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
//	long long itemInstance;
//	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
//	long long* pointAggr;
//	sdaiGetAttrBN(itemInstance, "Points", sdaiAGGR, &pointAggr);
//	long long point0Instance, point1Instance;
//	engiGetAggrElement(pointAggr, 0, sdaiINSTANCE, &point0Instance);
//	engiGetAggrElement(pointAggr, 1, sdaiINSTANCE, &point1Instance);
//	long long* coordinates0Aggr;
//	long long* coordinates1Aggr;
//	sdaiGetAttrBN(point0Instance, "Coordinates", sdaiAGGR, &coordinates0Aggr);
//	sdaiGetAttrBN(point1Instance, "Coordinates", sdaiAGGR, &coordinates1Aggr);
//	double x0, y0, x1, y1;
//	engiGetAggrElement(coordinates0Aggr, 0, sdaiREAL, &x0);
//	engiGetAggrElement(coordinates0Aggr, 1, sdaiREAL, &y0);
//	engiGetAggrElement(coordinates1Aggr, 0, sdaiREAL, &x1);
//	engiGetAggrElement(coordinates1Aggr, 1, sdaiREAL, &y1);
//	return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
//}
//double IFCTranslator::getColumenLength(const long long& elemInstance)
//{
//	long long repreInstance;
//	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
//	long long* repreAggr;
//	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
//	long long shaperepInstance;
//	engiGetAggrElement(repreAggr, 0, sdaiINSTANCE, &shaperepInstance);
//	long long* itemsAggr = nullptr;
//	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
//	long long itemInstance;
//	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
//	std::vector<double> sect(2);
//	long long msInstance;
//	sdaiGetAttrBN(itemInstance, "MappingSource", sdaiINSTANCE, &msInstance);
//	long long mpInstance;
//	sdaiGetAttrBN(msInstance, "MappedRepresentation", sdaiINSTANCE, &mpInstance);
//	long long* itemsAggr2 = nullptr;
//	sdaiGetAttrBN(mpInstance, "Items", sdaiAGGR, &itemsAggr2);
//	long long itemInstance2;
//	engiGetAggrElement(itemsAggr2, 0, sdaiINSTANCE, &itemInstance2);
//	long long posInstance;
//	sdaiGetAttrBN(itemInstance2, "Position", sdaiINSTANCE, &posInstance);
//	long long locInstance;
//	sdaiGetAttrBN(posInstance, "Location", sdaiINSTANCE, &locInstance);
//	long long* coordAggr = nullptr;
//	double len;
//	sdaiGetAttrBN(itemInstance2, "Depth", sdaiREAL, &len);
//	return len;
//}
//double IFCTranslator::getH(const long long& elemInstance)
//{
//	long long opInstance;
//	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
//	long long prtInstance;
//	sdaiGetAttrBN(opInstance, "PlacementRelTo", sdaiINSTANCE, &prtInstance);
//	long long* poAggr;
//	sdaiGetAttrBN(prtInstance, "PlacesObject", sdaiAGGR, &poAggr);
//	long long bsInstance;
//	engiGetAggrElement(poAggr, 0, sdaiINSTANCE, &bsInstance);
//	double h;
//	sdaiGetAttrBN(bsInstance, "Elevation", sdaiREAL, &h);
//	return h;
//}
//std::vector<double> IFCTranslator::getBeamCoordinates(const long long& elemInstance)
//{
//	long long opInstance;
//	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
//	long long rpInstance;
//	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
//	long long locInstance;
//	sdaiGetAttrBN(rpInstance, "Location", sdaiINSTANCE, &locInstance);
//	long long* coordAggr;
//	sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &coordAggr);
//	double x0, y0, z0;
//	engiGetAggrElement(coordAggr, 0, sdaiREAL, &x0);
//	engiGetAggrElement(coordAggr, 1, sdaiREAL, &y0);
//	engiGetAggrElement(coordAggr, 2, sdaiREAL, &z0);
//	long long directInstance = 0;
//	sdaiGetAttrBN(rpInstance, "RefDirection", sdaiINSTANCE, &directInstance);
//	std::vector<double> vd = { 1,0,0 };
//	if (directInstance)
//	{
//		long long* drAggr;
//		sdaiGetAttrBN(directInstance, "DirectionRatios", sdaiAGGR, &drAggr);
//		engiGetAggrElement(drAggr, 0, sdaiREAL, &vd[0]);
//		engiGetAggrElement(drAggr, 1, sdaiREAL, &vd[1]);
//		engiGetAggrElement(drAggr, 2, sdaiREAL, &vd[2]);
//	}
//	double len = getBeamCalcuLength(elemInstance);
//	//	double h = getH(elemInstance);
//	return roundv({ x0,y0, 0,x0 + len * vd[0],y0 + len * vd[1], len * vd[2] });
//}
//
//std::vector<double> IFCTranslator::getColumnCoordinates(const long long& elemInstance)
//{
//	long long opInstance;
//	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
//	long long rpInstance;
//	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
//	long long locInstance;
//	sdaiGetAttrBN(rpInstance, "Location", sdaiINSTANCE, &locInstance);
//	long long* coordAggr = nullptr;
//	sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &coordAggr);
//	double x0, y0, z0;
//	engiGetAggrElement(coordAggr, 0, sdaiREAL, &x0);
//	engiGetAggrElement(coordAggr, 1, sdaiREAL, &y0);
//	engiGetAggrElement(coordAggr, 2, sdaiREAL, &z0);
//	//double len = getColumenLength(elemInstance);
//	return roundv({ x0,y0 ,0 });
//}


void IFCTranslator::setOutputpath(const CString& opath)
{
	memcpy(outputpath_, opath, opath.GetLength() * sizeof(wchar_t));
	outputpath_[opath.GetLength()] = 0;
}

std::wstring IFCTranslator::getMatName(const long long& elemInstance)
{
	long long* associationAggr = nullptr;
	sdaiGetAttrBN(elemInstance, "HasAssociations", sdaiAGGR, &associationAggr);
	if(associationAggr==nullptr)
		return  L"empty";
	long long associationInstance;
	engiGetAggrElement(associationAggr, 0, sdaiINSTANCE, &associationInstance);
	if (associationInstance < 0)
		return  L"empty";
	long long relatingMaterialInstance;
	sdaiGetAttrBN(associationInstance, "RelatingMaterial", sdaiINSTANCE, &relatingMaterialInstance);
	if (relatingMaterialInstance < 0)
		return  L"empty";
	wchar_t* matname = 0;
	sdaiGetAttrBN(relatingMaterialInstance, "Name", sdaiUNICODE, &matname);
	return matname;
}

//std::wstring IFCTranslator::getSectName(const long long& elemInstance)
//{
//	wchar_t* objecttype;
//	sdaiGetAttrBN(elemInstance, "ObjectType", sdaiUNICODE, &objecttype);
//	return objecttype;
//}

//void IFCTranslator::splitBeams(std::vector<BeamElement>& v)
//{
//	for (int i = 0; i < v.size(); i++)
//		splitBeam(v[i], v);
//}
bool unique(std::pair<double, double> p1, std::pair<double, double> p2)
{
	if (abs(p1.first - p2.first) < ERRORDOUBLE && abs(p1.second - p2.second) < ERRORDOUBLE)
		return true;
	else
		return false;
}
bool cmp(std::pair<double, double> p1, std::pair<double, double> p2)
{
	if (abs(p1.first - p2.first) < ERRORDOUBLE)
		return p1.second < p2.second;

	return p1.first < p2.first;
}
//void IFCTranslator::splitBeam(BeamElement& e, std::vector<BeamElement> v)
//{
//	e.mElemCrossPoints.push_back({ e.mNode1[0],e.mNode1[1] });
//	e.mElemCrossPoints.push_back({ e.mNode2[0],e.mNode2[1] });
//	for (auto itr : v)
//	{
//		std::vector<double> crosspoint;
//		int n = e.mElemCrossPoints.size();
//		crosspoint = getCrossPoint(e, itr);
//		if (crosspoint.size() > 0)
//		{
//			e.mElemCrossPoints.push_back({ crosspoint[0],crosspoint[1] });
//		}
//	}
//
//	sort(e.mElemCrossPoints.begin(), e.mElemCrossPoints.end(), cmp);
//	auto new_end = unique(e.mElemCrossPoints.begin(), e.mElemCrossPoints.end(), unique);
//	e.mElemCrossPoints.erase(new_end, e.mElemCrossPoints.end());
//	if (equalThick({ e.mElemCrossPoints[0].first,e.mElemCrossPoints[0].second }, { e.mElemCrossPoints[1].first,e.mElemCrossPoints[1].second }))
//	{
//		e.mElemCrossPoints.erase(e.mElemCrossPoints.begin() + 1);
//	}
//	int n = e.mElemCrossPoints.size();
//	if (equalThick({ e.mElemCrossPoints[n - 1].first,e.mElemCrossPoints[n - 1].second }, { e.mElemCrossPoints[n - 2].first,e.mElemCrossPoints[n - 2].second }))
//	{
//		e.mElemCrossPoints.erase(e.mElemCrossPoints.begin() + n - 2);
//	}
//}
//void IFCTranslator::setBeams(const std::vector<BeamElement>& velem)
//{
//	for (auto elem : velem)
//	{
//		for (int i = 0; i < elem.mElemCrossPoints.size() - 1; i++)
//		{
//			BeamElement newelem;
//			newelem.mMatNum = elem.mMatNum;
//			newelem.mSectNum = elem.mSectNum;
//			newelem.mNode1 = { elem.mElemCrossPoints[i].first,elem.mElemCrossPoints[i].second,elem.mNode1[2] };
//			newelem.mNode2 = { elem.mElemCrossPoints[i + 1].first,elem.mElemCrossPoints[i + 1].second,elem.mNode1[2] };
//			mBeamElems.push_back(newelem);
//		}
//	}
//}
//
