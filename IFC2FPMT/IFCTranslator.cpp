#include "stdafx.h"
#include "IFCTranslator.h"
#include <string>
#include <set>
#include <fstream>
#include<algorithm>
#include "ifcengine/include/engine.h"
#include "ifcengine/include/ifcengine.h"

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
		if (v1[i] - v2[i] > ERRORTHICK||v1[i]-v2[i]<-ERRORTHICK)return false;
	}
	return true;
}

std::vector<double> IFCTranslator::getCrossPoint(Element elem1, Element elem2)
{
	std::vector<double> e1n1 = elem1.node1;
	std::vector<double> e1n2 = elem1.node2;
	std::vector<double> e2n1 = elem2.node1;
	std::vector<double> e2n2 = elem2.node2;
	if (abs(e1n1[0] - e1n2[0]) < ERRORDOUBLE)
	{
		double x = e1n1[0];
		double y = e2n1[1];
		if (abs(e2n1[0] - e2n2[0]) < ERRORDOUBLE) return{}; //平行
		if (abs(x - e2n1[0]) > ERRORTHICK && abs(x - e2n2[0]) > ERRORTHICK && (x - e2n1[0]) * (x - e2n2[0]) > 0) return {};  //交点在e2延长线
		if (
			(y - e1n1[1]) * (y - e1n2[1]) < 0
			)
		{
			vcross_.insert({ x,y,e1n1[2] });
			return { x,y };
		}
		else if (abs(y - e1n1[1]) < ERRORTHICK || abs(y - e1n2[1]) < ERRORTHICK)
		{
			vcross_.insert({ x,y,e1n1[2] });
			return { x,y };
		}
		else
			return {};
	}
	else
	{
		double x = e2n1[0];
		double y = e1n1[1];
		if (abs(e2n1[1] - e2n2[1]) < ERRORDOUBLE) return{}; //平行
		if (abs(y - e2n1[1]) > ERRORTHICK && abs(y - e2n2[1]) > ERRORTHICK && (y - e2n1[1]) * (y - e2n2[1]) > 0) return {};  //交点在e2延长线
		if (
			(x - e1n1[0]) * (x - e1n2[0]) < 0
			)
		{
			vcross_.insert({ x,y,e1n1[2] });
			return { x,y };
		}
		else if (abs(x - e1n1[0]) < ERRORTHICK || abs(x - e1n2[0]) < ERRORTHICK)
		{
			vcross_.insert({ x,y,e1n1[2] });
			return { x,y };
		}
		else
			return {};
	}
}

void IFCTranslator::test1()
{
	std::basic_string<wchar_t> ifcSchemaName_IFC2x3 = L"IFC2X3_TC1.exp";
	setOutputpath(_T("output.fpmt"));
	//CString m_path = _T("10层框架.ifc");
	//CString m_path = _T("安中大楼.ifc");
	CString m_path = _T("教学楼项目.ifc");
	//CString m_path = _T("15#梁柱.0001.ifc");
	//CString m_path = _T("小高层商住楼.0001.ifc");
	translate(m_path, ifcSchemaName_IFC2x3);
}

