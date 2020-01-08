#include "stdafx.h"
#include "IFCTranslator.h"
#include <string>
#include <set>
#include <fstream>
#include<algorithm>
#include "ifcengine/include/engine.h"
#include "ifcengine/include/ifcengine.h"



using namespace std;
using namespace Eigen;
const double ERRORTHICK = 800;
const double ERRORDOUBLE = 0.01;

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
	if (v1.size() != v2.size())return false;
	int n = v2.size();
	for (int i = 0; i < n; i++)
	{
		if (v1[i] - v2[i] > ERRORDOUBLE || v1[i] - v2[i] < -ERRORDOUBLE)return false;
	}
	return true;
}

vector<double> mynorm(vector<double> v)
{
	double len = 0;
	for (int i = 0; i < v.size(); i++)
		len += v[i] * v[i];
	for (int i = 0; i < v.size(); i++)
		v[i] = v[i] / len;
	return v;
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

vector<double> vectorAdd(vector<double> v1, vector<double> v2)
{
	vector<double> v(v1.size());
	for (int i = 0; i < v1.size(); i++)
	{
		v[i] = v1[i] + v2[i];
	}
	return v;
}


vector<vector<double>> getDirectMatrixByVector(vector<double> v1, vector<double> v2)
{
	vector<vector<double>>rev(3, vector<double>(3));
	for (int j = 0; j < 3; j++)
		rev[0][j] = v1[j];
	for (int j = 0; j < 3; j++)
		rev[1][j] = v2[j];
	rev[2][0] = v1[1] * v2[2] - v1[2] * v2[1];
	rev[2][1] = -v1[0] * v2[2] + v1[2] * v2[0];
	rev[2][2] = v1[0] * v2[1] - v1[1] * v2[0];
	return rev;
}

vector<double> crossProduct(vector<double> v1, vector<double> v2)
{
	vector<double> rev(3);
	rev[0] = v1[1] * v2[2] - v1[2] * v2[1];
	rev[1] = -v1[0] * v2[2] + v1[2] * v2[0];
	rev[2] = v1[0] * v2[1] - v1[1] * v2[0];
	return rev;
}

vector<double> vectorMulMatrix(vector<double> v, vector<vector<double>> m)
{
	vector<double> rev(3);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			rev[i] += v[j] * m[j][i];
	return rev;
}

vector<double> MatrixMulVector(vector<double> v, vector<vector<double>> m)
{
	vector<double> rev(3);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			rev[i] += m[i][j] * v[j];
	return rev;
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


bool isPointSplitLine(vector<double> p1,vector<double> p2, vector<double> p)
{
	if ((p[0] - p1[0])*(p[0] - p2[0]) < ERRORDOUBLE && (p[1] - p1[1])*(p[1] - p2[1]) < ERRORDOUBLE&& !equalThick(p1, p) && !equalThick(p2, p))
	{
			return true;
	}
	return false;
}


vector<double> getParamByline(double x0,double y0,double x1,double y1)
{
	return
	{
	y0 - y1,
	x1 - x0,
	x0*y1 - x1 * y0
	};
}
//求两条直线的交点
std::vector<double> IFCTranslator::getBeamCrossPoint(Beam elem1, Beam elem2)
{
	std::vector<double> e1n1 = mNodeTable[elem1.mNode1];
	std::vector<double> e1n2 = mNodeTable[elem1.mNode2];
	double x1 = e1n1[0],y1=e1n1[1], z1 = e1n1[2],x2=e1n2[0],y2=e1n2[1],z2=e1n2[2];
	if (abs(z1-z2)>ERRORDOUBLE)return{};   //如果不是梁，返回空集{};
	double z = z1;
	std::vector<double> e2n1 = mNodeTable[elem2.mNode1];
	std::vector<double> e2n2 = mNodeTable[elem2.mNode2];
	double x3 = e2n1[0], y3 = e2n1[1],z3=e2n1[2], x4 = e2n2[0], y4 = e2n2[1],z4=e2n2[2];
	if (abs(z3-z4)>ERRORDOUBLE)
	{
		return {};
		//if (abs(x3-x4)<ERRORTHICK&&abs(y3-y4)< ERRORTHICK)
		//{
		//	double x = x3;
		//	double y = y3;
		//	if (abs((y - y1)*(x2 - x) - (x - x1)*(y2 - y)) < ERRORDOUBLE)
		//		return { x,y,z };
		//}
	}
	else 
	{
		vector<double> param1 = getParamByline(x1, y1, x2, y2);
		vector<double> param2 = getParamByline(x3, y3, x4, y4);
		double a0 = param1[0], b0 = param1[1], c0 = param1[2];
		double a1 = param2[0], b1 = param2[1], c1 = param2[2];
		double D = a0 * b1 - a1 * b0;
		if (abs(D-0) < ERRORDOUBLE)
			return {};
		else
		{
			return { (b0*c1 - b1 * c0) / D,(a1*c0 - a0 * c1) / D,z };
		}
	}
	return {};
}

void IFCTranslator::test1()
{
	setOutputpath(_T("output.fpmt"));
	//std::basic_string<wchar_t> ifcSchemaName = L"IFC2X3_TC1.exp";
	std::basic_string<wchar_t> ifcSchemaName = L"IFC4_ADD2.exp";

	//2_3版本
	//CString m_path = _T("10层框架.ifc");
	//CString m_path = _T("安中大楼.ifc");
	CString m_path = _T("教学楼项目.ifc");
	//CString m_path = _T("15#梁柱.0001.ifc");
	//CString m_path = _T("公益小桥.ifc");
	//CString m_path = _T("小高层商住楼.0001.ifc");
	//CString m_path = _T("03IDC.ifc");
	//CString m_path = _T("别墅项目.ifc");

	//4_2版本
	//CString m_path = _T("CSGprimitive.ifc");
	//CString m_path = _T("ExtrudedSolid.ifc");
	//CString m_path = _T("SurfaceModel.ifc");
	//CString m_path = _T("BrepModel.ifc");
	//CString m_path = _T("MappedModel.ifc");
	//CString m_path = _T("MappedItemWithTrans.ifc");
	//CString m_path = _T("MappedMulti.ifc");
	//CString m_path = _T("BeamStandard.ifc");
	//CString m_path = _T("Wall.ifc");
	//translateNewVersion(m_path, ifcSchemaName_IFC4_2);
	translateNewVersion(m_path, ifcSchemaName);
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
bool IFCTranslator::findNodeTable(std::vector<double> vnode)
{
	for (auto node : mNodeTable)
	{
		if (equalDouble(node.second, vnode))
		{
			return true;
		}
	}
	return false;
}
int IFCTranslator::recordNodeTable(std::vector<double> vnode)
{
	if (abs(vnode[2] - mElevations[mStoreyNum]) < ERRORDOUBLE)
	{
		vnode[2] = mElevations[mStoreyNum];
	}
	int nodeno;
	bool isFind = false;
	for (auto node : mNodeTable)
	{
		if (equalDouble(node.second, vnode))
		{
			isFind = true;
			nodeno = node.first;
			break;
		}
	}
	if (!isFind)
	{
		int n = mNodeTable.size();
		nodeno = n + 1;
		mNodeTable[nodeno] = vnode;
	}
	return nodeno;
	//if (mNodeTable.find(vnode) == mNodeTable.end())
	//{
	//	int n = mNodeTable.size();
	//	nodeno = n + 1;
	//	mNodeTable[vnode] = nodeno;
	//}
	//else
	//	nodeno = mNodeTable[vnode];
	//return nodeno;
}

int IFCTranslator::recordSectTable(std::vector<double> vsect)
{
	int sectno;
	bool isFind = false;
	for (auto sect : mSectTable)
	{
		if (equalDouble(sect.first, vsect))
		{
			isFind = true;
			sectno = sect.second;
			break;
		}
	}
	if (!isFind)
	{
		int n = mSectTable.size();
		sectno = n + 1;
		mSectTable[vsect] = sectno;
	}
	return sectno;
}

//int IFCTranslator::recordSectTable(std::vector<double> vsect)
//{
//	int sectno;
//	if (mSectTable.find(vsect) == mSectTable.end())
//	{
//		int n = mSectTable.size();
//		sectno = n + 1;
//		mSectTable[vsect] = sectno;
//	}
//	else
//		sectno = mSectTable[vsect];
//	return sectno;
//}


//int IFCTranslator::recordSectNameTable(std::wstring sectname)
//{
//	int sectno;
//	if (mSectNameTable.find(sectname) == mSectNameTable.end())
//	{
//		int n = mSectNameTable.size();
//		sectno = n + 1;
//		mSectNameTable[sectname] = sectno;
//	}
//	else
//		sectno = mSectNameTable[sectname];
//	return sectno;
//}



int IFCTranslator::recordNodeTableByVector(Eigen::Vector4d v)
{
	return recordNodeTable({ v(0),v(1),v(2) });
}

void IFCTranslator::recordSolidElement(int n1, int n2, int n3, int n4, int matno)
{
	Solid elem;
	elem.mMatNum = matno;
	elem.mNode1 = n1;
	elem.mNode2 = n2;
	elem.mNode3 = n3;
	elem.mNode4 = n4;
	mSolids.push_back(elem);
}

void IFCTranslator::recordBeamElement(int n1, int n2, int matno, int sectno)
{
	Beam elem;
	elem.mMatNum = matno;

	elem.mSectNum = sectno;
	elem.mNode1 = n1;
	elem.mNode2 = n2;
	mBeams[mStoreyNum].push_back(elem);
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

void IFCTranslator::parseIFCCSGSOLID(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix)
{
	long long treeRootInstance;
	sdaiGetAttrBN(itemInstance, "TreeRootExpression", sdaiINSTANCE, &treeRootInstance);
	double xlength, ylength, zlength;
	sdaiGetAttrBN(treeRootInstance, "XLength", sdaiREAL, &xlength);
	sdaiGetAttrBN(treeRootInstance, "YLength", sdaiREAL, &ylength);
	sdaiGetAttrBN(treeRootInstance, "ZLength", sdaiREAL, &zlength);
	long long positionInstance;
	sdaiGetAttrBN(treeRootInstance, "Position", sdaiINSTANCE, &positionInstance);
	long long locInstance;
	sdaiGetAttrBN(positionInstance, "Location", sdaiINSTANCE, &locInstance);
	long long* coordAggr;
	sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &coordAggr);
	vector<double> localCoord = getCoordByCoordAggr(coordAggr);
	//vector<double> coord = vectorAdd(relativeCoord, LocalCoord);
	double x1 = localCoord[0];
	double y1 = localCoord[1];
	double z1 = localCoord[2];

	//长方体共8个顶点，最少可分割为5个四面体单元
	vector<Vector4d> vv;
	Vector4d node;
	node << x1, y1, z1, 1;
	vv.push_back(node);
	node << x1 + xlength, y1, z1, 1;
	vv.push_back(node);
	node << x1, y1 + ylength, z1, 1;
	vv.push_back(node);
	node << x1 + xlength, y1 + ylength, z1, 1;
	vv.push_back(node);
	node << x1, y1, z1 + zlength, 1;
	vv.push_back(node);
	node << x1 + xlength, y1, z1 + zlength, 1;
	vv.push_back(node);
	node << x1, y1 + ylength, z1 + zlength, 1;
	vv.push_back(node);
	node << x1 + xlength, y1 + ylength, z1 + zlength, 1;
	vv.push_back(node);

	vector<int> vnodes;
	for (Vector4d localCoord : vv)
	{
		Vector4d wordCoord = relativeTmatrix * localCoord;
		vnodes.push_back(recordNodeTableByVector(wordCoord));
	}
	record5SolidElement(vnodes[0], vnodes[1], vnodes[2], vnodes[3], vnodes[4],
		vnodes[5], vnodes[6], vnodes[7], matno);
}

void IFCTranslator::parseCfsFaces(long long* cfsfacesAggr, int matno, Eigen::Matrix4d relativeTmatrix)
{
	int facecnt = sdaiGetMemberCount(cfsfacesAggr);
	//六面体的处理
	if (facecnt == 6)
	{
		set<vector<double>> points;
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
					vector<double> localCoord = getCoordByCoordAggr(coordAggr);
					points.insert(localCoord);
				}
			}
		}
		vector <int>vnodes;
		for (auto itr : points)
		{
			Vector4d vtemp;
			vtemp << itr[0], itr[1], itr[2], 1;
			vnodes.push_back(recordNodeTableByVector(relativeTmatrix*vtemp));
		}
		record5SolidElement(vnodes[0], vnodes[1], vnodes[2], vnodes[3], vnodes[4],
			vnodes[5], vnodes[6], vnodes[7], matno);
	}
}
void IFCTranslator::parseIFCFACEBASEDSURFACEMODEL(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix)
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
		parseCfsFaces(cfsfacesAggr, matno, relativeTmatrix);
	}
}
void IFCTranslator::parseIFCFACETEDBREP(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix)
{
	long long outerInstacne;
	sdaiGetAttrBN(itemInstance, "Outer", sdaiINSTANCE, &outerInstacne);
	long long* cfsfacesAggr;
	sdaiGetAttrBN(outerInstacne, "CfsFaces", sdaiAGGR, &cfsfacesAggr);
	parseCfsFaces(cfsfacesAggr, matno, relativeTmatrix);
}

