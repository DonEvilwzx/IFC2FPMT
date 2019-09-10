#include "stdafx.h"
#include "IFCTranslator.h"
#include <string>
#include <set>
#include <fstream>
#include "ifcengine/include/engine.h"
#include "ifcengine/include/ifcengine.h"

IFCTranslator::IFCTranslator()
{
}
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
void IFCTranslator::test1()
{
	std::basic_string<wchar_t> ifcSchemaName_IFC2x3 = L"IFC2X3_TC1.exp";
	setOutputpath(_T("output.fpmt"));
	//CString m_path = _T("10层框架.ifc");
	CString m_path = _T("安中大楼.ifc");
	//CString m_path = _T("公益小桥.ifc");
	translate(m_path, ifcSchemaName_IFC2x3);
}
void IFCTranslator::translate(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName)
{
	wchar_t fileName[512];
	memcpy(fileName, ifcFileName, ifcFileName.GetLength() * sizeof(wchar_t));
	fileName[ifcFileName.GetLength()] = 0;
	long long model = 0;
	model = sdaiOpenModelBNUnicode(0, (char*)fileName, (char*)ifcSchemaName.c_str());
	long long* columnAggr = sdaiGetEntityExtentBN(model, "IFCCOLUMN");
	long long* beamAggr = sdaiGetEntityExtentBN(model, "IFCBEAM");
	setElem2Fpmt(&columnAggr, "column");
	setElem2Fpmt(&beamAggr, "beam");
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
	std::vector<double> sect(2);
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
	sdaiGetAttrBN(areaInstance, "YDim", sdaiREAL, &sect[0]);
	sdaiGetAttrBN(areaInstance, "XDim", sdaiREAL, &sect[1]);
	return roundv(sect);
}

void IFCTranslator::setElem2Fpmt(long long** aggr, std::string elemtype)
{
	int count = sdaiGetMemberCount(*aggr);
	for (int i = 0; i < count; i++)
	{
		long long instance;
		engiGetAggrElement(*aggr, i, sdaiINSTANCE, &instance);
		std::vector<double> sect;
		if (elemtype == "beam")
			sect = getBeamRectSect(instance);
		else if (elemtype == "column")
			sect = getColumnRectSect(instance);
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
		if (elemtype == "beam")
			coord = getBeamCoordinates(instance);
		else if (elemtype == "column")
			coord = getColumnCoordinates(instance);
		std::vector<double> node1 = { coord[0],coord[1],coord[2] };
		std::vector<double> node2 = { coord[3],coord[4],coord[5] };
		int no1, no2;
		if (vnode_.find(node1) == vnode_.end())
		{
			int n = vnode_.size();
			vnode_[node1] = n + 1;
			fpmtwriter_.addNode(n + 1, coord[0] / 1000.0, coord[1] / 1000.0, coord[2] / 1000.0);
			no1 = n + 1;
		}
		else
			no1 = vnode_[node1];

		if (vnode_.find(node2) == vnode_.end())
		{
			int n = vnode_.size();
			vnode_[node2] = n + 1;
			fpmtwriter_.addNode(n + 1, coord[3] / 1000.0, coord[4] / 1000.0, coord[5] / 1000.0);
			no2 = n + 1;
		}
		else
			no2 = vnode_[node2];
		fpmtwriter_.addBeamElem(++elemcount_, no1, no2, vsect_[sect], 1);
	}
}
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
double IFCTranslator::getColumenLength(const long long& elemInstance)
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
	std::vector<double> sect(2);
	long long msInstance;
	sdaiGetAttrBN(itemInstance, "MappingSource", sdaiINSTANCE, &msInstance);
	long long mpInstance;
	sdaiGetAttrBN(msInstance, "MappedRepresentation", sdaiINSTANCE, &mpInstance);
	long long* itemsAggr2 = nullptr;
	sdaiGetAttrBN(mpInstance, "Items", sdaiAGGR, &itemsAggr2);
	long long itemInstance2;
	engiGetAggrElement(itemsAggr2, 0, sdaiINSTANCE, &itemInstance2);
	long long posInstance;
	sdaiGetAttrBN(itemInstance2, "Position", sdaiINSTANCE, &posInstance);
	long long locInstance;
	sdaiGetAttrBN(posInstance, "Location", sdaiINSTANCE, &locInstance);
	long long* coordAggr = nullptr;
	double len;
	sdaiGetAttrBN(itemInstance2, "Depth", sdaiREAL, &len);
	return len;
}
double IFCTranslator::getH(const long long& elemInstance)
{
	long long opInstance;
	sdaiGetAttrBN(elemInstance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	long long prtInstance;
	sdaiGetAttrBN(opInstance, "PlacementRelTo", sdaiINSTANCE, &prtInstance);
	long long* poAggr;
	sdaiGetAttrBN(prtInstance, "PlacesObject", sdaiAGGR, &poAggr);
	long long bsInstance;
	engiGetAggrElement(poAggr, 0, sdaiINSTANCE, &bsInstance);
	double h;
	sdaiGetAttrBN(bsInstance, "Elevation", sdaiREAL, &h);
	return h;
}
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
	double h = getH(elemInstance);
	return roundv({ x0,y0, h,x0 + len * vd[0],y0 + len * vd[1], h + len * vd[2] });
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
	double len = getColumenLength(elemInstance);
	return roundv({ x0,y0,abs(z0),x0,y0,abs(z0) + len });
}

std::vector<double> IFCTranslator::getMat(const long long& elemInstance)
{
	long long* associationAggr = nullptr;
	sdaiGetAttrBN(elemInstance, "HasAssociations", sdaiAGGR, &associationAggr);
	long long associationInstance;
	engiGetAggrElement(associationAggr, 0, sdaiINSTANCE, &associationInstance);
	long long relatingMaterialInstance;
	sdaiGetAttrBN(associationInstance, "RelatingMaterial", sdaiINSTANCE, &relatingMaterialInstance);
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

void IFCTranslator::setOutputpath(const CString& opath)
{
	memcpy(outputpath_, opath, opath.GetLength() * sizeof(wchar_t));
	outputpath_[opath.GetLength()] = 0;
}