//
//void IFCTranslator::splitBeam(Element e,const std::vector<Element>& v)
//{
//	std::vector<double> crosspoint;
//	int elemNum2=0;
//	Element e2;
//	for (auto itr : mElemTable)
//	{
//		if (itr.first == elemNum)continue;
//		crosspoint = getCrossPoint(e, itr.second);
//		if (crosspoint.size() > 0)
//		{
//			elemNum2 = itr.first;
//			e2 = itr.second;
//			/*两个单元不相交或者两个单元端点相交*/
//			if (crosspoint.empty() || (equalDouble(mNodeTable[e.nodeNum1], crosspoint) || equalDouble(mNodeTable[e.nodeNum2], crosspoint)
//				&& equalDouble(mNodeTable[e2.nodeNum1], crosspoint) || equalDouble(mNodeTable[e2.nodeNum2], crosspoint)))continue;
//			/*两个单元相交生成4个单元的情况*/
//			else if (!(equalThick(mNodeTable[e.nodeNum1], crosspoint) && !equalThick(mNodeTable[e.nodeNum2], crosspoint)
//				&& !equalThick(mNodeTable[e2.nodeNum1], crosspoint) && !equalThick(mNodeTable[e2.nodeNum2], crosspoint)))
//			{
//				int newnodeNum = mNodeTable.size() + 1;
//				mNodeTable[newnodeNum] = crosspoint;
//				splitByNode(newnodeNum, elemNum, e);
//				splitByNode(newnodeNum, elemNum2, e2);
//			}
//			/*两个单元相交生成3个单元的情况*/
//			else
//			{
//				int splitnodeNum;	//交点编号
//				int splitelemNum;	//被分割单元编号
//				Element splitElem;	//被分割单元
//				if (equalThick(mNodeTable[e.nodeNum1], crosspoint))
//				{
//					splitnodeNum = e.nodeNum1;
//					splitElem = e2;
//					splitelemNum = elemNum2;
//				}
//				else if (equalThick(mNodeTable[e.nodeNum2], crosspoint))
//				{
//					splitnodeNum = e.nodeNum2;
//					splitElem = e2;
//					splitelemNum = elemNum2;
//				}
//				else if (equalThick(mNodeTable[e2.nodeNum1], crosspoint))
//				{
//					splitnodeNum = e2.nodeNum1;
//					splitElem = e;
//					splitelemNum = elemNum;
//				}
//				else
//				{
//					splitnodeNum = e2.nodeNum2;
//					splitElem = e;
//					splitelemNum = elemNum;
//				}
//				//消除梁厚度影响
//				mNodeTable[splitnodeNum] = crosspoint;
//				splitByNode(splitnodeNum, splitelemNum, splitElem);
//			}
//		}
//	}
//
//}
//
//void IFCTranslator::splitByNode(int splitnodeNum, int splitelemNum, Element splitElem)
//{
//	Element newelem;		//新单元
//	newelem.nodeNum1 = splitnodeNum;
//	newelem.nodeNum2 = splitElem.nodeNum2;
//	newelem.matNum = splitElem.matNum;
//	newelem.sectNum = splitElem.sectNum;
//	int newElemNum = mElemTable.size()+1;
//	mElemTable[newElemNum] = newelem;
//	mElemTable[splitelemNum].nodeNum2 = splitnodeNum;
//}

std::vector<Element> IFCTranslator::getBeams(const long long& bsInstance)
{
	double elevation;
	sdaiGetAttrBN(bsInstance, "Elevation", sdaiREAL, &elevation);
	long long* containElemAggr;
	sdaiGetAttrBN(bsInstance, "ContainsElements", sdaiAGGR, &containElemAggr);
	long long containElemInstance;
	engiGetAggrElement(containElemAggr, 0, sdaiINSTANCE, &containElemInstance);
	long long* relatedElemAggr;
	sdaiGetAttrBN(containElemInstance, "RelatedElements", sdaiAGGR, &relatedElemAggr);
	int count = sdaiGetMemberCount(relatedElemAggr);
	std::vector<Element> rev;
	for (int i = 0; i < count; i++)
	{
		long long instance;
		engiGetAggrElement(relatedElemAggr, i, sdaiINSTANCE, &instance);
		std::string elemtype = engiGetInstanceClassInfoUC(instance);
		if (elemtype != "IFCBEAM")continue;
		std::wstring matname;
		matname = getMatName(instance);
		int matno;
		if (vmat_.find(matname) == vmat_.end())
		{
			int n = vmat_.size();
			matno = n + 1;
			vmat_[matname] = matno;
		}
		else
			matno = vmat_[matname];

		std::wstring sectname;
		sectname = getSectName(instance);
		int sectno;
		if (vsect_.find(sectname) == vsect_.end())
		{
			int n = vsect_.size();
			sectno = n + 1;
			vsect_[sectname] = sectno;
		}
		else
			sectno = vsect_[sectname];

		std::vector<double> coord;
		coord = getBeamCoordinates(instance);
		coord[2] += round(elevation);
		coord[5] += round(elevation);

		std::vector<double> node1 = { coord[0],coord[1],coord[2] };
		std::vector<double> node2 = { coord[3],coord[4],coord[5] };

		Element elem;
		elem.matNum = matno;
		elem.sectNum = sectno;
		elem.node1 = node1;
		elem.node2 = node2;
		rev.push_back(elem);
	}
	splitBeams(rev);
	return rev;
}