void IFCTranslator::parseIFCEXTRUDEDAREASOLID(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix, bool isWall)
{
	double depth = 0;
	sdaiGetAttrBN(itemInstance, "Depth", sdaiREAL, &depth);
	long long extrudedDirectInstance;
	sdaiGetAttrBN(itemInstance, "ExtrudedDirection", sdaiINSTANCE, &extrudedDirectInstance);
	vector<double> exdirect = getDirectByDirectInstance(extrudedDirectInstance);
	exdirect = mynorm(exdirect);
	long long positionInstance;
	sdaiGetAttrBN(itemInstance, "Position", sdaiINSTANCE, &positionInstance);
	Matrix4d localT1 = getTMatrixByPositionInstance(positionInstance);
	int sectno = 1;
	long long sweptAreaInstance;
	sdaiGetAttrBN(itemInstance, "SweptArea", sdaiINSTANCE, &sweptAreaInstance);
	long long positionInstance2;
	sdaiGetAttrBN(sweptAreaInstance, "Position", sdaiINSTANCE, &positionInstance2);
	Matrix4d localT2 = getTMatrixByPositionInstance(positionInstance2);
	Matrix4d localTmatrix = localT1 * localT2;
	string areaName = engiGetInstanceClassInfoUC(sweptAreaInstance);
	//这里的1表示截面类型
	Vector4d node1;
	node1 << 0, 0, 0, 1;
	Vector4d node2;
	node2 << depth * exdirect[0], depth * exdirect[1], depth * exdirect[2], 1;
	if (areaName == "IFCRECTANGLEPROFILEDEF")
	{
		double sectx, secty;
		sdaiGetAttrBN(sweptAreaInstance, "XDim", sdaiREAL, &sectx);
		sdaiGetAttrBN(sweptAreaInstance, "YDim", sdaiREAL, &secty);
		if (isWall)		//如果是墙，变为壳单元
		{
			double t;
			Vector4d node11 = node1;
			Vector4d node12 = node1;
			Vector4d node21 = node2;
			Vector4d node22 = node2;
			if (secty > sectx)
			{
				t = sectx;
				node11(1) = node21(1) = -secty / 2;
				node12(1) = node22(1) = secty / 2;
			}
			else
			{
				t = secty;
				node11(0) = node21(0) = -sectx / 2;
				node12(0) = node22(0) = sectx / 2;
			}
			Vector4d worldNode11 = relativeTmatrix * localTmatrix*node11;
			Vector4d worldNode12 = relativeTmatrix * localTmatrix*node12;
			Vector4d worldNode21 = relativeTmatrix * localTmatrix*node21;
			Vector4d worldNode22 = relativeTmatrix * localTmatrix*node22;
			int no1 = recordNodeTableByVector(worldNode11);
			int no2 = recordNodeTableByVector(worldNode12);
			int no3 = recordNodeTableByVector(worldNode21);
			int no4 = recordNodeTableByVector(worldNode22);
			int sectno = recordSectTable({ 2,t });
			recordShellElement(no1, no2, no3, matno, sectno);
			recordShellElement(no2, no3, no4, matno, sectno);
		}
		else    //如果是梁柱，变为梁单元
		{
			double h = sectx * mMappedXscale;
			double b = secty * mMappedYscale;
			if (h < b)swap(h, b);

			sectno = recordSectTable({ 1,b,h,mMappedAngle });
			Vector4d worldNode1 = relativeTmatrix * localTmatrix*node1;
			Vector4d worldNode2 = relativeTmatrix * localTmatrix*node2;
			int no1 = recordNodeTableByVector(worldNode1);
			int no2 = recordNodeTableByVector(worldNode2);
			recordBeamElement(no1, no2, matno, sectno);
		}
	}
	//else if (areaName == "IFCARBITRARYCLOSEDPROFILEDEF")
	//{
	//	//if (isWall)
	//	//{
	//	//	long long ocInstance;
	//	//	sdaiGetAttrBN(itemInstance, "OuterCurve", sdaiINSTANCE, &ocInstance);
	//	//	vector<long long> pointInstances = getInstancesByInstance(ocInstance, "Points");
	//	//	vector<vector<double>> points;
	//	//	if (pointInstances.size() == 9)
	//	//	{
	//	//		for (auto pointInstance : pointInstances)
	//	//		{
	//	//			long long* coordAggr;
	//	//			sdaiGetAttrBN(pointInstance, "Coordinates", sdaiAGGR, &coordAggr);
	//	//			vector<double> coord = getCoordByCoordAggr(coordAggr);
	//	//			points.push_back(coord);
	//	//		}
	//	//	}
	//	//}
	//}
	else //其他截面暂做简单处理
	{
		Vector4d worldNode1 = relativeTmatrix * localTmatrix*node1;
		Vector4d worldNode2 = relativeTmatrix * localTmatrix*node2;
		int no1 = recordNodeTableByVector(worldNode1);
		int no2 = recordNodeTableByVector(worldNode2);
		sectno = recordSectTable({ 1,300,300,0 });
		recordBeamElement(no1, no2, matno, sectno);
	}

}
void IFCTranslator::recordShellElement(int n1, int n2, int n3, int matno, int sectno)
{
	Shell shell;
	shell.mNode1 = n1;
	shell.mNode2 = n2;
	shell.mNode3 = n3;
	shell.mMatNum = matno;
	shell.mSectNum = sectno;
	mShells.push_back(shell);
}

