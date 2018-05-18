//////////////////////////////////////////////////////////////////////////////////////////////////
// 
// File: generator.h
// 
// Author: Victor Grinevich (C) All Rights Reserved
//
// System: Intel Pentium b970, 4096 DDR, Geforce 620m, Windows 7 Ultra, MSVC++ 12.0
//
// Desc: Provides utility functions for simplifying common tasks.
//          
//////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __generatorH__
#define __generatorH__


#include <string>
#include <limits>
#include <vector>
#include <iostream>



#pragma comment( lib,"winmm.lib" )

#define SETCOUNT 10
class Set{
public:
	Set();
	~Set();
	bool In( int el );
	void Init( int first, int last );
	void Add( int el );
	int Delete( int el ); 
	int Size();
	int MaxCount();
private:
	int _mas[SETCOUNT];
	int _curr;
	
};



enum LOGICOPERATOR{
		//NOT,
		AND,
		OR,
		XOR,//���������� ���� 0
		IMPLICATION,// �� 0 -> 1 ��� 0, �� 1 -> 1
		COUNT,
		LOGICZERO,
		LOGICONE
};

enum LEVEL{
	EASY ,
	MEDIUM ,
	HARD,
	COMBI
	//COUNT
};

enum {
	FREE = 0,
	NONFREE = 1,
	NILL = -1
};

const int MAS[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

#define MAXCOUNT  sizeof(MAS)/sizeof(int)

struct LogicObject{
	LOGICOPERATOR _lgcObj;
	int _in1, _in2, _out;
};

#define SHFOUT 81
//c���� ��� ��������� �������� ������

class Lobj{
public:
	Lobj();
	~Lobj();
	void Init( int key );
	void SetLvl( LEVEL lvl );
	int GetCount();
	int GetCountSocket();
	int GetOut( int curr );
	int GetIn1( int curr );
	int GetIn2( int curr );
	int GetObj( int curr );
	void SwapIn( int curr );
	void PrepareOper();
	LOGICOPERATOR RandomOper();
	
	void Releas();
	void PrintResult();
	int *_pSource, *_pReceiver;	
	LogicObject _pLgcObj[10];

private:
	
	int _row, _colomn, _cnt, _flag;
	
	LEVEL _lvl;
	
	Set _set;

	LOGICOPERATOR _prepare_mas[MAXCOUNT];
	int _prepare_size;

	bool Bind();
	int RollOperIn();
	int _riCount;// ����� ������ ��� ������� RollOperIn()
	int _riMas[ MAXCOUNT ];  // ������ ���  ������� RollOperIn()
	void RiInit();

	int RollOperOut( int num );
	int _roCount;// ����� ������ ��� ������� RollOperOut()
	int _roMas[ MAXCOUNT ];  // ������ ���  ������� RollOperOut()

	int RollSource();
	int _rsCount;// ����� ������ ��� ������� RollSource()
	int _rsMas[ MAXCOUNT ];  // ������ ���  ������� RollSource()
	void RsInit();
	void SecondRollSource( int * a, int curr);

	int RollReceiver();
	int _rrCount;// ����� ������ ��� ������� RollReceiver()
	int _rrMas[ MAXCOUNT ];  // ������ ���  ������� RollReceiver()
	void RrInit();
};


#endif // __generatorH__