void IFCTranslator::translate(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName)
{
	wchar_t fileName[512];
	memcpy(fileName, ifcFileName, ifcFileName.GetLength() * sizeof(wchar_t));
	fileName[ifcFileName.GetLength()] = 0;
	long long model = 0;
	model = sdaiOpenModelBNUnicode(0, (char*)fileName, (char*)ifcSchemaName.c_str());
	long long* buildingstoreyAggr = sdaiGetEntityExtentBN(model, "IFCBUILDINGSTOREY");
	int bscnt= sdaiGetMemberCount(buildingstoreyAggr);
	
	//梁
	for (int i = 0; i < bscnt; i++)
	{
		long long bsInstance;
		engiGetAggrElement(buildingstoreyAggr,i,sdaiINSTANCE,&bsInstance);
		std::vector<Element> vbeam= getBeams(bsInstance);
		setBeams(vbeam);
	}

	//根据梁单元对节点编号
	for (auto elem : velem_)
	{
		std::vector<double> node1 = elem.node1;
		if (vnode_.find(node1) == vnode_.end())
		{
			int n = vnode_.size() + 1;
			vnode_.insert({ node1,n });
		}

		std::vector<double> node2 = elem.node2;
		if (vnode_.find(node2) == vnode_.end())
		{
			int n = vnode_.size() + 1;
			vnode_.insert({ node2,n });
		}
	}


	//柱
	for (int i = 0; i < bscnt-1; i++)
	{
		long long bsInstance1;
		long long bsInstance2;
		engiGetAggrElement(buildingstoreyAggr, i, sdaiINSTANCE, &bsInstance1);
		engiGetAggrElement(buildingstoreyAggr, i+1, sdaiINSTANCE, &bsInstance2);
		double elevation1,elevation2;
		sdaiGetAttrBN(bsInstance1, "Elevation", sdaiREAL, &elevation1);
		sdaiGetAttrBN(bsInstance2, "Elevation", sdaiREAL, &elevation2);
		long long* containElemAggr;
		sdaiGetAttrBN(bsInstance1, "ContainsElements", sdaiAGGR, &containElemAggr);
		long long containElemInstance;
		engiGetAggrElement(containElemAggr, 0, sdaiINSTANCE, &containElemInstance);
		long long* relatedElemAggr;
		sdaiGetAttrBN(containElemInstance, "RelatedElements", sdaiAGGR, &relatedElemAggr);
		int membercount = sdaiGetMemberCount(relatedElemAggr);
		
		for (int j = 0; j < membercount; j++)
		{
			long long instance;
			engiGetAggrElement(relatedElemAggr, j, sdaiINSTANCE, &instance);
			std::string elemtype = engiGetInstanceClassInfoUC(instance);
			if (elemtype != "IFCCOLUMN")continue;
			std::wstring matname;
			matname = getMatName(instance);
			int matno;
			if (vmat_.find(matname) == vmat_.end())
			{
				int n = vmat_.size();
				matno = n + 1;
				vmat_[matname] = matno;
			}
			else
				matno = vmat_[matname];

			std::wstring sectname;
			sectname = getSectName(instance);
			int sectno;
			if (vsect_.find(sectname) == vsect_.end())
			{
				int n = vsect_.size();
				sectno = n + 1;
				vsect_[sectname] = sectno;
			}
			else
				sectno = vsect_[sectname];

			std::vector<double> coord;
			coord = getColumnCoordinates(instance);
			coord[2] += round(elevation2);
			std::vector<double> node1, node2;
			//for (auto itr : vnode_)
			//{
			//	if (equalThick(itr.first, coord))
			//	{
			//		node2 = itr.first;
			//		node1 = node2;
			//		node1[2] = round(elevation1);
			//		break;
			//	}
			//}
			//

			for (auto itr : vcross_)
			{
				if (equalThick(itr, coord))
				{
					node2 = itr;
					node1 = node2;
					node1[2] = round(elevation1);
					break;
				}
			}

			//当楼层为第0层时，添加底层节点
			if ((i==0||i==1)&&node1.size()>0)
			{
				if (vnode_.find(node1) == vnode_.end())
				{
					int n = vnode_.size() + 1;
					vnode_[node1] = n;
				}
			}
			if (node1.size() > 0)
			{
				Element elem;
				elem.matNum = matno;
				elem.sectNum = sectno;
				elem.node1 = node1;
				elem.node2 = node2;
				velem_.push_back(elem);
			}
		}
	}


	//写入fpmt
	std::map<int, std::vector<double>> nodeTable;
	for (auto itr : vnode_)
		nodeTable[itr.second] = itr.first;
	
	for(int i=0;i<vmat_.size();i++)
		fpmtwriter_.addLEMat(i+1);
	for (int i = 0; i < vsect_.size(); i++)
		fpmtwriter_.addRectSect(i+1);
	for (auto itr : nodeTable)
		fpmtwriter_.addNode(itr.first, itr.second[0]/1000.0, itr.second[1]/1000.0, itr.second[2]/1000.0);
	int elemno = 1;
	for (auto itr : velem_)
		fpmtwriter_.addBeamElem(elemno++, vnode_[itr.node1], vnode_[itr.node2], itr.sectNum, itr.matNum);

	fpmtwriter_.writeFpmt(outputpath_);
}