Eigen::Matrix4d IFCTranslator::getTMatrixByIfcAxis2Placement2D(const long long& instance)
{
	Matrix4d T = Matrix4d::Identity();
	if (instance <= 0)
		return T;
	//获取原点坐标
	long long locInstance;
	sdaiGetAttrBN(instance, "Location", sdaiINSTANCE, &locInstance);
	if (locInstance > 0)
	{
		long long* loccoordAggr = nullptr;
		sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &loccoordAggr);
		vector<double> pan = getCoordByCoordAggr(loccoordAggr);
		for (int i = 0; i < sdaiGetMemberCount(loccoordAggr); i++)
			T(i, 3) = pan[i];
	}
	return T;
}

Eigen::Matrix4d IFCTranslator::getTMatrixByPositionInstance(const long long& positionInstance)
{
	Matrix4d T = Matrix4d::Identity();
	if (positionInstance > 0)
	{
		string positionName = engiGetInstanceClassInfoUC(positionInstance);
		if (positionName == "IFCAXIS2PLACEMENT3D")
			T = getTMatrixByIfcAxis2Placement3D(positionInstance);
		else if (positionName == "IFCAXIS2PLACEMENT2D")
			T = getTMatrixByIfcAxis2Placement2D(positionInstance);
	}
	return T;
}
void IFCTranslator::parseIFCMAPPEDITEM(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix, bool isWall)
{
	Matrix4d D = Matrix4d::Identity();
	Matrix4d R = Matrix4d::Identity();
	Matrix4d S = Matrix4d::Identity();
	vector<double> pan = { 0,0,0 };
	vector<double> xdim = { 1,0,0 };
	vector<double> ydim = { 0,1,0 };
	vector<double> zdim = { 0,0,1 };
	vector<double> scale = { 1,1,1 };
	long long mappingTargetInstance;
	sdaiGetAttrBN(itemInstance, "MappingTarget", sdaiINSTANCE, &mappingTargetInstance);
	long long localOriginInstance;
	sdaiGetAttrBN(mappingTargetInstance, "LocalOrigin", sdaiINSTANCE, &localOriginInstance);
	long long* coorAggr;
	sdaiGetAttrBN(localOriginInstance, "Coordinates", sdaiAGGR, &coorAggr);
	pan = getCoordByCoordAggr(coorAggr);
	long long axis1Instance, axis2Instance, axis3Instance;
	sdaiGetAttrBN(mappingTargetInstance, "Axis1", sdaiINSTANCE, &axis1Instance);
	sdaiGetAttrBN(mappingTargetInstance, "Axis2", sdaiINSTANCE, &axis2Instance);
	sdaiGetAttrBN(mappingTargetInstance, "Axis3", sdaiINSTANCE, &axis3Instance);
	sdaiGetAttrBN(mappingTargetInstance, "Scale", sdaiREAL, &scale[0]);
	sdaiGetAttrBN(mappingTargetInstance, "Scale2", sdaiREAL, &scale[1]);
	sdaiGetAttrBN(mappingTargetInstance, "Scale3", sdaiREAL, &scale[2]);
	if (axis1Instance > 0)
	{
		long long* xdimAggr = nullptr;
		sdaiGetAttrBN(axis1Instance, "DirectionRatios", sdaiAGGR, &xdimAggr);
		xdim = mynorm(getCoordByCoordAggr(xdimAggr));
	}
	if (axis2Instance > 0)
	{
		long long* ydimAggr = nullptr;
		sdaiGetAttrBN(axis2Instance, "DirectionRatios", sdaiAGGR, &ydimAggr);
		ydim = mynorm(getCoordByCoordAggr(ydimAggr));
	}
	if (axis3Instance > 0)
	{
		long long* zdimAggr = nullptr;
		sdaiGetAttrBN(axis3Instance, "DirectionRatios", sdaiAGGR, &zdimAggr);
		zdim = mynorm(getCoordByCoordAggr(zdimAggr));
	}
	for (int i = 0; i < 3; i++)
	{
		R(i, 0) = xdim[i];
		R(i, 1) = ydim[i];
		R(i, 2) = zdim[i];
		D(i, 3) = pan[i];
		S(i, i) = scale[i];
	}
	mMappedXscale = scale[0];
	mMappedYscale = scale[1];
	mMappedAngle = -atan(xdim[1] / xdim[0]) * 180 / 3.14;
	Matrix4d localTmatrix = D * R*S;
	long long mappingSourceInstance;
	sdaiGetAttrBN(itemInstance, "MappingSource", sdaiINSTANCE, &mappingSourceInstance);
	long long mappedRepreInstance;
	sdaiGetAttrBN(mappingSourceInstance, "MappedRepresentation", sdaiINSTANCE, &mappedRepreInstance);
	vector<long long> itemInstances = getInstancesByInstance(mappedRepreInstance, "Items");
	for (auto itemInstance : itemInstances)
	{
		parseItems(itemInstance, matno, relativeTmatrix*localTmatrix, isWall);
		resetMappedAttribute();
	}
}
void IFCTranslator::resetMappedAttribute()
{
	double mMappedXscale = 1;
	double mMappedYscale = 1;
	double mMappedZscale = 1;
	double mMappedAngle = 0;
}

