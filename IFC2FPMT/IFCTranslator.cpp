#include "stdafx.h"
#include "IFCTranslator.h"
#include <string>
#include <set>
#include <fstream>
#include "ifcengine/include/engine.h"
#include "ifcengine/include/ifcengine.h"

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

std::vector<double> getCrossPoint(std::vector<double> a, std::vector<double> b, std::vector<double> c, std::vector<double> d)
{
	double denominator = (b[1] - a[1]) * (d[0] - c[0]) - (a[0] - b[0]) * (c[1] - d[1]);
	if (denominator == 0) {
		return {};
	}
	// 线段所在直线的交点坐标 (x , y)      
	double x = ((b[0] - a[0]) * (d[0] - c[0]) * (c[1] - a[1])
		+ (b[1] - a[1]) * (d[0] - c[0]) * a[0]
		- (d[1] - c[1]) * (b[0] - a[0]) * c[0]) / denominator;
	double y = -((b[1] - a[1]) * (d[1] - c[1]) * (c[0] - a[0])
		+ (b[0] - a[0]) * (d[1] - c[1]) * a[1]
		- (d[0] - c[0]) * (b[1] - a[1]) * c[1]) / denominator;

	/** 2 判断交点是否在两条线段上 **/
	if (
		// 交点在线段1上  
		(x - a[0]) * (x - b[0]) <= 0 && (y - a[1]) * (y - b[1]) <= 0
		// 且交点也在线段2上  
		&& (x - c[0]) * (x - d[0]) <= 0 && (y - c[1]) * (y - d[1]) <= 0
		) {
		// 返回交点p  
		return { x,y };
	}
	//否则不相交  
	return {};
}
void IFCTranslator::test1()
{
	std::basic_string<wchar_t> ifcSchemaName_IFC2x3 = L"IFC2X3_TC1.exp";
	setOutputpath(_T("output.fpmt"));
	//CString m_path = _T("10层框架.ifc");
	//CString m_path = _T("安中大楼.ifc");
	//CString m_path = _T("公益小桥.ifc");
	CString m_path = _T("教学楼项目.ifc");
	translate(m_path, ifcSchemaName_IFC2x3);
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
	for (int i = 0; i < bscnt; i++)
	{
		bool laststoreyflag = true;
		if (i < bscnt - 1)laststoreyflag = false;
		long long bsInstance1,bsInstance2;
		engiGetAggrElement(buildingstoreyAggr,i,sdaiINSTANCE,&bsInstance1);
		if(!laststoreyflag)
			engiGetAggrElement(buildingstoreyAggr, i+1, sdaiINSTANCE, &bsInstance2);
		double elevation1 = 0;
		double elevation2 = 0;
		sdaiGetAttrBN(bsInstance1, "Elevation", sdaiREAL, &elevation1);
		if (!laststoreyflag)
			sdaiGetAttrBN(bsInstance2, "Elevation", sdaiREAL, &elevation2);
		long long* containElemAggr;
		sdaiGetAttrBN(bsInstance1, "ContainsElements", sdaiAGGR, &containElemAggr);
		long long containElemInstance;
		engiGetAggrElement(containElemAggr, 0, sdaiINSTANCE, &containElemInstance);
		long long* relatedElemAggr;
		sdaiGetAttrBN(containElemInstance, "RelatedElements", sdaiAGGR, &relatedElemAggr);
		int count = sdaiGetMemberCount(relatedElemAggr);
		for (int i = 0; i < count; i++)
		{
			long long instance;
			engiGetAggrElement(relatedElemAggr, i, sdaiINSTANCE, &instance);
			std::string elemtype = engiGetInstanceClassInfoUC(instance);
			if (elemtype != "IFCCOLUMN" && elemtype != "IFCBEAM")continue;
			std::vector<double> sect;
			if (elemtype == "IFCBEAM")
				sect = getBeamRectSect(instance);
			else if (elemtype == "IFCCOLUMN")
			{
				sect = getColumnRectSect(instance);
				if (sect.size() == 0)
					sect = getSect(instance);
			}
			//sect = getSect(instance);
			if (vsect_.find(sect) == vsect_.end())
			{
				int n = vsect_.size();
				vsect_[sect] = n + 1;
				fpmtwriter_.addRectSect(n + 1, sect[0] / 1000.0, sect[1] / 1000.0);
			}
			std::vector<double> mat;
			mat = getMat(instance);
			bool isFind = false;
			for (auto itr : vmat_)
			{
				if (itr.first[0] == mat[0] && itr.first[1] == mat[1] && itr.first[2] - mat[2] < 10e-6)
				{
					isFind = true;
					break;
				}
			}
			if (!isFind)
			{
				int n = vmat_.size();
				vmat_[mat] = n + 1;
				fpmtwriter_.addLEMat(n + 1, mat[0], mat[1], mat[2]);
			}
			std::vector<double> coord;
			if (elemtype == "IFCBEAM")
			{
				coord = getBeamCoordinates(instance);
				coord[2] += round(elevation1);
				coord[5] += round(elevation1);
			}
			else if (elemtype == "IFCCOLUMN")
			{
				coord = getColumnCoordinates(instance);
				coord[2] = round(elevation1);
				coord[5] = round(elevation2);
			}
			std::vector<double> node1 = { coord[0],coord[1],coord[2] };
			std::vector<double> node2 = { coord[3],coord[4],coord[5] };
			int no1, no2;
			isFind = false;
			std::vector<double> key;
			for (auto itr : vnode_)
			{
				if (abs(itr.first[0] -node1[0])<800 && abs(itr.first[1] - node1[1]) < 800 && abs(itr.first[2] - node1[2]) < 800)
				{
					isFind = true;
					key = itr.first;
					break;
				}
			}
			if (!isFind)
			{
				int n = vnode_.size();
				vnode_[node1] = n + 1;
				fpmtwriter_.addNode(n + 1, node1[0] / 1000.0, node1[1] / 1000.0, node1[2] / 1000.0);
				no1 = n + 1;
			}
			else
				no1 = vnode_[key];

			isFind = false;
			key.clear();
			for (auto itr : vnode_)
			{
				if (abs(itr.first[0] - node2[0]) < 800 && abs(itr.first[1] - node2[1]) < 800 && abs(itr.first[2] - node2[2]) < 800)
				{
					isFind = true;
					key = itr.first;
					break;
				}
			}
			if (!isFind)
			{
				int n = vnode_.size();
				vnode_[node2] = n + 1;
				fpmtwriter_.addNode(n + 1, node2[0] / 1000.0, node2[1] / 1000.0, node2[2] / 1000.0);
				no2 = n + 1;
			}
			else
				no2 = vnode_[key];
			fpmtwriter_.addBeamElem(++elemcount_, no1, no2, vsect_[sect], vmat_[mat]);
		}

	}

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
	return roundv({ x0,y0,0,x0,y0,0 });
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