std::vector<double> IFCTranslator::getBeamRectSect(const long long& elemInstance)
{
	long long repreInstance = 0;
	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
	long long* repreAggr = nullptr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	long long shaperepInstance;
	engiGetAggrElement(repreAggr, 1, sdaiINSTANCE, &shaperepInstance);
	long long* itemsAggr;
	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
	long long itemInstance;
	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
	std::vector<double> sect(2);
	long long areaInstance;
	sdaiGetAttrBN(itemInstance, "SweptArea", sdaiINSTANCE, &areaInstance);
	sdaiGetAttrBN(areaInstance, "YDim", sdaiREAL, &sect[0]);
	sdaiGetAttrBN(areaInstance, "XDim", sdaiREAL, &sect[1]);
	return roundv(sect);
}

std::vector<double> IFCTranslator::getColumnRectSect(const long long& elemInstance)
{
	long long repreInstance;
	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
	long long* repreAggr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	long long shaperepInstance;
	engiGetAggrElement(repreAggr, 0, sdaiINSTANCE, &shaperepInstance);
	long long* itemsAggr = nullptr;
	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
	long long itemInstance;
	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
	std::vector<double> sect = { 200,200 };
	long long msInstance;
	sdaiGetAttrBN(itemInstance, "MappingSource", sdaiINSTANCE, &msInstance);
	long long mpInstance;
	sdaiGetAttrBN(msInstance, "MappedRepresentation", sdaiINSTANCE, &mpInstance);
	long long* itemsAggr2 = nullptr;
	sdaiGetAttrBN(mpInstance, "Items", sdaiAGGR, &itemsAggr2);
	long long itemInstance2;
	engiGetAggrElement(itemsAggr2, 0, sdaiINSTANCE, &itemInstance2);
	long long areaInstance;
	sdaiGetAttrBN(itemInstance2, "SweptArea", sdaiINSTANCE, &areaInstance);
	if (areaInstance < 0) return {};
	sdaiGetAttrBN(areaInstance, "YDim", sdaiREAL, &sect[0]);
	sdaiGetAttrBN(areaInstance, "XDim", sdaiREAL, &sect[1]);
	return roundv(sect);
}
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

double IFCTranslator::getBeamRealLength(const long long& elemInstance)
{
	long long repreInstance;
	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
	long long* repreAggr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	long long shaperepInstance;
	engiGetAggrElement(repreAggr, 1, sdaiINSTANCE, &shaperepInstance);
	long long* itemsAggr = nullptr;
	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
	long long itemInstance;
	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
	double elementlength = 0;
	sdaiGetAttrBN(itemInstance, "Depth", sdaiREAL, &elementlength);
	return elementlength;
}
double IFCTranslator::getBeamCalcuLength(const long long& elemInstance)
{
	long long repreInstance;
	sdaiGetAttrBN(elemInstance, "Representation", sdaiINSTANCE, &repreInstance);
	long long* repreAggr;
	sdaiGetAttrBN(repreInstance, "Representations", sdaiAGGR, &repreAggr);
	long long shaperepInstance;
	engiGetAggrElement(repreAggr, 0, sdaiINSTANCE, &shaperepInstance);
	long long* itemsAggr = nullptr;
	sdaiGetAttrBN(shaperepInstance, "Items", sdaiAGGR, &itemsAggr);
	long long itemInstance;
	engiGetAggrElement(itemsAggr, 0, sdaiINSTANCE, &itemInstance);
	long long* pointAggr;
	sdaiGetAttrBN(itemInstance, "Points", sdaiAGGR, &pointAggr);
	long long point0Instance, point1Instance;
	engiGetAggrElement(pointAggr, 0, sdaiINSTANCE, &point0Instance);
	engiGetAggrElement(pointAggr, 1, sdaiINSTANCE, &point1Instance);
	long long* coordinates0Aggr;
	long long* coordinates1Aggr;
	sdaiGetAttrBN(point0Instance, "Coordinates", sdaiAGGR, &coordinates0Aggr);
	sdaiGetAttrBN(point1Instance, "Coordinates", sdaiAGGR, &coordinates1Aggr);
	double x0, y0, x1, y1;
	engiGetAggrElement(coordinates0Aggr, 0, sdaiREAL, &x0);
	engiGetAggrElement(coordinates0Aggr, 1, sdaiREAL, &y0);
	engiGetAggrElement(coordinates1Aggr, 0, sdaiREAL, &x1);
	engiGetAggrElement(coordinates1Aggr, 1, sdaiREAL, &y1);
	return sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
}
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
std::vector<double> IFCTranslator::getBeamCoordinates(const long long& elemInstance)
{
	long long opInstance;
	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	long long rpInstance;
	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
	long long locInstance;
	sdaiGetAttrBN(rpInstance, "Location", sdaiINSTANCE, &locInstance);
	long long* coordAggr;
	sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &coordAggr);
	double x0, y0, z0;
	engiGetAggrElement(coordAggr, 0, sdaiREAL, &x0);
	engiGetAggrElement(coordAggr, 1, sdaiREAL, &y0);
	engiGetAggrElement(coordAggr, 2, sdaiREAL, &z0);
	long long directInstance = 0;
	sdaiGetAttrBN(rpInstance, "RefDirection", sdaiINSTANCE, &directInstance);
	std::vector<double> vd = { 1,0,0 };
	if (directInstance)
	{
		long long* drAggr;
		sdaiGetAttrBN(directInstance, "DirectionRatios", sdaiAGGR, &drAggr);
		engiGetAggrElement(drAggr, 0, sdaiREAL, &vd[0]);
		engiGetAggrElement(drAggr, 1, sdaiREAL, &vd[1]);
		engiGetAggrElement(drAggr, 2, sdaiREAL, &vd[2]);
	}
	double len = getBeamCalcuLength(elemInstance);