void IFCTranslator::parseItems(const long long& itemInstance, int matno, Eigen::Matrix4d relativeTmatrix, bool isWall)
{
	string  itemName = engiGetInstanceClassInfoUC(itemInstance);
	//当实体为IfcCSGSolid实体时
	if (itemName == "IFCCSGSOLID")
	{
		parseIFCCSGSOLID(itemInstance, matno, relativeTmatrix);
	}
	//当实体为ExtrudedSolid实体
	else if (itemName == "IFCEXTRUDEDAREASOLID")
	{
		parseIFCEXTRUDEDAREASOLID(itemInstance, matno, relativeTmatrix, isWall);
	}
	//当实体为SurfceModel实体
	else if (itemName == "IFCFACEBASEDSURFACEMODEL")
	{
		parseIFCFACEBASEDSURFACEMODEL(itemInstance, matno, relativeTmatrix);
	}
	else if (itemName == "IFCFACETEDBREP")
	{
		parseIFCFACETEDBREP(itemInstance, matno, relativeTmatrix);
	}
	else if (itemName == "IFCMAPPEDITEM")
	{
		parseIFCMAPPEDITEM(itemInstance, matno, relativeTmatrix, isWall);
	}
}

void IFCTranslator::splitBeams()
{
	for (int i = 0; i <= mStoreyNum; i++)
	{
		int j = 0;
		int storeyBeamCnt = mBeams[i].size();
		while(j<storeyBeamCnt)
		{
			vector<double> node1 = mNodeTable[mBeams[i][j].mNode1];
			vector<double> node2 = mNodeTable[mBeams[i][j].mNode2];
			int tempStoreyBeamCnt = storeyBeamCnt;
			for (int k=0;k<tempStoreyBeamCnt;k++)
			{
				vector<double> crossPoint = getBeamCrossPoint(mBeams[i][j], mBeams[i][k]);
				if (crossPoint.size() > 0)
				{
					if (equalDouble(crossPoint, node1) || equalDouble(crossPoint, node2))
						continue;
					if (isPointSplitLine( node1, node2,crossPoint))
					{
						int n3 = recordNodeTable(crossPoint);
						int oldn2 = mBeams[i][j].mNode2;
						mBeams[i][j].mNode2 = n3;
						node2 = crossPoint;
						Beam newbeam;
						newbeam.mNode1 = n3;
						newbeam.mNode2 = oldn2;
						newbeam.mMatNum = mBeams[i][j].mMatNum;
						newbeam.mSectNum = mBeams[i][j].mSectNum;
						mBeams[i].push_back(newbeam);
						++storeyBeamCnt;
					}
					else if (equalThick(crossPoint, node1))
					{
						mBeams[i][j].mNode1 = recordNodeTable(crossPoint);
					}
					else if (equalThick(crossPoint, node2))
					{
						mBeams[i][j].mNode2 = recordNodeTable(crossPoint);
					}
				}
			}
			++j;
		}
	}

}

