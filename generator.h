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
		XOR,//одинаковые дают 0
		IMPLICATION,// из 0 -> 1 или 0, из 1 -> 1
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
//cдвиг для нумерации выходных портов

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
	int _riCount;// число входов для функции RollOperIn()
	int _riMas[ MAXCOUNT ];  // массив для  функции RollOperIn()
	void RiInit();

	int RollOperOut( int num );
	int _roCount;// число входов для функции RollOperOut()
	int _roMas[ MAXCOUNT ];  // массив для  функции RollOperOut()

	int RollSource();
	int _rsCount;// число входов для функции RollSource()
	int _rsMas[ MAXCOUNT ];  // массив для  функции RollSource()
	void RsInit();
	void SecondRollSource( int * a, int curr);

	int RollReceiver();
	int _rrCount;// число входов для функции RollReceiver()
	int _rrMas[ MAXCOUNT ];  // массив для  функции RollReceiver()
	void RrInit();
};


#endif // __generatorH__