//	double h = getH(elemInstance);
	return roundv({ x0,y0, 0,x0 + len * vd[0],y0 + len * vd[1], len * vd[2] });
}

std::vector<double> IFCTranslator::getColumnCoordinates(const long long& elemInstance)
{
	long long opInstance;
	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	long long rpInstance;
	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
	long long locInstance;
	sdaiGetAttrBN(rpInstance, "Location", sdaiINSTANCE, &locInstance);
	long long* coordAggr = nullptr;
	sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &coordAggr);
	double x0, y0, z0;
	engiGetAggrElement(coordAggr, 0, sdaiREAL, &x0);
	engiGetAggrElement(coordAggr, 1, sdaiREAL, &y0);
	engiGetAggrElement(coordAggr, 2, sdaiREAL, &z0);
	//double len = getColumenLength(elemInstance);
	return roundv({ x0,y0 ,0});
}

std::vector<double> IFCTranslator::getMat(const long long& elemInstance)
{
	long long* associationAggr = nullptr;
	sdaiGetAttrBN(elemInstance, "HasAssociations", sdaiAGGR, &associationAggr);
	long long associationInstance;
	engiGetAggrElement(associationAggr, 0, sdaiINSTANCE, &associationInstance);
	long long relatingMaterialInstance;
	sdaiGetAttrBN(associationInstance, "RelatingMaterial", sdaiINSTANCE, &relatingMaterialInstance);
	if(relatingMaterialInstance<0)
		return  { 7850,2.06e11,0.3 };
	wchar_t* matname=0;
	sdaiGetAttrBN(relatingMaterialInstance, "Name", sdaiUNICODE, &matname);
	std::vector<double> mat = { 7850,2.06e11,0.3 };
	if (wcsstr(matname, L"混凝土") || StrStrIW(matname, L"concrete"))
	{
		mat[0] = 2500;
		if (wcsstr(matname, L"25"))
			mat[1] = 2.2e10;
		else if (wcsstr(matname, L"35"))
			mat[1] = 2.55e10;
		else if (wcsstr(matname, L"30"))
			mat[1] = 2.8e10;
		else if (wcsstr(matname, L"35"))
			mat[1] = 3.0e10;
		else if (wcsstr(matname, L"40"))
			mat[1] = 3.25e10;
		else if (wcsstr(matname, L"45"))
			mat[1] = 3.35e10;
		else if (wcsstr(matname, L"50"))
			mat[1] = 3.45e10;
		else if (wcsstr(matname, L"55"))
			mat[1] = 3.55e10;
		else if (wcsstr(matname, L"60"))
			mat[1] = 3.6e10;
		else if (wcsstr(matname, L"65"))
			mat[1] = 3.65e10;
		else if (wcsstr(matname, L"70"))
			mat[1] = 3.70e10;
		else if (wcsstr(matname, L"75"))
			mat[1] = 3.75e10;
		else if (wcsstr(matname, L"80"))
			mat[1] = 3.80e10;
		else 
			mat[1]= 3.25e10;
	}
	return mat;
}
std::vector<double> IFCTranslator::getSect(const long long& elemInstance)
{
	wchar_t* objecttype;	
	sdaiGetAttrBN(elemInstance, "ObjectType", sdaiUNICODE, &objecttype);
	std::wstring s(objecttype);
	int index = s.find('x');
	if (index < 0)
		index = s.find('*');
	int temp = index;
	int start1, end1,start2,end2;
	index--;
	while(s[index]==L' ')
		index--;
	end1 = index;
	while (index>=0&& s[index] >= L'0' && s[index] <= L'9')
	{
		index--;
	}
	start1 = index + 1;
	index = temp+1;
	while (s[index] == L' ')
		index++;
	start2 = index;
	while (index<s.size()&&s[index] >= L'0' && s[index] <= L'9')
		index++;
	end2 = index - 1;
	double xdim = stoi(s.substr(start1, end1 - start1 + 1));
	double ydim = stoi(s.substr(start2,end2-start2+1));
	return { xdim ,ydim };
}
void IFCTranslator::setOutputpath(const CString& opath)
{
	memcpy(outputpath_, opath, opath.GetLength() * sizeof(wchar_t));
	outputpath_[opath.GetLength()] = 0;
}