void IFCTranslator::parseBuildingStorey(const long long& instance, Eigen::Matrix4d relativeTmatrix)
{
	Matrix4d localTmatrix = getTMatrixByInstance(instance);
	vector<long long> containElemInstances = getInstancesByInstance(instance, "ContainsElements");
	for (auto containElemInstance : containElemInstances)
	{
		vector<long long> relatedInstances = getInstancesByInstance(containElemInstance, "RelatedElements");
		for (auto instance : relatedInstances)
		{
			string elemtype = engiGetInstanceClassInfoUC(instance);

			if (/*elemtype == "IFCBUILDINGELEMENTPROXY" ||*/ elemtype == "IFCCOLUMN" || elemtype == "IFCBEAMSTANDARDCASE" || elemtype == "IFCBEAM")
				parseElement(instance, relativeTmatrix*localTmatrix, false);
			else if (elemtype == "IFCWALL" || elemtype == "IFCWALLSTANDARDCASE" || elemtype == "IFCSLAB")
			{
				//parseElement(instance, relativeTmatrix*localTmatrix, true);
			}
		}
	}
}

void IFCTranslator::parseElement(const long long& instance, Eigen::Matrix4d relativeTmatrix, bool isWall)
{

	Matrix4d localTmatrix = getTMatrixByInstance(instance);
	//获取材料名并录入到材料表
	wstring matname = getMatName(instance);
	int matno = recordMatTable(matname);
	long long rpInstance;
	sdaiGetAttrBN(instance, "Representation", sdaiINSTANCE, &rpInstance);
	vector<long long> repInstances = getInstancesByInstance(rpInstance, "Representations");
	for (auto repInstance : repInstances)
	{
		vector<long long> itemInstances = getInstancesByInstance(repInstance, "Items");
		for (auto itemInstance : itemInstances)
			parseItems(itemInstance, matno, relativeTmatrix*localTmatrix, isWall);
	}
}

