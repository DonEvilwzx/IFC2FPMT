#pragma once
class IFCWriter
{
public:
	IFCWriter();
	~IFCWriter();
	void test();
	void writeIFC(wchar_t* filename);
};