std::wstring IFCTranslator::getMatName(const long long& elemInstance)
{
	long long* associationAggr = nullptr;
	sdaiGetAttrBN(elemInstance, "HasAssociations", sdaiAGGR, &associationAggr);
	long long associationInstance;
	engiGetAggrElement(associationAggr, 0, sdaiINSTANCE, &associationInstance);
	long long relatingMaterialInstance;
	sdaiGetAttrBN(associationInstance, "RelatingMaterial", sdaiINSTANCE, &relatingMaterialInstance);
	if (relatingMaterialInstance < 0)
		return  L"empty";
	wchar_t* matname = 0;
	sdaiGetAttrBN(relatingMaterialInstance, "Name", sdaiUNICODE, &matname);
	return matname;
}

std::wstring IFCTranslator::getSectName(const long long& elemInstance)
{
	wchar_t* objecttype;
	sdaiGetAttrBN(elemInstance, "ObjectType", sdaiUNICODE, &objecttype);
	return objecttype;
}

void IFCTranslator::splitBeams(std::vector<Element>& v)
{
	for(int i=0;i<v.size();i++)
		splitBeam(v[i], v);
}
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
void IFCTranslator::splitBeam(Element& e, std::vector<Element> v)
{
	e.vcross.push_back({ e.node1[0],e.node1[1] });
	e.vcross.push_back({ e.node2[0],e.node2[1] });
	for (auto itr : v)
	{
		std::vector<double> crosspoint;
		int n = e.vcross.size();
		crosspoint = getCrossPoint(e,itr);
		if (crosspoint.size() > 0)
		{
			e.vcross.push_back({ crosspoint[0],crosspoint[1] });
		}
	}

	sort(e.vcross.begin(), e.vcross.end(),cmp);
	auto new_end=unique(e.vcross.begin(), e.vcross.end(),unique);
	e.vcross.erase(new_end, e.vcross.end());
}
void IFCTranslator::setBeams(const std::vector<Element>& velem)
{
	for (auto elem : velem)
	{
		for (int i = 0; i < elem.vcross.size() - 1; i++)
		{
			Element newelem;
			newelem.matNum = elem.matNum;
			newelem.sectNum = elem.sectNum;
			newelem.node1 = { elem.vcross[i].first,elem.vcross[i].second,elem.node1[2] };
			newelem.node2= { elem.vcross[i+1].first,elem.vcross[i+1].second,elem.node1[2] };
			velem_.push_back(newelem);
		}
	}
}
void IFCTranslator::setNodeTableDouble()
{

}

void IFCTranslator::setNodeTableThick()
{
	for (auto elem : velem_)
	{
		bool isFind = false;
		std::vector<double> node1 = elem.node1;
		if (vnode_.find(node1) == vnode_.end())
		{
			int n = vnode_.size() + 1;
			vnode_.insert({ node1,n });
		}

		std::vector<double> node2 = elem.node2;
		if (vnode_.find(node2) == vnode_.end())
		{
			int n = vnode_.size() + 1;
			vnode_.insert({ node2,n });
		}
	}
}