std::vector<double> IFCTranslator::getCoordByCoordAggr(long long*  coordAggr)
{
	double x0 = 0, y0 = 0, z0 = 0;
	int cnt = sdaiGetMemberCount(coordAggr);
	engiGetAggrElement(coordAggr, 0, sdaiREAL, &x0);
	engiGetAggrElement(coordAggr, 1, sdaiREAL, &y0);
	if (cnt == 3)
	{
		engiGetAggrElement(coordAggr, 2, sdaiREAL, &z0);
	}
	//double len = getColumenLength(elemInstance);
	return { x0,y0 ,z0 };
}


std::vector<double> IFCTranslator::getDirectByDirectInstance(const long long& directInstance)
{
	double x, y, z;
	long long* drAggr;
	sdaiGetAttrBN(directInstance, "DirectionRatios", sdaiAGGR, &drAggr);
	engiGetAggrElement(drAggr, 0, sdaiREAL, &x);
	engiGetAggrElement(drAggr, 1, sdaiREAL, &y);
	engiGetAggrElement(drAggr, 2, sdaiREAL, &z);
	return { x,y,z };
}

void IFCTranslator::parseBuilding(const long long& buildingInstance, Eigen::Matrix4d relativeTmatrix)
{
	Matrix4d localTmatrix = getTMatrixByInstance(buildingInstance);
	vector<long long> isDecInstances = getInstancesByInstance(buildingInstance, "IsDecomposedBy");
	for (auto isDecInstance : isDecInstances)
	{
		vector<long long> bsInstances = getInstancesByInstance(isDecInstance, "RelatedObjects");
		for (auto bsInstance : bsInstances)
		{
			string elemtype = engiGetInstanceClassInfoUC(bsInstance);
			if (elemtype == "IFCBUILDINGSTOREY")
			{
				double elevation;
				sdaiGetAttrBN(bsInstance, "Elevation", sdaiREAL, &elevation);
				mElevations[mStoreyNum] = elevation;
				parseBuildingStorey(bsInstance, relativeTmatrix*localTmatrix);
				mStoreyNum++;
			}
		}
	}
	//如果没有楼层概念，则直接执行解析构件
	parseBuildingStorey(buildingInstance, relativeTmatrix*localTmatrix);
}

