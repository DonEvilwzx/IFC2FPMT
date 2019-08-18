#pragma once
#include<vector>
#include<unordered_set>
#include<sstream>
#include<string>
class FpmtWriter
{
	class Node {
	public:
		Node(int id, double x, double y, double z):id_(id),x_(x),y_(y),z_(z)
		{
		}
		std::string getOutputString()
		{
			std::ostringstream ss;
			ss << id_ << "," << x_ << "," << y_ << "," << z_<<std::endl;
			return ss.str();
		}
		//void setId(int id) { id_ = id; }
		//void setX(double x) { x_ = x; }
		//void setY(double y) { y_ = y; }
		//void setZ(double z) {z_ = z;}
	private:
		int id_;
		double x_;
		double y_;
		double z_;
	};
	class Element {
	public:
		std::string getOutputString()
		{
			std::ostringstream ss;
			ss << id_ << "," << type_ << "," ;
			for (auto itr : vnodenum_)
				ss << itr << ",";
			ss << realno_ << "," << sectno_ << "," << matno_<<std::endl;
			return ss.str();
		}
		Element(int id,int type,std::vector<int> vnodenum,int realno,int sectno,int matno):
		id_(id),type_(type),vnodenum_(vnodenum),realno_(realno),sectno_(sectno),matno_(matno)
		{	
		}
		//void setId(int id) { elemid_ = id; }
		//void setType(int type) { type_ = type; }
		//void setVnodeNum(std::vector<int> vnodenum) { vnodenum_ = vnodenum; }
		//void setRealNo(int realno) { realno_ = realno; }
		//void setSectNo(int sectno) { sectno_ = sectno; }
		//void setMatNo(int matno) { matno_ = matno; }
	private:
		int id_;
		int type_;
		std::vector<int> vnodenum_;
		int realno_;
		int sectno_;
		int matno_;
	};
public:
	void writeFpmt();
	void addNode(int id, double x, double y, double z);
	void addElem(int id, int type, std::vector<int> vnodenum, int realno, int sectno, int matno);
	bool emptySect();
	void addSect(int id,int b,int h);
	FpmtWriter();
	~FpmtWriter();
private:
	std::unordered_set<std::string> smaterial_;
	std::unordered_set<std::string> ssect_;
	std::unordered_set<std::string> sreal_;
	std::vector<Node> vnode_;
	std::vector<Element> velem_;
};