std::vector<long long> IFCTranslator::getInstancesByAggr(long long* aggr)
{
	vector<long long> rev;
	int cnt = sdaiGetMemberCount(aggr);
	for (int i = 0; i < cnt; i++)
	{
		long long instance;
		engiGetAggrElement(aggr, i, sdaiINSTANCE, &instance);
		rev.push_back(instance);
	}
	return rev;
}

vector<long long> IFCTranslator::getInstancesByInstance(long long instance, char* name)
{
	long long* aggr = nullptr;
	sdaiGetAttrBN(instance, name, sdaiAGGR, &aggr);
	return getInstancesByAggr(aggr);
}

void IFCTranslator::parseSite(const long long& siteInstance)
{
	Matrix4d localTmatrix = getTMatrixByInstance(siteInstance);
	vector<long long> isDecInstances = getInstancesByInstance(siteInstance, "IsDecomposedBy");
	for (auto isDecInstance : isDecInstances)
	{
		vector<long long> buildingInstances = getInstancesByInstance(isDecInstance, "RelatedObjects");
		for (auto buildingInstance : buildingInstances)
		{
			parseBuilding(buildingInstance, localTmatrix);
		}
	}
}

Eigen::Matrix4d IFCTranslator::getTMatrixByInstance(const long long& instance)
{
	long long opInstance;
	sdaiGetAttrBN(instance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	if (opInstance < 0)return Matrix4d::Identity();
	long long rpInstance;
	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
	if (rpInstance < 0)return Matrix4d::Identity();
	return getTMatrixByIfcAxis2Placement3D(rpInstance);
}

Eigen::Matrix4d IFCTranslator::getTMatrixByIfcAxis2Placement3D(const long long& instance)
{
	Matrix4d T = Matrix4d::Identity();
	if (instance <= 0)
		return T;
	//获取原点坐标
	long long locInstance;
	sdaiGetAttrBN(instance, "Location", sdaiINSTANCE, &locInstance);
	if (locInstance > 0)
	{
		long long* loccoordAggr = nullptr;
		sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &loccoordAggr);
		vector<double> pan = getCoordByCoordAggr(loccoordAggr);
		for (int i = 0; i < 3; i++)
			T(i, 3) = pan[i];
	}
	//获取z轴方向向量
	long long axisInstance;
	sdaiGetAttrBN(instance, "Axis", sdaiINSTANCE, &axisInstance);
	vector<double> zdim = { 0,0,1 };
	if (axisInstance > 0)
	{
		long long* zdimAggr = nullptr;
		sdaiGetAttrBN(axisInstance, "DirectionRatios", sdaiAGGR, &zdimAggr);
		zdim = getCoordByCoordAggr(zdimAggr);
	}
	//获取x轴方向向量
	long long refInstance;
	sdaiGetAttrBN(instance, "RefDirection", sdaiINSTANCE, &refInstance);
	vector<double> xdim = { 1,0,0 };
	if (refInstance > 0)
	{
		long long* xdimAggr = nullptr;
		sdaiGetAttrBN(refInstance, "DirectionRatios", sdaiAGGR, &xdimAggr);
		xdim = getCoordByCoordAggr(xdimAggr);
	}
	vector<double> ydim = crossProduct(zdim, xdim);
	for (int i = 0; i < 3; i++)
	{
		T(i, 0) = xdim[i];
		T(i, 1) = ydim[i];
		T(i, 2) = zdim[i];
	}
	return T;
}

std::vector<double> IFCTranslator::getCoordinate(const long long & instance)
{
	long long opInstance;
	sdaiGetAttrBN(instance, "ObjectPlacement", sdaiINSTANCE, &opInstance);
	long long rpInstance;
	sdaiGetAttrBN(opInstance, "RelativePlacement", sdaiINSTANCE, &rpInstance);
	long long locInstance;
	sdaiGetAttrBN(rpInstance, "Location", sdaiINSTANCE, &locInstance);
	if (locInstance > 0)
	{
		long long* coordAggr = nullptr;
		sdaiGetAttrBN(locInstance, "Coordinates", sdaiAGGR, &coordAggr);
		return getCoordByCoordAggr(coordAggr);
	}
	else
		return { 0,0,0 };
}

void IFCTranslator::translateNewVersion(CString ifcFileName, std::basic_string<wchar_t> ifcSchemaName)
{
	wchar_t fileName[512];
	memcpy(fileName, ifcFileName, ifcFileName.GetLength() * sizeof(wchar_t));
	fileName[ifcFileName.GetLength()] = 0;
	long long model = 0;
	model = sdaiOpenModelBNUnicode(0, (char*)fileName, (char*)ifcSchemaName.c_str());
	long long* siteAggr = sdaiGetEntityExtentBN(model, "IFCSITE");
	vector<long long> siteInstances = getInstancesByAggr(siteAggr);
	if (siteInstances.size() > 0)
	{
		for (auto siteInstance : siteInstances)
		{
			parseSite(siteInstance);
		}
	}
	else
	{
		long long* buildingAggr = sdaiGetEntityExtentBN(model, "IFCBUILDING");
		vector<long long> buildingInstances = getInstancesByAggr(buildingAggr);
		for (auto buildingInstance : buildingInstances)
			parseBuilding(buildingInstance, Matrix4d::Identity());
	}

	splitBeams();
	writeFPMT();
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
	if (associationAggr == nullptr)
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
	if (matname == 0)return L"empty";
	return matname;
}

void IFCTranslator::writeFPMT()
{
	//建立新表，使键值为编号，属性值对应材料、截面、坐标参数，以使得编号值能够得到排序
	//map<int, vector<double>> nodeTable;
	map<int, vector<double>> sectTable;
	//map<int, std::wstring> sectTable;
	map<int, wstring> matTable;
	//for (auto itr : mNodeTable)
	//	nodeTable[itr.second] = itr.first;
	for (auto itr : mMatTable)
		matTable[itr.second] = itr.first;
	for (auto itr : mSectTable)
		sectTable[itr.second] = itr.first;

	for (auto itr : matTable)
		mFPMTWriter.addLEMat(itr.first);

	for (auto itr : sectTable)
	{
		if (abs(itr.second[0] - 1) < ERRORDOUBLE)
			mFPMTWriter.addRectSect(itr.first, itr.second[1] / 1000.0, itr.second[2] / 1000.0, itr.second[3]);
		else if (abs(itr.second[0] - 2) < ERRORDOUBLE)
			mFPMTWriter.addThickSect(itr.first, itr.second[1] / 1000.0);
	}
	for (auto itr : mNodeTable)
		mFPMTWriter.addNode(itr.first, itr.second[0] / 1000.0, itr.second[1] / 1000.0, itr.second[2] / 1000.0);
	int elemno = 1;
	for (auto itr1 : mBeams)
		for (auto itr : itr1.second)
			mFPMTWriter.addBeamElem(elemno++, itr.mNode1, itr.mNode2, itr.mSectNum, itr.mMatNum);
	for (auto itr : mSolids)
	for (auto itr : mSolids)
		mFPMTWriter.addSolidElem(elemno++, itr.mNode1, itr.mNode2, itr.mNode3, itr.mNode4, itr.mMatNum);
	for (auto itr : mShells)
		mFPMTWriter.addShellElem(elemno++, itr.mNode1, itr.mNode2, itr.mNode3, itr.mSectNum, itr.mMatNum);
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
